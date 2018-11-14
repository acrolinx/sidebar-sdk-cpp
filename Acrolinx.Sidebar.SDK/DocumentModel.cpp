/* Copyright Acrolinx GmbH */

// DocumentModel.cpp : Implementation of CDocumentModel

#include "stdafx.h"
#include "DocumentModel.h"
#include "easylogging++.h"
#include "Range.h"
#include "DiffMatchPatchWorker.h"

// CDocumentModel

STDMETHODIMP CDocumentModel::InitInstance(BSTR originalContent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(!Instantiate())
    {
        return S_FALSE;
    }

    ASSERT(originalContent != nullptr);
    m_originalContent = originalContent;
    m_currentContent = originalContent;

    LTRACE << "Original Content: "<< m_originalContent.GetString();

    return S_OK;
}


STDMETHODIMP CDocumentModel::GetOriginalContent(BSTR* originalContent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize DocumentModel object by calling InitInstance() before using it");

    *originalContent = m_originalContent.AllocSysString();

    return S_OK;
}


STDMETHODIMP CDocumentModel::GetCurrentContent(BSTR* currentContent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize DocumentModel object by calling InitInstance() before using it");

    *currentContent = m_currentContent.AllocSysString();

    return S_OK;
}


STDMETHODIMP CDocumentModel::Update(IRange* originalRange, BSTR replacement)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize DocumentModel object by calling InitInstance() before using it");
    ACROASSERT(originalRange != nullptr, "original range is not valid");
    ACROASSERT(replacement != nullptr, "replacement string is not valid");

    LONG start = 0, end = 0, length = 0;
    originalRange->GetStart(&start);
    ASSERT( start >= 0);
    originalRange->GetLength(&length);
    ASSERT( length >= 0);
    originalRange->GetEnd(&end);
    ASSERT( end <= m_originalContent.GetLength());

    IRange* relativeRange = nullptr;
    HRESULT hRes = GetOriginalToCurrentRange(originalRange, &relativeRange);
    if(hRes == S_OK)
    {
        relativeRange->GetStart(&start);
        relativeRange->GetLength(&length);

        m_currentContent.Delete(start, length);
        m_currentContent.Insert(start, replacement);
        LTRACE << "New Content: "<< m_currentContent.GetString();
        relativeRange->Release();
    }
    return hRes;
}


STDMETHODIMP CDocumentModel::GetOriginalToCurrentRange(IRange* originalRange, IRange** currentRange)
{
    ASSERT(*currentRange == nullptr);
#ifdef _PERFORMANCE
    clock_t tStart = clock();
#endif

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize DocumentModel object by calling InitInstance() before using it");

    CComObject<CRange>* currentRangeInstance = nullptr;
    CComObject<CRange>::CreateInstance(&currentRangeInstance);    
    currentRangeInstance->AddRef();

    CDiffMatchPatchWorker dmpWorker;
    dmpWorker.LookupMatches(originalRange, currentRangeInstance, m_originalContent, m_currentContent);

    HRESULT hRes = MatchSurface(originalRange, currentRangeInstance);
    if(hRes == S_OK)
    {
        *currentRange = currentRangeInstance;
    }
    else
    {
        currentRangeInstance->Release();
    }
#ifdef _PERFORMANCE
    CString log;
    log.Format(_T("Range conversion time : %.2fs"), (double)(clock() - tStart) / CLOCKS_PER_SEC);
    LINFO << log.GetString();
#endif

    return hRes;
}

HRESULT CDocumentModel::MatchSurface(IRange* originalRange, IRange* currentRange)
{
    LONG start = 0, length = 0;
    originalRange->GetStart(&start);
    originalRange->GetLength(&length);
    CString orgSurface = m_originalContent.Mid(start, length);

    start = length = 0;
    currentRange->GetStart(&start);
    currentRange->GetLength(&length);

    CString newSurface = m_currentContent.Mid(start, length);

    LTRACE << "Org surface: " << orgSurface.GetString()  << " New surface: " <<newSurface.GetString();

    return (newSurface == orgSurface) ? S_OK : S_FALSE;
}

STDMETHODIMP CDocumentModel::SetCurrentContent(BSTR currentContent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_currentContent = currentContent;
    LTRACE << "New Content: "<< m_currentContent.GetString();

    return S_OK;
}
