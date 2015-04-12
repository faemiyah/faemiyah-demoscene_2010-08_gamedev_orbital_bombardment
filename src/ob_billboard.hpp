#ifndef OB_BILLBOARD_HPP
#define OB_BILLBOARD_HPP

#include <gfx/point_sprite.hpp>
#include <gfx/point_sprite_array.hpp>

namespace gfx
{
  class Texture2D;
}

namespace ob
{
  /** \brief Convenience typedef. */
  typedef gfx::PointSpriteArray<gfx::PointSprite> BillboardArray;

  /** \brief Billboarding entity.
   */
  class Billboard
  {
    protected:
      /** Texture associated. */
      const gfx::Texture2D *m_billboard;

      /** Disable billboard rendering flag. */
      bool m_billboard_disabled;

    public:
      /** Accessor.
       *
       * \return Billboard texture.
       */
      const gfx::Texture2D* getBillboard() const
      {
        return m_billboard;
      }

    public:
      /** \brief Empty constructor. */
      Billboard() { }

      /** \brief Constructor.
       *
       * \param tex Texture to use.
       */
      Billboard(const gfx::Texture2D *tex);

      /** \brief Destructor. */
      virtual ~Billboard() { }

    public:
      /** \brief Enable billboard on random level.
       *
       * Disables the billboard if random value between 0 level is 0.
       *
       * \param level Randomize level.
       */
      void enableRandom(int level);

      /** \brief Fill to 2D billboard buffer.
       *
       * \param tex Previous texture.
       * \return The texture of this if filled, NULL if nothing done.
       */
      virtual const gfx::Texture2D* fillBillboard(BillboardArray &parray,
          const gfx::Shader &sh, const gfx::Texture2D *tex) const = 0;

    protected:
      /** \brief Check if matching texture, if not, flush.
       *
       * Check if given texture matches the texture in this, and if not,
       * flushes previous billboards and starts writing new ones with
       * the texture of this.
       *
       * \param tex Previous texture.
       */
      const gfx::Texture2D* updateBillboard(BillboardArray &parray,
          const gfx::Shader &sh, const gfx::Texture2D *tex) const;

    public:
      /** \brief Set the billboard of this.
       *
       * \param op New billboard.
       */
      void setBillboard(const gfx::Texture2D *op)
      {
        m_billboard = op;
      }
  };
}

#endif
