#ifndef DATA_GENERIC_HPP
#define DATA_GENERIC_HPP

#include "defaults.hpp"

#include <vector>

#include <boost/filesystem.hpp>

namespace data
{
  /** Convenience typedef. */
  typedef boost::shared_ptr<std::istream> shristr;

  /** Convenience typedef. */
  typedef boost::shared_ptr<std::ostream> shrostr;

  /** \brief Tell if file exists.
   *
   * Technically, this will not tell if the exact file exists, but rather whether or not data::open_read() will
   * find the file and be able to at least try to open it.
   *
   * \return True if found from a valid search location, false if not.
   */
  extern bool file_exists(const boost::filesystem::path &pfname);

  /** \brief Open an input stream.
   *
   * The user must delete the returned ifstream object.
   *
   * \param pfname Filename to open.
   * \return Newly reserved ifstream.
   */
  extern shristr open_read(const boost::filesystem::path &pfname);

  /** \brief Wrapper for open_write.
   *
   * \param pfname Filename to open.
   * \return Newly reserved ifstream.
   */
  inline shristr open_read(const char* pfname)
  {
    return open_read(boost::filesystem::path(pfname));
  }

  /** \brief Open an output stream.
   *
   * The user must delete the returned ofstream object.
   *
   * \param pfname Filename to open.
   * \return Newly reserved ifstream.
   */
  extern shrostr open_write(const boost::filesystem::path &pfname);

  /** \brief Wrapper for open_read.
   *
   * \param pfname Filename to open.
   * \return Newly reserved ifstream.
   */
  inline shrostr open_write(const char* pfname)
  {
    return open_write(boost::filesystem::path(pfname));
  }

  /** \brief Find a filename for input file.
   *
   * Throws an error if the file is not found.
   *
   * \param pfname Filename to open.
   * \return Full path filename.
   */
  extern boost::filesystem::path open_search(const boost::filesystem::path &pfname);

  /** Wrapper for find a filename.
   *
   * \param pfname Filename to open.
   * \return Full path filename.
   */
  inline boost::filesystem::path open_search(const std::string &pfname)
  {
    return open_search(boost::filesystem::path(pfname));
  }

  /** \brief Tell if a filename is for an armature file.
   * 
   * Note that not accepting filenames in caps is intentional.
   *
   * \param pfname Filename.
   * \return True if yes, false if no.
   */
  inline bool filename_is_armature(const boost::filesystem::path &pfname)
  {
    boost::filesystem::path ext = pfname.extension();

    return (ext == boost::filesystem::path(".armature"));
  }

  /** \brief Tell if a filename is for a JPEG file.
   *
   * Note that not accepting filenames in caps is intentional.
   *
   * \param pfname Filename.
   * \return True if yes, false if no.
   */
  inline bool filename_is_jpeg(const boost::filesystem::path &pfname)
  {
    boost::filesystem::path ext = pfname.extension();

    return ((ext == boost::filesystem::path(".jpeg")) || (ext == boost::filesystem::path(".jpg")));
  }

  /** \brief Tell if a filename is for a mesh file.
   * 
   * Note that not accepting filenames in caps is intentional.
   *
   * \param pfname Filename.
   * \return True if yes, false if no.
   */
  inline bool filename_is_mesh(const boost::filesystem::path &pfname)
  {
    boost::filesystem::path ext = pfname.extension();

    return (ext == boost::filesystem::path(".mesh"));
  }

  /** \brief Tell if a filename is for a mesh collection.
   * 
   * Note that not accepting filenames in caps is intentional.
   *
   * \param pfname Filename.
   * \return True if yes, false if no.
   */
  inline bool filename_is_mesh_collection(const boost::filesystem::path &pfname)
  {
    boost::filesystem::path ext = pfname.extension();

    return ((ext == boost::filesystem::path(".mesh_collection")) || (ext == boost::filesystem::path(".mmesh")));
  }

  /** \brief Tell if a filename is for a PNG file.
   *
   * Note that not accepting filenames in caps is intentional.
   *
   * \param pfname Filename.
   * \return True if yes, false if no.
   */
  inline bool filename_is_png(const boost::filesystem::path &pfname)
  {
    boost::filesystem::path ext = pfname.extension();

    return (ext == boost::filesystem::path(".png"));
  }

  /** \brief Tell if a filename is for a sample.
   *
   * Note that not accepting filenames in caps is intentional.
   *
   * \param pfname Filename.
   * \return True if yes, false if no.
   */
  inline bool filename_is_sample(const boost::filesystem::path &pfname)
  {
    boost::filesystem::path ext = pfname.extension();

    return (ext == boost::filesystem::path(".wav"));
  }

  /** \brief Tell if a filename is for a meta sample.
   *
   * Note that not accepting filenames in caps is intentional.
   *
   * \param pfname Filename.
   * \return True if yes, false if no.
   */
  inline bool filename_is_sample_meta(const boost::filesystem::path &pfname)
  {
    boost::filesystem::path ext = pfname.extension();

    return (ext == boost::filesystem::path(".sample"));
  }

  /** \brief Trim excess memory usage from an STL vector.
   *
   * Also resizes the vector to eliminate the issue.
   */
  template<typename T> void stl_trim(std::vector<T> &stlvec)
  {
    if(stlvec.capacity() != stlvec.size())
    {
      //std::cout << "Capacity change: " << stlvec.capacity() << " -> " << stlvec.size() << std::endl;
      stlvec.resize(stlvec.size());
    }
  }
}

#endif
