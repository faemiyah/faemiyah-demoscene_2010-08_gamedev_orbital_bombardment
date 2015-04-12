#ifndef DATA_XML_FILE_HPP
#define DATA_XML_FILE_HPP

#include "data/generic.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

namespace data
{
  /** \brief XML file abstraction.
   *
   * Class used to simplify reading XML files.
   */
  class XmlFile
  {
    private:
      /** Tree object. */
      boost::property_tree::ptree m_xtree;

    public:
      /** \brief Constructor.
       *
       * \param pfname Filename to load.
       */
      XmlFile(const char *pfname);

      /** \brief Constructor.
       *
       * \param pfname Filename to load.
       */
      XmlFile(const std::string &pfname);

      /** \brief Constructor.
       *
       * \param pfname Filename to load.
       */
      XmlFile(const boost::filesystem::path &pfname);

    private:
      /** \brief Load the tree internally.
       */
      void load(const boost::filesystem::path &pfname);

    public:
      /** \brief Get the root node.
       *
       * \return Root node reference.
       */
      const std::string& rootType() const
      {
        return m_xtree.front().first;
      }

      /** \brief Get the tree under the root node.
       *
       * \return Tree reference.
       */
      const boost::property_tree::ptree& tree() const
      {
        return m_xtree.front().second;
      }
  };
}

#endif
