#include "snd/stream.hpp"

#include "data/generic.hpp"
#include "ui/generic.hpp"

#include <fstream>

using namespace snd;
namespace fs = boost::filesystem;

/** Size of one block read. */
static const unsigned BLOCK_READ = 8192; // two pages at least

/** Number of samples in a buffer to be fed. */
static const int BLOCK_SAMPLES = 8192; // ~5 samples per second

/** Number of queue block, should be about a second? */
static const unsigned QUEUE_BLOCKS = 4; // <1 second

/** \brief Convert a vorbis sample into 16-bit audio.
 *
 * \param sample Floating-point sample in.
 * \return 16-bit sample out.
 */
int16_t vorbis_sample16(float sample)
{
  int ret = math::lround(sample * 32767.0f);
  if(ret > 32767)
  {
    return static_cast<int16_t>(32767);
  }
  if(ret < -32768)
  {
    return static_cast<int16_t>(-32768);
  }
  return static_cast<int16_t>(ret);
}

Stream::Stream() :
  m_serial(-1),
  m_has_vorbis_header(false),
  m_has_vorbis_synthesis(false),
  m_stop(false)
{
  ogg_sync_init(&m_sync);
}

Stream::Stream(const fs::path &pfname) :
  m_serial(-1),
  m_has_vorbis_header(false),
  m_has_vorbis_synthesis(false),
  m_stop(false)
{
  ogg_sync_init(&m_sync);
  this->load(pfname);
}

Stream::~Stream()
{
  this->unreserve();
}

void Stream::appendPage(ogg_page *page)
{
  int err = ogg_stream_pagein(&m_stream, page);
  if(0 != err)
  {
    std::stringstream sstr;
    sstr << "ogg_stream_pagein: " << err;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
}

void Stream::eos()
{
  this->rewind();
}

void Stream::load(const fs::path &pfname, int pstream)
{
  m_filename = pfname;
  m_file = data::open_read(pfname);
  m_file->seekg(0);

  // read packets until a correct stream is found
  ogg_page page;
  for(;;)
  {
    if(!this->readPage(&page))
    {
      std::stringstream sstr;
      sstr << "desired stream " << pstream << " not found";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    int serial = ogg_page_serialno(&page);
    if((0 > pstream) || (pstream == serial))
    {
      if(ogg_page_bos(&page))
      {
        ogg_stream_init(&m_stream, serial);
        m_serial = serial;
      }
      break;
    }
  }
  this->appendPage(&page);
  
  // read header
  vorbis_comment_init(&m_comment);
  vorbis_info_init(&m_info);
  m_has_vorbis_header = true;
  ogg_packet packet;
  for(unsigned ii = 0; (ii < 3); ++ii)
  {
    if(!this->readPacket(&packet))
    {
      std::stringstream sstr;
      sstr << "error reading vorbis header packet " << ii;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
    int err = vorbis_synthesis_headerin(&m_info, &m_comment, &packet);
    if(0 != err)
    {
      std::stringstream sstr;
      sstr << "vorbis_synthesis_headerin: " << err;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }

  // sanity
  if(2 < m_info.channels)
  {
    std::stringstream sstr;
    sstr << "channel count: " << m_info.channels;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }

  // initialize blocks
  int err = vorbis_synthesis_init(&m_dsp, &m_info);
  if(0 != err)
  {
    std::stringstream sstr;
    sstr << "vorbis_synthesis_init: " << err;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  err = vorbis_block_init(&m_dsp, &m_block);
  if(0 != err)
  {
    std::stringstream sstr;
    sstr << "vorbis_block_init: " << err;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  m_has_vorbis_synthesis = true;
}

void Stream::pause()
{
  if(m_thread.get())
  {
    m_stop = true;
    m_thread->join();
    m_thread = thr::ThreadSptr(static_cast<boost::thread*>(NULL));
  }
}

void Stream::play()
{
  if(!m_thread.get())
  {
    m_thread = thr::ThreadSptr(new boost::thread(
          boost::bind(&Stream::run, this)));
  }
}

ogg_packet* Stream::readPacket(ogg_packet *dst)
{
  while(1 != ogg_stream_packetout(&m_stream, dst))
  {
    ogg_page page;
    for(;;)
    {
      if(!this->readPage(&page))
      {
        return NULL;
      }
      int serial = ogg_page_serialno(&page);
      if(serial == m_serial)
      {
        this->appendPage(&page);
        break;
      }
    }
  }
  return dst;
}

ogg_page* Stream::readPage(ogg_page *dst)
{
  while(1 != ogg_sync_pageout(&m_sync, dst))
  {
    if(m_file->eof())
    {
      return NULL;
    }

    char *buffer = ogg_sync_buffer(&m_sync, static_cast<long>(BLOCK_READ));
    m_file->read(buffer, BLOCK_READ);
    if(m_file->bad())
    {
      std::stringstream sstr;
      sstr << "read error in " << m_filename;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    if(0 != ogg_sync_wrote(&m_sync, static_cast<long>(m_file->gcount())))
    {
      std::stringstream sstr;
      sstr << "could not write to sync state in " << m_filename;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }
  return dst;
}

bool Stream::readWave(int16_t *dst, int num_samples)
{
  int16_t *buffer = dst;
  Sample *new_sample = NULL;
  bool ret = true;

  // stereo (or larger, which gets ignored)
  if(2 <= m_info.channels)
  {
    for(;;)
    {
      int amount = vorbis_synthesis_pcmout(&m_dsp, NULL);
      if(0 >= amount)
      {
        if(!this->synthesis())
        {
          ret = false;
          break;
        }
        continue;
      }

      float **samples;
      amount = std::min(num_samples, vorbis_synthesis_pcmout(&m_dsp, &samples));
      int left = amount;
      float *src0 = samples[0],
            *src1 = samples[1];
      do {
        dst[0] = vorbis_sample16(*src0);
        dst[1] = vorbis_sample16(*src1);
        dst += 2;
        ++src0;
        ++src1;
      } while(--left);
      vorbis_synthesis_read(&m_dsp, amount);
      if(!(num_samples -= amount))
      {
        break;
      }
    }
  }
  // mono
  else
  {
    for(;;)
    {
      int amount = vorbis_synthesis_pcmout(&m_dsp, NULL);
      if(0 >= amount)
      {
        if(!this->synthesis())
        {
          ret = false;
          break;
        }
        continue;
      }

      float **samples;
      amount = std::min(num_samples, vorbis_synthesis_pcmout(&m_dsp, &samples));
      int left = amount;
      float *src = samples[0];
      do {
        *dst = vorbis_sample16(*src);
        ++dst;
        ++src;
      } while(--left);
      vorbis_synthesis_read(&m_dsp, amount);
      if(!(num_samples -= amount))
      {
        break;
      }
    }
  }

  // done, create sample
  if(buffer < dst)
  {
    unsigned sample_size = static_cast<unsigned>(
        reinterpret_cast<uint8_t*>(dst) - reinterpret_cast<uint8_t*>(buffer));
    new_sample = new Sample(reinterpret_cast<uint8_t*>(buffer),
        sample_size,
        static_cast<unsigned>(m_info.channels),
        static_cast<unsigned>(m_info.rate));
    m_samples.push_front(SampleSptr(new_sample));
    m_source.queue(*(m_samples.front()));
    alSourcef(m_source.getId(), AL_GAIN, volume_music);
  }
  return ret;
}

void Stream::rewind()
{
  m_file->clear();
  m_file->seekg(0, std::ios_base::beg);
  ogg_sync_reset(&m_sync);
  vorbis_synthesis_restart(&m_dsp);
  vorbis_block_clear(&m_block);
  vorbis_block_init(&m_dsp, &m_block);

  // read first 3 packets away
  ogg_packet packet;
  for(unsigned ii = 0; (ii < 3); ++ii)
  {
    if(!this->readPacket(&packet))
    {
      std::stringstream sstr;
      sstr << "error rereading vorbis header packet " << ii << "in " << m_filename;
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }
}

void Stream::run()
{
  int16_t buffer[BLOCK_SAMPLES * 2];

  while(!m_stop)
  {
    if(m_source.numQueuedBuffers() < QUEUE_BLOCKS)
    {
      if(!this->readWave(buffer, BLOCK_SAMPLES))
      {
        this->eos();
      }
    }
    else
    {
      // sleep 10ms, it's pretty safe
      thr::usec_sleep(10000); 
    }

    // unqueue last buffer
    ALuint released = m_source.unqueueOne();
    if(released > 0)
    {
      if(m_samples.back()->getBuffer() != released)
      {
        BOOST_THROW_EXCEPTION(std::runtime_error(
              "released buffer is not last in line"));
      }
      m_samples.pop_back();
    }
  }
}

bool Stream::synthesis()
{
  ogg_packet packet;
  if(!this->readPacket(&packet))
  {
    return false;
  }

  int err = vorbis_synthesis(&m_block, &packet);
  if(0 != err)
  {
    std::stringstream sstr;
    sstr << "vorbis_synthesis: " << err;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  err = vorbis_synthesis_blockin(&m_dsp, &m_block);
  if(0 != err)
  {
    std::stringstream sstr;
    sstr << "vorbis_synthesis_blockin: " << err;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  return true;
}

void Stream::unreserve()
{
  this->pause();

  ogg_sync_clear(&m_sync);
  if(0 <= m_serial)
  {
    ogg_stream_clear(&m_stream);
  }
  if(m_has_vorbis_header)
  {
    vorbis_comment_clear(&m_comment);
    vorbis_info_clear(&m_info);
  }
  if(m_has_vorbis_synthesis)
  {
    vorbis_dsp_clear(&m_dsp);
  }
}

