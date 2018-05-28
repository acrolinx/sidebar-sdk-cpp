// Ranges.h : Declaration of the CRanges

#pragma once
#include "resource.h"       // main symbols
#include "Acrolinx.Sidebar.SDK_i.h"
#include <atlcoll.h>
#include "Range.h"

using namespace ATL;


// CRanges

class ATL_NO_VTABLE CRanges :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRanges, &CLSID_Ranges>,
	public IDispatchImpl<IRanges, &IID_IRanges, &LIBID_AcrolinxSidebarSDKLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public CAtlArray<CComObject<CRange>*>
{
public:
	CRanges()
	{
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_RANGES)


BEGIN_COM_MAP(CRanges)
	COM_INTERFACE_ENTRY(IRanges)
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
            IRange* range = GetAt(0);
            RemoveAt(0);
            range->Release();
        }
	}

public:



    STDMETHOD(GetRangeAt)(LONG index, IRange** range);
    STDMETHOD(GetRangeCount)(LONG* count);
    STDMETHOD(AddRange)(LONG start, LONG end);
};

//OBJECT_ENTRY_AUTO(__uuidof(Ranges), CRanges)
