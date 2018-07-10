/* Copyright Acrolinx GmbH */

// CheckResult.h : Declaration of the CCheckResult

#pragma once
#include "resource.h"       // main symbols
#include "JsonUtil.h"
#include "Acrolinx.Sidebar.SDK_i.h"
#include "COMInstanceMngr.h"

using namespace ATL;


// CCheckResult

class ATL_NO_VTABLE CCheckResult :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CCheckResult, &CLSID_CheckResult>,
    public IDispatchImpl<ICheckResult, &IID_ICheckResult, &LIBID_AcrolinxSidebarSDKLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public CCOMInstanceMngr
{
public:
    CCheckResult()
    {
    }


    BEGIN_COM_MAP(CCheckResult)
        COM_INTERFACE_ENTRY(ICheckResult)
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

    STDMETHOD(GetCheckId)(BSTR* checkId);
    STDMETHOD(GetEmbedCheckInformation)(BSTR* embedCheckInformation);
    STDMETHOD(GetRange)(IRange** range);
private:
    WDocument m_checkResult;
public:
    BOOL InitInstance(CString checkResult);
};

