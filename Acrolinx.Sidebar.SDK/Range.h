/* Copyright Acrolinx GmbH */

#pragma once
#include "resource.h"       // main symbols
#include "JsonUtil.h"
#include "Acrolinx.Sidebar.SDK_i.h"
#include "COMInstanceMngr.h"

using namespace ATL;

class CRange : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CRange, &CLSID_Range>,
    public IDispatchImpl<IRange, &__uuidof(IRange), &LIBID_AcrolinxSidebarSDKLib, /* wMajor = */ 1, /* wMinor = */ 0>,
    public CCOMInstanceMngr
{
public:
    CRange(void);

    BEGIN_COM_MAP(CRange)
        COM_INTERFACE_ENTRY(IRange)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

    void FinalRelease()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_RANGE)
private:
    LONG m_end;
    LONG m_start;

// IRange Methods
public:
    STDMETHOD(GetEnd)(LONG* endOffset);
    STDMETHOD(GetStart)(LONG* startOffset);
    STDMETHOD(GetLength)(LONG* length);
    STDMETHOD(InitInstance)(LONG start, LONG end);
};

OBJECT_ENTRY_AUTO(__uuidof(Range), CRange)