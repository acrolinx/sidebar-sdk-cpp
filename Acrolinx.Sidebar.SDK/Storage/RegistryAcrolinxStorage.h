/* Copyright Acrolinx GmbH */

#pragma once

#include "..\Acrolinx.Sidebar.SDK_i.h"
#include "JsonUtil.h"

using namespace ATL;

class CRegistryAcrolinxStorage : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public IAcrolinxStorage
{
public:
    CRegistryAcrolinxStorage(void);

    BEGIN_COM_MAP(CRegistryAcrolinxStorage)
        COM_INTERFACE_ENTRY(IUnknown)
    END_COM_MAP()

    virtual ~CRegistryAcrolinxStorage(void);

    STDMETHOD(GetItem)(BSTR key, BSTR* data);
    STDMETHOD(RemoveItem)(BSTR key);
    STDMETHOD(SetItem)(BSTR key, BSTR data);
	STDMETHOD(GetAllItems)(BSTR* data);
    

private:
    const CString keyPath;
};

