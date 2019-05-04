/* Copyright Acrolinx GmbH */

#include "StdAfx.h"
#include "AcrolinxInterface.h"
#include "Dokument.h"
#include "Match.h"
#include "Matches.h"

CMultiAdapter::CMultiAdapter(CString docReference, CString tagName, Input_Format format)
    : m_docReference(docReference)
    , m_format(format)
    , m_tagName(tagName)
{
    AddStartTag();
}


CMultiAdapter::~CMultiAdapter(void)
{
    m_adapterMap.clear();
    m_adapterList.clear();
}

void CMultiAdapter::AddStartTag()
{
    if(m_format != IF_TEXT || !m_tagName.IsEmpty())
    {
        m_extractedContent = _T("<") + m_tagName + _T(">");
    }
    else
    {
        m_extractedContent = _T("");
    }
}

void CMultiAdapter::RegisterAdapter(CUniversalAdapter* adapter)
{
    if(adapter != nullptr)
    {
        m_adapterList.push_back(adapter);
    }
    else
    {
        LOGE << "Adapter is not valid";
    }
}

void CMultiAdapter::RegisterAdapter(CArray<CUniversalAdapter*>* adapters)
{
    for(int idx = 0; idx < adapters->GetCount(); idx++)
    {
        RegisterAdapter(adapters->GetAt(idx));
    }
}


IDokument* CMultiAdapter::GetDocument(void)
{
    CComObject<CDokument>* document = nullptr;
    HRESULT hRes = CComObject<CDokument>::CreateInstance(&document);

    if(SUCCEEDED(hRes))
    {
        CString content  = m_extractedContent + _T("/n/n") ;
        if(m_format != IF_TEXT || !m_tagName.IsEmpty())
        {
            content = m_extractedContent + _T("</") + m_tagName + _T(">");
        }

        document->InitInstance(content.AllocSysString(), m_docReference.AllocSysString(), m_format);
    }
    return document;
}


void CMultiAdapter::Extract(void)
{
    AddStartTag();
    AdapterList::iterator listItr = m_adapterList.begin();
    m_adapterMap.clear();
    while(listItr != m_adapterList.end())
    {
        LONG startOffset = m_extractedContent.GetLength();
        CString content = (*listItr)->Extract(m_format);
        m_adapterMap[startOffset] = (*listItr);
        (*listItr)->SetRelativePosition(startOffset);
        m_extractedContent.Append(content);
        listItr++;
    }
}


void CMultiAdapter::SelectRanges(IMatches* matches)
{
    AdapterMap::iterator adapterItr = FindMapElement(matches);
    adapterItr->second->SelectRanges(matches, m_format);
}


void CMultiAdapter::ReplaceRanges(IMatches* matches)
{
    AdapterMap::iterator adapterItr = FindMapElement(matches);
    adapterItr->second->ReplaceRanges(matches, m_format);
}


// Returns pair of offset and adapter
AdapterMap::iterator CMultiAdapter::FindMapElement(IMatches* matches)
{
    ASSERT(matches != nullptr);

    IMatch* match = nullptr;
    matches->GetMatchAt(0, &match);

    IRange* range = nullptr;
    match->GetRange(&range);

    LONG matchStart = 0;
    range->GetStart(&matchStart);

    AdapterMap::iterator findElement = m_adapterMap.upper_bound(matchStart);

    range->Release();
    match->Release();

    if(findElement == m_adapterMap.begin())
    {
        return findElement;
    }
    return std::prev(findElement);
}
