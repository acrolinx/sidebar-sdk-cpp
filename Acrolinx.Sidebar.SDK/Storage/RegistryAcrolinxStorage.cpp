/* Copyright Acrolinx GmbH */

#include "StdAfx.h"
#include "RegistryAcrolinxStorage.h"
#include <Windows.h>
#include "DllUtil.h"


CRegistryAcrolinxStorage::CRegistryAcrolinxStorage(void)
    : keyPath(_T("Software\\Acrolinx\\Plugins\\Storage\\AcrolinxStorage"))
{
}


CRegistryAcrolinxStorage::~CRegistryAcrolinxStorage(void)
{
}


STDMETHODIMP CRegistryAcrolinxStorage::RemoveItem(BSTR key)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    try
    {
        HKEY hKey;
        LONG regAccess;
        regAccess = ::RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &hKey);
        if(regAccess == ERROR_SUCCESS)
        {
            if(::RegDeleteValue(hKey,CString(key)) != ERROR_SUCCESS)
            {
                LOGE << "Fail to remove values";
            }
            ::RegCloseKey(hKey);
        }
    }
    catch(...)
    {
        LOGE << "Exception thrown by registry";
    }

    return S_OK;
}


STDMETHODIMP CRegistryAcrolinxStorage::SetItem(BSTR key, BSTR data)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    try
    {
        HKEY hKey;
        LONG regAccess;
        regAccess = ::RegCreateKeyEx(HKEY_CURRENT_USER, keyPath, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, 0);
        if(regAccess == ERROR_SUCCESS)
        {
            CString value(data);
            if(::RegSetValueEx(hKey, key, 0, REG_SZ, (LPBYTE)value.GetString(), (value.GetLength()*(sizeof(WCHAR))) + 1) != ERROR_SUCCESS)
            {
                LOGE << "Fail to store values";
            }

            ::RegCloseKey(hKey);
        }
    }
    catch(...)
    {
        LOGE << "Exception thrown by registry";
    }

    return S_OK;
}


STDMETHODIMP CRegistryAcrolinxStorage::GetItem(BSTR key, BSTR* data)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *data = CString().AllocSysString();

    try
    {
        HKEY hKey;
        WCHAR szData[2056];
        DWORD dwKeyDataType = REG_SZ;
        DWORD dwDataBufSize = 2056;
        LONG regAccess;


        regAccess = ::RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_QUERY_VALUE, &hKey);
        if (regAccess != ERROR_SUCCESS)
        {
            regAccess = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_QUERY_VALUE, &hKey);
        }

        if (regAccess == ERROR_SUCCESS)
        {
            if (::RegQueryValueEx(hKey, CString(key), nullptr, &dwKeyDataType, (LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
            {
                *data = CString(szData).AllocSysString();
            }
            else
            {
                LOGD << "Fail to retrieve the storage values " << Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
            }
            ::RegCloseKey( hKey );
        }
        else
        {
            LOGE << "Fail to open registry";
        }
    }
    catch(...)
    {
        LOGE << "Exception thrown by registry" << Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
    }

    return S_OK;
}
