/* Copyright Acrolinx GmbH */

#include "StdAfx.h"
#include <regex>
#include "AcrolinxInterface.h"
#include "Range.h"
#include "DocumentModel.h"
#include "Dokument.h"


CUniversalAdapter::CUniversalAdapter(void)
    : m_tagName("x")
    , m_encodeContent(FALSE)
    , m_relativePosition(0)
    , m_documentModel(nullptr)
    , m_document(nullptr)
{

}


CUniversalAdapter::CUniversalAdapter(CString tagName, BOOL encodeContent)
    : m_tagName(tagName)
    , m_encodeContent(encodeContent)
    , m_relativePosition(0)
    , m_documentModel(nullptr)
    , m_document(nullptr)
{
}

CUniversalAdapter::~CUniversalAdapter(void)
{
    if(m_document != nullptr)
    {
        m_document->Release();
    }
    m_document = nullptr;
    if(m_documentModel != nullptr)
    {
        m_documentModel->Release();
    }
    m_documentModel = nullptr;
}


void CUniversalAdapter::Reset(CString content, CString documentReference, Input_Format format)
{
    if(m_documentModel != nullptr)
    {
        m_documentModel->Release();
    }
    CComObject<CDocumentModel >* documentModel = nullptr;
    CComObject<CDocumentModel>::CreateInstance(&documentModel);
    documentModel->AddRef();
    m_documentModel = documentModel;

    m_documentModel->InitInstance(content.AllocSysString());

    if(m_document != nullptr)
    {
        m_document->Release();
    }
    CComObject<CDokument>* document = nullptr;
    CComObject<CDokument>::CreateInstance(&document);
    document->AddRef();
    m_document = document;

    m_document->InitInstance(content.AllocSysString(), documentReference.AllocSysString(), format);
}

IDokument* CUniversalAdapter::GetDocument()
{
    return m_document;
}


CString CUniversalAdapter::Extract(Input_Format format, CString documentReference, BOOL resetModel)
{
    CString rawContent = ExtractRaw();

    if(m_encodeContent && format != IF_TEXT)
    {
        MSXML::IXMLDOMDocument2Ptr xmlDoc;
        HRESULT hr = xmlDoc.CreateInstance(__uuidof(DOMDocument));
        if(!SUCCEEDED(hr))
        {
            LOGE << "Could not create XML document.";
            return CString();
        }

        MSXML::IXMLDOMElementPtr element = xmlDoc->createElement(m_tagName.AllocSysString());
        element->Puttext(rawContent.AllocSysString());

        rawContent = element->xml.GetBSTR();
    }

    if(resetModel)
    {
        Reset(rawContent, documentReference, format);
    }
    return rawContent;
}


void CUniversalAdapter::SelectRanges(IMatches* matches, Input_Format format)
{
    ACROASSERT(matches != nullptr, "matches object is not valid");

    CString currentExtraction =  Extract(format, _T(""), FALSE);
    m_documentModel->SetCurrentContent(currentExtraction.AllocSysString());

    LONG matchCount = 0;
    matches->GetMatchCount(&matchCount);

    IMatch* startMatch = nullptr;
    IMatch* endMatch = nullptr;

    matches->GetMatchAt(0, &startMatch);
    matches->GetMatchAt(matchCount - 1, &endMatch);
    IRange* startRange = nullptr;
    IRange* endRange = nullptr;

    startMatch->GetRange(&startRange);
    endMatch->GetRange(&endRange);

    IRange* adapterStartRange = ConvertToAdapterRange(startRange);
    IRange* adapterEndRange = ConvertToAdapterRange(endRange);

    IRange* currentStartRange = nullptr;
    HRESULT hRes = m_documentModel->GetOriginalToCurrentRange(adapterStartRange, &currentStartRange);
    if(hRes == S_OK)
    {
        IRange* currentEndRange = nullptr;
        hRes = m_documentModel->GetOriginalToCurrentRange(adapterEndRange, &currentEndRange);
        if(hRes == S_OK)
        {
            CComObject<CRange>* currentRangeInstance = nullptr;
            CComObject<CRange>::CreateInstance(&currentRangeInstance);
            currentRangeInstance->AddRef();

            LONG startOffset = 0, endOffset = 0;
            currentStartRange->GetStart(&startOffset);
            currentEndRange->GetEnd(&endOffset);

            currentRangeInstance->InitInstance(startOffset, endOffset);


            IRange* rawRange = nullptr;
            ToRawRange(currentRangeInstance, &rawRange, format);

            SelectRawRange(rawRange, format);

            rawRange->Release();
            currentRangeInstance->Release();

            currentEndRange->Release();
        }
        currentStartRange->Release();
    }

    startMatch->Release();
    endMatch->Release();

    adapterEndRange->Release();
    adapterStartRange->Release();

    startRange->Release();
    endRange->Release();

    if(hRes == S_FALSE)
    {
        throw std::runtime_error("Lookup failed- Maybe user changed the source document");
    }
}


void CUniversalAdapter::ReplaceRanges(IMatches* matches, Input_Format format)
{
    ACROASSERT(matches != nullptr, "matches object is not valid");

    CString currentExtraction = Extract(format, _T(""), FALSE);
    m_documentModel->SetCurrentContent(currentExtraction.AllocSysString());

    LONG matchCount = 0;
    matches->GetMatchCount(&matchCount);

    for(int idx = matchCount - 1; idx >= 0 ; idx --)
    {
        IMatch* matchToUpdate = nullptr;
        matches->GetMatchAt(idx, &matchToUpdate);

        IRange* relativeRange = nullptr;
        matchToUpdate->GetRange(&relativeRange);

        IRange* adapterRange = ConvertToAdapterRange(relativeRange);

        IRange* currentRange = nullptr;
        HRESULT hRes = m_documentModel->GetOriginalToCurrentRange(adapterRange, &currentRange);
        if(hRes == S_OK)
        {
            IRange* rawRange;
            ToRawRange(currentRange, &rawRange, format);

            _bstr_t replacmentContent;
            matchToUpdate->GetMatchReplacement(replacmentContent.GetAddress());
            ReplaceRawRange(rawRange, replacmentContent.GetBSTR(), format);

            m_documentModel->Update(adapterRange, replacmentContent);

            rawRange->Release();
            currentRange->Release();
        }
        adapterRange->Release();
        relativeRange->Release();
        matchToUpdate->Release();
        if(hRes == S_FALSE)
        {
            throw std::runtime_error("Lookup failed- Maybe user changed the source document");
        }
    }
}



void CUniversalAdapter::ToRawRange(IRange* currentRange, IRange** rawRange, Input_Format format)
{
    if(m_encodeContent && format != IF_TEXT)
    {
        LONG startOffset = 0, endOffset = 0;
        currentRange->GetStart(&startOffset);
        currentRange->GetEnd(&endOffset);

        _bstr_t currentContent;
        m_documentModel->GetCurrentContent(currentContent.GetAddress());
        CString currentStr(currentContent.GetBSTR());
        LONG tagOffsetAdjustment = m_tagName.GetLength() + 2;
        LONG startDiff =  DecodeLength(currentStr.Left(startOffset));
        LONG lengthDiff = DecodeLength(currentStr.Mid(startOffset, endOffset - startOffset));

        startOffset = startOffset - startDiff - tagOffsetAdjustment;
        endOffset = endOffset - (startDiff + lengthDiff) - tagOffsetAdjustment;

        CComObject<CRange>* rawRangeInstance = nullptr;
        CComObject<CRange>::CreateInstance(&rawRangeInstance);
        rawRangeInstance->AddRef();

        rawRangeInstance->InitInstance(startOffset, endOffset);
        *rawRange = rawRangeInstance;
    }
    else
    {
        currentRange->AddRef();
        *rawRange = currentRange;
    }
}


LONG CUniversalAdapter::DecodeLength(CString content)
{
    LONG encodedContentLength = 0;
    const LONG entitySize = 1;
    std::wstring contentForMatch = content.GetString();
    std::wregex findXmlEntities(L"&\\w+?;");
    auto entityIterator = std::wsregex_iterator(contentForMatch.cbegin(), contentForMatch.cend(), findXmlEntities);
    auto entityEnd = std::wsregex_iterator();

    while (entityIterator != entityEnd)
    {
        encodedContentLength += entityIterator->str().length() - entitySize ;
        ++entityIterator;
    }

    return encodedContentLength;
}


void CUniversalAdapter::SetRelativePosition(LONG relativePosition)
{
    ASSERT(relativePosition >= 0);

    m_relativePosition = relativePosition;
}

IRange* CUniversalAdapter::ConvertToAdapterRange(IRange* relativeRange)
{
    CComObject<CRange>* adapterRange = nullptr;

    CComObject<CRange>::CreateInstance(&adapterRange);
    adapterRange->AddRef();

    LONG startOffset = 0, endOffset = 0;
    relativeRange->GetStart(&startOffset);
    relativeRange->GetEnd(&endOffset);

    adapterRange->InitInstance(startOffset - m_relativePosition, endOffset - m_relativePosition);

    return adapterRange;

}
