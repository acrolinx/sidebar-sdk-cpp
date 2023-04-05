#include "stdafx.h"
#include "FileUtil.h"
#include <string>
#include "DllUtil.h"
#include "Resource.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

Acrolinx_Sdk_Sidebar_Util::FileUtil::FileUtil(void)
{
}

Acrolinx_Sdk_Sidebar_Util::FileUtil::~FileUtil(void)
{
}

void Acrolinx_Sdk_Sidebar_Util::FileUtil::ExtractEmbeddedStartPage()
{
    HINSTANCE hwnd = DllUtil::GetDllHandle();
    HRSRC hResource = FindResource(hwnd, MAKEINTRESOURCE(IDR_STARTPAGE), L"BINARY");
    HGLOBAL hResData = LoadResource(hwnd, hResource);
    DWORD  resSize = SizeofResource(hwnd, hResource);

    LPBYTE lpRes = (LPBYTE)LockResource(hResData);

    if (!hResource || !hResData || !lpRes || !resSize) {
        LOGE << "Error reading embedded start page resource";
    }

    WCHAR wcharPath[MAX_PATH];
    CString tempPath = _T("");
    if (GetTempPathW(MAX_PATH, wcharPath))
    {
        tempPath = wcharPath;
    }

    tempPath = tempPath + _T("Acrolinx\\");
    CreateDirectory(tempPath, NULL);

    CString startPageZip = tempPath + _T("startpage.zip");

    tempPath = tempPath + _T("startpage-") +DllUtil::GetAppName() + _T("\\");
    CreateDirectory(tempPath, NULL);

    HANDLE hFile = CreateFile(startPageZip, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        // TODO: Handle error
    }

    DWORD dwBytesWritten = 0;
    BOOL bSuccess = WriteFile(hFile, lpRes, resSize, &dwBytesWritten, NULL);
    if (!bSuccess || dwBytesWritten != resSize) {
        // TODO: Handle error
    }

    CloseHandle(hFile);
}
