/* Copyright (c) 2018 Acrolinx GmbH */

#pragma once

#include<map>
#include<vector>

#ifdef ACROLINX_SDK_EXPORTS
#include "Acrolinx.Sidebar.SDK_i.h"
#define ACROLINX_SDK_API __declspec(dllexport)
#else
#define ACROLINX_SDK_API __declspec(dllimport)
#endif


class ACROLINX_SDK_API CUniversalAdapter
{
public:
    CUniversalAdapter(void);
    CUniversalAdapter(CString tagName, BOOL encodeContent);
    virtual ~CUniversalAdapter(void);

    CString Extract(Input_Format format, CString documentReference = _T(""), BOOL resetModel = TRUE);
    void SelectRanges(IMatches* matches, Input_Format format);
    void ReplaceRanges(IMatches* matches, Input_Format format);

    virtual CString ExtractRaw() = 0;
    virtual BOOL SelectRawRange(IRange* rawRange, Input_Format format) = 0;
    virtual BOOL ReplaceRawRange(IRange* rawRange, CString replacement, Input_Format format) = 0;

    IDokument* GetDocument();
private:
    CString m_tagName;
    BOOL m_encodeContent;
    IDocumentModel* m_documentModel;
    IDokument* m_document;
    LONG m_relativePosition;
private:
    IRange* ConvertToAdapterRange(IRange* relativeRange);
    void ToRawRange(IRange* currentRange, IRange** rawRange, Input_Format format);
    LONG DecodeLength(CString content);
    void Reset(CString content, CString documentReference, Input_Format format);
public:
    void SetRelativePosition(LONG relativePosition);
};


typedef std::map<LONG, CUniversalAdapter*> AdapterMap;
typedef std::vector<CUniversalAdapter*> AdapterList;
class ACROLINX_SDK_API CMultiAdapter
{
public:
    CMultiAdapter(CString docReference, CString tagName, Input_Format format);
    ~CMultiAdapter(void);
    void RegisterAdapter(CUniversalAdapter* adapter);
    void RegisterAdapter(CArray<CUniversalAdapter*>* adapters);
    void Extract(void);
    void SelectRanges(IMatches* matches);
    void ReplaceRanges(IMatches* matches);
    IDokument* GetDocument(void);
private:
    void AddStartTag();
    // Returns pair of offset and adpter
    AdapterMap::iterator FindMapElement(IMatches* matches);
private:
    AdapterMap m_adapterMap;
    AdapterList m_adapterList;
    CString m_extractedContent;
    CString m_docReference;
    Input_Format m_format;
    CString m_tagName;
};
