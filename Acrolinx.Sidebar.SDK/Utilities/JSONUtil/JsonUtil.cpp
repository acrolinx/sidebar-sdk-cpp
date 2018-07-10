/* Copyright Acrolinx GmbH */

#include "StdAfx.h"
#include "JsonUtil.h"

Acrolinx_Sdk_Sidebar_Util::CJsonUtil::CJsonUtil(void)
{
}


Acrolinx_Sdk_Sidebar_Util::CJsonUtil::~CJsonUtil(void)
{
}


CString Acrolinx_Sdk_Sidebar_Util::CJsonUtil::Stringify(const WDocument& json)
{
    ASSERT(&json != nullptr);
    WStringBuffer sb;
    WWriter writer(sb);
    if (json.Accept(writer))
    {
        return CString(sb.GetString());
    }
    TRACE(_T("Strigify failed"));
    return CString();
}


void Acrolinx_Sdk_Sidebar_Util::CJsonUtil::Parse(const CString& jsonString, WDocument& json)
{
    ASSERT(&json != nullptr);
    ASSERT(&jsonString != nullptr);
    json.Parse(jsonString.GetString());
}


void Acrolinx_Sdk_Sidebar_Util::CJsonUtil::SetString(WDocument& json, const CString& domKey, const CString& value)
{
    ASSERT(&json != nullptr);
    ASSERT(&domKey != nullptr);
    ASSERT(&value != nullptr);

    WPointer(domKey.GetString()).Set(json,value.GetString());
}

void Acrolinx_Sdk_Sidebar_Util::CJsonUtil::SetInt(WDocument& json, const CString& domKey, const int& value)
{
    ASSERT(&json != nullptr);
    ASSERT(&domKey != nullptr);
    ASSERT(&value != nullptr);

    WPointer(domKey.GetString()).Set(json,value);
}

void Acrolinx_Sdk_Sidebar_Util::CJsonUtil::SetBool(WDocument& json, const CString& domKey, const bool& value)
{
    ASSERT(&json != nullptr);
    ASSERT(&domKey != nullptr);
    ASSERT(&value != nullptr);

    WPointer(domKey.GetString()).Set(json,value);
}
