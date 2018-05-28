/* Copyright (c) 2018 Acrolinx GmbH */

#include "StdAfx.h"
#include "LoggerHelper.h"
#include <string>
#include "easylogging++.h"
#include "DllUtil.h"


EXTERN_C IMAGE_DOS_HEADER __ImageBase;
CString Acrolinx_Sdk_Sidebar_Util::LoggerHelper::m_logFileName = _T("");
Acrolinx_Sdk_Sidebar_Util::LoggerHelper::LoggerHelper(void)
{
}


Acrolinx_Sdk_Sidebar_Util::LoggerHelper::~LoggerHelper(void)
{
}


bool Acrolinx_Sdk_Sidebar_Util::LoggerHelper::Init()
{
    try{
        // Do default settings
        WCHAR   dllName[MAX_PATH] = {0};
        if(!GetModuleFileNameW((HINSTANCE)&__ImageBase, dllName, _countof(dllName)))
        {
            OutputDebugString(_T("Unable to get path for the loaded dll file"));
            return false;
        }
        CString dllPathWithFileName(dllName);
        int lastIndexOfBackSlash = dllPathWithFileName.ReverseFind('\\');
        CString dllPath = dllPathWithFileName.Left(lastIndexOfBackSlash + 1);
        std::string initFileName = std::string((CStringA)dllPath);
        FILE *file = nullptr;
        initFileName.append("AcrolinxLog.properties");
        if (!(fopen_s(&file, initFileName.c_str(), "r")))
        {
            fclose(file);
            easyloggingpp::Configurations confFromFile(initFileName);  // Load configuration from file
            easyloggingpp::Loggers::reconfigureAllLoggers(confFromFile); // Re-configures all the loggers to current configuration file
            confFromFile.clear();
        }
        else
        {
            easyloggingpp::Configurations defaultConf;
            defaultConf.setToDefault();
            defaultConf.set(easyloggingpp::Level::All, easyloggingpp::ConfigurationType::Format, "%datetime | [%level] %loc: %log");
            easyloggingpp::Loggers::reconfigureAllLoggers(defaultConf);
            defaultConf.clear();
        }
    }catch (...){
        OutputDebugString(_T("Unable to read AcrolinxLog.property file hence setting default logging settings"));
    }

    // Do default settings
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
    tempPath = tempPath + _T("Acrolinx\\Logs\\");

    m_logFileName = tempPath + curDate +_T("-") + DllUtil::GetAppName() +  _T(".log");
    std::string defaultLogFileName ((CStringA)m_logFileName);

    easyloggingpp::Loggers::reconfigureAllLoggers(easyloggingpp::ConfigurationType::Filename, defaultLogFileName);

    //Delete empty log file & folder: easylogging++ creates an empty file(shortcoming of v8.91)
    WCHAR currentPath[MAX_PATH];
    DWORD dwRet;
    dwRet = GetCurrentDirectory(MAX_PATH, currentPath);
    if( dwRet == 0 || dwRet > MAX_PATH)
    {
        LERROR << "Failed to get temp log file path: " << DllUtil::GetLastErrorAsString().GetString();
    }
    else
    {
        CString filePath(currentPath);
        filePath.Append(_T("\\logs\\myeasylog.log"));
        BOOL retVal = DeleteFile(filePath);
        if(retVal)
        {
            CString logPath(currentPath);
            logPath.Append(_T("\\logs"));
            retVal = RemoveDirectory(logPath);
        }

        if (!retVal)
        {
            LERROR << "Failed to delete temp log file or path: " << DllUtil::GetLastErrorAsString().GetString();
        }
    }

    return true;
}

CString Acrolinx_Sdk_Sidebar_Util::LoggerHelper::GetLogFileName(void)
{
    return m_logFileName;
}
