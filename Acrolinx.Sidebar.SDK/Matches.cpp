/* Copyright Acrolinx GmbH */

// Matches.cpp : Implementation of CMatches

#include "stdafx.h"
#include "Matches.h"

// CMatches


STDMETHODIMP CMatches::GetCheckId(BSTR* checkId)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(checkId == nullptr)
    {
        LOGE << "Create string before calling GetCheckId()";
    }
    else
    {
        *checkId = m_checkId.AllocSysString();
    }

    return S_OK;
}

STDMETHODIMP CMatches::SetCheckId(BSTR checkId)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_checkId = checkId;

    return S_OK;
}


STDMETHODIMP CMatches::GetMatchCount(LONG* count)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(count != nullptr, "It seems count pointer is not valid");

    *count = this->GetCount();

    return S_OK;
}


STDMETHODIMP CMatches::GetMatchAt(LONG index, IMatch** match)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    LOGD << "Populating match";
    this->GetAt(index)->AddRef();
    *match = this->GetAt(index);

    return S_OK;
}
