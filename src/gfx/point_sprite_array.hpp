#ifndef GFX_POINT_SPRITE_ARRAY_HPP
#define GFX_POINT_SPRITE_ARRAY_HPP

#include "gfx/buffer.hpp"
#include "gfx/geometry_array.hpp"
#include "gfx/shader.hpp"
#include "gfx/texture_2d.hpp"

namespace gfx
{
  template<class T> class PointSpriteArray : public GeometryArray<T>
  {
    public:
      /** \brief Constructor.
       *
       * \param psize Initial size (default: 64).
       * \param pmax Maximum size (default: 8192), zero for unlimited.
       * \param pbillboardmin Minimum billboard size (default: 1/50 of screen).
       * \param pbillboardmax Maximum billboard size (default: 2/5 of screen).
       * \param pbillboardmul Billboard size multiplier (default: 1).
       */
      PointSpriteArray(unsigned psize = 64, unsigned pmax = 8192, float pbillboardmin = 0.02f,
          float pbillboardmax = 0.4f, float pbillboardmul = 1.0f) :
        GeometryArray<T>(psize, pmax, pbillboardmin, pbillboardmax, pbillboardmul) { }
    
    public:
      /** \brief Feed this to GPU.
       *
       * Requires appropriate functions from the instanciated class.
       *
       * \param sh Shader to use.
       * \param tex Texture to use.
       */
      void feed(const Shader &sh, const gfx::Texture2D &tex) const
      {
        if(!this->empty())
        {
          const Attribute &tt = sh.getAttrTexCoord();
          const Attribute &cc = sh.getAttrColor();
          const Attribute &vv = sh.getAttrVertex();
          const T* element = this->getArray();
          ptrdiff_t blocksize = reinterpret_cast<const uint8_t*>(&element[1]) -
            reinterpret_cast<const uint8_t*>(&element[0]);

          sh.getUniform("billboard_params")->update(GeometryArray<T>::m_billboard_size_min,
              GeometryArray<T>::m_billboard_size_max, GeometryArray<T>::m_billboard_size_mul);

          tex.bind();

          enable_buffers(tt, cc, vv);
          glVertexAttribPointer(tt.id(), 4, GL_FLOAT, GL_FALSE, blocksize, element->texCoordPointer());
          glVertexAttribPointer(cc.id(), 4, GL_FLOAT, GL_FALSE, blocksize, element->colorPointer());
          glVertexAttribPointer(vv.id(), 4, GL_FLOAT, GL_FALSE, blocksize, element->vertexPointer());

          glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(this->size()));
        }
      }
  };
}

#endif
