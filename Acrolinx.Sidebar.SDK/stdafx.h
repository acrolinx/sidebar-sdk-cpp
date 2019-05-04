/* Copyright Acrolinx GmbH */

#pragma once

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#define _ATL_APARTMENT_THREADED 
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _ATL_NO_AUTOMATIC_NAMESPACE             // avoid class name conflicts

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Comon Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>


#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <windows.h>

#import "MSXML6.dll"  no_auto_exclude rename_namespace(_T("MSXML"))
#include <msxml2.h>

#include <plog/Log.h>

#ifdef _DEBUG
#define ACROASSERT(f, m) ASSERT(f) ; LOGE_IF(!(f)) << m
#else
#define ACROASSERT(f, m)  LOGE_IF(!(f)) << m
#endif