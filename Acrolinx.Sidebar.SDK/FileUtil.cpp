/* Copyright 2023-present Acrolinx GmbH */

#include "stdafx.h"
#include "FileUtil.h"
#include <string>
#include "DllUtil.h"
#include "Resource.h"

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
        LOGE << "Creating startpage zip file failed.";
        startPageTempPath = _T("");
    }

    DWORD dwBytesWritten = 0;
    BOOL bSuccess = WriteFile(hFile, lpRes, resSize, &dwBytesWritten, NULL);
    if (!bSuccess || dwBytesWritten != resSize) {
        LOGE << "Writing startpage temp file failed .";
        startPageTempPath = _T("");
    }

    CloseHandle(hFile);

    bool success = FileUtil::UnzipToFolder(startPageZip.AllocSysString(), startPageTempPath.AllocSysString());
    if (!success)
    {
        LOGE << "Unzipping startpage failed";
        startPageTempPath = _T("");
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

    long filesCount = 0;
    IDispatch* pItem = 0L;
    FolderItems *pFilesInside = 0L;

    VARIANT options, outFolder, inZipFile, item;
    CoInitialize(NULL);
    try {
        if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD) != S_OK)
            return false;

        inZipFile.vt = VT_BSTR;
        inZipFile.bstrVal = lpZipFile;
        pISD->NameSpace(inZipFile, &pZippedFile);
        if (!pZippedFile)
        {
            pISD->Release();
            return false;
        }

        outFolder.vt = VT_BSTR;
        outFolder.bstrVal = lpFolder;
        pISD->NameSpace(outFolder, &pDestination);
        if (!pDestination)
        {
            pZippedFile->Release();
            pISD->Release();
            return false;
        }

        pZippedFile->Items(&pFilesInside);
        if (!pFilesInside)
        {
            pDestination->Release();
            pZippedFile->Release();
            pISD->Release();
            return false;
        }

        pFilesInside->get_Count(&filesCount);
        if (filesCount < 1)
        {
            pFilesInside->Release();
            pDestination->Release();
            pZippedFile->Release();
            pISD->Release();
            return false;
        }

        pFilesInside->QueryInterface(IID_IDispatch, (void**)&pItem);

        item.vt = VT_DISPATCH;
        item.pdispVal = pItem;

        options.vt = VT_I4;
        options.lVal = 1024 | 512 | 16 | 4;

        bool retval = pDestination->CopyHere(item, options) == S_OK;

        pItem->Release(); pItem = 0L;
        pFilesInside->Release(); pFilesInside = 0L;
        pDestination->Release(); pDestination = 0L;
        pZippedFile->Release(); pZippedFile = 0L;
        pISD->Release(); pISD = 0L;

        return retval;

    }
    catch (std::exception& exception)
    {
        LOGE << "Failed to unzip startpage: " << exception.what();
    }

    CoUninitialize();
}
