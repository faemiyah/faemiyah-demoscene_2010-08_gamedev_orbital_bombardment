#ifndef DATA_STORE_HPP
#define DATA_STORE_HPP

#include "data/generic.hpp"
#include "math/generic.hpp"
#include "thr/dispatch.hpp"

#include <boost/filesystem.hpp>

#include <map>
#include <sstream>
#include <vector>

namespace data
{
  /** \brief Store container type.
   *
   * Can store one or many objects of given type.
   */
  template <class T> class StoreContainer
  {
    public:
      /** Convenience typedef. */
      typedef boost::shared_ptr<T> value_type;

      /** Convenience typedef. */
      typedef std::vector<value_type> vector_type;

      /** Convenience typedef. */
      typedef typename vector_type::iterator iterator;

      /** Convenience typedef. */
      typedef typename vector_type::const_iterator const_iterator;

    private:
      /** Contents of this. */
      std::vector<value_type> m_contents;

      /** Cache hit value. */
      unsigned m_cache_value;

      /** Is this value persistent (should it be ignored for clear and purge purposes)? */
      bool m_persistent;

    public:
      /** \brief Accessor to actual value.
       *
       * \param op Index to get.
       * \return Pointer to value.
       */
      T* at(unsigned op = 0)
      {
        BOOST_ASSERT(m_contents.size() > op);

        return m_contents[op].get();
      }

      /** \brief Const accessor to actual value.
       *
       * \param op Index to get.
       * \return Pointer to value.
       */
      const T* at(unsigned op = 0) const
      {
        BOOST_ASSERT(m_contents.size() > op);

        return m_contents[op].get();
      }

      /** \brief Iterator to begin.
       */
      iterator begin()
      {
        return m_contents.begin();
      }

      /** \brief Iterator to begin.
       */
      const_iterator begin() const
      {
        return m_contents.begin();
      }

      /** \brief Const iterator to begin.
       */
      const_iterator cbegin() const
      {
        return m_contents.cbegin();
      }

      /** \brief Decrement cache value.
       *
       * Used to normalize cache values.
       *
       * \param op New cache zero point.
       */
      void decrementCacheValue(unsigned op)
      {
        BOOST_ASSERT(op <= m_cache_value);

        m_cache_value -= op;
      }

      /** \brief Is the container empty?
       *
       * \return True if yes, false if no.
       */
      bool empty() const
      {
        return m_contents.empty();
      }

      /** \brief Iteration.
       *
       * \return Iterator to end.
       */
      iterator end()
      {
        return m_contents.end();
      }

      /** \brief Const iterator to end.
       *
       * \return Iterator to end.
       */
      const_iterator end() const
      {
        return m_contents.end();
      }

      /** \brief Const iterator to end.
       *
       * \return Iterator to end.
       */
      const_iterator cend() const
      {
        return m_contents.cend();
      }

      /** \brief Accessor.
       *
       * \param op Element to get (default: first).
       * \return Shared pointer containing the value.
       */
      boost::shared_ptr<T>& get(unsigned op = 0)
      {
        BOOST_ASSERT(m_contents.size() > op);

        return m_contents[op];
      }

      /** \brief Const accessor.
       *
       * \param op Element to get (default: first).
       * \return Shared pointer containing the value.
       */
      const boost::shared_ptr<T>& get(unsigned op = 0) const
      {
        BOOST_ASSERT(m_contents.size() > op);

        return m_contents[op];
      }

      /** \brief Accessor.
       *
       * \return Cache value.
       */
      unsigned getCacheValue() const
      {
        return m_cache_value;
      }

      /** \brief Should this container be treated as persistent?
       *
       * \return True if yes, false if no.
       */
      bool isPersistent() const
      {
        return m_persistent;
      }

      /** \brief Setter
       *
       * \param op New cache value.
       */
      void setCacheValue(unsigned op)
      {
        m_cache_value = op;
      }

      /** \brief Turn persistence on or off.
       *
       * \param op New persistence flat (default: true).
       */
      void setPersistent(bool op = true)
      {
        m_persistent = op;
      }

      /** \brief Size of the container.
       *
       * \return Number of elements.
       */
      unsigned size() const
      {
        return static_cast<unsigned>(m_contents.size());
      }

    public:
      /** \brief Empty constructor. */
      StoreContainer() :
        m_cache_value(0) { }

      /** \brief Constructor.
       *
       * \param op Element to start with.
       */
      StoreContainer(T* op) :
        m_cache_value(0)
      {
        this->add(op);
        this->trim();
      }

      /** \brief Destructor. */
      ~StoreContainer() { }

    public:
      /** \brief Add a new entry.
       *
       * \param op Entry to add.
       */
      void add(T* op)
      {
        m_contents.push_back(value_type(op));
      }

      /** \brief Trim used space.
       */
      void trim()
      {
        stl_trim(m_contents);
      }
  };

  /** \brief Store union class. */
  template <class T> class Store
  {
    public:
      /** Convenience typedef. */
      typedef StoreContainer<T> container_type;

      /** Convenience typedef. */
      typedef std::map<boost::filesystem::path, container_type> map_type;

      /** Convenience typedef. */
      typedef typename map_type::iterator iterator;

      /** Convenience typedef. */
      typedef typename map_type::const_iterator const_iterator;

    private:
      /** Storage for all objects. */
      map_type m_data;

      /** Safety for operations within the storage. */
      boost::mutex m_mutex;

      /** Current cache value. */
      unsigned m_cache_value;

    public:
      /** \brief Iterator begin.
       *
       * \return Iterator to beginning.
       */
      iterator begin()
      {
        return m_data.begin();
      }

      /** \brief Const iterator begin.
       *
       * \return Const iterator to beginning.
       */
      const_iterator cbegin()
      {
        return m_data.cbegin();
      }

      /** \brief Iterator end.
       *
       * \return Iterator to end.
       */
      iterator end()
      {
        return m_data.end();
      }

      /** \brief Const iterator end.
       *
       * \return Const iterator to end.
       */
      const_iterator cend()
      {
        return m_data.cend();
      }

    public:
      /** \brief Default constructor.
       *
       * \param pcv Cache value to start with.
       */
      Store(unsigned pcv = 0) :
        m_cache_value(pcv) { }

      /** \brief Destructor. */
      ~Store() { }

    public:
      /** \brief Clear everything. */
      void clear()
      {
        boost::mutex::scoped_lock scope(m_mutex);

        for(iterator ii = this->begin(), ee = this->end(); (ii != ee);)
        {
          if(!ii->second.isPersistent())
          {
            ii = this->erase(ii, scope);
          }
          else
          {
            ++ii;
          }
        }

        m_cache_value = 0;
      }

      /** \brief Erase from iterator.
       *
       * \param ii Iterator to erase from.
       * \param scope Already acquired lock.
       * \return Iterator to next position.
       */
      iterator erase(iterator &ii, const boost::mutex::scoped_lock &scope)
      {
        boost::ignore_unused_variable_warning(scope);

        iterator jj = ii;
        ++jj;

        m_data.erase(ii);

        return jj;
      }

      /** \brief Wrapper to erase.
       *
       * \param ii Iterator to erase from.
       * \return Iterator to next position.
       */
      iterator erase(iterator &ii)
      {
        boost::mutex::scoped_lock scope(m_mutex);

        return erase(ii, scope);
      }

      /** \brief Test if an entry already exists.
       *
       * \param pfname Name to test.
       * \param scope Already acquired lock.
       * \return True if exists, false if no.
       */
      bool exists(const boost::filesystem::path &pfname, const boost::mutex::scoped_lock &scope)
      {
        boost::ignore_unused_variable_warning(scope);

        return (m_data.find(pfname) != m_data.end());
      }

      /** \brief Wrapper to exists.
       *
       * \param op Filename to test.
       * \return True if exists, false if no.
       */
      bool exists(const boost::filesystem::path &pfname)
      {
        boost::mutex::scoped_lock scope(m_mutex);

        return this->exists(pfname, scope);
      }

      /** Increment cache value. */
      void incrementCacheValue()
      {
        ++m_cache_value;
      }

      /** \brief Accessor.
       *
       * \param pfname Object to access.
       */
      container_type& locate(const boost::filesystem::path &pfname)
      {
        boost::mutex::scoped_lock scope(m_mutex);
        iterator ii = m_data.find(pfname);

        if(m_data.end() == ii)
        {
          std::stringstream sstr;
          sstr << "no " << pfname << " available in the store";
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
        ii->second.setCacheValue(m_cache_value);
        return ii->second;
      }

      /** \brief Purge everything with cache value smaller than given.
       *
       * Will move all cache values downward thus that elements that had the exact cache value will now have it
       * set to 0.
       *
       * \param op Minimum cache value to keep.
       */
      void purge(unsigned op, const boost::mutex::scoped_lock &scope)
      {
        boost::ignore_unused_variable_warning(scope);

        BOOST_ASSERT(op <= m_cache_value);

        for(iterator ii = this->begin(), ee = this->end(); (ii != ee);)
        {
          container_type &vv = ii->second;

          if(!vv.isPersistent() && (vv.getCacheValue() < op))
          {
            ii = this->erase(ii, scope);
          }
          else
          {
            ii.second.decrementCacheValue(op);
            ++ii;
          }
        }
      }

      /** \brief Wrapper to purging.
       *
       * \param op Minimum cache value to keep.
       */
      void purge(unsigned op)
      {
        boost::mutex::scoped_lock scope;

        this->purge(op, scope);
      }

      /** \brief Purge the oldest entries.
       *
       * Will decrement cache values thus that entries with cache value one greater than the oldest one will
       * have their values set to 0.
       */
      void purge()
      {
        boost::mutex::scoped_lock scope(m_mutex);
        unsigned minimum_cache_value = UINT_MAX;

        for(iterator ii = this->begin(), ee = this->end(); (ii != ee); ++ii)
        {
          minimum_cache_value = math::min(minimum_cache_value, ii.second.getCacheValue());
        }

        if(minimum_cache_value < UINT_MAX)
        {
          this->purge(minimum_cache_value + 1, scope);
        }
      }

      /** \brief Remove based on name.
       *
       * \param pfname Name to erase.
       */
      void remove(const boost::filesystem::path &pfname)
      {
        boost::mutex::scoped_lock scope;
        iterator ii = m_data.find(pfname);

        BOOST_ASSERT(m_data.end() != ii);

        m_data.erase(ii);
      }

      /** \brief Store an object.
       *
       * \param pfname Name to store under.
       * \param op An object to store.
       */
      container_type& store(const boost::filesystem::path &pfname, T *op)
      {
        boost::mutex::scoped_lock scope(m_mutex);

        BOOST_ASSERT(!this->exists(pfname, scope));

        return (m_data[pfname] = container_type(op));
      }

      /** \brief Store an object collection.
       *
       * \param pfname Name to store under.
       * \param op An object to store.
       */
      container_type& store(const boost::filesystem::path &pfname, const container_type &op)
      {
        boost::mutex::scoped_lock scope(m_mutex);

        BOOST_ASSERT(!this->exists(pfname, scope));

        return (m_data[pfname] = op);
      }
  };

  /** \brief Generic data storage.
   *
   * Saves loaded or just stored objects into singular entities or chunks of entities.
   *
   * Should be inherited by objects desiring to be stored. The store provides generic utilities, but specific
   * load code should be implemented by the inheriting class. The name of this function should be
   * createImplementation() and it should return a newly reserved object of the base type.
   *
   * This class only has static functionality, and as such, no constructors or destructors.
   */
  template <class T, class S> class Storable
  {
    public:
      /** Convenience typedef. */
      typedef T value_type;

      /** Type for store data union. */
      typedef Store<T> store_type;

      /** Convenience typedef. */
      typedef typename store_type::container_type container_type;

    protected:
      /** All storage information. */
      static store_type g_store;

    protected:
      /** \brief Create storage name from load name.
       *
       * Quite equal to basename of a file.
       *
       * \param pfname Filename used for loading before search or append.
       * \return Canonical name.
       */
      static boost::filesystem::path canonize(const boost::filesystem::path &op)
      {
        return op.stem();
      }

      /** \brief Create a 'load name'.
       *
       * i.e. remove everything after the last dot. Makes it easier to check for alternatives.
       *
       * \param pfname Filename passed to loader function.
       * \return Search name.
       */
      static boost::filesystem::path loaderize(const boost::filesystem::path &op)
      {
        return op.parent_path() / op.stem();
      }

    public:
      /** \brief Access an object in storage.
       *
       * The returned object may contain any amount of grouped elements. It is the responsibility of the user
       * to examine the returned container.
       *
       * Throws an error if not found.
       *
       * \return pfname Filename originally loaded as.
       */
      static container_type& locate(const boost::filesystem::path &pfname)
      {
        return g_store.locate(canonize(pfname));
      }

      /** \brief Store an object.
       *
       * \param pfname Path to store on.
       * \param op Object to store.
       */
      static container_type& store(const boost::filesystem::path &pfname, T* op)
      {
        return g_store.store(canonize(pfname), op);
      }

      /** \brief Store an object container.
       *
       * \param pfname Path to store on.
       * \param op Object to store.
       */
      static container_type& store(const boost::filesystem::path &pfname, const container_type& op)
      {
        return g_store.store(canonize(pfname), op);
      }

      /** \brief Clear storage.
       */
      static void storageClear()
      {
        g_store.clear();
      }

      /** \brief Tell if store contains given object.
       *
       * \param pfname Filename to check.
       */
      static bool storageContains(const boost::filesystem::path &pfname)
      {
        return g_store.exists(canonize(pfname));
      }

      /** \brief Purge oldest elements from storage.
       */
      static void storagePurge()
      {
        g_store.purge();
      }

      /** \brief Purge elements from storage.
       *
       * \param op Oldest elements to purge.
       */
      static void storagePurge(unsigned op)
      {
        g_store.purge(op);
      }

    public:
      /** \brief Remove an object in storage.
       *
       * \param pfname Object to erase.
       */
      static void storageRemove(const boost::filesystem::path &pfname)
      {
        if(!storageContains(pfname))
        {
          std::ostringstream sstr;
          sstr << "no '" << canonize(pfname) << "' in storage";
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }

        g_store.remove(canonize(pfname));
      }

    public:
      /** Function to create a new object.
       *
       * Assumes the existence of a function named createImplementation on the inheriting class.
       *
       * \param pfname File path passed to createImplementation.
       * \param op Operand passed to createImplementation.
       * \return Newly created container.
       */
      static container_type& create(const boost::filesystem::path &pfname, const S& op = S())
      {
        if(storageContains(pfname))
        {
          std::ostringstream sstr;
          sstr << "storage already contains '" << canonize(pfname) << '\'';
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }

        container_type newObject = T::createImplementation(pfname, op);

        return store(pfname, newObject);
      }

      /** \brief Access an instance.
       *
       * Create the instance if it's not available in storage.
       *
       * \param pfname File path passed to createImplementation.
       * \param op Operand passed to createImplementation.
       * \return Existing or newly created container.
       */
      static container_type& instanciate(const boost::filesystem::path &pfname, const S& op = S())
      {
        if(storageContains(pfname))
        {
          return locate(pfname);
        }
        return create(pfname, op);
      }

    private:
      /** \brief Function for parallel creation.
       *
       * \param pfname Filename passed to createImplementation.
       * \param op Operand passed to createImplementation.
       */
      static void createParallerWorker(const boost::filesystem::path &pfname, const S& op)
      {
        create(pfname, op);
      }

    public:
      /** \brief Create a new object in a parallel fashion.
       *
       * \param pfname File path passed to createImplementation.
       * \param op Operand passed to createImplementation.
       */
      static void createParaller(const boost::filesystem::path &pfname, const S& op = S())
      {
        thr::dispatch(createParallerWorker, pfname, op);
      }

      /** \brief Instanciate a new object in a parallel fashion.
       *
       * \param pfname File path passed to createImplementation.
       * \param op Operand passed to createImplementation.
       */
      static void instanciateParaller(const boost::filesystem::path &pfname, const S& op = S())
      {
        if(!storageContains(pfname))
        {
          thr::dispatch(createParallerWorker, pfname, op);
        }
      }
  };
}

#endif
