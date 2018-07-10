/* Copyright Acrolinx GmbH */

// Matches.h : Declaration of the CMatches

#pragma once
#include "resource.h"       // main symbols

#include "Acrolinx.Sidebar.SDK_i.h"
#include <atlcoll.h>
#include "Match.h"

using namespace ATL;

// CMatches

class ATL_NO_VTABLE CMatches :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CMatches, &CLSID_Matches>,
    public IDispatchImpl<IMatches, &IID_IMatches, &LIBID_AcrolinxSidebarSDKLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public CAtlArray<CComObject<CMatch>*>
{
public:
    CMatches()
        : m_checkId(_T(""))
    {
    }


    BEGIN_COM_MAP(CMatches)
        COM_INTERFACE_ENTRY(IMatches)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
        while(GetCount() > 0)
        {
            IMatch* match = GetAt(0);
            RemoveAt(0);
            match->Release();
        }
    }

public:
    STDMETHOD(GetCheckId)(BSTR* checkId);
    STDMETHOD(SetCheckId)(BSTR checkId);
    STDMETHOD(GetMatchCount)(LONG* count);
    STDMETHOD(GetMatchAt)(LONG index, IMatch** match);

private:
    CString m_checkId;
};

