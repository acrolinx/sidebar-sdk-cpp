/* Copyright (c) 2018 Acrolinx GmbH */

// SidebarControl.cpp : implementation file
//

#include "stdafx.h"
#include "Acrolinx.Sidebar.SDK.h"
#include "SidebarControl.h"
#include "afxdialogex.h"
#include "RegistryAcrolinxStorage.h"
#include "AcrolinxSidebar.h"
#include "DllUtil.h"
#include "CheckOptions.h"
#include "CheckResult.h"
#include "Range.h"
#include "LoggerHelper.h"
#include "easylogging++.h"

using namespace Acrolinx_Sdk_Sidebar_Util;

// CSidebarControl dialog

IMPLEMENT_DYNAMIC(CSidebarControl, CDialogEx)

CSidebarControl::CSidebarControl(CWnd* pParent /*=NULL*/)
    : CDialogEx(CSidebarControl::IDD, pParent)
    , m_startPageSourceLocation(_T(""))
    , m_acrolinxStorage(NULL)
    , m_initParameters()
    , m_sidebar(NULL)
    , m_clientComponents(0)
    , m_isMainCategorySet(FALSE)
    , m_scriptHandler(nullptr)
{
    SetShowServerSelector(TRUE);
    SetServerAddress(_T(""));
    SetClientSignature(_T(""));
    SetReadOnlySuggestions(FALSE);
    SetClientLocale(_T(""));
    CJsonUtil::SetString(m_initParameters, _T("/logFileLocation"), LoggerHelper::GetLogFileName());
    SetMinimumSidebarVersion(_T("14.5.0"));
    SetSupportCheckSelection(false);
}

CSidebarControl::~CSidebarControl()
{
    if(m_acrolinxStorage)
    {
        m_acrolinxStorage->Release();
        m_acrolinxStorage = NULL;
    }
    m_sidebar = NULL;
    //Logger::LogShutdown();
}

void CSidebarControl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LABEL, m_label);
    DDX_Control(pDX, IDC_WEB_BROWSER, m_webBrowser);
}


BEGIN_MESSAGE_MAP(CSidebarControl, CDialogEx)
END_MESSAGE_MAP()


// CSidebarControl message handlers


BOOL CSidebarControl::Create(CWnd* pParent)
{
    return CDialogEx::Create(IDD_ACROLINX_SIDEBAR, pParent);
}


void CSidebarControl::Start(CString serverAddress)
{
    ACROASSERT(!GetClientSignature().IsEmpty(), "You do not have specified a client signature. Please ask Acrolinx for a client signature and set the client signature via acrolinxSidebar.SetClientSignature().");
    SetDefaults(serverAddress);

    CString startPageURL = GetStartPageURL();
    if(startPageURL.IsEmpty())
    {
        m_label.SetWindowText(_T("\n\n\nOops, something went wrong with loading the Sidebar. Check the log file for any errors."));
        UpdateWindow();
        return;
    }

    VARIANT url;
    url.vt = VT_BSTR;
    url.bstrVal =  startPageURL.AllocSysString();
    m_webBrowser.Navigate2(&url,NULL,NULL,NULL,NULL);

    m_label.ShowWindow(SW_HIDE);
    UpdateWindow();
}


CString CSidebarControl::GetStartPageURL(void)
{
    CString startPageURL(m_startPageSourceLocation);
    if(m_startPageSourceLocation.IsEmpty())
    {
        WCHAR szPath[MAX_PATH] = {0};
        if(!GetModuleFileName(GetModuleHandle(_T("Acrolinx.Sidebar.SDK.dll")), szPath, sizeof(szPath)/sizeof(szPath[0])))
        {
            LERROR << "Cannot find path, error: " <<  Acrolinx_Sdk_Sidebar_Util::DllUtil::GetLastErrorAsString().GetString();
            return CString();
        }
        startPageURL = szPath;
        if(startPageURL.Replace(_T("Acrolinx.Sidebar.SDK.dll"), _T("Acrolinx.Startpage.dll"))==0)
        {
            LERROR << "Path may be wrong, replacement failed: " << startPageURL.GetString();
            return CString();
        }
    }

    if(PathFileExists(startPageURL))
    {
        startPageURL.Insert(0,_T("res://"));
        startPageURL.Append(_T("//index.html"));
    }
    else
    {
        LERROR << "Startpage url is not correctly build: "<< startPageURL.GetString() << ". May be Acrolinx.Startpage.dll is missing from the path";
        return CString();
    }

    return startPageURL;
}

void CSidebarControl::SetServerAddress(CString serverAddress)
{
    CJsonUtil::SetString(m_initParameters, _T("/serverAddress"), serverAddress);
}


CString CSidebarControl::GetServerAddress(void)
{
    return CString(m_initParameters[_T("serverAddress")].GetString());
}


void CSidebarControl::SetStartPageSourceLocation(CString startPageDirPath)
{
    m_startPageSourceLocation = startPageDirPath;
}


CString CSidebarControl::GetStartPageSourceLocation(void)
{
    return m_startPageSourceLocation;
}


void CSidebarControl::SetShowServerSelector(BOOL showServerSelector)
{
    CJsonUtil::SetBool(m_initParameters, _T("/showServerSelector"),(showServerSelector?true:false));
}


BOOL CSidebarControl::GetShowServerSelector(void)
{
    return BOOL(m_initParameters[_T("showServerSelector")].GetBool());
}


void CSidebarControl::SetDefaults(CString serverAddress)
{
    ShowHideServerSelectorIfServerAddressParameterSet(serverAddress);

    //ClientLocale
    if(GetClientLocale().IsEmpty())
    {
        LANGID defaultLang = GetUserDefaultUILanguage();

        WCHAR strNameBuffer[LOCALE_NAME_MAX_LENGTH] = {0};
        if (LCIDToLocaleName(defaultLang, strNameBuffer, LOCALE_NAME_MAX_LENGTH, 0) == 0)
        {
            LERROR << DllUtil::GetLastErrorAsString().GetString();
        }
        else
        {
            CString locale(strNameBuffer);
            int position = 0;
            locale = locale.Tokenize(_T("-"), position);
            SetClientLocale(locale);
            LINFO << "Default locale is set " << locale.GetString();
        }
    }

    //Default Storage
    if(m_acrolinxStorage == NULL)
    {
        CComObject<CRegistryAcrolinxStorage>* instance;
        CComObject<CRegistryAcrolinxStorage>::CreateInstance(&instance);
        instance->AddRef();
        m_acrolinxStorage = instance;
    }

    //Default Client components
    RegisterComponents();
}


void CSidebarControl::ShowHideServerSelectorIfServerAddressParameterSet(CString serverAddress)
{
    if (!serverAddress.IsEmpty())
    {
        SetServerAddress(serverAddress);
        SetShowServerSelector(FALSE);
    }
    else if(GetServerAddress().IsEmpty())
    {
        SetShowServerSelector(TRUE);
    }

}
BEGIN_EVENTSINK_MAP(CSidebarControl, CDialogEx)
    ON_EVENT(CSidebarControl, IDC_WEB_BROWSER, 259, CSidebarControl::DocumentCompleteWebBrowser, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()


void CSidebarControl::DocumentCompleteWebBrowser(LPDISPATCH pDisp, VARIANT* URL)
{
    CComPtr<IDispatch> document = m_webBrowser.get_Document();
    if (document != nullptr && m_scriptHandler == nullptr)
    {
        CComPtr<IOleObject> oleObject = nullptr;
        HRESULT hRes = document->QueryInterface(IID_IOleObject, (void**)&oleObject);
        if (!SUCCEEDED(hRes))
        {
            LERROR << "Could not find OleObject";
            return;
        }
        // Get the new scriptHandler
        hRes = CComObject<CScriptHandler>::CreateInstance(&m_scriptHandler);
        if (!SUCCEEDED(hRes))
        {
            LERROR << "CreateInstance script handle instance failed";
            return;
        }

        CComPtr<IOleClientSite> clientSite = nullptr;
        hRes = oleObject->GetClientSite(&clientSite);
        if (!SUCCEEDED(hRes) || nullptr == clientSite)
        {
            LERROR << "Could not find client site";
            return;            
        }

        m_scriptHandler->SetMsHtmlDefaults(clientSite);
        clientSite.Release();

        oleObject->SetClientSite(nullptr);
        hRes = oleObject->SetClientSite(m_scriptHandler);
        if (!SUCCEEDED(hRes))
        {
            LERROR << "Client site setup failed";
            return;
        }
        oleObject.Release();
        m_scriptHandler->SetSidebarControl(this);
        m_scriptHandler->SetWebBrowser(&m_webBrowser);
        m_scriptHandler->OnAfterObjectSet();

        m_sidebar->SidebarLoaded(URL->bstrVal);

        AdjustZoomFactor();
    }
    document.Release();
}


BOOL CSidebarControl::DestroyWindow()
{
    HRESULT hRes = S_FALSE;
    if(m_scriptHandler)
    {
        CComPtr<IDispatch> document = m_webBrowser.get_Document();
        if (document != nullptr)
        {
            CComPtr<IOleObject> oleObject = nullptr;
            hRes = document->QueryInterface(IID_IOleObject, (void**)&oleObject);
            if (!SUCCEEDED(hRes))
            {
                LERROR << "Could not find OleObject";
                return hRes;
            }
            IOleClientSite* clientsite;
            oleObject->GetClientSite(&clientsite);

            hRes = oleObject->SetClientSite(nullptr);
            hRes = oleObject->SetClientSite(m_scriptHandler->GetDeafultClientSite());
            if (!SUCCEEDED(hRes))
            {
                LERROR << "Client site setup failed even after retries";
                return hRes;
            }
            oleObject.Release();
        }
        m_scriptHandler->Release();
        document.Release();
    }
    return CDialog::DestroyWindow();
}


void CSidebarControl::SetStorage(IAcrolinxStorage* storage)
{
    ACROASSERT(m_acrolinxStorage == NULL, "Look like you have already loaded sidebar. Set storage before starting sidebar.");

    if(m_acrolinxStorage == NULL)
    {
        m_acrolinxStorage = storage;
    }
}


IAcrolinxStorage* CSidebarControl::GetStorage(void)
{
    return m_acrolinxStorage;
}


CString CSidebarControl::RequestInit(void)
{
    return CJsonUtil::Stringify(m_initParameters);
}


void CSidebarControl::SetClientLocale(CString locale)
{
    CJsonUtil::SetString(m_initParameters, _T("/clientLocale"), locale);
}


void CSidebarControl::SetClientSignature(CString signature)
{
    CJsonUtil::SetString(m_initParameters, _T("/clientSignature"), signature);
}


void CSidebarControl::SetMinimumSidebarVersion(CString version)
{
    CJsonUtil::SetString(m_initParameters, _T("/minimumSidebarVersion"), version);
}


void CSidebarControl::SetReadOnlySuggestions(BOOL isReadOnly)
{
    CJsonUtil::SetBool(m_initParameters, _T("/readOnlySuggestions"), (isReadOnly?true:false));
}


void CSidebarControl::SetSupportCheckSelection(BOOL isCheckSelectionSupported)
{
    CJsonUtil::SetBool(m_initParameters, _T("/supported/checkSelection"), (isCheckSelectionSupported?true:false));
}


CString CSidebarControl::GetClientSignature(void)
{
    return CString(m_initParameters[_T("clientSignature")].GetString());
}


CString CSidebarControl::GetMinimumSidebarVersion(void)
{
    return CString(m_initParameters[_T("minimumSidebarVersion")].GetString());
}


BOOL CSidebarControl::GetReadOnlySuggestions(void)
{
    return BOOL(m_initParameters[_T("readOnlySuggestions")].GetBool());
}


BOOL CSidebarControl::GetSupportCheckSelection(void)
{
    return BOOL(m_initParameters[_T("supported")][_T("checkSelection")].GetBool());
}


CString CSidebarControl::GetClientLocale(void)
{
    return CString(m_initParameters[_T("clientLocale")].GetString());
}


void CSidebarControl::AdjustControlSize(long width, long height)
{
    this->SetWindowPos(NULL, 0, 0, width, height, SWP_NOZORDER);
    m_webBrowser.put_Width(width);
    m_webBrowser.put_Height(height);
    IDispatchPtr document = m_webBrowser.get_Document();
    if (document != nullptr)
    {
        AdjustZoomFactor();
    }
}


void CSidebarControl::AdjustZoomFactor()
{
    try
    {
        CDC* pdc = GetDC();
        HDC screen = pdc->GetSafeHdc();

        double horizontalPPI = GetDeviceCaps(screen, LOGPIXELSX);
        double scalingFactor = horizontalPPI / 96;

        VARIANT opticalZoom;
        opticalZoom.vt = VT_I4;
        opticalZoom.lVal = (LONG)(m_webBrowser.get_Width() * 100 * scalingFactor / 300);

        m_webBrowser.ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &opticalZoom, NULL);
    }
    catch (...)
    {
        LERROR << "Unable to set zoom to sidebar";
    }
}


void CSidebarControl::SetSidebar(CAcrolinxSidebar* sidebar)
{
    m_sidebar = sidebar;
}


void CSidebarControl::FireInitFinished(void)
{
    m_sidebar->InitFinished();
}


void CSidebarControl::RegisterComponents(void)
{
    CString id, name, version;
    if(DllUtil::GetAppInfo(id, name, version))
    {
        RegisterClientComponent(id, name, version, CC_MAIN);
    }
    if(DllUtil::GetDllInfo(id, name, version))
    {
        RegisterClientComponent(id, name, version, CC_DEFAULT);
    }
    if(DllUtil::GetOSInfo(id, name, version))
    {
        RegisterClientComponent(id, name, version, CC_DEFAULT);
    }
}


void CSidebarControl::RegisterClientComponent(CString id, CString name, CString version, Software_Component_Category category)
{
    if(category == CC_MAIN && m_isMainCategorySet)
    {
        LWARNING << "MAIN component is already set. If you want to set MAIN compoment do it before sidebar start";
        return;
    }
    if(category == CC_MAIN)
    {
        m_isMainCategorySet = TRUE;
    }

    CString softwareComponentCategory = GetSoftwareComponentCategoryAsString(category);

    CString domKey;
    domKey.Format(_T("/clientComponents/%d"), m_clientComponents);
    CJsonUtil::SetString(m_initParameters, domKey +_T("/id"), id);
    CJsonUtil::SetString(m_initParameters, domKey +_T("/name"), name);
    CJsonUtil::SetString(m_initParameters, domKey +_T("/version"), version);
    CJsonUtil::SetString(m_initParameters, domKey +_T("/category"), softwareComponentCategory);
    m_clientComponents++;
}


void CSidebarControl::FireRequestCheck(CString checkOptions)
{
    CComObject<CCheckOptions>* checkOptionsObj = nullptr;
    HRESULT hRes = CComObject<CCheckOptions>::CreateInstance(&checkOptionsObj);
    if (SUCCEEDED(hRes))
    {
        checkOptionsObj->AddRef();
        checkOptionsObj->InitInstance(checkOptions.AllocSysString());
        m_sidebar->RequestCheck(checkOptionsObj);
        checkOptionsObj->Release();
    }
}


CString CSidebarControl::Check(CDokument* document)
{
    BSTR content, reference;
    CString format;
    WDocument selectRangesJson;
    document->GetContent(&content);
    document->GetReference(&reference);
    format = document->GetFormatAsString();

    CString selectionRanges = _T("[]");
    IRanges* ranges = document->GetSelectionRanges();
    if(ranges != NULL)
    {
        LONG selectionRangeCount = 0;
        ranges->GetRangeCount(&selectionRangeCount);

        for(size_t index = 0 ; index < (size_t)selectionRangeCount; index++)
        {
            IRange* selRange = nullptr;
            ranges->GetRangeAt(index, &selRange);
            LONG startoffset = 0, endOffset = 0;
            selRange->GetStart(&startoffset);
            selRange->GetEnd(&endOffset);
            CString key;
            key.Format(_T("/%d/0"), index);
            CJsonUtil::SetInt(selectRangesJson, key, startoffset);
            key.Format(_T("/%d/1"), index);
            CJsonUtil::SetInt(selectRangesJson, key, endOffset);
            selRange->Release();
        }

        ranges->Release();
        selectionRanges = CJsonUtil::Stringify(selectRangesJson);
    }
    return m_scriptHandler->Check(content, reference, format, selectionRanges);
}


void CSidebarControl::FireChecked(CString checkResult)
{
    CComObject<CCheckResult>* checkResultObj = nullptr;
    HRESULT hRes = CComObject<CCheckResult>::CreateInstance(&checkResultObj);
    if (SUCCEEDED(hRes))
    {
        checkResultObj->AddRef();
        checkResultObj->InitInstance(checkResult);
        m_sidebar->Checked(checkResultObj);
        checkResultObj->Release();
    }
}


void CSidebarControl::OpenWindow(CString urlJson)
{
    WDocument urlDom;
    CJsonUtil::Parse(urlJson, urlDom);
    CString urlStr(urlDom[_T("url")].GetString());
    if(urlStr.IsEmpty())
    {
        LERROR << "URL is empty";
        return;
    }
    CString urlTemp(urlStr);
    urlTemp.MakeLower();
    if (urlTemp.Find(_T("http://")) == 0 || urlTemp.Find(_T("https://")) == 0 ||
        urlTemp.Find(_T("mailto:")) == 0 || urlTemp.Find(_T("www.")) == 0)
    {
        ShellExecute(0, 0, urlStr, 0, 0, SW_SHOW);
    }
    else
    {
        LERROR << "URL is not valid: " << urlStr.GetString();
    }
}


void CSidebarControl::FireSelectRanges(CString checkId, CString matches)
{
    IMatches* matchesObj = ConvertToMatches(checkId, matches);
    if(matchesObj != nullptr)
    {
        m_sidebar->SelectRanges(matchesObj);
        matchesObj->Release();
    }
    else
    {
        LERROR << "Fail to prepare matches" << matches.GetString();
    }
}


void CSidebarControl::FireReplaceRanges(CString checkId, CString matchesWithReplacement)
{
    IMatches* matchesObj = ConvertToMatches(checkId, matchesWithReplacement, TRUE);
    if(matchesObj != nullptr)
    {
        m_sidebar->ReplaceRanges(matchesObj);
        matchesObj->Release();
    }
    else
    {
        LERROR << "Fail to prepare matches" << matchesWithReplacement.GetString();
    }
}


IMatches* CSidebarControl::ConvertToMatches(CString checkId, CString matches, BOOL isReplacement)
{
    CComObject<CMatches>* matchesObj = nullptr;
    try{
        HRESULT hRes = CComObject<CMatches>::CreateInstance(&matchesObj);
        if (SUCCEEDED(hRes))
        {
            matchesObj->AddRef();
            matchesObj->SetCheckId(checkId.AllocSysString());

            WDocument matchesDom;
            CJsonUtil::Parse(matches, matchesDom);
            LTRACE << "matches dom is ready";
            for(size_t i = 0; i < matchesDom.Size(); i++)
            {
                CComObject<CMatch>* match = nullptr;
                CComObject<CMatch>::CreateInstance(&match);
                match->AddRef();
                CComObject<CRange>* range = nullptr;
                CComObject<CRange>::CreateInstance(&range);
                range->AddRef();
                range->InitInstance(matchesDom[i][_T("range")][0].GetInt(), matchesDom[i][_T("range")][1].GetInt());
                CComObject<CRange>* extractedRange = nullptr;
                CComObject<CRange>::CreateInstance(&extractedRange);
                extractedRange->AddRef();
                extractedRange->InitInstance(matchesDom[i][_T("extractedRange")][0].GetInt(), matchesDom[i][_T("extractedRange")][1].GetInt());
                CString content = matchesDom[i][_T("content")].GetString();
                CString replacement = CString();
                if(isReplacement)
                {
                    replacement = matchesDom[i][_T("replacement")].GetString();
                }
                match->InitInstance(content.AllocSysString(), replacement.AllocSysString(), range, extractedRange);
                LTRACE << "match added to matches list";
                matchesObj->Add(match);
            }
        }
    }catch(...)
    {
        LERROR << "Fail to prepare matches";
        if(matchesObj != nullptr)
        {
            matchesObj->Release();
        }
        matchesObj = nullptr;
    }

    return matchesObj;
}


void CSidebarControl::InvalidateRanges(CMatches* matches)
{
    WDocument matchRanges;

    for(size_t i=0; i<matches->GetCount(); i++)
    {
        BSTR checkId;
        matches->GetCheckId(&checkId);
        CString domKey;
        domKey.Format(_T("/%d/checkId"),i);
        CJsonUtil::SetString(matchRanges, domKey, CString(checkId));
        IMatch* match = nullptr;
        matches->GetMatchAt(i, &match);
        IRange* range = nullptr;
        match->GetRange(&range);
        LONG start = 0, end = 0;
        range->GetStart(&start);
        range->GetEnd(&end);
        domKey.Format(_T("/%d/range/0"),i);
        CJsonUtil::SetInt(matchRanges, domKey, start);
        domKey.Format(_T("/%d/range/1"),i);
        CJsonUtil::SetInt(matchRanges, domKey, end);

        range->Release();
        match->Release();
    }
    m_scriptHandler->InvalidateRanges(CJsonUtil::Stringify(matchRanges));
    matches->Release();
}


CString CSidebarControl::GetSoftwareComponentCategoryAsString(Component_Category category)
{
    CString softwareComponentCategory = _T("DEFAULT");

    if(CC_MAIN == category)
    {
        softwareComponentCategory = _T("MAIN");
    }
    else if(CC_DETAIL == category)
    {
        softwareComponentCategory = _T("DETAIL");
    }

    return softwareComponentCategory;
}
