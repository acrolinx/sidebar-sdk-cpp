// Ranges.cpp : Implementation of CRanges

#include "stdafx.h"
#include "Ranges.h"
#include "easylogging++.h"


// CRanges



STDMETHODIMP CRanges::GetRangeAt(LONG index, IRange** range)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    LTRACE << "Populating range";
    this->GetAt(index)->AddRef();
    *range = this->GetAt(index);

    return S_OK;
}


STDMETHODIMP CRanges::GetRangeCount(LONG* count)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ASSERT(count != nullptr);

    *count = this->GetCount();

    return S_OK;
}


STDMETHODIMP CRanges::AddRange(LONG start, LONG end)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CComObject<CRange>* range = nullptr;
    CComObject<CRange>::CreateInstance(&range);
    range->AddRef();

    range->InitInstance(start, end);
    this->Add(range);
    LTRACE << "Added range [" << start << "," << end << "]";
    return S_OK;
}
