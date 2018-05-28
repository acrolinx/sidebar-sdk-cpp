/* Copyright (c) 2018 Acrolinx GmbH */

// AcrolinxSidebar.cpp : Implementation of the CAcrolinxSidebar ActiveX Control class.

#include "stdafx.h"
#include "Acrolinx.Sidebar.SDK.h"
#include "AcrolinxSidebar.h"
#include "AcrolinxSidebarPpg.h"
#include "afxdialogex.h"
#include "easylogging++.h"
#include "Dokument.h"
#include "Ranges.h"


IMPLEMENT_DYNCREATE(CAcrolinxSidebar, COleControl)


// Message map

BEGIN_MESSAGE_MAP(CAcrolinxSidebar, COleControl)
    ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
    ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CAcrolinxSidebar, COleControl)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "Start", dispidStart, Start, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "StartWithServerAddress", dispidStartWithServerAddress, StartWithServerAddress, VT_EMPTY, VTS_BSTR)
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "ServerAddress", dispidServerAddress, GetServerAddress, SetServerAddress, VT_BSTR)
    DISP_DEFVALUE(CAcrolinxSidebar, "ServerAddress")
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "StartPageSourceLocation", dispidStartPageSourceLocation, GetStartPageSourceLocation, SetStartPageSourceLocation, VT_BSTR)
    DISP_DEFVALUE(CAcrolinxSidebar, "StartPageSourceLocation")
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "ShowServerSelector", dispidShowServerSelector, GetShowServerSelector, SetShowServerSelector, VT_BOOL)
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "ClientSignature", dispidClientSignature, GetClientSignature, SetClientSignature, VT_BSTR)
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "MinimumSidebarVersion", dispidMinimumSidebarVersion, GetMinimumSidebarVersion, SetMinimumSidebarVersion, VT_BSTR)
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "ReadOnlySuggestions", dispidReadOnlySuggestions, GetReadOnlySuggestions, SetReadOnlySuggestions, VT_BOOL)
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "SupportCheckSelection", dispidSupportCheckSelection, GetSupportCheckSelection, SetSupportCheckSelection, VT_BOOL)
    DISP_PROPERTY_EX_ID(CAcrolinxSidebar, "ClientLocale", dispidClientLocale, GetClientLocale, SetClientLocale, VT_BSTR)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "LogInfo", dispidLogInfo, LogInfo, VT_EMPTY, VTS_BSTR)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "LogTrace", dispidLogTrace, LogTrace, VT_EMPTY, VTS_BSTR)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "LogError", dispidLogError, LogError, VT_EMPTY, VTS_BSTR)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "LogWarn", dispidLogWarn, LogWarn, VT_EMPTY, VTS_BSTR)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "CreateDocument", dispidCreateDocument, CreateDocument, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "Check", dispidCheck, Check, VT_BSTR, VTS_DISPATCH)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "InvalidateRanges", dispidInvalidateRanges, InvalidateRanges, VT_EMPTY, VTS_DISPATCH)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "RegisterClientComponent", dispidRegisterClientComponent, RegisterClientComponent, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4)
    DISP_FUNCTION_ID(CAcrolinxSidebar, "CreateRangeList", dispidCreateRangeList, CreateRangeList, VT_DISPATCH, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CAcrolinxSidebar, COleControl)
    EVENT_CUSTOM_ID("SidebarLoaded", eventidSidebarLoaded, SidebarLoaded, VTS_BSTR)
    EVENT_CUSTOM_ID("InitFinished", eventidInitFinished, InitFinished, VTS_NONE)
    EVENT_CUSTOM_ID("RequestCheck", eventidRequestCheck, RequestCheck, VTS_DISPATCH)
    EVENT_CUSTOM_ID("Checked", eventidChecked, Checked, VTS_DISPATCH)
    EVENT_CUSTOM_ID("SelectRanges", eventidSelectRanges, SelectRanges, VTS_DISPATCH)
    EVENT_CUSTOM_ID("ReplaceRanges", eventidReplaceRanges, ReplaceRanges, VTS_DISPATCH)
END_EVENT_MAP()



// Property pages
BEGIN_PROPPAGEIDS(CAcrolinxSidebar, 1)
    PROPPAGEID(CAcrolinxSidebarPropPage::guid)
END_PROPPAGEIDS(CAcrolinxSidebar)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAcrolinxSidebar, "ACROSIDEBAR.AcrolinxSidebar.1",
    0x32245f63, 0x9e1d, 0x412b, 0xa3, 0x61, 0xad, 0x40, 0x85, 0x6c, 0x4b, 0xd9)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CAcrolinxSidebar, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID IID_DAcrolinxSidebarSDK = { 0x3474A21B, 0x4818, 0x4CA2, { 0xAF, 0x38, 0x3F, 0xC9, 0xF5, 0xA9, 0xBE, 0x13 } };
const IID IID_DAcrolinxSidebarSDKEvents = { 0x7A68FB9C, 0x778D, 0x4DC1, { 0xB3, 0xD7, 0x26, 0x3B, 0xA7, 0x67, 0xCC, 0xB2 } };


// Control type information

static const DWORD _dwAcrolinxSidebarSDKOleMisc =
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST |
    OLEMISC_INSIDEOUT |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CAcrolinxSidebar, IDS_ACROLINXSIDEBARSDK, _dwAcrolinxSidebarSDKOleMisc)



// CAcrolinxSidebar::CAcrolinxSidebarFactory::UpdateRegistry -
// Adds or removes system registry entries for CAcrolinxSidebar

BOOL CAcrolinxSidebar::CAcrolinxSidebarFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterControlClass(
        AfxGetInstanceHandle(),
        m_clsid,
        m_lpszProgID,
        IDS_ACROLINXSIDEBARSDK,
        IDB_ACROLINXSIDEBARSDK,
        afxRegInsertable | afxRegApartmentThreading,
        _dwAcrolinxSidebarSDKOleMisc,
        _tlid,
        _wVerMajor,
        _wVerMinor);
    else
        return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CAcrolinxSidebar::CAcrolinxSidebar - Constructor

CAcrolinxSidebar::CAcrolinxSidebar()
    : m_isSidebarStarted(FALSE)
{
    InitializeIIDs(&IID_DAcrolinxSidebarSDK, &IID_DAcrolinxSidebarSDKEvents);
}



// CAcrolinxSidebar::~CAcrolinxSidebar - Destructor

CAcrolinxSidebar::~CAcrolinxSidebar()
{
}



// CAcrolinxSidebar::OnDraw - Drawing function

void CAcrolinxSidebar::OnDraw(
    CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
    if(!m_isSidebarStarted)
    {
        if (!pdc)
        {
            return;
        }
        CBitmap bmpImage;
        bmpImage.LoadBitmapW(IDB_LOADING);
        BITMAP  bi;
        bmpImage.GetBitmap(&bi);
        CDC otherDC;
        otherDC.CreateCompatibleDC(pdc);
        CBitmap* pOldbmp;
        pOldbmp = otherDC.SelectObject(&bmpImage);
        pdc->BitBlt((rcBounds.Width() - bi.bmWidth)/2, (rcBounds.Height() - bi.bmHeight)/2,
            bi.bmWidth,bi.bmHeight,&otherDC,0,0,SRCCOPY);
        otherDC.SelectObject(pOldbmp);
    }
}


// CAcrolinxSidebar::DoPropExchange - Persistence support
void CAcrolinxSidebar::DoPropExchange(CPropExchange* pPX)
{
    ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
    COleControl::DoPropExchange(pPX);

}



// CAcrolinxSidebar::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
DWORD CAcrolinxSidebar::GetControlFlags()
{
    DWORD dwFlags = COleControl::GetControlFlags();

    // The control will not be redrawn when making the transition
    // between the active and inactivate state.
    dwFlags |= noFlickerActivate;
    return dwFlags;
}



// CAcrolinxSidebar::OnResetState - Reset control to default state

void CAcrolinxSidebar::OnResetState()
{
    COleControl::OnResetState();  // Resets defaults found in DoPropExchange

}



// CAcrolinxSidebar message handlers


int CAcrolinxSidebar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (COleControl::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }
    m_sidebarCtrl.SetSidebar(this);
    if(!m_sidebarCtrl.Create(this))
    {
        return -1;
    }

    return 0;
}


void CAcrolinxSidebar::Start(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.Start();
    m_isSidebarStarted = true;
}


void CAcrolinxSidebar::StartWithServerAddress(LPCTSTR serverUrl)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.Start(serverUrl);
    m_isSidebarStarted = true;
}


BSTR CAcrolinxSidebar::GetServerAddress(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult = m_sidebarCtrl.GetServerAddress();

    return strResult.AllocSysString();
}


void CAcrolinxSidebar::SetServerAddress(LPCTSTR serverAddress)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetServerAddress(serverAddress);

    SetModifiedFlag();
}


BSTR CAcrolinxSidebar::GetStartPageSourceLocation(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult = m_sidebarCtrl.GetStartPageSourceLocation();

    return strResult.AllocSysString();
}


void CAcrolinxSidebar::SetStartPageSourceLocation(LPCTSTR startPageDirPath)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetStartPageSourceLocation(startPageDirPath);

    SetModifiedFlag();
}


VARIANT_BOOL CAcrolinxSidebar::GetShowServerSelector(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (VARIANT_BOOL)m_sidebarCtrl.GetShowServerSelector();
}


void CAcrolinxSidebar::SetShowServerSelector(VARIANT_BOOL showServerSelector)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetShowServerSelector(showServerSelector);

    SetModifiedFlag();
}


BSTR CAcrolinxSidebar::GetClientSignature(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult = m_sidebarCtrl.GetClientSignature();

    return strResult.AllocSysString();
}


void CAcrolinxSidebar::SetClientSignature(LPCTSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetClientSignature(newVal);

    SetModifiedFlag();
}


BSTR CAcrolinxSidebar::GetMinimumSidebarVersion(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult = m_sidebarCtrl.GetMinimumSidebarVersion();

    return strResult.AllocSysString();
}


void CAcrolinxSidebar::SetMinimumSidebarVersion(LPCTSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetMinimumSidebarVersion(newVal);

    SetModifiedFlag();
}


VARIANT_BOOL CAcrolinxSidebar::GetReadOnlySuggestions(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return (VARIANT_BOOL)m_sidebarCtrl.GetReadOnlySuggestions();
}


void CAcrolinxSidebar::SetReadOnlySuggestions(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetReadOnlySuggestions(newVal);

    SetModifiedFlag();
}


VARIANT_BOOL CAcrolinxSidebar::GetSupportCheckSelection(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return (VARIANT_BOOL)m_sidebarCtrl.GetSupportCheckSelection();
}


void CAcrolinxSidebar::SetSupportCheckSelection(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetSupportCheckSelection(newVal);

    SetModifiedFlag();
}


BSTR CAcrolinxSidebar::GetClientLocale(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult = m_sidebarCtrl.GetClientLocale();

    return strResult.AllocSysString();
}


void CAcrolinxSidebar::SetClientLocale(LPCTSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl.SetClientLocale(newVal);

    SetModifiedFlag();
}


void CAcrolinxSidebar::LogInfo(LPCTSTR msg)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    LINFO << msg;
}


void CAcrolinxSidebar::LogTrace(LPCTSTR msg)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    LTRACE << msg;
}


void CAcrolinxSidebar::LogError(LPCTSTR msg)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    LERROR << msg;
}


void CAcrolinxSidebar::LogWarn(LPCTSTR msg)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    LWARNING << msg;
}


void CAcrolinxSidebar::RegisterClientComponent(LPCTSTR id, LPCTSTR name, LPCTSTR version, Software_Component_Category category)
{
    m_sidebarCtrl.RegisterClientComponent(id, name, version, category);
}


IDispatch* CAcrolinxSidebar::CreateDocument(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CComObject<CDokument>* document = nullptr;
    HRESULT hRes = CComObject<CDokument>::CreateInstance(&document);

    if(SUCCEEDED(hRes))
    {
        document->AddRef();
    }

    return document;
}


BSTR CAcrolinxSidebar::Check(IDispatch* document)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IDokument* documentObj = nullptr;
    document->QueryInterface(IID_IDokument, (void**)&documentObj);


    CString strResult = m_sidebarCtrl.Check((CDokument*)documentObj);
    documentObj->Release();
    return strResult.AllocSysString();
}


void CAcrolinxSidebar::InvalidateRanges(IDispatch* matches)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IMatches* matchesObj = nullptr;
    matches->QueryInterface(IID_IMatches, (void**)&matchesObj);

    m_sidebarCtrl.InvalidateRanges((CMatches*)matchesObj);
}


IDispatch* CAcrolinxSidebar::CreateRangeList(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CComObject<CRanges>* rangeList = nullptr;
    HRESULT hRes = CComObject<CRanges>::CreateInstance(&rangeList);

    if(SUCCEEDED(hRes))
    {
        rangeList->AddRef();
    }

    return rangeList;
}


void CAcrolinxSidebar::OnDestroy()
{
    COleControl::OnDestroy();

    m_sidebarCtrl.DestroyWindow();
}


BOOL CAcrolinxSidebar::PreTranslateMessage(MSG* pMsg)
{
    if( pMsg->message == WM_KEYDOWN )
    {
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;
        }
    }
    return COleControl::PreTranslateMessage(pMsg);
}


void CAcrolinxSidebar::OnSize(UINT nType, int cx, int cy)
{
    COleControl::OnSize(nType, cx, cy);

    m_sidebarCtrl.AdjustControlSize(cx,cy);
}


BOOL CAcrolinxSidebar::OnSetObjectRects(LPCRECT lpRectPos, LPCRECT lpRectClip)
{
    //Do not reset the object just keep the existing size as it is
    return TRUE;
}
