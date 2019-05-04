/* Copyright Acrolinx GmbH */

#pragma once

namespace Acrolinx_Sdk_Sidebar_Util
{
    class LoggerHelper
    {
    private:
        LoggerHelper(void);
        ~LoggerHelper(void);
    public:
        static void Init();
        static CString GetLogFileName(void);
    private:
        static CString m_logFileName;
    private:
        static bool IsLogLevelDebug();
        static CString CreateLogFileName();
    };
}
