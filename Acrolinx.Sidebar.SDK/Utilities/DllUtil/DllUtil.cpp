/* Copyright (c) 2018 Acrolinx GmbH */

#include "StdAfx.h"
#include <Winnt.h>
#include "DllUtil.h"
#include <Wbemcli.h>
#include "easylogging++.h"
#include <Strsafe.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

Acrolinx_Sdk_Sidebar_Util::DllUtil::DllUtil(void)
{
}


Acrolinx_Sdk_Sidebar_Util::DllUtil::~DllUtil(void)
{
}


BOOL Acrolinx_Sdk_Sidebar_Util::DllUtil::GetOSInfo(CString& id, CString& name, CString& version)
{
    if(!GetOSNameString(name, version))
    {
        LERROR << "Acrolinx_Sdk_Sidebar_Util::DllUtil::GetOSInfo: GetOSNameString failed";
        return FALSE;
    }
    id = GetFormatedID(name);
    return TRUE;
}


BOOL Acrolinx_Sdk_Sidebar_Util::DllUtil::GetAppInfo(CString& id, CString& name, CString& version)
{
    try
    {
        WCHAR   szPath[MAX_PATH] = {0};
        if(!GetModuleFileName(nullptr, szPath, _countof(szPath)))
        {
            LERROR << "Acrolinx_Sdk_Sidebar_Util::DllUtil::GetAppInfo: Failed to get executable path";
            return FALSE;
        }
        version = GetFileVersion(CString(szPath));
        name = GetAppName();
        id = GetFormatedID(name);
        return TRUE;
    }
    catch(...)
    {
        LERROR << "Failed to get application info";
    }
    return FALSE;
}


BOOL Acrolinx_Sdk_Sidebar_Util::DllUtil::GetDllInfo(CString& id, CString& name, CString& version)
{
    try
    {
        id = _T("acrolinx.sidebar");
        WCHAR   szPath[MAX_PATH] = {0};
        if(!GetModuleFileName((HINSTANCE)&__ImageBase, szPath, _countof(szPath)))
        {
            LERROR << "Acrolinx_Sdk_Sidebar_Util::DllUtil::GetAppInfo: Failed to get executable path";
            return FALSE;
        }
        name = szPath;
        name = name.Right(name.GetLength() - name.ReverseFind('\\') - 1);
        version = GetFileVersion(CString(szPath));
        return TRUE;
    }
    catch(...)
    {
        LERROR << "Failed to get dll info";
    }
    return FALSE;
}


CString Acrolinx_Sdk_Sidebar_Util::DllUtil::GetFileVersion(CString fileName)
{
    CString dllVersion = _T("");
    char* verData = nullptr;
    try
    {
        DWORD verHandle = 0;
        UINT size = 0;
        LPBYTE lpBuffer = nullptr;
        DWORD verSize = GetFileVersionInfoSize( fileName, &verHandle);

        if (verSize != 0)
        {
            verData = new char[verSize];
            if (GetFileVersionInfo( fileName, verHandle, verSize, verData))
            {
                if (VerQueryValue(verData, _T("\\"),(VOID FAR* FAR*)&lpBuffer,&size))
                {
                    if (size)
                    {
                        VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
                        if (verInfo->dwSignature == 0xfeef04bd)
                        {
                            dllVersion.Format(_T("%d.%d.%d.%d"),
                                ( verInfo->dwProductVersionMS >> 16 ) & 0xffff,
                                ( verInfo->dwProductVersionMS >>  0 ) & 0xffff,
                                ( verInfo->dwProductVersionLS >> 16 ) & 0xffff,
                                ( verInfo->dwProductVersionLS >>  0 ) & 0xffff);
                        }
                    }
                }
            }
        }
    }
    catch(...)
    {
        LERROR << "Failed to get file version";
    }
    delete[] verData;
    return dllVersion;
}


CString Acrolinx_Sdk_Sidebar_Util::DllUtil::GetAppName(CString fileName)
{
    CString appName = _T("");
    char* verData = nullptr;
    try
    {
        LPBYTE lpBuffer = nullptr;
        DWORD verHandle = 0;
        UINT size = 0;
        DWORD verSize = GetFileVersionInfoSize( fileName, &verHandle);
        if (verSize != 0 )
        {
            verData = new char[verSize];
            if (GetFileVersionInfo( fileName, verHandle, verSize, verData))
            {
                // To read English file description
                if(VerQueryValue(verData, TEXT("\\StringFileInfo\\040904b0\\FileDescription"), (LPVOID*)&lpBuffer, &size))
                {
                    appName = (LPTSTR)lpBuffer;
                }
            }
        }
    }
    catch(...)
    {
        LERROR << "Failed to get app name";
    }
    delete[] verData;
    return appName;
}


BOOL Acrolinx_Sdk_Sidebar_Util::DllUtil::GetOSNameString(CString& osName, CString& osVersion)
{
    HRESULT hr = S_FALSE;
    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;
    IEnumWbemClassObject *pEnumerator = nullptr;

    try
    {
        PCWSTR pszMachineName = L".";

        LTRACE << "Initialize COM process security by calling CoInitializeSecurity.";
        hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IDENTIFY, nullptr, EOAC_NONE, nullptr);
        if (FAILED(hr) && hr != RPC_E_TOO_LATE)
        {
            goto Cleanup;
        }

        LTRACE << "Obtain the initial locator to WMI by calling CoCreateInstance.";
        hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<LPVOID *>(&pLoc));
        if (FAILED(hr))
        {
            goto Cleanup;
        }

        LTRACE << "Connect to WMI through the IWbemLocator::ConnectServer method.";
        wchar_t szPath[200];
        hr = StringCchPrintf(szPath, ARRAYSIZE(szPath), L"\\\\%s\\root\\cimv2", pszMachineName);
        if (FAILED(hr))
        {
            goto Cleanup;
        }
        hr = pLoc->ConnectServer(bstr_t(szPath), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc);
        if (FAILED(hr))
        {
            goto Cleanup;
        }

        LTRACE << "Set security levels on a WMI connection";
        hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_DEFAULT, RPC_C_AUTHZ_DEFAULT, COLE_DEFAULT_PRINCIPAL,
            RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        if (FAILED(hr))
        {
            goto Cleanup;
        }

        LTRACE << "Use the IWbemServices pointer to make requests of WMI";
        hr = pSvc->ExecQuery(bstr_t(L"WQL"), bstr_t(L"SELECT Caption, Version FROM Win32_OperatingSystem"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
        if (FAILED(hr))
        {
            goto Cleanup;
        }

        LTRACE << "Secure the enumerator proxy.";
        hr = CoSetProxyBlanket(pEnumerator, RPC_C_AUTHN_DEFAULT, RPC_C_AUTHZ_DEFAULT,
            COLE_DEFAULT_PRINCIPAL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        if (FAILED(hr))
        {
            goto Cleanup;
        }

        LTRACE << "Get the data from the above query.";
        IWbemClassObject *pclsObj = nullptr;
        ULONG uReturn = 0;
        while (pEnumerator)
        {
            LTRACE << "Get one object.";
            HRESULT hrTmp = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (0 == uReturn)
            {
                break;
            }
            VARIANT vtProp;

            LTRACE << "Get the value of the Win32_OperatingSystem.Caption property.";
            hrTmp = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hrTmp))
            {
                wchar_t pszName[1024];
                hr = StringCchCopy(pszName, ARRAYSIZE(szPath), vtProp.bstrVal);
                VariantClear(&vtProp);
                osName = pszName;
            }

            LTRACE << "Get the value of the Win32_OperatingSystem.version property.";
            hrTmp = pclsObj->Get(L"Version", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hrTmp))
            {
                wchar_t pszVersion[1024];
                hr = StringCchCopy(pszVersion, ARRAYSIZE(szPath), vtProp.bstrVal);
                VariantClear(&vtProp);
                osVersion = pszVersion;
            }

            pclsObj->Release();
            pclsObj = nullptr;
        }
    }
    catch(...)
    {
        LERROR << "Failed to get OS name";
    }
Cleanup:
    // Centralized cleanup for all allocated resources
    if (pLoc)
    {
        pLoc->Release();
        pLoc = nullptr;
    }
    if (pSvc)
    {
        pSvc->Release();
        pSvc = nullptr;
    }
    if (pEnumerator)
    {
        pEnumerator->Release();
        pEnumerator = nullptr;
    }
    return (hr == S_OK)?TRUE:FALSE;
}


CString Acrolinx_Sdk_Sidebar_Util::DllUtil::GetAppName(void)
{
    try
    {
        WCHAR   szPath[MAX_PATH] = {0};
        if(!GetModuleFileName(nullptr, szPath, _countof(szPath)))
        {
            LERROR << "Failed to get executable path: " << GetLastErrorAsString().GetString();
            return CString();
        }
        CString name = GetAppName(szPath);
        if(name.IsEmpty())
        {
            name = szPath;
            name = name.Right(name.GetLength() - name.ReverseFind('\\') - 1);
        }
        return name;
    }
    catch(...)
    {
        LERROR << "Failed to get application name";
    }
    return CString();
}


CString Acrolinx_Sdk_Sidebar_Util::DllUtil::GetFormatedID(const CString& name)
{
    CString id = name;
    id = id.Trim();
    id.Replace(L" ", L".");
    id.MakeLower();
    return id;
}

CString Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString(void)
{
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return CString();

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    CString message(messageBuffer, size);
    LocalFree(messageBuffer);

    return message;
}