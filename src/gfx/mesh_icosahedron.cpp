#include "gfx/mesh_icosahedron.hpp"

#include "data/generic.hpp"
#include "gfx/edge.hpp"
#include "gfx/lod_icosahedron.hpp"
#include "ui/generic.hpp"

static const float PHI = 0.5f * (1 + sqrtf(5.0f)),
             IRAD = sqrtf(PHI * PHI + 1),
             IB = PHI / IRAD,
             IS = 1.0f / IRAD;

/** \brief Icosahedron vertex table.
 *
 * The specifyign order is identical to the Wikipedia Icosahedron vertex
 * specification, scaled down to radius of 1:
 * (0, ±1, ±φ)
 * (±1, ±φ, 0)
 * (±φ, 0, ±1)
 */
static const float icosahedron_ver[] = 
{
  0.0f, IB, IS, // 0
  0.0f, -IB, IS, // 1
  0.0f, IB, -IS, // 2
  0.0f, -IB, -IS, // 3
  IB, IS, 0.0f, // 4
  -IB, IS, 0.0f, // 5
  IB, -IS, 0.0f, // 6
  -IB, -IS, 0.0f, // 7
  IS, 0.0f, IB, // 8
  -IS, 0.0f, IB, // 9
  IS, 0.0f, -IB, // 10
  -IS, 0.0f, -IB // 11
};

/** \brief Icosahedron vertex table.
 *
 * These are specified in clumps of two faces, each one of which is valid for
 * one texture when finally mapped, thus, when starting to subdivide the
 * icosahedron, every two rectangles are spliced into their individual new
 * non-connected vertex blocks.
 *
 * Facets are specified thus that vertices 1-2 and 3-4 are common and 0 and 5
 * are unique. In practice, facet vertex 1 is always facet vertex 4 and facet
 * vertex 2 is always facet vertex 3.
 */
static const unsigned icosahedron_idx[] =
{
  // Front facet.
  0, 9, 8,
  8, 9, 1,

  // Top right facet.
  8, 4, 0,
  0, 4, 2,

  // Top left facet.
  9, 0, 5,
  5, 0, 2,
  
  // Front left facet.
  5, 7, 9,
  9, 7, 1,

  // Front right facet.
  4, 8, 6,
  6, 8, 1,

  // Bottom facet.
  7, 3, 1,
  1, 3, 6,

  // Back left facet.
  7, 5, 11,
  11, 5, 2,

  // Remainder facet 1.
  4, 6, 10,
  10, 6, 3,

  // Remaineder facet 2.
  11, 2, 10,
  10, 2, 4,

  // Remainder facet 3.
  10, 3, 11,
  11, 3, 7
};

using namespace gfx;

/** \brief Internal calclulation only. */
typedef std::map<uint64_t, std::pair<unsigned, Edge> > IcoEdgeMap;

/** \brief Internal calclulation only. */
IcoEdgeMap edges;

/** \brief Add a new edge to the temporary internal map.
 *
 * \param msh Mesh to modify if new edge.
 * \param pe Edge to save.
 * \return Corresponding vertex index.
 */
unsigned calc_edge_vertex(MeshIcosahedron &msh, const Edge &pe)
{
  uint64_t eid = pe.idSameEdge();
  IcoEdgeMap::iterator iter = edges.find(eid);
  if(edges.end() == iter)
  {
    unsigned ret = msh.createVertex(pe.a(), pe.b());
    edges[eid] = std::pair<unsigned, Edge>(ret, pe);
    return ret;
  }
  return (*iter).second.first;
}

unsigned MeshIcosahedron::createVertex(unsigned pa, unsigned pb)
{
  Color ca(m_color[pa]),
        cb(m_color[pb]);
  m_color.push_back((ca + cb) * 0.5f);

  // Double calculation in this function done for accuracy, errors here would
  // propagate.
  math::vec2d ta(m_texcoord[pa]),
    tb(m_texcoord[pb]);
  m_texcoord.push_back(math::vec2f((ta + tb) * 0.5));

  unsigned ret = static_cast<unsigned>(m_vertex.size());
  math::vec3d va(m_vertex[pa]),
    vb(m_vertex[pb]);
  m_vertex.push_back(math::vec3f(math::normalize((va + vb) * 0.5)));
  return ret;
}

void MeshIcosahedron::coalesce(unsigned cnt, unsigned subdivision)
{
  if(cnt <= 0)
  {
    return;
  }

  // Unspecified subdivision -> no check.
  if(subdivision == UINT_MAX)
  {
    BOOST_FOREACH(LodSptr &vv, m_lod.getRecursive())
    {
      (*vv).coalesce(cnt);
    }
    return;
  }

  // Fallback with check.
  if(subdivision > 0)
  {
    BOOST_FOREACH(LodSptr &vv, m_lod.getRecursive())
    {
      (*vv).coalesce(math::min(subdivision, cnt));
    }
  }
}

void MeshIcosahedron::init()
{
  m_vertex.clear();
  for(unsigned ii = 0;
      (ii < (sizeof(icosahedron_ver) / sizeof(float)));
      ii += 3)
  {
    m_vertex.push_back(math::vec3f(icosahedron_ver[ii + 0],
          icosahedron_ver[ii + 1],
          icosahedron_ver[ii + 2]));
  }
  data::stl_trim(m_vertex);

  m_lod.unreserve();
  for(unsigned ii = 0;
      (ii < (sizeof(icosahedron_idx) / sizeof(unsigned)));
      ii += 3)
  {
    m_lod.addFace(Triangle(icosahedron_idx[ii + 0],
          icosahedron_idx[ii + 1],
          icosahedron_idx[ii + 2]));
  }

  m_color.resize(m_vertex.size(), Color(1.0f, 1.0f, 1.0f, 1.0f));
  m_texcoord.resize(m_vertex.size(), math::vec2f(1.0f, 1.0f));
}

void MeshIcosahedron::subdivide(unsigned subdivision, bool split_for_texturing)
{
  // Must first initialize data, since compilation is destructive.
  this->init();

  if(subdivision > 0)
  {
    //ui::log("subdividing icosahedron mesh");
    TriVec &faces = m_lod.getFaces();

    // Correct texturing requires each facet double to have individual vertices.
    if(split_for_texturing)
    {
      std::map<unsigned, bool> vtaken;
      for(unsigned ii = 0; (ii < faces.size()); ii += 2)
      {
        Triangle &f1 = faces[ii + 0],
                 &f2 = faces[ii + 1];

        // First.
        if(vtaken.end() != vtaken.find(f1.a()))
        {
          m_color.push_back(Color(1.0f, 1.0f, 1.0f, 1.0f));
          m_texcoord.push_back(math::vec2f(0.0f, 0.0f));
          m_vertex.push_back(m_vertex[f1.a()]);
          f1.a() = static_cast<unsigned>(m_vertex.size()) - 1;
        }
        else
        {
          m_texcoord[f1.a()] = math::vec2f(0.0f, 0.0f);
        }
        vtaken[f1.a()] = true;
        // Last.
        if(vtaken.end() != vtaken.find(f2.c()))
        {
          m_color.push_back(Color(1.0f, 1.0f, 1.0f, 1.0f));
          m_texcoord.push_back(math::vec2f(1.0f, 1.0f));
          m_vertex.push_back(m_vertex[f2.c()]);
          f2.c() = static_cast<unsigned>(m_vertex.size()) - 1;
        }
        else
        {
          m_texcoord[f2.c()] = math::vec2f(1.0f, 1.0f);
        }
        vtaken[f2.c()] = true;
        // Connecting 1.
        if(vtaken.end() != vtaken.find(f1.b()))
        {
          m_color.push_back(Color(1.0f, 1.0f, 1.0f, 1.0f));
          m_texcoord.push_back(math::vec2f(1.0f, 0.0f));
          m_vertex.push_back(m_vertex[f1.b()]);
          f1.b() = static_cast<unsigned>(m_vertex.size()) - 1;
          f2.b() = f1.b();
        }
        else
        {
          m_texcoord[f1.b()] = math::vec2f(1.0f, 0.0f);
        }
        vtaken[f1.b()] = true;
        // Connecting 2.
        if(vtaken.end() != vtaken.find(f1.c()))
        {
          m_color.push_back(Color(1.0f, 1.0f, 1.0f, 1.0f));
          m_texcoord.push_back(math::vec2f(0.0f, 1.0f));
          m_vertex.push_back(m_vertex[f1.c()]);
          f1.c() = static_cast<unsigned>(m_vertex.size()) - 1;
          f2.a() = f1.c();
        }
        else
        {
          m_texcoord[f1.c()] = math::vec2f(0.0f, 1.0f);
        }
        vtaken[f1.c()] = true;
      }
    }

    // First phase.
    BOOST_FOREACH(const Triangle &vv, faces)
    {
      m_lod.addRecursive(new LodIcosahedron(vv.a(), vv.b(), vv.c()));
    }

    // Subsequent phases.
    for(unsigned ii = 0; (ii < subdivision); ++ii)
    {
      std::vector<Lod*> old_lods;
      m_lod.collectLowest(old_lods);

      BOOST_FOREACH(Lod *vv, old_lods)
      {
        const Triangle &tri = vv->getFaces().front();
        Edge e1(tri.a(), tri.b()),
             e2(tri.b(), tri.c()),
             e3(tri.c(), tri.a());
        unsigned v1 = calc_edge_vertex(*this, e1),
                 v2 = calc_edge_vertex(*this, e2),
                 v3 = calc_edge_vertex(*this, e3);
        vv->addRecursive(new LodIcosahedron(tri.a(), v1, v3));
        vv->addRecursive(new LodIcosahedron(tri.b(), v2, v1));
        vv->addRecursive(new LodIcosahedron(tri.c(), v3, v2));
        vv->addRecursive(new LodIcosahedron(v1, v2, v3));
      }
      edges.clear();
    }
  }
}

