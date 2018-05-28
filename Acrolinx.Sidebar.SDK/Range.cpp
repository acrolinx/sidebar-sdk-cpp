/* Copyright (c) 2018 Acrolinx GmbH */

#include "StdAfx.h"
#include "Range.h"
#include "easylogging++.h"

CRange::CRange(void)
    : m_start(0)
    , m_end(0)
{
}

STDMETHODIMP CRange::GetEnd(LONG* endOffset)
{
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Range object by calling InitInstance() before using it");
    *endOffset = m_end;
    return S_OK;
}

STDMETHODIMP CRange::GetStart(LONG* startOffset)
{
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Range object by calling InitInstance() before using it");

    *startOffset = m_start;
    return S_OK;

}

STDMETHODIMP CRange::GetLength(LONG* length)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Range object by calling InitInstance() before using it");

    if(m_end < m_start)
    {
        LERROR<< "Someting is wrong - range is not set properly";
        return S_FALSE;
    }

    *length = m_end - m_start;
    return S_OK;
}

STDMETHODIMP CRange::InitInstance(LONG start, LONG end)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(start <= end, "wrong range");

    if(!Instantiate())
    {
        return S_FALSE;
    }

    m_start = start;
    m_end = end;
    LTRACE << "range [" << start << "," << end << "]";
    return S_OK;
}
