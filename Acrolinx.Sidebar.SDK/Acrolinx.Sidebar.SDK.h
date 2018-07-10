/* Copyright Acrolinx GmbH */

#pragma once

// Acrolinx.Sidebar.SDK.h : main header file for Acrolinx.Sidebar.SDK.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols
#include "Acrolinx.Sidebar.SDK_i.h"


// CAcrolinxSidebarSDKApp : See Acrolinx.Sidebar.SDK.cpp for implementation.

class CAcrolinxSidebarSDKApp : public COleControlModule
{
public:
    BOOL InitInstance();
    int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

