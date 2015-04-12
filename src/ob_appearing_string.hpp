#ifndef OB_APPEARING_STRING_HPP
#define OB_APPEARING_STRING_HPP

#include "defaults.hpp"
#include "ob_visualization.hpp"

#include <string>
#include <vector>

namespace ob
{
  /** \brief Super-wanker -appearing string.
   */
  class AppearingString
  {
    private:
      /** Current string. */
      std::wstring m_curr;

      /** Previous string. */
      std::wstring m_prev;

      /** Visualization required for the string. */
      std::vector<Visualization> m_vis;

    public:
      /** \brief Get the current string.
       *
       * \return Current string non-obfuscated.
       */
      const std::wstring& getCurrent()
      {
        return m_curr;
      }

    public:
      /** \brief Empty constructor. */
      AppearingString() { }

      /** \brief Constructor.
       *
       * \param op String to init to.
       */
      AppearingString(const std::wstring &op);

      /** \brief Destructor. */
      ~AppearingString() { }

    public:
      /** \brief Get the current string.
       *
       * \return String, state may vary.
       */
      std::wstring get() const;

      /** \brief Set a new string.
       *
       * \param op New string.
       */
      void set(const std::wstring &op);

      /** \brief Update this. */
      void update();

      /** \brief Reset visuals. */
      void updateReset();
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<AppearingString> AppearingStringSptr;
}

#endif
