/* Copyright Acrolinx GmbH */

// Dokument.h : Declaration of the CDokument

#pragma once
#include "resource.h"       // main symbols
#include "Acrolinx.Sidebar.SDK_i.h"
#include "COMInstanceMngr.h"

using namespace ATL;


// CDokument

class ATL_NO_VTABLE CDokument :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CDokument, &CLSID_Dokument>,
    public IDispatchImpl<IDokument, &IID_IDokument, &LIBID_AcrolinxSidebarSDKLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public CCOMInstanceMngr
{
public:
    CDokument()
        : m_content(_T(""))
        , m_reference(_T(""))
        , m_format(IF_AUTO)
        , m_selectionRanges(nullptr)
    {
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_DOKUMENT)


    BEGIN_COM_MAP(CDokument)
        COM_INTERFACE_ENTRY(IDokument)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
        m_content = "";
    }

public:
    STDMETHOD(GetContent)(BSTR* content);
    STDMETHOD(GetReference)(BSTR* reference);
    STDMETHOD(GetFormat)(Input_Format* format);
    STDMETHOD(InitInstance)(BSTR content, BSTR reference, Input_Format inputFormat);
    STDMETHOD(InitInstanceWithSelectionRanges)(BSTR content, BSTR reference, Input_Format inputFormat, IRanges* selectionRanges);
private:
    CString m_content;
    CString m_reference;
    Input_Format m_format;
    IRanges* m_selectionRanges;
public:
    CString GetFormatAsString(void);
    IRanges* GetSelectionRanges(void);
private:
    void SetDocument(BSTR content, BSTR reference, Input_Format inputFormat, IRanges* selectionRanges);
};

OBJECT_ENTRY_AUTO(__uuidof(Dokument), CDokument)
