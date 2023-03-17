/* Copyright Acrolinx GmbH */

#include "stdafx.h"
#include <Windows.h>
#include "RegistryAcrolinxStorage.h"
#include "DllUtil.h"

using namespace Acrolinx_Sdk_Sidebar_Util;

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

STDMETHODIMP CRegistryAcrolinxStorage::GetAllItems(BSTR* data)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *data = CString().AllocSysString();

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

    WDocument registryStorage;

    try
    {
        HKEY hKey;
        
        DWORD dwKeyDataType = REG_SZ;
        DWORD dwDataBufSize = 2056;
        WCHAR szData[2056];
        LONG regAccess;

        TCHAR    achKey[MAX_KEY_LENGTH];        // buffer for subkey name
        DWORD    cbName;                        // size of name string 
        TCHAR    achClass[MAX_PATH] = TEXT(""); // buffer for class name 
        DWORD    cchClassName = MAX_PATH;       // size of class string 
        DWORD    cSubKeys = 0;                  // number of subkeys 
        DWORD    cbMaxSubKey;                   // longest subkey size 
        DWORD    cchMaxClass;                   // longest class string 
        DWORD    cValues;                       // number of values for key 
        DWORD    cchMaxValue;                   // longest value name 
        DWORD    cbMaxValueData;                // longest value data 
        DWORD    cbSecurityDescriptor;          // size of security descriptor 
        FILETIME ftLastWriteTime;               // last write time 


        TCHAR  achValue[MAX_VALUE_NAME];
        DWORD cchValue = MAX_VALUE_NAME;
        DWORD i, retCode;


        regAccess = ::RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_QUERY_VALUE, &hKey);
        if (regAccess != ERROR_SUCCESS)
        {
            regAccess = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_QUERY_VALUE, &hKey);
        }

        if (regAccess == ERROR_SUCCESS)
        {
            retCode = RegQueryInfoKey(
                hKey,
                achClass,
                &cchClassName,
                NULL,
                &cSubKeys,
                &cbMaxSubKey,
                &cchMaxClass,
                &cValues,
                &cchMaxValue,
                &cbMaxValueData,
                &cbSecurityDescriptor,
                &ftLastWriteTime);

            // Enumerate the key values.

            if (cValues)
            {
                for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
                {
                    cchValue = MAX_VALUE_NAME;
                    achValue[0] = '\0';
                    retCode = RegEnumValue(hKey, i,
                        achValue,
                        &cchValue,
                        NULL,
                        NULL,
                        NULL,
                        NULL);

                    if (retCode == ERROR_SUCCESS)
                    {
                        DWORD lpData = cbMaxValueData;
                        LONG dwRes = ::RegQueryValueEx(hKey, achValue, 0, &dwKeyDataType, (LPBYTE)&szData, &lpData);

                        if (dwRes == ERROR_SUCCESS)
                        {
                            CString key = CString(L"/");
                            key.Append(achValue);
                            CString value = CString(szData);
                            CJsonUtil::SetString(registryStorage, key, value);
                            int a = 10;
                        }
                        else
                        {
                            LOGE << "Could not read value from registry " << achValue << " " << Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
                        }
                    }
                }
            }

            *data = CJsonUtil::Stringify(registryStorage).AllocSysString();

            ::RegCloseKey(hKey);
        }
        else
        {
            LOGE << "Fail to open registry";
        }
    }
    catch (...)
    {
        LOGE << "Exception thrown by registry" << Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
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
