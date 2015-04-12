#include "gfx/mesh_planet.hpp"

#include "data/log.hpp"
#include "gfx/height_map_ball.hpp"
#include "gfx/shader.hpp"
#include "ui/generic.hpp"
#include "thr/dispatch.hpp"

namespace fs = boost::filesystem;
using namespace gfx;

MeshPlanet::MeshPlanet(unsigned subdivision,
    unsigned subdivision_coalesce, const HeightMapBall *hmap,
    unsigned texture_detail)
{
  this->compile(subdivision, subdivision_coalesce, hmap, texture_detail);
}

void MeshPlanet::compile()
{
  this->compile(0);
}

void MeshPlanet::compile(unsigned subdivision, unsigned subdivision_coalesce, const HeightMapBall *hmap,
    unsigned texture_detail, unsigned volume_detail, const std::string &fname_header, bool psave)
{
  this->subdivide(subdivision, true);

  // Height mapping phase. The image loads will throw an exception, and either all images are specified or none
  // are.
  if(hmap)
  {
    for(unsigned kk = 0; (kk < 20); kk += 2)
    {
      fs::path fname;
      {
        std::ostringstream sstream_fname;
        sstream_fname << fname_header << "_map_" << texture_detail << "_" << (kk / 2) << ".png";
        fname = sstream_fname.str();
      }

      if(data::file_exists(fname))
      {
        this->addTextureFile(std::string("texture"), fname, ImageLoader().clamp().noPremultiplyAlpha());
      }
      else
      {
        ImageRGBA mtex(texture_detail, texture_detail);
        {
          std::stringstream sstr;
          sstr << "creating planet facets " << kk << " and " << (kk + 1);
          data::log(sstr.str());
        }

        Lod &lod1 = *(m_lod.getRecursive()[kk + 0]),
            &lod2 = *(m_lod.getRecursive()[kk + 1]);
        Triangle &tt1 = lod1.getFaces().front(),
                 &tt2 = lod2.getFaces().front();
        if((lod1.getFaces().size() > 1) || (lod2.getFaces().size() > 1))
        {
          BOOST_THROW_EXCEPTION(std::runtime_error("non-single LOD unit"));
        }
        if((tt1.b() != tt2.b()) || (tt1.c() != tt2.a()))
        {
          BOOST_THROW_EXCEPTION(std::runtime_error("malformed triangle"));
        }
        math::vec3f &vv1 = m_vertex[tt1.a()],
          &vv2 = m_vertex[tt1.b()],
          &vv3 = m_vertex[tt1.c()],
          &vv4 = m_vertex[tt2.c()],
          vx1(vv2 - vv1),
          vy1(vv3 - vv1),
          vx2(vv3 - vv4),
          vy2(vv2 - vv4);

        float fwidth = static_cast<float>(mtex.getWidth() - 1),
              fheight = static_cast<float>(mtex.getHeight() - 1);
        for(unsigned ii = 0; (ii < mtex.getWidth()); ++ii)
        {
          float fi = static_cast<float>(ii) / fwidth;

          for(unsigned jj = 0; (jj < mtex.getHeight()); ++jj)
          {
            float fj = static_cast<float>(jj) / fheight;

            math::vec3f vv;
            if(fi + fj < 1.0f)
            {
              vv = (fi * vx1 + fj * vy1) + vv1;
            }
            else
            {
              vv = ((1.0f - fi) * vx2 + (1.0f - fj) * vy2) + vv4;
            }

            float ht = hmap->calcTerrain(vv);
            math::vec3f nn = hmap->calcNormal(vv, 1.0f / (static_cast<float>(texture_detail) * 0.125f), ht);
            nn = nn * 0.5f + 0.5f;
            mtex.setPixel(ii, jj, math::vec4f(nn.x(), nn.y(), nn.z(), ht));
          }
        }
        if(psave)
        {
          std::stringstream sstr;
          sstr << "saving " << fname;
          data::log(sstr.str());
          mtex.write(fname);
        }
        Texture2D *tex = new Texture2D(mtex, ImageLoader().clamp());
        Texture2D::store(canonize(fname), tex);
        this->addTexture("texture", tex);
      }
    }

    // Vertex height set phase after creating the correct polygons.
    for(unsigned ii = 0; (ii < m_vertex.size()); ++ii)
    {
      m_vertex[ii] = hmap->normalizeHeight(m_vertex[ii]);
    }
  }

  this->coalesce(subdivision_coalesce, subdivision);
  this->createVolumes(fname_header, volume_detail, psave);

  m_color.clear();
  m_normal.clear();
  data::stl_trim(m_color);
  data::stl_trim(m_normal);
  data::stl_trim(m_texcoord);
  data::stl_trim(m_vertex);

  m_lod.compile(m_vertex);

  thr::wait_privileged(&Mesh::upload, this);
}

void MeshPlanet::unreserve()
{
  m_buf.unreserve();
  m_volumes.clear();
  this->Mesh::unreserve();
}

void MeshPlanet::upload()
{
    m_buf.upload(m_texcoord, m_vertex);
    m_elem.upload(m_lod);
}
