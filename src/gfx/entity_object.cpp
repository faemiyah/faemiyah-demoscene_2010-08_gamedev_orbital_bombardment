#include "gfx/entity_object.hpp"

#include "gfx/mesh.hpp"
#include "gfx/surface.hpp"

using namespace gfx;

EntityObject::EntityObject()
{
  this->enableDraw();
}

EntityObject::EntityObject(const math::vec3d &ppos, const math::vec3d &prot, const Mesh *pmesh) :
  Entity(ppos, prot),
  m_draw_disabled(false)
{
  this->addMesh(pmesh);
}

EntityObject::EntityObject(const math::vec3d &ppos, const math::vec3d &prot, const Mesh::container_type &op) :
  Entity(ppos, prot),
  m_draw_disabled(false)
{
  this->addMesh(op);
}

EntityObject::EntityObject(const Mesh *pmesh) :
  m_draw_disabled(false)
{
  this->addMesh(pmesh);
}

EntityObject::EntityObject(const Mesh::container_type &op) :
  m_draw_disabled(false)
{
  this->addMesh(op);
}

void EntityObject::addMesh(const Mesh *op)
{
  m_meshes.push_back(op);
  data::stl_trim(m_meshes);
}

void EntityObject::addMesh(const Mesh::container_type &op)
{
  BOOST_FOREACH(const MeshSptr &vv, op)
  {
    if(vv->isSubElement())
    {
      EntityObject *obj = new EntityObject();

      obj->addMesh(vv.get());
      obj->setPos(math::vec3d(vv->getOffset()));

      this->addSub(obj);
    }
    else
    {
      this->addMesh(vv.get());
    }
  }
}

void EntityObject::addSub(const std::string &pfname)
{
  Mesh::container_type &con = Mesh::locate(pfname);
  
  EntityObject *obj = new EntityObject();

  BOOST_FOREACH(const MeshSptr &vv, con)
  {
    obj->addMesh(vv.get());
    obj->setPos(math::vec3d(vv->getOffset()));
  }

  this->addSub(obj);
}

void EntityObject::addSub(EntityObject *obj)
{
  m_objects.push_back(EntityObjectSptr(obj));

  data::stl_trim(m_objects);

  obj->m_wm.loadTranslation(math::vec3f(obj->m_pos));
}

void EntityObject::draw(const Shader &psh, const math::mat4f &pmat) const
{
  if(m_draw_disabled)
  {
    return;
  }

  math::mat4f tm = this->orient(pmat);

  BOOST_FOREACH(const Mesh* vv, m_meshes)
  {
    vv->draw(psh, tm);
  }
  BOOST_FOREACH(const EntityObjectSptr &vv, m_objects)
  {
    //std::cout << vv->getPos() << std::endl;
    vv->draw(psh, tm);
  }
}

math::mat4f EntityObject::orient(const math::mat4f &pmat) const
{
  math::mat4f ret = pmat * m_wm;
  load_transform(ret);
  return ret;
}

void EntityObject::subDir(unsigned idx, const math::vec3f &fw, const math::vec3f &up)
{
  EntityObject *obj = m_objects[idx].get();

  math::mat4f cm = math::transpose(m_wm);
  cm(3, 0) = 0.0f;
  cm(3, 1) = 0.0f;
  cm(3, 2) = 0.0f;

  math::vec4f tpos = cm * math::vec4f(fw.x(), fw.y(), fw.z(), 1.0f);
  math::vec3f fpos(obj->m_pos);

  obj->m_wm.loadLookAt(fpos,
      fpos + math::vec3f(tpos.x(), tpos.y(), tpos.z()),
      up);
  //std::cout << m_objects[idx]->m_wm;
}

void EntityObject::subLoadIdentity(unsigned idx)
{
  EntityObject *obj = m_objects[idx].get();

  obj->m_wm.loadTranslation(math::vec3f(obj->m_pos));
}

void EntityObject::subLookAt(unsigned idx, const math::vec3f &tgt, const math::vec3f &up)
{
  EntityObject *obj = m_objects[idx].get();

  math::mat4f cm = m_wm;
  cm.convertToView();

  math::vec4f tpos = cm * math::vec4f(tgt.x(), tgt.y(), tgt.z(), 1.0f);
  math::vec3f fpos(obj->m_pos),
    rpos(tpos.x(), tpos.y(), tpos.z());

  obj->m_wm.loadLookAt(fpos,
      math::vec3f(tpos.x(), tpos.y(), tpos.z()),
      up);
}

