#ifndef COLOR_GRADIENT_HPP
#define COLOR_GRADIENT_HPP

#include "gfx/color.hpp"

#include <list>

namespace gfx
{
  /** Convenience typedef. */
  typedef std::pair<float, Color> ColorGradientPair;

  /** \brief Color gradient.
   *
   * Used for mapping values into colors.
   *
   * There is currently no checking over the values, so they must be added
   * to the gradient in increasing order.
   */
  class ColorGradient
  {
    private:
      /** Color list. */
      std::list<ColorGradientPair> m_colors;

    public:
      /** \brief Constructor. */
      ColorGradient() { }

      /** \brief Constructor.
       *
       * \param pv Value.
       * \param pc Color.
       */
      ColorGradient(float pv, const gfx::Color &pc);

      /** \brief Destructor. */
      ~ColorGradient() { };

    public:
      /** \brief Add a color into the gradient.
       *
       * \param pv Value.
       * \param pc Color.
       */
      void add(float pv, const Color &pc);

      /** \brief Map a value into a color.
       *
       * \param pv Value.
       * \return Color.
       */
      Color getColor(float pv) const;
  };
}

#endif
