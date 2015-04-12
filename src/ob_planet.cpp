#include "ob_planet.hpp"

#include "data/log.hpp"
#include "math/random.hpp"
#include "gfx/color_gradient.hpp"
#include "gfx/shader.hpp"
#include "gfx/volume.hpp"
#include "ui/generic.hpp"
#include "thr/dispatch.hpp"
#include "ob_constants.hpp"
#include "ob_height_map_planet.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

/** \brief Generator for city noise.
 */
class PerlinCity :
  public gfx::Perlin
{
  public:
    /** \brief Constructor. */
    PerlinCity() { }

    /** \brief Destructor. */
    virtual ~PerlinCity() { }

  public:
    /** \cond */
    virtual gfx::Color noise(const std::list<gfx::VolumeGray8Sptr> &op, const math::vec3d &tt);
    /** \cond */
};

/** \brief Generator for rubble noise.
 */
class PerlinRubble :
  public gfx::Perlin
{
  private:
    /** Gradient for city colors. */
    gfx::ColorGradient m_gradient;

  public:
    /** \brief Constructor. */
    PerlinRubble();

    /** \brief Destructor. */
    virtual ~PerlinRubble() { }

  public:
    /** \cond */
    virtual gfx::Color noise(const std::list<gfx::VolumeGray8Sptr> &op,
        const math::vec3d &tt);
    /** \cond */
};

gfx::Color PerlinCity::noise(const std::list<gfx::VolumeGray8Sptr> &op,
    const math::vec3d &tt)
{
  float ns = 0.0f,
        wt = 1.0f;

  for(std::list<gfx::VolumeGray8Sptr>::const_reverse_iterator ii = op.rbegin(), ee = op.rend();
      (ii != ee); ++ii)
  {
    gfx::VolumeGray8 *vv = ii->get();
    ns += wt * math::abs((vv->getAveragePixel(tt.x(), tt.y(), tt.z()) - 0.5f));
    wt *= 0.5f;
  }
  //ns = ns*ns;

  //ns = ns + 0.5f;
  if(ns < 0.0f)
  {
    ns = -ns;
  }
  else if(ns > 1.0f)
  {
    ns = 2.0f - ns;
  }

  return gfx::Color(ns, ns, ns);
  //return m_gradient.getColor(ns);
}

PerlinRubble::PerlinRubble() :
  m_gradient(0.0f, gfx::Color(0.0f, 0.0f, 0.0f))
{
  m_gradient.add(0.4f, gfx::Color(0.1f, 0.05f, 0.0f));
  m_gradient.add(0.5f, gfx::Color(1.0f, 0.5f, 0.1f));
  m_gradient.add(0.6f, gfx::Color(0.1f, 0.05f, 0.0f));
  m_gradient.add(1.0f, gfx::Color(0.0f, 0.0f, 0.0f));
}

gfx::Color PerlinRubble::noise(const std::list<gfx::VolumeGray8Sptr> &op, const math::vec3d &tt)
{
  float ns = 0.0f,
        wt = 1.0f;

  for(std::list<gfx::VolumeGray8Sptr>::const_reverse_iterator ii = op.rbegin(), ee = op.rend();
      (ii != ee); ++ii)
  {
    gfx::VolumeGray8 *vv = ii->get();
    ns += wt * math::abs(vv->getAveragePixel(tt.x(), tt.y(), tt.z()) - 0.5f);
    wt *= 0.6f;
  }
  //ns = ns*ns;

  ns = ns + 0.5f;
  if(ns < 0.0f)
  {
    ns = -ns;
  }
  if(ns > 1.0f)
  {
    ns = 2.0f - ns;
  }
  if(ns < 0.0f)
  {
    ns = -ns;
  }

  /*gfx::Color ret;
  ret.r() = ns * 1.0f;
  ret.g() = ns * 0.8f;
  ret.b() = ns * 0.1f;
  ret.a() = 1.0f;
  return ret;*/

  return m_gradient.getColor(ns);
  //return gfx::Color(0.0f, 0.0f, 0.0f);
}

/** City generator. */
PerlinCity perlin_city;

/** Eubble generator. */
PerlinRubble perlin_rubble;

using namespace ob;

/** Storage name. */
const std::string ob::PLANET_FILENAME("gfx/maps/planet");

Planet::Planet(unsigned ps, unsigned pc, unsigned pt, unsigned pv, const HeightMapPlanet *hmap, bool generate)
{
  if(hmap)
  {
    this->compile(ps, pc, hmap, pt, pv, PLANET_FILENAME, generate);
  }
  else
  {
    HeightMapPlanet tmp_hmap;
    this->compile(ps, pc, &tmp_hmap, pt, pv, PLANET_FILENAME, generate);
  }
}

void Planet::createVolumes(const std::string &fname_header, unsigned vside, bool psave)
{
  for(unsigned ii = 0; (ii < 2); ++ii)
  {
    std::string volume_filename;
    {
      std::ostringstream sstream_fname;
      sstream_fname << fname_header << "_volume_" << vside << "_" << ii << ".png";
      volume_filename = sstream_fname.str();
    }

    if(data::file_exists(volume_filename))
    {
      this->addTextureFile(std::string("volume"), volume_filename, gfx::ImageLoader().noPremultiplyAlpha());
    }
    else
    {
      gfx::VolumeRGBA vol(vside, vside, vside);
      {
        std::ostringstream sstr;
        sstr << "creating volume " << ii;
        data::log(sstr.str());
      }
      
      switch(ii)
      {
        case 0:
          {
            gfx::VolumeGray8 vol1(vside, vside, vside),
                             vol2(vside, vside, vside),
                             vol3(vside, vside, vside),
                             vol4(vside, vside, vside);
            vol1.perlinNoise();
            vol2.perlinNoise();
            vol3.perlinNoise();
            vol4.perlinNoise();
            vol.combine(vol1, vol2, vol3, vol4);
          }
          break;
          
        case 1:
        default:
          {
            gfx::VolumeRGB vol1(vside, vside, vside); 
            gfx::VolumeGray8 vol2(vside, vside, vside);
            vol1.perlinNoise(&perlin_rubble);
            vol2.perlinNoise(&perlin_city);
            vol.combine(vol1, vol2);
          }
          break;
      }
      if(psave)
      {
        std::stringstream sstr;
        sstr << "saving '" << volume_filename << "'";
        data::log(sstr.str());
        vol.write(volume_filename);
      }
      
      gfx::Texture3D *tex = new gfx::Texture3D(vol);
      gfx::Texture3D::store(volume_filename, tex);
      this->addTexture("volume", tex);
    }
  }

#if 0
  for(unsigned ii = 0; (ii < vside); ++ii)
  {
    for(unsigned jj = 0; (jj < vside); ++jj)
    {
      for(unsigned kk = 0; (kk < vside); ++kk)
      {
        // Water.
        vol0.setPixel(kk, jj, ii,
          static_cast<uint8_t>(math::mrand(0, 20)),
          static_cast<uint8_t>(math::mrand(10, 30)),
          static_cast<uint8_t>(math::mrand(50, 130)));

        // Lowlands.
        vol1.setPixel(kk, jj, ii,
          static_cast<uint8_t>(math::mrand(30, 50)),
          static_cast<uint8_t>(math::mrand(50, 90)),
          static_cast<uint8_t>(math::mrand(30, 40)));
        
        // Mountains.
        vol2.setPixel(kk, jj, ii,
          static_cast<uint8_t>(math::mrand(120, 160)),
          static_cast<uint8_t>(math::mrand(70, 110)),
          26);
        
        // Arctic.
        vol3.setPixel(kk, jj, ii,
          static_cast<uint8_t>(math::mrand(175, 220)),
          static_cast<uint8_t>(math::mrand(175, 220)),
          static_cast<uint8_t>(math::mrand(175, 220)));
      }
    }
  }
#endif
  
  data::stl_trim(m_volumes);
}

void Planet::draw(const gfx::Shader &psh, const math::mat4f &pmat) const
{
  m_buf.bind(psh.getAttrTexCoord(), psh.getAttrVertex());
  m_elem.bind();

  if(!game || !game->isModeSchematic())
  {
    m_volumes[0]->bind(*(psh.getUniform("volume0")), 1); // Terrain intensities (4)
    m_volumes[1]->bind(*(psh.getUniform("volume1")), 2); // Rubble decal & city decal

    // City parameters.
    psh.getUniform("decal_0_color")->update(0.88f, 0.84f, 0.47f);
    psh.getUniform("decal_0_params")->update(0.06f, 0.35f, 0.01f, 2.7f);
    psh.getUniform("decal_1_params")->update(0.2f, 1.0f, 0.15f, 8.0f);
    
    // Terrain noise parameters.
    psh.getUniform("noise_scale")->update(0.00413f, 0.00131f, 0.00093f);
    // Water.
    psh.getUniform("terrain_0_color_0")->update(0.0f, 0.04f, 0.2f);
    psh.getUniform("terrain_0_color_1")->update(0.08f, 0.12f, 0.51f);
    // Lowlands.
    psh.getUniform("terrain_1_color_0")->update(0.06f, 0.12f, 0.08f);
    psh.getUniform("terrain_1_color_1")->update(0.18f, 0.37f, 0.14f);
    // Mountains
    psh.getUniform("terrain_2_color_0")->update(0.35f, 0.24f, 0.10f);
    psh.getUniform("terrain_2_color_1")->update(0.67f, 0.41f, 0.12f);
    // Arctic.
    psh.getUniform("terrain_3_color_0")->update(0.31f, 0.31f, 0.34f);
    psh.getUniform("terrain_3_color_1")->update(0.70f, 0.62f, 0.70f);
  }
  else // In schematic mode, any fancy texturing is not used.
  {
    //std::cout << "ht" << std::endl;
    psh.getUniform("schematic_height")->update(0.6f, 0.4f, 0.6f);
    //std::cout << "neutral" << std::endl;
    psh.getUniform("schematic_neutral")->update(0.3f, 0.2f, 0.3f);
    //std::cout << "pop0" << std::endl;
    psh.getUniform("schematic_pop_0")->update(0.9f, 0.3f, 0.1f);
    //std::cout << "pop1" << std::endl;
    psh.getUniform("schematic_pop_1")->update(1.0f, 0.6f, 0.2f);
  }
  
  const std::vector<gfx::LodSptr> &recursive(m_lod.getRecursive());
  for(unsigned ii = 0, tt = 0; (ii < recursive.size()); ii += 2, ++tt)
  {
    gfx::Lod &l1 = *(recursive[ii + 0]),
             &l2 = *(recursive[ii + 1]);
    m_textures[tt]->bind(psh.getUniformTex(), 0);
    l1.draw(pmat);
    l2.draw(pmat);
  }
}

#if 0
void PerlinCity::fill(gfx::VolumeGray8 &op, unsigned level)
{
  uint8_t *data = op.getData();
  
  unsigned vside = op.getDepth();
  for(unsigned ii = 0; (ii < vside); ++ii)
  {
    unsigned idx_ii = ii * vside * vside;
    unsigned aa = ii % 2;

    for(unsigned jj = 0; (jj < vside); ++jj)
    {
      unsigned idx_jj = idx_ii + jj * vside;
      unsigned bb = jj % 2;

      for(unsigned kk = 0; (kk < vside); ++kk)
      {
        unsigned idx_kk = idx_jj + kk;
        unsigned cc = kk % 2;

        if(1 < aa + bb + cc)
        {
          data[idx_kk] = math::mrand(128, 255);
        }
        else
        {
          data[idx_kk] = math::mrand(0, 127);
        }
      }
    }
  }
}
#endif

