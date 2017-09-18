#pragma once

#include <objbase.h>

#include "ComponentDll.h"

/**
 * summary:
 *
 *      Manages component installation by manipulating the
 *      registry keys.
 *
 * remarks:
 *
 *      The component should be registered in the Windows
 *      Registry in the following way:
 *
 *      (Optional)
 *      HKEY_CLASSES_ROOT\<ProgIdVerInd> = <Name>
 *      HKEY_CLASSES_ROOT\<ProgIdVerInd>\CurVer = <ProgId> (Optional)
 *      HKEY_CLASSES_ROOT\<ProgIdVerInd>\CLSID = <CLSID> (Optional)
 *
 *      (Optional)
 *      HKEY_CLASSES_ROOT\<ProgId> = <Name>
 *      HKEY_CLASSES_ROOT\<ProgId>\CurVer = <ProgId> (Possibly other) (Optional)
 *      HKEY_CLASSES_ROOT\<ProgId>\CLSID = <CLSID>
 *
 *      HKEY_CLASSES_ROOT\CLSID\<CLSID> = <Name> (Optional)
 *      HKEY_CLASSES_ROOT\CLSID\<CLSID>\ProgID = <ProgId> (Optional)
 *      HKEY_CLASSES_ROOT\CLSID\<CLSID>\VersionIndependentProgID = <ProgIdVerInd> (Optional)
 *      HKEY_CLASSES_ROOT\CLSID\<CLSID>\InprocServer32 = <PathToDll>
 *      HKEY_CLASSES_ROOT\CLSID\<CLSID>\InprocServer32\ThreadingModel = <ThreadingModel> (Optional)
 *
 *      Here we use Apartment threading model since no explicit
 *      thread synchronization of component internal variables
 *      was implemented.
 *
 * see also:
 *
 *      https://www.codeproject.com/Articles/338268/COM-in-C
 *      https://msdn.microsoft.com/en-us/library/ms809971.aspx
 *      https://www.codeproject.com/Articles/9190/Understanding-The-COM-Single-Threaded-Apartment-Pa
 */

namespace ComponentDll
{
    namespace Impl
    {
        extern HMODULE g_hCurrentModule;

        static const TCHAR Name[]  = _T("List Class");
        static const TCHAR ProgIdVerInd[] = _T("CList.Component");
        static const TCHAR ProgId[] = _T("CList.Component.1");
    }
}

STDAPI DllInstall(BOOL bInstall, PCWSTR pszCmdLine);
STDAPI DllRegisterServer();
STDAPI DllUnregisterServer();
