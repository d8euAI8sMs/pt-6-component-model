#include "stdafx.h"

#include <tchar.h>  // _tscspy_s, etc.
#include <olectl.h> // SELFREG_E_CLASS

#include "ComponentRegistration.h"

HMODULE ComponentDll::Impl::g_hCurrentModule;

STDAPI DllInstall(BOOL bInstall, PCWSTR pszCmdLine)
{
    // do nothing
    return S_OK;
}

BOOL SetRegistryKeyValue(_In_     LPCTSTR pszKey,       _In_opt_ LPCTSTR pszSubKey,
                         _In_opt_ LPCTSTR pszValueName, _In_     LPCTSTR pszValue)
{
    TCHAR buffer[MAX_PATH + 1];
    _tcscpy_s(buffer, pszKey);
    if (pszSubKey)
    {
        _tcscat_s(buffer, _T("\\"));
        _tcscat_s(buffer, pszSubKey);
    }

    HKEY hKeyUserClasses;
    LSTATUS lResult = RegCreateKeyEx(HKEY_CURRENT_USER,
	                                 _T("Software\\Classes"),
	                                 0, NULL, REG_OPTION_NON_VOLATILE,
	                                 KEY_ALL_ACCESS, NULL,
	                                 &hKeyUserClasses, NULL);
	if (lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}

    HKEY hKey;
    lResult = RegCreateKeyEx(hKeyUserClasses,
	                         buffer,
	                         0, NULL, REG_OPTION_NON_VOLATILE,
	                         KEY_ALL_ACCESS, NULL,
	                         &hKey, NULL);
	if (lResult != ERROR_SUCCESS)
	{
        RegCloseKey(hKeyUserClasses);
		return FALSE;
	}

	lResult = RegSetValueEx(hKey, pszValueName, 0, REG_SZ,
	                        (BYTE *)pszValue,
	                        (_tcslen(pszValue) + 1) * sizeof(TCHAR));

    RegCloseKey(hKey);
    RegCloseKey(hKeyUserClasses);

    return (lResult == ERROR_SUCCESS);
}

BOOL DeleteRegistryHive(_In_ HKEY hKeyParent, _In_ LPCTSTR pszKey)
{
    HKEY hKey;
    LSTATUS lResult = RegOpenKeyEx(hKeyParent,
	                               pszKey,
	                               REG_OPTION_NON_VOLATILE,
	                               KEY_ALL_ACCESS,
                                   &hKey);
	if (lResult != ERROR_SUCCESS)
	{
        if (lResult == ERROR_FILE_NOT_FOUND) return TRUE;
		return FALSE;
	}

    FILETIME time;
	TCHAR szBuffer[MAX_PATH + 1];
	DWORD dwSize = MAX_PATH + 1;
	while (RegEnumKeyEx(hKey, 0, szBuffer, &dwSize, NULL,
	                    NULL, NULL, &time) == ERROR_SUCCESS)
	{
        BOOL ok = DeleteRegistryHive(hKey, szBuffer);
		if (!ok)
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		dwSize = MAX_PATH + 1;
	}

    RegCloseKey(hKey);

    lResult = RegDeleteKey(hKeyParent, pszKey);

    return (lResult == ERROR_SUCCESS);
}

STDAPI DllRegisterServer()
{
    const SIZE_T clsidSize = 39;
    TCHAR clsid[clsidSize];
    TCHAR path[MAX_PATH + 1];

    // get module path
	if (!GetModuleFileName(ComponentDll::Impl::g_hCurrentModule, path, MAX_PATH + 1))
    {
        return SELFREG_E_CLASS;
    }

    // get clsid as string
    LPOLESTR pszCLSID = NULL;
	HRESULT hr = StringFromCLSID(ComponentDll::CLSID_CLIST, &pszCLSID);
    _tcscpy_s(clsid, pszCLSID);
	CoTaskMemFree(pszCLSID);

    BOOL result = TRUE;

    TCHAR buffer[MAX_PATH + 1] = _T("CLSID\\");
    _tcscat_s(buffer, clsid);

    // provide registration of version independent component information
    result &= SetRegistryKeyValue(ComponentDll::Impl::ProgIdVerInd, NULL, NULL, ComponentDll::Impl::Name);
    result &= SetRegistryKeyValue(ComponentDll::Impl::ProgIdVerInd, _T("CLSID"), NULL, clsid);
    result &= SetRegistryKeyValue(ComponentDll::Impl::ProgIdVerInd, _T("CurVer"), NULL, ComponentDll::Impl::ProgId);

    // provide registration of version dependent component information
    result &= SetRegistryKeyValue(ComponentDll::Impl::ProgId, NULL, NULL, ComponentDll::Impl::Name);
    result &= SetRegistryKeyValue(ComponentDll::Impl::ProgId, _T("CLSID"), NULL, clsid);
    result &= SetRegistryKeyValue(ComponentDll::Impl::ProgId, _T("CurVer"), NULL, ComponentDll::Impl::ProgId);

    // provide registration of clsid
    result &= SetRegistryKeyValue(buffer, NULL, NULL, ComponentDll::Impl::Name);
    result &= SetRegistryKeyValue(buffer, _T("ProgId"), NULL, ComponentDll::Impl::ProgId);
    result &= SetRegistryKeyValue(buffer, _T("VersionIndependentProgId"), NULL, ComponentDll::Impl::ProgIdVerInd);
    result &= SetRegistryKeyValue(buffer, _T("InprocServer32"), NULL, path);
    result &= SetRegistryKeyValue(buffer, _T("InprocServer32"), _T("ThreadingModel"), _T("Apartment"));

    return (result ? S_OK : SELFREG_E_CLASS);
}

STDAPI DllUnregisterServer()
{
    const SIZE_T clsidSize = 39;
    TCHAR clsid[clsidSize];

    // get clsid as string
    LPOLESTR pszCLSID = NULL;
	HRESULT hr = StringFromCLSID(ComponentDll::CLSID_CLIST, &pszCLSID);
    _tcscpy_s(clsid, pszCLSID);
	CoTaskMemFree(pszCLSID);

    BOOL result = TRUE;

    TCHAR buffer[MAX_PATH + 1] = _T("CLSID\\");
    _tcscat_s(buffer, clsid);

    HKEY hKeyUserClasses;
    LSTATUS lResult = RegCreateKeyEx(HKEY_CURRENT_USER,
	                                 _T("Software\\Classes"),
	                                 0, NULL, REG_OPTION_NON_VOLATILE,
	                                 KEY_ALL_ACCESS, NULL,
	                                 &hKeyUserClasses, NULL);
	if (lResult != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}

    result &= DeleteRegistryHive(hKeyUserClasses, ComponentDll::Impl::ProgIdVerInd);
    result &= DeleteRegistryHive(hKeyUserClasses, ComponentDll::Impl::ProgId);
    result &= DeleteRegistryHive(hKeyUserClasses, buffer);

    RegCloseKey(hKeyUserClasses);

    return (result ? S_OK : SELFREG_E_CLASS);
}
