#pragma once

#include <objbase.h>

#include "ComponentDll.h"

namespace ComponentDll
{
    namespace Impl
    {
        extern HMODULE hCurrentModule;

        static const WCHAR Name[]  = L"List Class";
        static const WCHAR ProgIdVerInd[] = L"CList.Component";
        static const WCHAR ProgId[] = L"CList.Component.1";
    }
}

STDAPI DllInstall(BOOL bInstall, PCWSTR pszCmdLine);
STDAPI DllRegisterServer();
STDAPI DllUnregisterServer();
