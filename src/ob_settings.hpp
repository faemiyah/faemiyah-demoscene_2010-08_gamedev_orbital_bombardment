#ifndef OB_SETTINGS_HPP
#define OB_SETTINGS_HPP

#include "ob_high_scores.hpp"

namespace ob
{
  /** \brief Setting struct.
   *
   * Singular setting value.
   */
  template<typename T> struct setting
  {
    private:
      /** Setting value. */
      T m_value;

      /** Minimum value. */
      T m_min;

      /** Maximum value. */
      T m_max;

    public:
      /** \brief Accessor.
       *
       * \return Current value.
       */
      const T& get() const
      {
        return m_value;
      }

      /** \brief Accessor.
       *
       * \return Minimum value.
       */
      const T& getMin() const
      {
        return m_min;
      }

      /** \brief Accessor.
       *
       * \return Maximum value.
       */
      const T& getMax() const
      {
        return m_max;
      }

    public:
      /** \brief Set the value.
       *
       * \param pv New value.
       */
      void set(const T &pv)
      {
        //std::cout << "setting to " << pv << " [ " << m_min << " : " << m_max << " ] \n";
        if(pv < m_min)
        {
          m_value = m_min;
        }
        else if(pv > m_max)
        {
          m_value = m_max;
        }
        else
        {
          m_value = pv;
        }
      }

      /** \brief Set the values.
       *
       * \param pv New value.
       */
      void set(const T &pv, const T &pmin, const T &pmax)
      {
        //std::cout << "setting to " << pv << " and  " << pmin << " : " << pmax << " ] \n";
        m_min = pmin;
        m_max = pmax;
        this->set(pv);
      }

    public:
      /** \brief Empty constructor.
       */
      setting() { }

      /** \brief Constructor.
       *
       * \param pv Value.
       * \param pmin Minimum value.
       * \param pmax Maximum value.
       */
      setting(const T &pv, const T &pmin, const T &pmax) :
        m_min(pmin), m_max(pmax) { this->set(pv); }
  };

  /** Convenience typedef. */
  typedef setting<int> settingi;

  /** Convenience typedef. */
  typedef setting<float> settingf;

  /** \brief Settings container.
   *
   * Contains retrievable and settable settings.
   */
  class Settings
  {
    public:
      /** High score table. */
      HighScores m_high_scores;

      /** Mouse rotation speed. */
      settingf m_camera_rot_speed_x;

      /** Mouse rotation speed. */
      settingf m_camera_rot_speed_y;

      /** Music volume. */
      settingf m_volume_music;

      /** Audio volume. */
      settingf m_volume_samples;

      /** Fullscreen mode. */
      settingi m_fullscreen;

      /** Fullscreen mode. */
      std::string m_detail;

      /** Resolution string. */
      std::string m_resolution;

      /** Available resolutions. */
      std::vector<const char*> m_detail_levels;

      /** Available resolutions. */
      std::vector<const char*> m_resolutions;

    public:
      /** \brief Default constructor.
       *
       * Initializes with defaults and loads from default configuration file
       * if available.
       */
      Settings();

      /** \brief Constructor.
       *
       * Silently initializes with defaults if settings can not be found.
       *
       * \param pfname Load settings from this file.
       */
      Settings(const std::string &pfname);

      /** \brief Destructor. */
      ~Settings() { }

    private:
      /** \brief Initialize with default settings.
       */
      void defaults();

    public:
      /** \brief Load settings from file.
       *
       * Silently ignores if file cannot be found.
       *
       * \param pfname File to load from.
       */
      void load(const std::string &pfname);

      /** \brief Save settings into a file.
       *
       * Will throw an error if the file cannot be opened.
       *
       * \param pfname File to load from.
       */
      void save(const std::string &pfname);

      /** \brief Set music volume.
       *
       * \param op New music volume
       */
      void setVolumeMusic(float op);

      /** \brief Set music volume.
       *
       * \param op New music volume
       */
      void setVolumeSamples(float op);

    public:
      /** \brief Accessor.
       *
       * \return Mouse rotation speed setting.
       */
      settingf& getCameraRotSpeedX()
      {
        return m_camera_rot_speed_x;
      }

      /** \brief Accessor.
       *
       * \return Mouse rotation speed setting.
       */
      const settingf& getCameraRotSpeedX() const
      {
        return m_camera_rot_speed_x;
      }

      /** \brief Accessor.
       *
       * \return Mouse rotation speed setting.
       */
      settingf& getCameraRotSpeedY()
      {
        return m_camera_rot_speed_y;
      }

      /** \brief Accessor.
       *
       * \return Mouse rotation speed setting.
       */
      const settingf& getCameraRotSpeedY() const
      {
        return m_camera_rot_speed_y;
      }

      /** \brief Accessor.
       *
       * \return Detail level string.
       */
      const std::string& getDetail() const
      {
        return m_detail;
      }

      /** \brief Accessor.
       *
       * \return Detail level string.
       */
      const std::vector<const char*>& getDetailLevels() const
      {
        return m_detail_levels;
      }

      /** \brief Accessor.
       *
       * \return Fullscreen setting.
       */
      settingi& getFullscreen()
      {
        return m_fullscreen;
      }

      /** \brief Accessor.
       *
       * \return Fullscreen setting.
       */
      const settingi& getFullscreen() const
      {
        return m_fullscreen;
      }

      /** \brief Accessor.
       *
       * \return high scores.
       */
      HighScores& getHighScores()
      {
        return m_high_scores;
      }

      /** \brief Accessor.
       *
       * \return high scores.
       */
      const HighScores& getHighScores() const
      {
        return m_high_scores;
      }

      /** \brief Accessor.
       *
       * \return Resolution.
       */
      const std::string& getResolution() const
      {
        return m_resolution;
      }

      /** \brief Accessor.
       *
       * \return Resolution.
       */
      const std::vector<const char*>& getResolutions() const
      {
        return m_resolutions;
      }

      /** \brief Get current sensitivity.
       *
       * \return Current sensitivity value.
       */
      float getSensitivity() const
      {
        return m_camera_rot_speed_y.get();
      }

      /** \brief Accessor.
       *
       * \return Volume level.
       */
      settingf& getVolumeMusic()
      {
        return m_volume_music;
      }

      /** \brief Accessor.
       *
       * \return Volume level.
       */
      const settingf& getVolumeMusic() const
      {
        return m_volume_music;
      }

      /** \brief Accessor.
       *
       * \return Volume level.
       */
      settingf& getVolumeSamples()
      {
        return m_volume_samples;
      }

      /** \brief Accessor.
       *
       * \return Volume level.
       */
      const settingf& getVolumeSamples() const
      {
        return m_volume_samples;
      }

      /** \brief Setter.
       *
       * \param op New detail level string.
       */
      void setDetail(const std::string &op)
      {
        m_detail = op;
      }

      /** \brief Set the current sensitivity.
       *
       * \param op New sensitivity.
       */
      void setSensitivity(float op)
      {
        m_camera_rot_speed_y.set(op);
        m_camera_rot_speed_x.set(
            (m_camera_rot_speed_x.get() > 0.0f) ? op : -op);
      }

      /** \brief Setter.
       *
       * \param op New resolution string.
       */
      void setResolution(const std::string &op)
      {
        m_resolution = op;
      }
  };

  /** \brief Init configuration.
   *
   * Will fail if configuration already init.
   */
  extern void conf_init();

  /** \brief Destroy and save configuration.
   *
   * Will fail if no configuration present.
   */
  extern void conf_quit();

  /** Main settings instance. */
  extern Settings *conf;
};

#endif
