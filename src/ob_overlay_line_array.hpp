#ifndef OB_OVERLAY_LINE_ARRAY_HPP
#define OB_OVERLAY_LINE_ARRAY_HPP

#include "gfx/buffer.hpp"
#include "gfx/geometry_array.hpp"
#include "gfx/shader.hpp"
#include "gfx/texture_2d.hpp"

template<class T> class OverlayLineArray : public gfx::GeometryArray<T>
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
    OverlayLineArray(unsigned psize = 64, unsigned pmax = 8192) :
      gfx::GeometryArray<T>(psize, pmax) { }

  public:
    /** \brief Feed this to GPU.
     *
     * Requires appropriate functions from the instanciated class.
     *
     * \param sh Shader to use.
     * \param tex Texture to use.
     */
    void feed(const gfx::Shader &sh, const gfx::Texture2D &tex) const
    {
      if(!this->empty())
      {
        const gfx::Attribute &tt = sh.getAttrTexCoord();
        const gfx::Attribute &cc = sh.getAttrColor();
        const gfx::Attribute *v1 = sh.getAttribute("vertex1");
        const gfx::Attribute *v2 = sh.getAttribute("vertex2");
        const T* element = this->getArray();
        ptrdiff_t blocksize = reinterpret_cast<const uint8_t*>(&element[1]) -
          reinterpret_cast<const uint8_t*>(&element[0]);

        BOOST_ASSERT(NULL != v1);
        BOOST_ASSERT(NULL != v2);

        tex.bind();

        gfx::enable_buffers(tt, cc, *v1, *v2);
        glVertexAttribPointer(tt.id(), 2, GL_FLOAT, GL_FALSE, blocksize, element->texcoordPointer());
        glVertexAttribPointer(cc.id(), 4, GL_FLOAT, GL_FALSE, blocksize, element->colorPointer());
        glVertexAttribPointer(v1->id(), 3, GL_FLOAT, GL_FALSE, blocksize, element->vertexPointer1());
        glVertexAttribPointer(v2->id(), 3, GL_FLOAT, GL_FALSE, blocksize, element->vertexPointer2());

        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(this->size()));
      }
    }
};

#endif
