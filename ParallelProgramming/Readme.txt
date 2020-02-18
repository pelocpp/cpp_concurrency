Projekt "Mandelbrot" in C++ mit C++-17 Threads und Win32 API

// =====================================================================

// Mini-Windows Anwendung:

// Mandelbrot.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Mandelbrot.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MANDELBROT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MANDELBROT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// =====================================================================
// =====================================================================

Mandelbrot Variante 1:
    
    = (+) Unterschiedliche Bilder werden gezeichnet
    = (-) Anwendung NICHT reaktionsfähig

// original limits
double XMIN = -2.0;   // minimum x-value (real part)
double XMAX = +0.75;  // maximum x-value (real part)
double YMIN = -1.25;  // minimum y-value (imaginary part)
double YMAX = +1.25;  // maximum y-value (imaginary part)

constexpr int NumColors = 256;
constexpr int AbsMax = 5;

constexpr int WindowHeight = 500;
constexpr int WindowWidth = 600;

// -----------------------------------------------------------------

unsigned int iterate(
    std::complex<float> point,
    unsigned int iterMax, 
    unsigned int absMax)
{
    std::complex<float> z(0.0);

    unsigned int iter = 0;
    while (iter < iterMax && std::abs(z) < absMax) {
        z = z * z + point;
        ++iter;
    }

    return iter;
}

// -----------------------------------------------------------------

std::vector<COLORREF> palette;

void fill(std::vector<COLORREF>& palette) {

    palette.reserve(NumColors);

    for (unsigned int x = 0; x < NumColors / 32; ++x) {
        palette.push_back(RGB(x * 32 * 255 / NumColors, 0, 0));
    }

    for (unsigned int x = 0; x < NumColors / 32; ++x) {
        palette.push_back(RGB(255, x * 32 * 255 / NumColors, 0));
    }

    for (unsigned int x = 0; x < NumColors / 16; ++x) {
        palette.push_back(RGB(((NumColors / 16 - x) * 16) * 255 / NumColors, 255, 0));
    }

    for (unsigned int x = 0; x < NumColors / 16; ++x) {
        palette.push_back(RGB(0, ((NumColors / 16 - x) * 16) * 255 / NumColors, x * 16 * 255 / NumColors));
    }

    for (unsigned int x = 0; x < NumColors / 16; ++x) {
        palette.push_back(RGB(x * 16 * 255 / NumColors, 0, 255));
    }

    for (unsigned int x = 0; x < NumColors / 4; ++x) {
        palette.push_back(RGB(((NumColors / 4 - x) * 4) * 255 / NumColors, x * 4 * 255 / NumColors, 255));
    }

    for (unsigned int x = 0; x < NumColors / 2; ++x) {
        palette.push_back(RGB(x * 2 * 255 / NumColors, 255, 255));
    }

    palette[NumColors - 1] = RGB(0, 0, 0);
}

// -----------------------------------------------------------------

std::complex<float> getComplex(int x, int y, int max_x, int max_y, double left, double top, double right, double bottom)
{
    return std::complex<double>(
        left + (right - left) * x / max_x,
        top + (bottom - top) * y / max_y
        );
}

// -----------------------------------------------------------------

void calc(HDC hDC, int width, int height) {

    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {

            std::complex<float> c = getComplex(x, y, width, height, XMIN, YMIN, XMAX, YMAX);

            unsigned int iterations = iterate(c, NumColors, AbsMax);

            // letzte Farbe in Palette ist schwarz !!!
            COLORREF cr = palette[iterations - 1];

            SetPixelV(hDC, x, y, cr);
        }
    }
}

// -----------------------------------------------------------------

Drei Aufrufe sind an geeigneten Stellen in der Mini-Windows Anwendung zu platzieren:

a)     fill(palette);

b)     calc(hdc, g_nWidth, g_nHeight);

c)    case WM_SIZE:
        // retrieve coordinates of window's client area
        GetClientRect(hWnd, &rect);

        g_nWidth = rect.right;
        g_nHeight = rect.bottom;
        return 0;


// =====================================================================
// =====================================================================

Mandelbrot Variante 2:
    
    = ( ) Ein Mandelbrot-Bild wird in mehreren kleineren Rechtecken
          sequentiell gezeichnet

    = ( ) Dieser Schritt dient zur Vorbereitung der Parallelisierung
          der Anwendung.

    = (-) Anwendung NICHT reaktionsfähig

// =====================================================================
// =====================================================================

Mandelbrot Variante 3:
    
    = (+) Ein Mandelbrot-Bild wird in mehreren kleineren Rechtecken
          PARALLEL gezeichnet

    = (-) Was beobachten Sie? Warum?

    = (-) Anwendung NICHT reaktionsfähig

// =====================================================================
// =====================================================================

Mandelbrot Variante 4:

    = (+) Die Anwendung wird reaktionsfähig
    
// =====================================================================
// =====================================================================

Projekt "C++, C++-17 Threads, Win32 API und DLLs"

// =====================================================================
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
static HANDLE g_hMutex;      // mutex to protect shared memory object
static HANDLE g_hMap;        // shared memory object
static char* g_pSharedMem;   // pointer to shared memory object

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

