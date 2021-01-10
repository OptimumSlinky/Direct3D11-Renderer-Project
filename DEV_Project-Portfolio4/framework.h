// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once
#include "targetver.h"

// Windows Header Files
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fstream>
#include <vector>

// Direct3D 11 Header Files
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h" // For texture loading

// D3D11 library dependencies
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Used to safely release a COM object and set the COM pointer to nullptr even if it has already been released before
template<typename T>
inline void SafeRelease(T& ptr)
{
    if (ptr != nullptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}