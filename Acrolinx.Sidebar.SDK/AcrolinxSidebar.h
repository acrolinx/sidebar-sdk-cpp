/* Copyright (c) 2018 Acrolinx GmbH */

#pragma once
#include "sidebarcontrol.h"

// AcrolinxSidebar.h : Declaration of the CAcrolinxSidebar ActiveX Control class.


// CAcrolinxSidebar : See AcrolinxSidebar.cpp for implementation.

class CAcrolinxSidebar : public COleControl
{
    DECLARE_DYNCREATE(CAcrolinxSidebar)

// Constructor
public:
    CAcrolinxSidebar();

// Overrides
public:
    virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
    virtual void DoPropExchange(CPropExchange* pPX);
    virtual void OnResetState();
    virtual DWORD GetControlFlags();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnSetObjectRects(LPCRECT lpRectPos, LPCRECT lpRectClip);

// Implementation
protected:
    ~CAcrolinxSidebar();

    DECLARE_OLECREATE_EX(CAcrolinxSidebar)    // Class factory and guid
    DECLARE_OLETYPELIB(CAcrolinxSidebar)      // GetTypeInfo
    DECLARE_PROPPAGEIDS(CAcrolinxSidebar)     // Property page IDs
    DECLARE_OLECTLTYPE(CAcrolinxSidebar)		// Type name and misc status

    // Message maps
    DECLARE_MESSAGE_MAP()

    // Dispatch maps
    DECLARE_DISPATCH_MAP()

    // Event maps
    DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
    enum {
        dispidCreateRangeList = 112L,
        dispidRegisterClientComponent = 111L,
        dispidInvalidateRanges = 110L,
        eventidReplaceRanges = 6L,
        eventidSelectRanges = 5L,
        eventidChecked = 4L,
        dispidCheck = 109L,
        dispidCreateDocument = 108L,
        eventidRequestCheck = 3L,
        dispidLogWarn = 106L,
        dispidLogError = 105L,
        dispidLogTrace = 104L,
        dispidLogInfo = 103L,
        eventidInitFinished = 2L,
        eventidSidebarLoaded = 1L,
        dispidClientLocale = 207,
        dispidSupportCheckSelection = 206,
        dispidReadOnlySuggestions = 205,
        dispidMinimumSidebarVersion = 204,
        dispidClientSignature = 203,
        dispidShowServerSelector = 202,
        dispidStartPageSourceLocation = 301,
        dispidServerAddress = 201,
        dispidStartWithServerAddress = 102L,
        dispidStart = 101L
    };
private:
    CSidebarControl m_sidebarCtrl;
    BOOL m_isSidebarStarted;

protected:
    void Start(void);
    void StartWithServerAddress(LPCTSTR serverUrl);
    BSTR GetServerAddress(void);
    void SetServerAddress(LPCTSTR serverAddress);
    BSTR GetStartPageSourceLocation(void);
    void SetStartPageSourceLocation(LPCTSTR startPageDirPath);
    VARIANT_BOOL GetShowServerSelector(void);
    void SetShowServerSelector(VARIANT_BOOL showServerSelector);
    BSTR GetClientSignature(void);
    void SetClientSignature(LPCTSTR newVal);
    BSTR GetMinimumSidebarVersion(void);
    void SetMinimumSidebarVersion(LPCTSTR newVal);
    VARIANT_BOOL GetReadOnlySuggestions(void);
    void SetReadOnlySuggestions(VARIANT_BOOL newVal);
    VARIANT_BOOL GetSupportCheckSelection(void);
    void SetSupportCheckSelection(VARIANT_BOOL newVal);
    BSTR GetClientLocale(void);
    void SetClientLocale(LPCTSTR newVal);
public:
    void SidebarLoaded(LPCTSTR url)
    {
        FireEvent(eventidSidebarLoaded, EVENT_PARAM(VTS_BSTR), url);
    }
    void InitFinished(void)
    {
        FireEvent(eventidInitFinished, EVENT_PARAM(VTS_NONE));
    }
    void RequestCheck(IDispatch* checkOptions)
    {
        FireEvent(eventidRequestCheck, EVENT_PARAM(VTS_DISPATCH), checkOptions);
    }
    void Checked(IDispatch* checkResult)
    {
        FireEvent(eventidChecked, EVENT_PARAM(VTS_DISPATCH), checkResult);
    }
    void SelectRanges(IDispatch* matches)
    {
        FireEvent(eventidSelectRanges, EVENT_PARAM(VTS_DISPATCH), matches);
    }
    void ReplaceRanges(IDispatch* matches)
    {
        FireEvent(eventidReplaceRanges, EVENT_PARAM(VTS_DISPATCH), matches);
    }

protected:
    HRESULT LogInit(LPCTSTR categoryName);
    void LogInfo(LPCTSTR msg);
    void LogTrace(LPCTSTR msg);
    void LogError(LPCTSTR msg);
    void LogWarn(LPCTSTR msg);
    void LogShutdown(void);
    IDispatch* CreateDocument(void);
    BSTR Check(IDispatch* document);
    void InvalidateRanges(IDispatch* matches);
    void RegisterClientComponent(LPCTSTR id, LPCTSTR name, LPCTSTR version, Software_Component_Category category);
    IDispatch* CreateRangeList(void);
};

