/* Copyright Acrolinx GmbH */

// Dokument.cpp : Implementation of CDokument

#include "stdafx.h"
#include "Dokument.h"

// CDokument



STDMETHODIMP CDokument::GetContent(BSTR* content)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Document object by calling InitInstance() before using it");

    *content = m_content.AllocSysString();

    return S_OK;
}


STDMETHODIMP CDokument::GetReference(BSTR* reference)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Document object by calling InitInstance() before using it");

    *reference = m_reference.AllocSysString();

    return S_OK;
}


STDMETHODIMP CDokument::GetFormat(Input_Format* format)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Document object by calling InitInstance() before using it");

    *format = m_format;

    return S_OK;
}


STDMETHODIMP CDokument::InitInstance(BSTR content, BSTR reference, Input_Format inputFormat)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(!Instantiate())
    {
        return S_FALSE;
    }

    SetDocument(content, reference, inputFormat, nullptr);

    return S_OK;
}


CString CDokument::GetFormatAsString(void)
{
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Document object by calling InitInstance() before using it");

    if(m_format == IF_XML)
        return CString(_T("XML"));
    else if (m_format == IF_HTML)
        return CString(_T("HTML"));
    else if (m_format == IF_MARKDOWN)
        return CString(_T("MARKDOWN"));
    else if (m_format == IF_TEXT)
        return CString(_T("TEXT"));
    else if (m_format == IF_WORD_XML)
        return CString(_T("WORD_XML"));
    else if (m_format == IF_AUTO)
        return CString(_T("AUTO"));

    return CString(_T("AUTO"));    
}


STDMETHODIMP CDokument::InitInstanceWithSelectionRanges(BSTR content, BSTR reference, Input_Format inputFormat, IRanges* selectionRanges)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(!Instantiate())
    {
        return S_FALSE;
    }

    ACROASSERT(selectionRanges != nullptr, "Selection ranges object is not valid");

    SetDocument(content, reference, inputFormat, selectionRanges);

    return S_OK;
}


void CDokument::SetDocument(BSTR content, BSTR reference, Input_Format inputFormat, IRanges* selectionRanges)
{
    ACROASSERT(inputFormat >= IF_XML && inputFormat <= IF_AUTO, "You are trying to set input format which is not supported.");
    ACROASSERT(m_selectionRanges == nullptr, "You have already called SetDocument(). Use the instance you have created.");


    if(m_selectionRanges != nullptr)
    {
        m_selectionRanges->Release();
        m_selectionRanges = nullptr;
    }

    m_content = content;
    LOGD << "Content: " << m_content.GetString();
    m_reference = reference;
    LOGD << "Reference: " << m_reference.GetString();
    m_format = inputFormat;
    LOGD << "Format: " << GetFormatAsString().GetString();
    m_selectionRanges = selectionRanges;
}


IRanges* CDokument::GetSelectionRanges(void)
{
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Document object by calling InitInstance() before using it");
    return m_selectionRanges;
}
