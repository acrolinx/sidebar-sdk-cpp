/* Copyright Acrolinx GmbH */

#pragma once
#include "JsonUtil.h"

namespace Acrolinx_Sdk_Sidebar_Util
{
    class DllUtil
    {
    public:
        DllUtil(void);
        ~DllUtil(void);
    protected:
        static CString GetFileVersion(CString fileName);
        static CString GetAppName(CString fileName);
        static BOOL DllUtil::GetOSNameString(CString& osName, CString& osVersion);
    public:
        static BOOL GetOSInfo(CString& id, CString& name, CString& version);
        static BOOL GetAppInfo(CString& id, CString& name, CString& version);
        static BOOL GetDllInfo(CString& id, CString& name, CString& version);
        static CString GetAppName(void);
        static CString GetLastErrorAsString(void);
        static HINSTANCE GetDllHandle(void);
        static HRSRC GetEmbeddedresource(int resourceId, CString type);
    private:
        static CString GetFormatedID(const CString& name);
    };
}

