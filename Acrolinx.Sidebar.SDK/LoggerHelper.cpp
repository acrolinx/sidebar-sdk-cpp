/* Copyright Acrolinx GmbH */

#include "StdAfx.h"
#include "LoggerHelper.h"
#include <string>
#include "DllUtil.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
CString Acrolinx_Sdk_Sidebar_Util::LoggerHelper::m_logFileName = _T("");
Acrolinx_Sdk_Sidebar_Util::LoggerHelper::LoggerHelper(void)
{
}


Acrolinx_Sdk_Sidebar_Util::LoggerHelper::~LoggerHelper(void)
{
}


void Acrolinx_Sdk_Sidebar_Util::LoggerHelper::Init()
{
    CString logFileName = CreateLogFileName();
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logFileName.GetString(), 10485760, 3);
    plog::init(IsLogLevelDebug()?plog::debug:plog::info, &fileAppender);
}

CString Acrolinx_Sdk_Sidebar_Util::LoggerHelper::GetLogFileName(void)
{
    return m_logFileName;
}


bool Acrolinx_Sdk_Sidebar_Util::LoggerHelper::IsLogLevelDebug()
{
    bool retVal = false;
    try
    {
        HKEY hKey;
        DWORD szData;
        DWORD dwKeyDataType = REG_SZ;
        DWORD dwDataBufSize = 1024;
        LONG regAccess;

        regAccess = ::RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Acrolinx\\Plugins\\SdkCpp"), 0, KEY_READ, &hKey);
        if (regAccess == ERROR_SUCCESS)
        {
            if (::RegQueryValueEx(hKey, _T("enable.debug.log"), nullptr, &dwKeyDataType, (LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
            {
                retVal = (szData == 1)?true:false;
            }
            RegCloseKey(hKey);
        }
    }
    catch (...){
        OutputDebugString(_T("Exception thrown by registry access"));
    }
    return retVal;
}

CString Acrolinx_Sdk_Sidebar_Util::LoggerHelper::CreateLogFileName()
{
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    CString curDate;
    curDate.Format(_T("%d-%d-%d"),1900 + ltm.tm_year, 1 + ltm.tm_mon, ltm.tm_mday);

    WCHAR wcharPath[MAX_PATH];
    CString tempPath = _T("");
    if (GetTempPathW(MAX_PATH, wcharPath))
    {
        tempPath = wcharPath;
    }

    tempPath = tempPath + _T("Acrolinx\\");
    CreateDirectory(tempPath, NULL);
    tempPath = tempPath + _T("Logs\\");
    CreateDirectory(tempPath, NULL);
    tempPath = tempPath + DllUtil::GetAppName() + _T("\\");
    CreateDirectory(tempPath, NULL);

    m_logFileName = tempPath + curDate +_T("-") + DllUtil::GetAppName() +  _T(".log");

    return m_logFileName;

}