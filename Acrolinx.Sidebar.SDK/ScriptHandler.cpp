/* Copyright Acrolinx GmbH */

// ScriptHandler.cpp : Implementation of CScriptHandler

#include "stdafx.h"
#include "ScriptHandler.h"
#include "SidebarControl.h"
#include "LoggerHelper.h"
#include "DllUtil.h"

// CScriptHandler

using namespace Acrolinx_Sdk_Sidebar_Util;

void CScriptHandler::OnAfterObjectSet(void)
{
    CString log(L"window.bridge = chrome.webview.hostObjects.bridge; if (!window.console) { window.console = {} }; window.console.logOld = window.console.log; window.console.log = function(msg) { window.bridge.Log(msg); }");
	m_sidebarCtrl->Eval(log);
   
    CString onerror(L"window.bridge = chrome.webview.hostObjects.bridge; window.onerror = function(msg, url, line, col, error) { window.bridge.OnError(msg, url, line, col, error); }");
	m_sidebarCtrl->Eval(onerror);

    BSTR registryStorage;;
    m_sidebarCtrl->GetStorage()->GetAllItems(&registryStorage);

    // Memory based asynchronous local Storage
    CString memoryStorage = CString(L"");
    memoryStorage.Append(L"{window.bridge = chrome.webview.hostObjects.bridge; ");
    memoryStorage.Append(L"window.acrolinxStorage = { ");
    memoryStorage.Append(L"memoryStorage: new Map(Object.entries(");
    memoryStorage.Append(registryStorage);
    memoryStorage.Append(L")), ");
    memoryStorage.Append(L"getItem: function(key) { return this.memoryStorage.get(key); }, ");
    memoryStorage.Append(L"removeItem: async function(key) {  this.memoryStorage.delete(key); await window.bridge.removeItem(key); }, ");
    memoryStorage.Append(L"setItem: async function(key, data) {  this.memoryStorage.set(key, data); await window.bridge.setItem(key, data); } } }");

    m_sidebarCtrl->Eval(memoryStorage);

    CString acrolinxPlugin("{window.bridge = chrome.webview.hostObjects.bridge; window.acrolinxPlugin =   {requestInit: function(){ window.bridge.requestInit()}, onInitFinished: function(finishResult) {window.bridge.onInitFinished(JSON.stringify(finishResult))}, configure: function(configuration) { window.bridge.configure(JSON.stringify(configuration)) }, requestGlobalCheck: function(options) { window.bridge.requestGlobalCheck(JSON.stringify(options)) }, onCheckResult: function(checkResult) {window.bridge.onCheckResult(JSON.stringify(checkResult)) }, selectRanges: function(checkId, matches) { setTimeout(() => { window.bridge.selectRanges(checkId, JSON.stringify(matches)) }, 10); }, replaceRanges: function(checkId, matchesWithReplacements) { window.bridge.replaceRanges(checkId, JSON.stringify(matchesWithReplacements)) }, download: function(downloadInfo) { window.bridge.download(JSON.stringify(downloadInfo))}, openWindow: function(openWindowParameters) { window.bridge.openWindow(JSON.stringify(openWindowParameters)) }, openLogFile: function() {window.bridge.openLogFile()}}; }");
	m_sidebarCtrl->Eval(acrolinxPlugin);
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
	m_sidebarCtrl->Eval(initScript);

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


    CString code = CString();
    code.Append(L"(async()=>{window.bridge = chrome.webview.hostObjects.bridge;");
    code.Append(L" var c = await window.bridge.getContent(); ");
    code.Append(L"return acrolinxSidebar.checkGlobal(c, {inputFormat:'");
    code.Append(format);
    code.Append(L"', requestDescription:{documentReference: '");
    code.Append(reference);
    code.Append(L"'}, selection:{ranges:");
    code.Append(selectionRanges);
    code.Append(L"}})})();");

    m_sidebarCtrl->Eval(code);
    return CString();
}


STDMETHODIMP CScriptHandler::getContent(BSTR* content)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *content = m_documentContent.AllocSysString();

    m_documentContent = _T("");
    return S_OK;
}


void CScriptHandler::InvalidateRanges(CString matchesJson)
{
    CString code = _T("new function(){ return acrolinxSidebar.invalidateRanges(") + matchesJson + _T(")}();");
    m_sidebarCtrl->Eval(code);
}
