/* Copyright Acrolinx GmbH */

#pragma once
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\document.h"
#include "rapidjson\pointer.h"

typedef rapidjson::GenericDocument<rapidjson::UTF16<>> WDocument;
typedef rapidjson::GenericValue<rapidjson::UTF16<>> WValue;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF16<>> WStringBuffer;
typedef rapidjson::Writer<WStringBuffer,rapidjson::UTF16<>, rapidjson::UTF16<>> WWriter;
typedef rapidjson::GenericPointer<WValue> WPointer;

namespace Acrolinx_Sdk_Sidebar_Util
{
    class CJsonUtil
    {
    public:
        CJsonUtil(void);
        ~CJsonUtil(void);
        static CString Stringify(const WDocument& json);
        static void Parse(const CString& jsonString, WDocument& json);
        static void SetString(WDocument& json, const CString& domKey, const CString& value);
        static void SetInt(WDocument& json, const CString& domKey, const int& value);
        static void SetBool(WDocument& json, const CString& domKey, const bool& value);
    };
}
