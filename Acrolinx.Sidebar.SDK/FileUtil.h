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
        static bool UnzipToFolder(BSTR lpZipFile, BSTR lpFolder);
    };

}
