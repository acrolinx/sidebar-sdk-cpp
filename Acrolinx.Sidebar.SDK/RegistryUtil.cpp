/* Copyright 2023-present Acrolinx GmbH */

#include "stdafx.h"
#include "RegistryUtil.h"

const CString Acrolinx_Sdk_Sidebar_Util::RegistryUtil::keyPath =
_T("Software\\Acrolinx\\Plugins");

Acrolinx_Sdk_Sidebar_Util::RegistryUtil::RegistryUtil(void)
{
}

Acrolinx_Sdk_Sidebar_Util::RegistryUtil::~RegistryUtil(void)
{

}

void Acrolinx_Sdk_Sidebar_Util::RegistryUtil::GetDWORDRegHKCU(CString relativeKeyPath, CString key, DWORD &value)
{
    DWORD result(0);
    DWORD dwBufferSize(sizeof(DWORD));
    DWORD dwKeyDataType = REG_DWORD;
    HKEY hKey;

    CString absKeyPath = keyPath + relativeKeyPath;

    LONG res = ::RegOpenKeyEx(HKEY_CURRENT_USER, absKeyPath, 0, KEY_QUERY_VALUE, &hKey);

    if (res == ERROR_SUCCESS)
    {
        res = ::RegQueryValueEx(hKey, key, nullptr, &dwKeyDataType, (LPBYTE)&result, &dwBufferSize);

        if (res == ERROR_SUCCESS)
        {
            value = result;
        }
        else
        {
            LOGE << "Failed to retrive value for: " << key;
        }

        ::RegCloseKey(hKey);
    }
    else
    {
        LOGE << "Unable to open registry key: " << absKeyPath;
    }
}
