#include <iostream>
#include <string>

namespace Strategized_Locking_Code_Project {


    class ILockable
    {
    public:
        // destructor
        virtual ~ILockable() { };

        /**
        * This method will take the mutex.
        *
        * @since    <30 01 2009 >
        */
        virtual void Take() = 0;

        /**
        * This method will release the mutex.
        *
        * @since    <30 01 2009 >
        */
        virtual void Release() = 0;
    };

    /**
* Use this class to guard a complete method. This method expects a
* mutex. The mutex will be taken and released automatically.
*
* Hamed Ebrahimmalek
*/
    class Guard
    {
    public:
        /**
        * Constructor
        *
        * @param    [in]    lock    the lockable object
        */
        explicit Guard(ILockable& lock);

        /* destructor */
        virtual ~Guard();

    private:
        /* private constructor */
        Guard() { };
        /* private copy constructor */
        Guard(const Guard&) { };
        /* private operator */
        Guard& operator= (const Guard&) { };

        /* true if owning a lockable object */
        bool m_owner;
        /* the lockable object */
        ILockable* m_lock;
    };

    /**
    * Use this template class to guard an object or a method. The mutex will be taken
    * and released automatically.
    *
    * Hamed Ebrahimmalek
    */

    template< class LOCKABLE >
    class Guarder
    {
    public:
        /**
        * Constructor
        *
        * @param    [in]    lock    the lockable object
        */
        explicit Guarder(LOCKABLE& lock)
            : m_lock(&lock),
            m_owner(false)
        {
            m_lock->Take();
            m_owner = true;
        };

        /* destructor */
        virtual ~Guarder()
        {
            // should be locked first
            if (m_owner)
            {
                m_lock->Release();
            }
            // do not delete. 
            // Guard is not the owner
            m_lock = NULL;
            m_owner = false;
        };

    private:
        /* no default constructor allowed */
        Guarder() { };
        /* no copy constructor allowed */
        Guarder(const Guarder&) { };
        /* no assignment operator allowed */
        Guarder& operator= (const Guarder&) { };

        /* the object to lock */
        LOCKABLE* m_lock;
        /* whether its owned */
        bool m_owner;
    };



    /**
    * This class can be used to protect data access from multiple
    * threads. Use this mutex class correctly. You must use this
    * class carefully. When taking a mutex, you must release it
    * when finished protecting data access.
    */
    class Lock : public ILockable
    {
    public:
        // constructor
        Lock();
        // overloaded constructor
        Lock(const std::string& mutexName);
        // destructor
        ~Lock();


        void Take();
        void Release();
        void Open();
        void Close();

    private:
        // private copy constructor
        Lock(const Lock&) { };
        // private operator
        Lock& operator= (const Lock&) { };

        /* the handle of the mutex */
    //    HANDLE m_handle;

        /*
        * The name of the mutex, this is only created via the
        * overloaded constructor
        */
        std::string m_mutexName;
    };



}