#ifndef DATA_REGISTRY_HPP
#define DATA_REGISTRY_HPP

#include "data/generic.hpp"

#include <map>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

namespace data
{
  /** \brief Registry base class.
   */
  template <typename T, typename L> class Registry
  {
    protected:
      /** Convenience typedef. */
      typedef T* (*CreateFunction)(const boost::filesystem::path&, const boost::property_tree::ptree&,
          const L&);

      /** Convenience typedef */
      typedef std::map<std::string, CreateFunction> registry_type;

    protected:
      /** Mapping of creation functions. */
      static registry_type *g_registry;

    protected:
      /** \brief Create a instance of registered class.
       *
       * \param pfname Filename to load from.
       * \param root Tree root from which to create.
       * \param loader Loader to be passed into construction.
       */
      static T* registryCreate(const boost::filesystem::path &pfname, const boost::property_tree::ptree &root,
          const L &loader);
  };

  /** \brief Registration template class to be instantiated.
   *
   * Each class wanting to register itself into the registry must instantiate a Registration template. This
   * should be done in their own source file.
   */
  template <class R, class L, class T> class Registration :
    public Registry<R, L>
  {
    private:
      /** \brief Private creator function.
       *
       * \param pfname Filename to load from.
       * \param root Tree root from which to create.
       * \param loader Loader to be passed into construction.
       */
      static R* privateCreate(const boost::filesystem::path &pfname, const boost::property_tree::ptree &root,
          const L &loader)
      {
        return new T(pfname, root, loader);
      }

    private:
      /** ID of this registration. */
      std::string m_id;

    public:
      /** \brief Constructor.
       *
       * \param pid ID to which register.
       */
      Registration(const char *pid) :
        m_id(pid)
      {
        if(NULL == Registry<R, L>::g_registry)
        {
          Registry<R, L>::g_registry = new typename Registry<R, L>::registry_type();
        }

        (*Registry<R, L>::g_registry)[m_id] = Registration<R, L, T>::privateCreate;
      }

      /** \brief Destructor. */
      ~Registration()
      {
        BOOST_ASSERT(NULL != (Registry<R, L>::g_registry));

        typename Registry<R, L>::registry_type::iterator iter = Registry<R, L>::g_registry->find(m_id);
        if(Registry<R, L>::g_registry->end() == iter)
        {
          std::ostringstream sstr;
          sstr << "trying to remove id '" << m_id << "' that is not registered";
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }

        if(0 >= Registry<R, L>::g_registry->size())
        {
          delete Registry<R, L>::g_registry;
          Registry<R, L>::g_registry = NULL;
        }
      }
  };
}

#endif
