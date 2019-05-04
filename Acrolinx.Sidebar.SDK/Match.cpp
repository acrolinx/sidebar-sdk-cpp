/* Copyright Acrolinx GmbH */

#include "StdAfx.h"
#include "Match.h"

CMatch::CMatch(void)
    : m_content(CString())
    , m_replacement(CString())
    , m_range(nullptr)
    , m_extractedRange(nullptr)
{
}


STDMETHODIMP CMatch::GetRange(IRange** range)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Match object by calling InitInstance() before using it");
    ACROASSERT(range != nullptr, "It seems range object is not valid");

    this->m_range->AddRef();
    *range = this->m_range;

    return S_OK;
}

STDMETHODIMP CMatch::InitInstance(BSTR content, BSTR replacement, IRange* range, IRange* extractedRange)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(!Instantiate())
    {
        return S_FALSE;
    }

    ASSERT(&content != nullptr);
    ASSERT(&replacement != nullptr);
    ASSERT(range != nullptr);
    ASSERT(extractedRange != nullptr);

    m_content = content;
    m_replacement = replacement;
    m_range = (CComObject<CRange>*)range;
    m_extractedRange = (CComObject<CRange>*)extractedRange;

    return S_OK;
}

STDMETHODIMP CMatch::GetMatchContent(BSTR* content)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Match object by calling InitInstance() before using it");
    ACROASSERT(content != nullptr, "It seems content string is not valid");

    *content = m_content.AllocSysString();

    return S_OK;
}


STDMETHODIMP CMatch::GetMatchReplacement(BSTR* replacement)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Match object by calling InitInstance() before using it");
    ACROASSERT(replacement != nullptr, "It seems replacement string is not valid");

    *replacement = m_replacement.AllocSysString();

    return S_OK;
}


STDMETHODIMP CMatch::GetExtractedRange(IRange** extractedRange)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Match object by calling InitInstance() before using it");
    ACROASSERT(extractedRange != nullptr, "It seems replacement string is not valid");

    this->m_extractedRange->AddRef();
    *extractedRange = this->m_extractedRange;

    return S_OK;
}
