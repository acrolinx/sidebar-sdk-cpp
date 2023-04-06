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

CString Acrolinx_Sdk_Sidebar_Util::FileUtil::ExtractEmbeddedStartPage()
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
    CString startPageTempPath = _T("");
    if (GetTempPathW(MAX_PATH, wcharPath))
    {
        startPageTempPath = wcharPath;
    }

    startPageTempPath = startPageTempPath + _T("Acrolinx\\");
    CreateDirectory(startPageTempPath, NULL);

    CString startPageZip = startPageTempPath + _T("startpage.zip");

    startPageTempPath = startPageTempPath + _T("startpage-") + DllUtil::GetAppName() + _T("\\");
    CreateDirectory(startPageTempPath, NULL);

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

    bool success = FileUtil::UnzipToFolder(startPageZip.AllocSysString(), startPageTempPath.AllocSysString());
    if (!success)
    {
        // TODO: Handle error
    }

    return startPageTempPath;
}

CString Acrolinx_Sdk_Sidebar_Util::FileUtil::GetWebViewUserDataDirectory()
{
    WCHAR wcharPath[MAX_PATH];
    CString webViewWorkingDir = _T("");
    if (GetTempPathW(MAX_PATH, wcharPath))
    {
        webViewWorkingDir = wcharPath;
        webViewWorkingDir = webViewWorkingDir + _T("Acrolinx\\");
        CreateDirectory(webViewWorkingDir, NULL);
        webViewWorkingDir = webViewWorkingDir + _T("webview-") + DllUtil::GetAppName() + _T("\\");
        CreateDirectory(webViewWorkingDir, NULL);
    }
    else
    {
        LOGE << "Creating working directory for webview failed.";
    }
    return webViewWorkingDir;
}

bool Acrolinx_Sdk_Sidebar_Util::FileUtil::UnzipToFolder(BSTR lpZipFile, BSTR lpFolder)
{
    IShellDispatch *pISD;

    Folder  *pZippedFile = 0L;
    Folder  *pDestination = 0L;

    long FilesCount = 0;
    IDispatch* pItem = 0L;
    FolderItems *pFilesInside = 0L;

    VARIANT Options, OutFolder, InZipFile, Item;
    CoInitialize(NULL);
    __try {
        if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD) != S_OK)
            return 1;

        InZipFile.vt = VT_BSTR;
        InZipFile.bstrVal = lpZipFile;
        pISD->NameSpace(InZipFile, &pZippedFile);
        if (!pZippedFile)
        {
            pISD->Release();
            return 1;
        }

        OutFolder.vt = VT_BSTR;
        OutFolder.bstrVal = lpFolder;
        pISD->NameSpace(OutFolder, &pDestination);
        if (!pDestination)
        {
            pZippedFile->Release();
            pISD->Release();
            return 1;
        }

        pZippedFile->Items(&pFilesInside);
        if (!pFilesInside)
        {
            pDestination->Release();
            pZippedFile->Release();
            pISD->Release();
            return 1;
        }

        pFilesInside->get_Count(&FilesCount);
        if (FilesCount < 1)
        {
            pFilesInside->Release();
            pDestination->Release();
            pZippedFile->Release();
            pISD->Release();
            return 0;
        }

        pFilesInside->QueryInterface(IID_IDispatch, (void**)&pItem);

        Item.vt = VT_DISPATCH;
        Item.pdispVal = pItem;

        Options.vt = VT_I4;
        Options.lVal = 1024 | 512 | 16 | 4;

        bool retval = pDestination->CopyHere(Item, Options) == S_OK;

        pItem->Release(); pItem = 0L;
        pFilesInside->Release(); pFilesInside = 0L;
        pDestination->Release(); pDestination = 0L;
        pZippedFile->Release(); pZippedFile = 0L;
        pISD->Release(); pISD = 0L;

        return retval;

    }
    __finally
    {
        CoUninitialize();
    }
}