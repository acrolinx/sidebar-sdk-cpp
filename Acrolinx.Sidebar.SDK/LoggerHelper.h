/* Copyright (c) 2018 Acrolinx GmbH */

#pragma once

namespace Acrolinx_Sdk_Sidebar_Util
{
    class LoggerHelper
    {
    private:
        LoggerHelper(void);
        ~LoggerHelper(void);
    public:
        static bool Init();
        static CString GetLogFileName(void);
    private:
        static CString m_logFileName;
    };
}
