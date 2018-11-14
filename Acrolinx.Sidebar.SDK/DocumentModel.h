/* Copyright Acrolinx GmbH */

// DocumentModel.h : Declaration of the CDocumentModel

#pragma once
#include "Acrolinx.Sidebar.SDK_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include "COMInstanceMngr.h"

using namespace ATL;



// CDocumentModel

class ATL_NO_VTABLE CDocumentModel :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CDocumentModel, &CLSID_DocumentModel>,
    public IDispatchImpl<IDocumentModel, &IID_IDocumentModel, &LIBID_AcrolinxSidebarSDKLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public CCOMInstanceMngr
{
public:
    CDocumentModel()
        : m_originalContent(_T(""))
        , m_currentContent(_T(""))
    {
    }

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
        m_originalContent = "";
        m_currentContent = "";
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_DOCUMENTMODEL)

    DECLARE_NOT_AGGREGATABLE(CDocumentModel)

    BEGIN_COM_MAP(CDocumentModel)
        COM_INTERFACE_ENTRY(IDocumentModel)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

private:
    CString m_originalContent;
    CString m_currentContent;
// IDocumentModel
public:
    STDMETHOD(InitInstance)(BSTR originalContent);
    STDMETHOD(GetOriginalContent)(BSTR* originalContent);
    STDMETHOD(GetCurrentContent)(BSTR* currentContent);
    STDMETHOD(SetCurrentContent)(BSTR currentContent);
    STDMETHOD(Update)(IRange* originalRange, BSTR replacement);
    STDMETHOD(GetOriginalToCurrentRange)(IRange* originalRange, IRange** currentRange);
private:
    HRESULT MatchSurface(IRange* originalRange, IRange* currentRange);
};

OBJECT_ENTRY_AUTO(__uuidof(DocumentModel), CDocumentModel)
