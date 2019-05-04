/* Copyright Acrolinx GmbH */

// CheckResult.cpp : Implementation of CCheckResult

#include "stdafx.h"
#include "CheckResult.h"
#include "Range.h"

// CCheckResult

using namespace Acrolinx_Sdk_Sidebar_Util;

STDMETHODIMP CCheckResult::GetCheckId(BSTR* checkId)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize CheckResult object by calling InitInstance() before using it");

    CString retval = m_checkResult[_T("checkedPart")][_T("checkId")].GetString();
    if(checkId == nullptr)
    {
        LOGE << "Create string before calling GetCheckId()";
    }
    else
    {
        *checkId = retval.AllocSysString();
    }

    return S_OK;
}


STDMETHODIMP CCheckResult::GetEmbedCheckInformation(BSTR* embedCheckInformation)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize CheckResult object by calling InitInstance() before using it");

    BOOL isValid = m_checkResult.HasMember(_T("embedCheckInformation"))?TRUE:FALSE;
    if(isValid)
    {
        WDocument retDom;
        retDom.Set(m_checkResult[_T("embedCheckInformation")].GetArray());
        CString retval = CJsonUtil::Stringify(retDom);
        if(embedCheckInformation != nullptr)
        {
            *embedCheckInformation = retval.AllocSysString();
        }
    }
    else
    {
        LOGE << "embedCheckInformation isn't present";
    }

    return S_OK;
}


STDMETHODIMP CCheckResult::GetRange(IRange** range)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    ACROASSERT(m_isInstanceCreated == TRUE, "Initialize CheckResult object by calling InitInstance() before using it");

    int start = m_checkResult[_T("checkedPart")][_T("range")][0].GetInt();
    int end = m_checkResult[_T("checkedPart")][_T("range")][1].GetInt();
    if(*range == nullptr)
    {
        CComObject<CRange>* rangeObj = nullptr;
        HRESULT hRes = CComObject<CRange>::CreateInstance(&rangeObj);
        if (SUCCEEDED(hRes))
        {
            rangeObj->AddRef();
            rangeObj->InitInstance(start, end);
            *range = rangeObj;
            LOGD << "Creating range [" << start << "," << end << "]";
        }
        else
        {
            LOGE << "Creating range failed";
        }
    }
    else
    {
        LOGD << "Populating range [" << start << "," << end << "]";
        (*range)->InitInstance(start, end);
    }

    return S_OK;
}


BOOL CCheckResult::InitInstance(CString checkResult)
{
    ASSERT(&checkResult != nullptr);

    if(!Instantiate())
    {
        return S_FALSE;
    }

    if( checkResult.IsEmpty())
    {
        LOGD << "checkResult is empty";
        checkResult = _T("{}");
    }
    CJsonUtil::Parse(checkResult, m_checkResult);

    return S_OK;
}
