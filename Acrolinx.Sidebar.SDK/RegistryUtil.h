#pragma once

namespace Acrolinx_Sdk_Sidebar_Util
{
    class RegistryUtil
    {
    public:
        RegistryUtil(void);
        ~RegistryUtil(void);
    public:
        static void GetDWORDRegHKCU(CString relativeKeyPath, CString key, DWORD &value);

    private:
        static const CString keyPath;
    };

}

