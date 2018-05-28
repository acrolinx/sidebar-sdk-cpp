/* Copyright (c) 2018 Acrolinx GmbH */

#pragma once
#include "Acrolinx.Sidebar.SDK_i.h"
#include "Range.h"
#include "COMInstanceMngr.h"

using namespace ATL;

class CMatch :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IDispatchImpl<IMatch, &__uuidof(IMatch), &LIBID_AcrolinxSidebarSDKLib, /* wMajor = */ 1, /* wMinor = */ 0>,
    public CCOMInstanceMngr
{
public:
    CMatch(void);

    BEGIN_COM_MAP(CMatch)
        COM_INTERFACE_ENTRY(IMatch)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()


    void FinalRelease()
    {
        m_range->Release();
        m_extractedRange->Release();
    }

// IMatch Methods
public:
    STDMETHOD(GetRange)(IRange** range);
    STDMETHOD(InitInstance)(BSTR content, BSTR replacement, IRange* range, IRange* extractedRange);
    STDMETHOD(GetMatchContent)(BSTR* content);
    STDMETHOD(GetMatchReplacement)(BSTR* replacement);
    STDMETHOD(GetExtractedRange)(IRange** extractedRange);

private:
    CString m_content;
    CString m_replacement;
    CComObject<CRange>* m_range;
    CComObject<CRange>* m_extractedRange;

};

