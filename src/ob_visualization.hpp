#ifndef OB_VISUALIZATION
#define OB_VISUALIZATION

namespace ob
{
  /** Default fadeout time. */
  static const int OB_VISUALIZATION_DEFAULT_FADEOUT = 100;

  /** \brief 2D visualization common functionality.
   */
  class Visualization
  {
    private:
      /** Visibility. */
      int m_visibility;

      /** Was this visible after last update? */
      bool m_visible;

    public:
      /** \brief Constructor. */
      Visualization();

      /** \brief Destructor. */
      ~Visualization() { }

    public:
      /** \brief Randomize the visibility status but do not alter state.
       *
       * \param Fadeout level.
       * \return Current visibility status.
       */
      bool randomizeVisibility(int fadeout = OB_VISUALIZATION_DEFAULT_FADEOUT);

      /** \brief Update visibility status.
       *
       * \param flag Visible this frame?
       * \return True if should be drawn, false otherwise.
       */
      bool updateVisibility(bool flag, int fadeout = OB_VISUALIZATION_DEFAULT_FADEOUT);

    public:
      /** \brief Can this be locked?
       *
       * \return True if yes, false if no.
       */
      bool canLock() const
      {
        return (m_visibility >= OB_VISUALIZATION_DEFAULT_FADEOUT);
      }

      /** \brief Can this be targeted?
       *
       * \return True if yes, false if no.
       */
      bool canTarget() const
      {
        return (m_visibility > 0);
      }

      /** \brief Accessor.
       *
       * User should generally not need this.
       *
       * \return Current visibility.
       */
      int getVisibility() const
      {
        return m_visibility;
      }

      /** \brief Tell if this is visible.
       *
       * Returns the last return value of updateVisibility.
       *
       * \return True if yes, false if no.
       */
      bool isVisible() const
      {
        return m_visible;
      }

      /** \brief Set the visibility.
       *
       * \param op New visibility.
       */
      void setVisibility(int op)
      {
        m_visibility = op;
      }
  };
}

#endif
