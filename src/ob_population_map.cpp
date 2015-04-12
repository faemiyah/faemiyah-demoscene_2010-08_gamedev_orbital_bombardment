#include "ob_population_map.hpp"

#include "math/random.hpp"
#include "gfx/shader.hpp"
#include "thr/dispatch.hpp"
#include "ob_constants.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** Scale to transform real-world coordinate into population space. */
const float POP_SCALE = static_cast<float>(OB_POPULATION_DETAIL / 2) - 1.0f;

/** Offset to transform real-world coordinate into population space. */
const float POP_CENTER = static_cast<float>(OB_POPULATION_DETAIL - 1) * 0.5f;

/** \brief Expand to the next power of two.
*/
PopulationMap::PopulationMap() :
  gfx::VolumeGray16(OB_POPULATION_DETAIL, OB_POPULATION_DETAIL, OB_POPULATION_DETAIL),
  m_population(0) { }

PopulationMap::~PopulationMap()
{
  const VolumeGray16* null_texture = NULL;

  thr::wait_privileged(&PopulationMap::taskTexture, this, null_texture);
}

void PopulationMap::feed(const gfx::Shader &sh, unsigned idx) const
{
  //std::cout << "dscale" << std::endl;
  sh.getUniform("decal_scale")->update(POP_CENTER,
      POP_SCALE / OB_PLANET_RADIUS / static_cast<float>(OB_POPULATION_DETAIL));
  //std::cout << "decal" << std::endl;
  m_texture->bind(*(sh.getUniform("decal")), idx);
}

void PopulationMap::filter()
{
  uint8_t *tmp = new uint8_t[this->getSizeBytes()];
  memset(tmp, 0, this->getSizeBytes());

  for(unsigned kk = 1; (kk < m_d - 1); ++kk)
  {
    for(unsigned jj = 1; (jj < m_h - 1); ++jj)
    {
      for(unsigned ii = 1; (ii < m_w - 1); ++ii)
      {
        tmp[(kk * m_h * m_w + jj * m_w + ii) * 2] = this->filterCollect(ii, jj, kk);
      }
    }
  }

  delete[] m_data;
  m_data = tmp;
}

uint8_t PopulationMap::filterCollect(unsigned px, unsigned py, unsigned pz)
{
  int ret = 0;
  int neighbouring[27] =
  {
    this->filterGet(px - 1, py - 1, pz - 1),
    this->filterGet(px - 1, py - 1, pz + 0),
    this->filterGet(px - 1, py - 1, pz + 1),
    this->filterGet(px - 1, py + 0, pz - 1),
    this->filterGet(px - 1, py + 0, pz + 0),
    this->filterGet(px - 1, py + 0, pz + 1),
    this->filterGet(px - 1, py + 1, pz - 1),
    this->filterGet(px - 1, py + 1, pz + 0),
    this->filterGet(px - 1, py + 1, pz + 1),
    this->filterGet(px + 0, py - 1, pz - 1),
    this->filterGet(px + 0, py - 1, pz + 0),
    this->filterGet(px + 0, py - 1, pz + 1),
    this->filterGet(px + 0, py + 0, pz - 1),
    this->filterGet(px + 0, py + 0, pz + 0),
    this->filterGet(px + 0, py + 0, pz + 1),
    this->filterGet(px + 0, py + 1, pz - 1),
    this->filterGet(px + 0, py + 1, pz + 0),
    this->filterGet(px + 0, py + 1, pz + 1),
    this->filterGet(px + 1, py - 0, pz - 1),
    this->filterGet(px + 1, py - 0, pz + 0),
    this->filterGet(px + 1, py - 0, pz + 1),
    this->filterGet(px + 1, py + 0, pz - 1),
    this->filterGet(px + 1, py + 0, pz + 0),
    this->filterGet(px + 1, py + 0, pz + 1),
    this->filterGet(px + 1, py + 1, pz - 1),
    this->filterGet(px + 1, py + 1, pz + 0),
    this->filterGet(px + 1, py + 1, pz + 1)
  };

  for(unsigned ii = 0; (ii < 27); ++ii)
  {
    ret += neighbouring[ii];
  }

  return static_cast<uint8_t>(ret / 27);
}

int PopulationMap::filterGet(unsigned px, unsigned py, unsigned pz)
{
  return static_cast<int>(m_data[(pz * m_w * m_h + py * m_w + px) * 2]);
}

int PopulationMap::paint(const math::vec3f &pos, float str, bool update)
{
  float abs = math::abs(str);
  math::vec3f spos = (math::normalize(pos) * POP_SCALE) + POP_CENTER;
  math::vec3i ca(math::floor(spos.x()), math::floor(spos.y()), math::floor(spos.z())),
    cb(math::ceil(spos.x()), math::ceil(spos.y()), math::ceil(spos.z()));
  int ret = 0;

  // When updating in realtime, area must be 2^x or strange shit happens.
  if(update)
  {
    int logvalue = math::ceil(logf(abs) / logf(2.0f));
    int rsize = 2;
    for(int ii = 1; (ii < logvalue); ++ii)
    {
      rsize *= 2;
    }
    ca -= rsize - 1;
    cb += rsize - 1;
  }
  else
  {
    int rsize = math::lround(abs);
    ca -= rsize;
    cb += rsize;
  }

  //std::cout << ca << " ; " << cb << std::endl;

  // Must bind ca & cb to limits.
  if(update)
  {
    if(ca.x() < 0)
    {
      cb.x() += -ca.x();
      ca.x() = 0; 
    }
    else if(cb.x() > 255)
    {
      ca.x() -= cb.x() - 255;
      cb.x() = 255;
    }
    if(ca.y() < 0)
    {
      cb.y() += -ca.y();
      ca.y() = 0; 
    }
    else if(cb.x() > 255)
    {
      ca.y() -= cb.y() - 255;
      cb.y() = 255;
    }
    if(ca.z() < 0)
    {
      cb.z() += -ca.z();
      ca.z() = 0; 
    }
    else if(cb.z() > 255)
    {
      ca.z() -= cb.z() - 255;
      cb.z() = 255;
    }
  }
  else
  {
    ca.x() = math::max(ca.x(), 0);
    ca.y() = math::max(ca.y(), 0);
    ca.z() = math::max(ca.z(), 0);
    cb.x() = math::min(cb.x(), 255);
    cb.y() = math::min(cb.y(), 255);
    cb.z() = math::min(cb.z(), 255);
  }

  // Correct area.
  math::vec3i cc = (cb - ca) + 1;

  // Not needed always.
  uint8_t *upd_data = NULL;
  unsigned upd_idx = 0;
  if(update)
  {
    upd_data = new uint8_t[cc.x() * cc.y() * cc.z() * 2];
  }

  for(int kk = ca.z(); (kk <= cb.z()); ++kk)
  {
    unsigned plane_idx = static_cast<unsigned>(kk) * m_w * m_h;

    for(int jj = ca.y(); (jj <= cb.y()); ++jj)
    {
      unsigned row_idx = static_cast<unsigned>(jj) * m_w;

      for(int ii = ca.x(); (ii <= cb.x()); ++ii)
      {
        unsigned idx = plane_idx + row_idx + static_cast<unsigned>(ii);
        uint8_t *ptr = m_data + (idx * 2);
        math::vec3f coord(static_cast<float>(ii), static_cast<float>(jj),
            static_cast<float>(kk));
        float dist = math::length(coord - spos),
              curr_str = 1.0f - dist / abs;
        if(update)
        {
          //std::cout << "dist " << dist;
        }
        if(curr_str > 0.0f)
        {
          int diff = math::lround((curr_str * str) * 255.0f);
          int old_pop = static_cast<int>(ptr[0]);
          int old_rubble = static_cast<int>(ptr[1]);
          int new_pop = math::min(math::max(old_pop + diff, 0), 255);
          int new_rubble = math::min(math::max(old_rubble - diff, 0), 255);

          ret += new_pop - old_pop;
          //std::cout << diff << std::endl;
          ptr[0] = static_cast<uint8_t>(new_pop);
          ptr[1] = static_cast<uint8_t>(new_rubble);
        }
        if(update)
        {
          upd_data[upd_idx + 0] = ptr[0];
          upd_data[upd_idx + 1] = ptr[1];
          //std::cout << " -> " << static_cast<int>(ptr[1]) << std::endl;
          upd_idx += 2;
        }
      }
    }
  }

  if(update)
  {
    thr::wait_privileged(&PopulationMap::taskSubTexture, this, upd_data, boost::ref(ca), boost::ref(cc));
    delete[] upd_data;
  }

  m_population += static_cast<unsigned>(ret);
  return ret;
}

void PopulationMap::scale(float op)
{
  for(unsigned ii = 0; (ii < this->getSizeBytes()); ii += 2)
  {
    uint8_t *ptr = m_data + ii;
    int curr_pop = static_cast<int>(*ptr);
    *ptr = static_cast<uint8_t>(math::lround(static_cast<float>(curr_pop) * op));
  }
}

void PopulationMap::refresh()
{
  for(unsigned ii = 0; (ii < this->getSizeBytes()); ii += 2)
  {
    uint8_t *ptr = m_data + ii;
    int curr_pop = static_cast<int>(*ptr);
    if(curr_pop > OB_POPULATION_RANDOM_LIMIT)
    {
      curr_pop -= math::mrand(0, curr_pop - OB_POPULATION_RANDOM_LIMIT);
      *ptr = static_cast<uint8_t>(curr_pop);
    }
  }

  //std::cout << "waiting\n";
  thr::wait_privileged(&PopulationMap::taskTexture, this, this);
  //std::cout << "done\n";
}

void PopulationMap::taskTexture(const gfx::VolumeGray16 *vol)
{
  if(vol)
  {
    //std::cout << "creating texture\n";
    m_texture = gfx::Texture3DSptr(new gfx::Texture3D(*vol, gfx::ImageLoader().generateMipmaps()));
    //std::cout << "done\n";
  }
  else
  {
    m_texture = gfx::Texture3DSptr();
  }
}

void PopulationMap::taskSubTexture(const uint8_t *data, const math::vec3i &idx,
    const math::vec3i &size)
{
  m_texture->bind();
  glTexSubImage3D(GL_TEXTURE_3D, 0, idx.x(), idx.y(), idx.z(), size.x(), size.y(), size.z(),
      GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
  // TODO: is this fast enough on all cards?
}

