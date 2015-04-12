#ifndef SND_STREAM_HPP
#define SND_STREAM_HPP

#include "snd/sample.hpp"
#include "snd/source.hpp"
#include "thr/generic.hpp"

#include "ogg/ogg.h"
#include "vorbis/codec.h"

namespace snd
{
  /** \brief One audio stream, loaded from a file.
   *
   * For simplicity, only Ogg Vorbis is supported.
   */
  class Stream
  {
    private:
      /** Thread. */
      thr::ThreadSptr m_thread;

      /** Private audio source. */
      Source m_source;

      /** Samples. */
      std::list<SampleSptr> m_samples;

      /** Stream state. */
      ogg_stream_state m_stream;

      /** Sync state. */
      ogg_sync_state m_sync;

      /** Vorbis block. */
      vorbis_block m_block;

      /** Vorbis comment. */
      vorbis_comment m_comment;

      /** Vorbis dsp state. */
      vorbis_dsp_state m_dsp;

      /** Vorbis information. */
      vorbis_info m_info;

      /** File pointer. */
      data::shristr m_file;

      /** Filename. */
      boost::filesystem::path m_filename;

      /** Stream being used (default: 0). */
      int m_serial;

      /** Vorbis header done? */
      bool m_has_vorbis_header;

      /** Vorbis synthesis done? */
      bool m_has_vorbis_synthesis;

      /** Quit flag. */
      bool m_stop;

    public:
       /** \brief Accessor.
       *
       * \return Get the filename.
       */
      const boost::filesystem::path& getFilename() const
      {
        return m_filename;
      }

    public:
      /** \brief Empty constructor. */
      Stream();

      /** \brief Constructor.
       *
       * Construction just loads the stream, but does not start it.
       *
       * \param pfname Filename to load from.
       */
      Stream(const boost::filesystem::path &pfname);

      /** \brief Destructor. */
      virtual ~Stream();

    public:
      /** \brief Load a file.
       *
       * If the stream already has content, unreserves the previous stream.
       *
       * \param pfname Filename to load from.
       * \param pstream Stream to use or -1 for first available.
       */
      void load(const boost::filesystem::path &pfname, int pstream = -1);

      /** \brief Pause playback.
       *
       * May only be called if stream has been correctly initialized.
       *
       * If playback not in progress, does nothing. Not thread-safe.
       *
       * OpenAL queued buffers will still be fed to the audio device regardless
       * of pausing.
       */
      void pause();

      /** \brief Start playback.
       *
       * May only be called if stream has been correctly initialized.
       *
       * If playback already in progress, does nothing. Not thread-safe.
       */
      void play();

      /** \brief Thread function.
       *
       * Only public for access from thread, not to be called manually.
       */
      void run();

    private:
      /** \brief Append a page into the current stream.
       *
       * Throws an exception on failure.
       *
       * \param page Page to append.
       */
      void appendPage(ogg_page *dst);

      /** \brief Read one packet.
       *
       * Stream must already be initialized.
       *
       * \param dst Destination.
       * \return Pointer to packet or NULL if at end of file.
       */
      ogg_packet* readPacket(ogg_packet *dst);

      /** \brief Read one page.
       *
       * Page read is stored into target pointer.
       *
       * \param dst Destination.
       * \return Pointer to page or NULL if at end of file.
       */
      ogg_page* readPage(ogg_page *dst);

      /** \brief Read until given amount of samples available.
       *
       * If given number can't be acquired, read as much as possible.
       *
       * The amount to acquire must be greater than zero.
       *
       * \param dst Destination to insert into.
       * \param num_samples Amount of samples to read.
       * \return True if still data left, false if EOF encountered.
       */
      bool readWave(int16_t *dst, int num_samples);

      /** \brief Rewind the stream to start.
       *
       * Unless called from the stream thread itself, may only be called either
       * if stream is not playing.
       */
      void rewind();

      /** \brief Synthesize one packet.
       *
       * \return True on success, false if nothing left.
       */
      bool synthesis();

      /** \brief Unreserve content.
       *
       * Returns the stream object to initial state.
       */
      void unreserve();

    public:
      /** \brief What to do at the end of stream?
       *
       * Default implementation seeks to the beginning of the stream.
       *
       * Alternative implementations could for example quickly load another
       * file.
       */
      virtual void eos();

    public:
      /** \brief Stop playback.
       *
       * Equal to calling pause() and rewind().
       */
      void stop()
      {
        this->pause();
        this->rewind();
      }
  };

  /** Convenience typedef. */
  typedef boost::shared_ptr<Stream> StreamSptr;
}

#endif
