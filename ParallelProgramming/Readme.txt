Projekt "C++, C++-17 Threads, Win32 API und DLLs"

// =====================================================================

/*
 * Erweiterungen
 */

// defines

// size of shared memory object
#define  SHAREDMEM_LENGTH          (4 * sizeof (long))

// names of kernel objects
#define  SHAREDMEM_NAME            "BANKHAUS"
#define  MUTEX_NAME                "BANKHAUS_MUTEX"

// command id's
#define  CMD_NO_REQUEST            0
#define  CMD_CREATE_ACCOUNT        1
#define  CMD_DEPOSIT               2
#define  CMD_WITHDRAW              3
#define  CMD_GETBALANCE            4
#define  CMD_LIST_ALL_ACCOUNTS     5

// function prototypes

extern "C" XXX_API  void initSharedMemory();
extern "C" XXX_API  void releaseSharedMemory();

extern "C" XXX_API  void attachSharedMemory();
extern "C" XXX_API  void detachSharedMemory();

// client side methods
extern "C" XXX_API  void createAccount(int accountNumber);

// server side requests
extern "C" XXX_API  bool checkClientRequest(long* command);
extern "C" XXX_API  void listAllAccounts();

// =====================================================================

/*
 * Erweiterungen
 */

#include <assert.h>
#include <iostream>
#include <map>

// global data - shared memory
static HANDLE g_hMutex;     // mutex to protect shared memory object
static HANDLE g_hMap;       // shared memory object
static char* g_pSharedMem; // pointer to shared memory object

// application data
std::map<long, double> g_accounts;


// functions - DLL level (( C based // Win32 API based ))

void initSharedMemory()
{
    // protect shared memory object against current access
    g_hMutex = ::CreateMutex(
        (SECURITY_ATTRIBUTES*)0,    // no security attributes
        TRUE,                       // calling thread wants to obtain
                                    // ownership of the mutex object
        MUTEX_NAME                  // name of mutex-object
    );
    assert(g_hMutex != (HANDLE)0);
    assert(GetLastError() != ERROR_ALREADY_EXISTS);

    // create a shared memory object
    g_hMap = ::CreateFileMapping(
        INVALID_HANDLE_VALUE,       // create a physical memory object
        (SECURITY_ATTRIBUTES*)0,    // ignored
        PAGE_READWRITE,             // need read / write access
        0,                          // high-order DWORD of size
        SHAREDMEM_LENGTH,           // low-order DWORD of size
        SHAREDMEM_NAME              // name of object
    );
    assert(g_hMap != (HANDLE)0);

    // gain access to shared memory object
    g_pSharedMem = (char*)::MapViewOfFile(
        g_hMap,                     // handle to file-mapping object
        FILE_MAP_ALL_ACCESS,        // access mode
        0,                          // high-order DWORD of offset
        0,                          // low-order DWORD of offset
        0                           // map entire file
    );
    assert(g_pSharedMem != (char*)0);

    // initialize shared memory object
    ((long*)g_pSharedMem)[0] = 0;
    ((long*)g_pSharedMem)[1] = 0;
    ((long*)g_pSharedMem)[2] = 0;

    // release ownership of mutex object so that
    // clients can start to place requests into the shared memory
    BOOL b;
    b = ::ReleaseMutex(g_hMutex);
    assert(b != 0);
}

void releaseSharedMemory()
{
    // unmap view from shared memory object
    ::UnmapViewOfFile(g_pSharedMem);

    // release all kernel objects
    if (g_hMap != (HANDLE)0)
        ::CloseHandle(g_hMap);

    if (g_hMutex != (HANDLE)0)
        ::CloseHandle(g_hMutex);
}

void attachSharedMemory() {
    // retrieve handle to an existing named mutex object
    g_hMutex = ::OpenMutex(
        MUTEX_ALL_ACCESS, // access flag
        TRUE,             // inherit flag
        MUTEX_NAME        // name of mutex-object
    );
    DWORD dwLastErr = GetLastError();
    assert(g_hMutex != (HANDLE)0);

    // retrieve handle to an existing shared memory
    g_hMap = ::OpenFileMapping(
        FILE_MAP_ALL_ACCESS, // access mode
        FALSE,               // inherit flag
        SHAREDMEM_NAME       // name of shared memory object
    );
    assert(g_hMap != (HANDLE)0);

    // map shared memory object into this address space
    g_pSharedMem = (char*)::MapViewOfFile(
        g_hMap,              // handle to file-mapping object
        FILE_MAP_ALL_ACCESS, // access mode
        0,                   // high-order DWORD of offset
        0,                   // low-order DWORD of offset
        0                    // map entire file
    );
    assert(g_pSharedMem != (char*)0);
}

void detachSharedMemory() {
    // release all kernel objects
    if (g_pSharedMem != (char*)0)
        ::UnmapViewOfFile(g_pSharedMem);

    if (g_hMap != (HANDLE)0)
        ::CloseHandle(g_hMap);

    if (g_hMutex != (HANDLE)0)
        ::CloseHandle(g_hMutex);
}

// called by client (!!!)
void createAccount(int accountNumber)
{
    // gain access to shared memory object
    ::WaitForSingleObject(g_hMutex, INFINITE);

    // create an account
    ((long*)g_pSharedMem)[0] = (long)CMD_CREATE_ACCOUNT; 
    ((long*)g_pSharedMem)[1] = (long)accountNumber;
    ((long*)g_pSharedMem)[2] = (long)0;

    // release access of shared memory object
    ::ReleaseMutex(g_hMutex);
}

// called by server (!!!)
extern "C" XXX_API  void listAllAccounts() {

    for (const auto& [key, value] : g_accounts) {
        std::cout << "Account No.: " << key << ", Balance =" << value << "\n";
    }
}

bool checkClientRequest(long* command) {

    // gain access to shared memory object
    ::WaitForSingleObject(g_hMutex, INFINITE);

    // read request from shared memory
    long cmd = ((long*)g_pSharedMem)[0];
    long par01 = ((long*)g_pSharedMem)[1];
    long par02 = ((long*)g_pSharedMem)[2];

    // clear shared memory !!!
    ((long*)g_pSharedMem)[0] = CMD_NO_REQUEST;

    // release access of shared memory object
    ReleaseMutex(g_hMutex);

    switch (cmd)
    {
    case CMD_NO_REQUEST:
        // printf ("No request available\n");
        return false;

    case CMD_CREATE_ACCOUNT:
        printf("Got CMD_CREATE_ACCOUNT request\n");

        // par1 contains requested account number
        g_accounts[par01] = 0;   // initial money

        *command = CMD_CREATE_ACCOUNT;
        return true;

    // case CMD_DEPOSIT:
    // TBD

    // case CMD_WITHDRAW:
    // TBD

    // case CMD_GETBALANCE:
    // TBD
    }

    return false;
}

// =====================================================================

