/* Copyright Acrolinx GmbH */

// ScriptHandler.cpp : Implementation of CScriptHandler

#include "stdafx.h"
#include "ScriptHandler.h"
#include "SidebarControl.h"
#include "LoggerHelper.h"
#include "DllUtil.h"

// CScriptHandler

using namespace Acrolinx_Sdk_Sidebar_Util;

void CScriptHandler::SetMsHtmlDefaults(IOleClientSite* pclientSite)
{
    pclientSite->AddRef();
    m_defaultClientSite = pclientSite;
    m_defaultClientSite->QueryInterface(IID_IDocHostUIHandler, (VOID **)&m_defaultDocHostUIHandler);
}

void CScriptHandler::SetWebBrowser(CWebBrowser* pwebBrowser)
{
    m_webBrowser = pwebBrowser;
}

void CScriptHandler::OnAfterObjectSet(void)
{
    CString log(L"if (!window.console) { window.console = {} }; window.console.logOld = window.console.log; window.console.log = function(msg) { window.external.Log(msg); }");
    injectScript(log);

    CString onerror(L"window.onerror = function(msg, url, line, col, error) { window.external.OnError(msg, url, line, col, error); }");
    injectScript(onerror);

    CString acrolinxStorage("window.acrolinxStorage = { getItem: function(key) { return window.external.getItem(key); }, removeItem: function(key) { window.external.removeItem(key); }, setItem: function(key, data) { window.external.setItem(key, data); } }");
    injectScript(acrolinxStorage);

    CString acrolinxPlugin("window.acrolinxPlugin =   {requestInit: function(){ window.external.requestInit()}, onInitFinished: function(finishResult) {window.external.onInitFinished(JSON.stringify(finishResult))}, configure: function(configuration) { window.external.configure(JSON.stringify(configuration)) }, requestGlobalCheck: function(options) { window.external.requestGlobalCheck(JSON.stringify(options)) }, onCheckResult: function(checkResult) {window.external.onCheckResult(JSON.stringify(checkResult)) }, selectRanges: function(checkId, matches) { window.external.selectRanges(checkId, JSON.stringify(matches))}, replaceRanges: function(checkId, matchesWithReplacements) { window.external.replaceRanges(checkId, JSON.stringify(matchesWithReplacements)) }, download: function(downloadInfo) { window.external.download(JSON.stringify(downloadInfo))}, openWindow: function(openWindowParameters) { window.external.openWindow(JSON.stringify(openWindowParameters)) }, openLogFile: function() {window.external.openLogFile()}}; ");
    injectScript(acrolinxPlugin);
}



ATL::CComVariant CScriptHandler::injectScript(CString script)
{
    DISPID   dispIdEval = 0;
    IDispatchPtr pScriptDisp = nullptr;

    HRESULT hRes = GetScriptDispatch(pScriptDisp);
    if (!SUCCEEDED(hRes) || (nullptr == pScriptDisp)) 
    {
        LOGE << "script dispatch failed: " << Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
        return S_FALSE;
    }

    if (dispIdEval <= 0)
    {
        CComBSTR  fktName(L"eval");
        hRes = pScriptDisp->GetIDsOfNames(IID_NULL, &fktName, 1, LOCALE_USER_DEFAULT, &dispIdEval);
        if (!SUCCEEDED(hRes))
        {
            LOGE << "script dispatch failed to get ids of name: " << Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
            pScriptDisp.Release();
            return S_FALSE;
        }
    }

    ATL::CComVariant retVariant;
    CComVariant varArgs[1];
    varArgs[0] = script.GetString();
    DISPPARAMS dispParams = { &varArgs[0], nullptr, 1, 0 };
    hRes = pScriptDisp->Invoke(dispIdEval, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD,
        &dispParams, &retVariant, nullptr, nullptr);

    pScriptDisp.Release();

    if (!SUCCEEDED(hRes))
    {
        LOGE << "script execution failed: " << Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
        return S_FALSE;
    }

    return retVariant;
}


HRESULT CScriptHandler::GetScriptDispatch(IDispatchPtr& scriptDisp)
{
    scriptDisp = nullptr;

    CComPtr<IDispatch>  pDocDisp;
    pDocDisp = m_webBrowser->get_Document();
    if ((nullptr == pDocDisp))
    {
        LOGE << "Browser has no document";
        return S_FALSE;
    }

    CComPtr<IHTMLDocument2>  pDocHtm;
    HRESULT hRes = pDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDocHtm);
    if (!SUCCEEDED(hRes) || (nullptr == pDocHtm))
    {
        LOGE << "Document has no html/script";
        return S_FALSE;
    }
    else
    {
        hRes = pDocHtm->get_Script(&scriptDisp);
    }
    pDocHtm.Release();
    pDocDisp.Release();

    return hRes;
}


IOleClientSite* CScriptHandler::GetDeafultClientSite(void)
{
    return m_defaultClientSite;
}

void CScriptHandler::SetSidebarControl(CSidebarControl* sidebar)
{
    m_sidebarCtrl = sidebar;
}

STDMETHODIMP CScriptHandler::Log(BSTR logMessage)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    LOGI << "JavaScript Log: "<< CString(logMessage).GetString();
    return S_OK;
}


STDMETHODIMP CScriptHandler::OnError(BSTR msg, BSTR url, BSTR line, BSTR col, BSTR error)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString scriptError(_T("JavaScript Error: "));
    scriptError.Append(msg);
    scriptError.Append(_T(", URl: "));
    scriptError.Append(url);
    scriptError.Append(L", Line: ");
    scriptError.Append(line);
    scriptError.Append(L", Column: ");
    scriptError.Append(col);
    scriptError.Append(L", Error:");
    scriptError.Append(error);

    LOGE << "CScriptHandler::OnError: " << scriptError.GetString();

    return S_OK;
}


STDMETHODIMP CScriptHandler::removeItem(BSTR key)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl->GetStorage()->RemoveItem(key);

    return S_OK;
}


STDMETHODIMP CScriptHandler::setItem(BSTR key, BSTR data)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl->GetStorage()->SetItem(key, data);

    return S_OK;
}


STDMETHODIMP CScriptHandler::getItem(BSTR key, BSTR* data)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl->GetStorage()->GetItem(key, data);

    return S_OK;
}


STDMETHODIMP CScriptHandler::requestInit(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString initParams = m_sidebarCtrl->RequestInit();
    CString initScript = _T("acrolinxSidebar.init(") +initParams + _T(")");
    injectScript(initScript);

    return S_OK;
}


STDMETHODIMP CScriptHandler::onInitFinished(BSTR initResult)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    LOGD << "Init Result: " << CString(initResult).GetString();
    m_sidebarCtrl->FireInitFinished();

    return S_OK;
}


STDMETHODIMP CScriptHandler::configure(BSTR configuration)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ASSERT(&configuration != nullptr);
    LOGI <<"configure: " << CString(configuration).GetString();
    return S_OK;
}


STDMETHODIMP CScriptHandler::requestGlobalCheck(BSTR options)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_sidebarCtrl->FireRequestCheck(options);

    return S_OK;
}


STDMETHODIMP CScriptHandler::onCheckResult(BSTR checkResult, BSTR* retValue)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl->FireChecked(checkResult);

    CString emptyJson(_T("{}"));
    *retValue = emptyJson.AllocSysString();
    return S_OK;
}


STDMETHODIMP CScriptHandler::selectRanges(BSTR checkId, BSTR jsonMatches)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl->FireSelectRanges(checkId, jsonMatches);
    LOGD <<"Selection Matches: " << CString(jsonMatches).GetString();

    return S_OK;
}


STDMETHODIMP CScriptHandler::replaceRanges(BSTR checkId, BSTR jsonMatchesWithReplacements)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl->FireReplaceRanges(checkId, jsonMatchesWithReplacements);
    LOGD <<"Replacement Matches: " << CString(jsonMatchesWithReplacements).GetString();

    return S_OK;
}


STDMETHODIMP CScriptHandler::download(BSTR downloadInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ASSERT(&downloadInfo != nullptr);

    LOGI << "downloadInfo: " << CString(downloadInfo).GetString();

    return S_OK;
}


STDMETHODIMP CScriptHandler::openWindow(BSTR url)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_sidebarCtrl->OpenWindow(url);

    return S_OK;
}


STDMETHODIMP CScriptHandler::openLogFile(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString logPath = Acrolinx_Sdk_Sidebar_Util::LoggerHelper::GetLogFileName();

    if (!logPath.IsEmpty())
    {
        CString strArgs;
        strArgs = _T("/select,\"");
        strArgs += logPath + _T("\"");
        ShellExecute(0, _T("open"), _T("explorer.exe"), strArgs.GetString(), 0, SW_NORMAL);
    }
    else
    {
        LOGE << "Log file path is empty";
    }

    return S_OK;
}


CString CScriptHandler::Check(CString content, CString reference, CString format, CString selectionRanges)
{
    m_documentContent = content;
    reference.Replace(_T("\\"), _T("\\\\"));
    reference.Replace(_T("'"), _T("\\'"));
    reference.Replace(_T("\n"), _T(""));
    reference.Replace(_T("\r"), _T(""));

    CString code = _T("new function(){var c = window.external.getContent(); return acrolinxSidebar.checkGlobal(c, {inputFormat:'") + format + _T("', requestDescription:{documentReference: '")
        + reference + _T("'}, selection:{ranges: "+ selectionRanges +"}})}();");


    ATL::CComVariant result = injectScript(code);
    if(result.vt == VT_BSTR)
    {
        return result.bstrVal;
    }

    m_documentContent = _T("");
    return CString();
}


STDMETHODIMP CScriptHandler::getContent(BSTR* content)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *content = m_documentContent.AllocSysString();

    return S_OK;
}


void CScriptHandler::InvalidateRanges(CString matchesJson)
{
    CString code = _T("new function(){ return acrolinxSidebar.invalidateRanges(") + matchesJson + _T(")}();");
    injectScript(code);
}
