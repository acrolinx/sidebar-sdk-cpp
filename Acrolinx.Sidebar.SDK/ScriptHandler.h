/* Copyright Acrolinx GmbH */

// ScriptHandler.h : Declaration of the CScriptHandler

#pragma once
#include "resource.h"       // main symbols
#include "Acrolinx.Sidebar.SDK_i.h"

using namespace ATL;

class CSidebarControl;
// CScriptHandler

class ATL_NO_VTABLE CScriptHandler :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IOleClientSite,
    public IDocHostUIHandler,
    public IDispatchImpl<IScriptHandler, &IID_IScriptHandler, &LIBID_AcrolinxSidebarSDKLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CScriptHandler()
        : m_defaultDocHostUIHandler(nullptr)
        , m_defaultClientSite(nullptr)
        , m_sidebarCtrl(nullptr)
        , m_documentContent(_T(""))
    {
    }


    BEGIN_COM_MAP(CScriptHandler)
        COM_INTERFACE_ENTRY(IOleClientSite)
        COM_INTERFACE_ENTRY(IScriptHandler)
        COM_INTERFACE_ENTRY(IDocHostUIHandler)
        COM_INTERFACE_ENTRY(IDispatch)

    END_COM_MAP()



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
        if (m_defaultClientSite != nullptr)
        {
            m_defaultClientSite->Release();
            m_defaultClientSite = nullptr;
        }

        if (m_defaultDocHostUIHandler != nullptr)
        {
            m_defaultDocHostUIHandler->Release();
            m_defaultDocHostUIHandler = nullptr;
        }

        m_documentContent = _T("");

        m_sidebarCtrl = nullptr;
    }

// IDocHostUIHandlerDispatch Methods
public:
    STDMETHOD(GetExternal)(IDispatch **ppDispatch) {
        return GetUnknown()->QueryInterface(IID_IScriptHandler, (void**)ppDispatch);
    };
    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown*, IDispatch*) { return S_OK; };
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo) { return S_OK; };
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject*, IOleCommandTarget*,
        IOleInPlaceFrame*, IOleInPlaceUIWindow* pDoc) {
            return S_OK;
    };
    STDMETHOD(HideUI)() { return S_OK; };
    STDMETHOD(UpdateUI)() { return S_OK; };
    STDMETHOD(EnableModeless)(BOOL fEnable) { return S_OK; };
    STDMETHOD(OnDocWindowActivate)(BOOL fActivate) { return S_OK; };
    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate) { return S_OK; };
    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) { return S_OK; };
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID) { return S_OK; };
    STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw) { return S_OK; };
    STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) { return S_OK; };
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) { return S_OK; };
    STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet) { return S_OK; };


    // ole client site impl
    STDMETHOD(SaveObject)() { return S_OK; };
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) { return S_OK; };
    STDMETHOD(GetContainer)(IOleContainer**ppContainer) { return S_OK; };
    STDMETHOD(ShowObject)() { return S_OK; };
    STDMETHOD(OnShowWindow)(BOOL fShow) { return S_OK; };
    STDMETHOD(RequestNewObjectLayout)() { return S_OK; };
private:
    IDocHostUIHandler* m_defaultDocHostUIHandler;
    IOleClientSite* m_defaultClientSite;
    CSidebarControl* m_sidebarCtrl;

public:
    void OnAfterObjectSet(void);
    IOleClientSite* GetDeafultClientSite(void);
    void SetSidebarControl(CSidebarControl* sidebar);
    CString Check(CString content, CString reference, CString format, CString selectionRanges);
    void InvalidateRanges(CString matchesJson);

public:
    STDMETHOD(Log)(BSTR logMessage);
    STDMETHOD(OnError)(BSTR msg, BSTR url, BSTR line, BSTR col, BSTR error);
    STDMETHOD(getItem)(BSTR key, BSTR* data);
    STDMETHOD(removeItem)(BSTR key);
    STDMETHOD(setItem)(BSTR key, BSTR data);
    STDMETHOD(requestInit)(void);
    STDMETHOD(onInitFinished)(BSTR initResult);
    STDMETHOD(configure)(BSTR configuration);
    STDMETHOD(requestGlobalCheck)(BSTR options);
    STDMETHOD(onCheckResult)(BSTR checkResult, BSTR* retValue);
    STDMETHOD(selectRanges)(BSTR checkId, BSTR jsonMatches);
    STDMETHOD(replaceRanges)(BSTR checkId, BSTR jsonMatchesWithReplacements);
    STDMETHOD(download)(BSTR downloadInfo);
    STDMETHOD(openWindow)(BSTR url);
    STDMETHOD(openLogFile)(void);
    STDMETHOD(getContent)(BSTR* content);
private:
    // This is extracted content fetched from IDokument
    CString m_documentContent;
};
