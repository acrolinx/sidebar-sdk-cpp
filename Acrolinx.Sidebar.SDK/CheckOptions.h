/* Copyright (c) 2018 Acrolinx GmbH */

// CheckOptions.h : Declaration of the CCheckOptions

#pragma once
#include "resource.h"       // main symbols
#include "JsonUtil.h"
#include "COMInstanceMngr.h"
#include "Acrolinx.Sidebar.SDK_i.h"

using namespace ATL;


// CCheckOptions

class ATL_NO_VTABLE CCheckOptions :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CCheckOptions, &CLSID_CheckOptions>,
    public IDispatchImpl<ICheckOptions, &IID_ICheckOptions, &LIBID_AcrolinxSidebarSDKLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public CCOMInstanceMngr
{
public:
    CCheckOptions()
    {
    }


    BEGIN_COM_MAP(CCheckOptions)
        COM_INTERFACE_ENTRY(ICheckOptions)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

public:
    STDMETHOD(GetSelection)(VARIANT_BOOL* selection);
    STDMETHOD(InitInstance)(BSTR checkOptions);
private:
    WDocument m_checkOptions;
};

