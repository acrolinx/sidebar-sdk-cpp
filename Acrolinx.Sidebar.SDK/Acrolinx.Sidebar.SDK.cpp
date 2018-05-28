/* Copyright (c) 2018 Acrolinx GmbH */

// Acrolinx.Sidebar.SDK.cpp : Implementation of CAcrolinxSidebarSDKApp and DLL registration.

#include "stdafx.h"
#include "Acrolinx.Sidebar.SDK.h"
#include <initguid.h>
#include "Acrolinx.Sidebar.SDK_i.c"
#include "easylogging++.h"
#include "LoggerHelper.h"


//This is important for logger initialization
_INITIALIZE_EASYLOGGINGPP

class CAcrolinxSidebarSDKModule :
public ATL::CAtlMfcModule
{
public:
DECLARE_LIBID(LIBID_AcrolinxSidebarSDKLib);
DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ACROLINXSIDEBARSDK, "{949B9D2D-B59A-487C-A958-AC6492821378}");};

CAcrolinxSidebarSDKModule _AtlModule;

CAcrolinxSidebarSDKApp theApp;

const GUID CDECL _tlid = { 0xC09DADB1, 0x142C, 0x440A, { 0xBF, 0x4A, 0x83, 0x99, 0xAA, 0xE8, 0x2, 0xB3 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CAcrolinxSidebarSDKApp::InitInstance - DLL initialization

BOOL CAcrolinxSidebarSDKApp::InitInstance()
{
    BOOL bInit = COleControlModule::InitInstance();

    if (bInit)
    {
        AfxEnableControlContainer();
    }

    Acrolinx_Sdk_Sidebar_Util::LoggerHelper::Init();
    return bInit;
}



// CAcrolinxSidebarSDKApp::ExitInstance - DLL termination

int CAcrolinxSidebarSDKApp::ExitInstance()
{
    return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    _AtlModule.UpdateRegistryAppId(TRUE);
    HRESULT hRes2 = _AtlModule.RegisterServer(TRUE);
    if (hRes2 != S_OK)
        return hRes2;
    AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
        return ResultFromScode(SELFREG_E_CLASS);

    return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    _AtlModule.UpdateRegistryAppId(FALSE);
    HRESULT hRes2 = _AtlModule.UnregisterServer(TRUE);
    if (hRes2 != S_OK)
        return hRes2;
    AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
        return ResultFromScode(SELFREG_E_CLASS);

    return NOERROR;
}

// DllCanUnloadNow - Allows COM to unload DLL
#if !defined(_WIN32_WCE) && !defined(_AMD64_) && !defined(_IA64_)
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")
#else
#if defined(_X86_) || defined(_SHX_)
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer,PRIVATE")
#endif // (_X86_)||(_SHX_)
#endif // !_WIN32_WCE && !_AMD64_ && !_IA64_ 

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (_AtlModule.GetLockCount() > 0)
        return S_FALSE;
    return S_OK;
}

// DllGetClassObject - Returns class factory
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (S_OK == _AtlModule.GetClassObject(rclsid, riid, ppv))
        return S_OK;
    return AfxDllGetClassObject(rclsid, riid, ppv);
}
