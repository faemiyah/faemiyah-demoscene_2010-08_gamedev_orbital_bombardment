#include "snd/sample.hpp"

#include "data/log.hpp"
#include "thr/generic.hpp"
#include "ui/generic.hpp"

#include <boost/property_tree/xml_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace snd;

namespace data
{
  template<> Sample::store_type data::Storable<Sample, SampleLoader>::g_store(0);
}

/** Time delay in microseconds after which to allow samples again. */
static const uint64_t SAMPLE_BLOCK_TIME = 200000;

/** \brief Convert an SDL audiospec into an OpenAl format.
 *
 * \param spec Input.
 * \return OpenAL format.
 */
ALenum sdl_spec_to_openal_format(const SDL_AudioSpec &spec)
{
  switch(spec.format)
  {
    case AUDIO_U8:
      return (spec.channels > 1) ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;

    case AUDIO_S8:
      return (spec.channels > 1) ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;

    case AUDIO_U16SYS:
      return (spec.channels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
  
    case AUDIO_S16SYS:
      return (spec.channels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

    default:
      break;
  }

  {
    std::ostringstream sstr;
    sstr << "unknown audio format: " << spec.format;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  return 0; // never reached
}

Sample::Sample() :
  m_buffer(0),
  m_last_play_time(0),
  m_freq(0),
  m_size(0),
  m_gain(1.0f) { }

Sample::Sample(const fs::path &pfname, const SampleLoader &loader) :
  m_buffer(0),
  m_last_play_time(0),
  m_freq(0),
  m_size(0),
  m_gain(loader.getGain())
{
  this->load(pfname, loader);
}

Sample::Sample(const uint8_t *data, unsigned size, unsigned channels, unsigned freq, float gain) :
  m_buffer(0),
  m_last_play_time(0),
  m_freq(freq),
  m_size(size),
  m_gain(gain)
{
  this->generateBuffer();
  if(2 <= channels)
  {
    alBufferData(m_buffer, AL_FORMAT_STEREO16, data, static_cast<ALsizei>(m_size),
        static_cast<ALsizei>(m_freq));
  }
  else
  {
    alBufferData(m_buffer, AL_FORMAT_MONO16, data, static_cast<ALsizei>(m_size),
        static_cast<ALsizei>(m_freq));
  }
}

Sample::~Sample()
{
  this->unreserve();
}

bool Sample::canPlay()
{
  uint64_t ts = thr::usec_get_timestamp();
  if(ts - m_last_play_time < SAMPLE_BLOCK_TIME)
  {
    return false;
  }
  m_last_play_time = ts;
  return true;
}

void Sample::generateBuffer()
{
  alGenBuffers(1, &m_buffer);
  if(0 == m_buffer)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error(std::string("could not create audio buffer")));
  }
}

void Sample::load(const fs::path &pfname, const SampleLoader &loader)
{
  boost::ignore_unused_variable_warning(loader);

  this->unreserve();

  fs::path location = data::open_search(pfname);
  {
    std::ostringstream sstr;
    sstr << "loading sample " << pfname;
    data::log(sstr.str());
  }

  this->generateBuffer();

  SDL_AudioSpec spec;
  uint8_t *buf;

  if(&spec != SDL_LoadWAV(location.generic_string().c_str(), &spec, &buf, &m_size))
  {
    std::ostringstream sstr;
    sstr << "could not load wav file: " << pfname << ": " << SDL_GetError();
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  m_freq = static_cast<unsigned>(spec.freq);
  m_format = sdl_spec_to_openal_format(spec);

  alBufferData(m_buffer, m_format, buf, static_cast<ALsizei>(m_size), static_cast<ALsizei>(m_freq));
  SDL_FreeWAV(buf);

#if defined(DEBUG)
  std::cout << *this << std::endl;
#endif
}

void Sample::unreserve()
{
  if(0 < m_buffer)
  {
    alDeleteBuffers(1, &m_buffer);
    m_buffer = 0;
  }
}

Sample::container_type Sample::createImplementation(const fs::path &pfname, const SampleLoader &loader)
{
  if(data::filename_is_sample(pfname))
  {
    return container_type(new Sample(pfname, loader));
  }

  // fall back to loading sample collection
  if(data::filename_is_sample_meta(pfname))
  {
    fs::path sample_filename;
    SampleLoader sample_loader(loader);

    {
      std::ostringstream sstr;
      sstr << "loading meta sample '" << pfname << '\'';
      data::log(sstr.str());
    }

    pt::ptree xtree;
    data::shristr istr = data::open_read(pfname);
    pt::read_xml(*istr, xtree);
    pt::ptree::value_type root = xtree.front();
    if(root.first.compare("sample"))
    {
      std::ostringstream err;
      err << "unknown root type: " << root.first;
      BOOST_THROW_EXCEPTION(std::invalid_argument(err.str()));
    }

    BOOST_FOREACH(boost::property_tree::ptree::value_type &vv, root.second)
    {
      const std::string &type = vv.first;
      const pt::ptree &subtree = vv.second;

      if(!type.compare("gain"))
      {
        sample_loader = sample_loader.gain(subtree.get<float>(""));
      }
      else if(!type.compare("file"))
      {
        std::string single_fname(subtree.get<std::string>(""));
        sample_filename = fs::path(pfname).parent_path() / fs::path(single_fname);
      }
    }

    return container_type(new Sample(sample_filename, sample_loader));
  }

  {
    std::ostringstream sstr;
    sstr << "unkown mesh file '" << pfname << '\'';
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
}

std::ostream& snd::operator<<(std::ostream &lhs, const Sample &rhs)
{
  lhs << "Sample: " << rhs.m_size << " bytes, " << rhs.m_freq << " Hz, ";
  switch(rhs.m_format)
  {
    case AL_FORMAT_MONO8:
      std::cout << "8-bit mono";
      break;

    case AL_FORMAT_STEREO8:
      std::cout << "8-bit stereo";
      break;

    case AL_FORMAT_MONO16:
      std::cout << "16-bit mono";
      break;

    case AL_FORMAT_STEREO16:
      std::cout << "16-bit stereo";
      break;

    default:
      std::cout << "unknown format";
  }
  return lhs;
}
