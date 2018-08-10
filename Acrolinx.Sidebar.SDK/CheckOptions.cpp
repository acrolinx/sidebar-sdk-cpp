/* Copyright Acrolinx GmbH */

// CheckOptions.cpp : Implementation of CCheckOptions

#include "stdafx.h"
#include "CheckOptions.h"
#include "easylogging++.h"

// CCheckOptions

using namespace Acrolinx_Sdk_Sidebar_Util;

STDMETHODIMP CCheckOptions::GetSelection(VARIANT_BOOL* selection)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize Checkoptions object by calling InitInstance() before using it");

    BOOL retVal = m_checkOptions.HasMember(_T("selection")) ? m_checkOptions[_T("selection")].GetBool() : FALSE;

    if(selection == nullptr)
    {
        LERROR <<  "Create out bool var before calling GetSelection()";
    }
    else
    {
        *selection = (VARIANT_BOOL)retVal;
    }
    return S_OK;
}


STDMETHODIMP CCheckOptions::InitInstance(BSTR checkOptions)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(!Instantiate())
    {
        return S_FALSE;
    }

    ASSERT(checkOptions != nullptr);
    CString jsonString(checkOptions);
    if( jsonString.IsEmpty())
    {
        jsonString = _T("{}");
    }
    CJsonUtil::Parse(jsonString, m_checkOptions);

    return S_OK;
}
