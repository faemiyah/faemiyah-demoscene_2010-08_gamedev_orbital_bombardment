#ifndef OB_FADE_HPP
#define OB_FADE_HPP

namespace gfx
{
  class SurfaceScreen;
}

namespace ob
{
  /** Fade delta thingy shit. */
  static const float OB_FADE_DELTA = 0.0125f;

  /** \brief Screen fade effect. */
  class Fade
  {
    private:
      /** Fade level. */
      float m_level;

      /** Fade delta. */
      float m_delta;

    public:
      /** \brief Set the delta.
       *
       * \param op New delta.
       */
      void setDelta(float op)
      {
        m_delta = op;
      }

    public:
      /** \brief Constructor. */
      Fade();

      /** \brief Destructor. */
      ~Fade() { }

    public:
      /** \brief Draw fade effect on screen.
       *
       * 2D mode must be selected on the screen already.
       *
       * Will NOP if no fade.
       */
      void draw(const gfx::SurfaceScreen &pscreen) const;

      /** \brief Update.
       */
      void update();
  };
}

#endif
