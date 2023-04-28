/* Copyright 2023-present Acrolinx GmbH */

#pragma once

namespace Acrolinx_Sdk_Sidebar_Util
{
    class FileUtil
    {
    public:
        FileUtil(void);
        ~FileUtil(void);
    public:
        static CString ExtractEmbeddedStartPage();
        static CString GetWebViewUserDataDirectory();
    private:
        static bool UnzipToFolder(BSTR lpZipFile, BSTR lpFolder);
    };

}

