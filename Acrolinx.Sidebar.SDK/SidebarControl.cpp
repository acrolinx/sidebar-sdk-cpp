/* Copyright Acrolinx GmbH */

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
#include "ViewComponent.h"


using namespace Acrolinx_Sdk_Sidebar_Util;
using namespace Microsoft::WRL;

// CSidebarControl dialog

static constexpr UINT s_runAsyncWindowMessage = WM_APP;

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
    SetMinimumSidebarVersion(_T("15.0.0"));
    SetSupportCheckSelection(false);
}

CSidebarControl::~CSidebarControl()
{
    if (m_acrolinxStorage)
    {
        m_acrolinxStorage->Release();
        m_acrolinxStorage = NULL;
    }
    m_sidebar = NULL;
}

void CSidebarControl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LABEL, m_label);
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

    BOOL isSuccess = InitializeWebView();
    if (!isSuccess)
    {
        LOGE << "Initializing WebView2 failed";
    }
    UpdateWindow();
}


CString CSidebarControl::GetStartPageURL(void)
{
    // TODO: Change to extracted path
    CString extractPath = CString(_T("C:\\Users\\abhijeetnarvekar\\AppData\\Local\\Temp\\Acrolinx\\cpp-sdk"));
    CString hostName = CString(_T("extensions.acrolinx.cloud"));

    Microsoft::WRL::ComPtr<ICoreWebView2_3> webView;
    m_webView->QueryInterface(IID_ICoreWebView2_3, (VOID **)&webView);
    if (!webView)
    {
        // WebView2 Win32 1.0.774.44 onwards
        LOGE << "SetVirtualHostNameToFolderMapping not supported";
        return CString();
    }

    // Virtual Host Mapping
    webView->SetVirtualHostNameToFolderMapping(hostName, extractPath, COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);

    CString startPageURL = CString(_T("https://"));
    startPageURL.Append(hostName);
    startPageURL.Append(_T("/dist-offline/index.html"));

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
    CJsonUtil::SetBool(m_initParameters, _T("/showServerSelector"), (showServerSelector ? true : false));
}


BOOL CSidebarControl::GetShowServerSelector(void)
{
    return BOOL(m_initParameters[_T("showServerSelector")].GetBool());
}


void CSidebarControl::SetDefaults(CString serverAddress)
{
    ShowHideServerSelectorIfServerAddressParameterSet(serverAddress);

    //ClientLocale
    if (GetClientLocale().IsEmpty())
    {
        LANGID defaultLang = GetUserDefaultUILanguage();

        WCHAR strNameBuffer[LOCALE_NAME_MAX_LENGTH] = { 0 };
        if (LCIDToLocaleName(defaultLang, strNameBuffer, LOCALE_NAME_MAX_LENGTH, 0) == 0)
        {
            LOGE << DllUtil::GetLastErrorAsString().GetString();
        }
        else
        {
            CString locale(strNameBuffer);
            int position = 0;
            locale = locale.Tokenize(_T("-"), position);
            SetClientLocale(locale);
            LOGI << "Default locale is set " << locale.GetString();
        }
    }

    //Default Storage
    if (m_acrolinxStorage == NULL)
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
    else if (GetServerAddress().IsEmpty())
    {
        SetShowServerSelector(TRUE);
    }

}
BEGIN_EVENTSINK_MAP(CSidebarControl, CDialogEx)
END_EVENTSINK_MAP()


BOOL CSidebarControl::DestroyWindow()
{
    HRESULT hRes = S_FALSE;
    if (m_scriptHandler)
    {
        m_scriptHandler->Release();
        m_webView->Release();
    }

    // TODO: Fix access violation exception
    // Release webview stuff
    return CDialog::DestroyWindow();
}


void CSidebarControl::SetStorage(IAcrolinxStorage* storage)
{
    ACROASSERT(m_acrolinxStorage == NULL, "Look like you have already loaded sidebar. Set storage before starting sidebar.");

    if (m_acrolinxStorage == NULL)
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
    CJsonUtil::SetBool(m_initParameters, _T("/readOnlySuggestions"), (isReadOnly ? true : false));
}


void CSidebarControl::SetSupportCheckSelection(BOOL isCheckSelectionSupported)
{
    CJsonUtil::SetBool(m_initParameters, _T("/supported/checkSelection"), (isCheckSelectionSupported ? true : false));
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
    /* this->SetWindowPos(NULL, 0, 0, width, height, SWP_NOZORDER);
     m_webBrowser.put_Width(width);
     m_webBrowser.put_Height(height);
     IDispatchPtr document = m_webBrowser.get_Document();
     if (document != nullptr)
     {
         AdjustZoomFactor();
     }*/
}


void CSidebarControl::AdjustZoomFactor()
{
    /* try
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
         LOGE << "Unable to set zoom to sidebar";
     }*/
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
    if (DllUtil::GetAppInfo(id, name, version))
    {
        RegisterClientComponent(id, name, version, CC_MAIN);
    }
    if (DllUtil::GetDllInfo(id, name, version))
    {
        RegisterClientComponent(id, name, version, CC_DEFAULT);
    }
    if (DllUtil::GetOSInfo(id, name, version))
    {
        RegisterClientComponent(id, name, version, CC_DEFAULT);
    }
}


void CSidebarControl::RegisterClientComponent(CString id, CString name, CString version, Software_Component_Category category)
{
    if (category == CC_MAIN && m_isMainCategorySet)
    {
        LOGW << "MAIN component is already set. If you want to set MAIN compoment do it before sidebar start";
        return;
    }
    if (category == CC_MAIN)
    {
        m_isMainCategorySet = TRUE;
    }

    CString softwareComponentCategory = GetSoftwareComponentCategoryAsString(category);

    CString domKey;
    domKey.Format(_T("/clientComponents/%d"), m_clientComponents);
    CJsonUtil::SetString(m_initParameters, domKey + _T("/id"), id);
    CJsonUtil::SetString(m_initParameters, domKey + _T("/name"), name);
    CJsonUtil::SetString(m_initParameters, domKey + _T("/version"), version);
    CJsonUtil::SetString(m_initParameters, domKey + _T("/category"), softwareComponentCategory);
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
    _bstr_t content, reference;
    CString format;
    WDocument selectRangesJson;
    document->GetContent(content.GetAddress());
    document->GetReference(reference.GetAddress());
    format = document->GetFormatAsString();

    CString selectionRanges = _T("[]");
    IRanges* ranges = document->GetSelectionRanges();
    if (ranges != NULL)
    {
        LONG selectionRangeCount = 0;
        ranges->GetRangeCount(&selectionRangeCount);

        for (size_t index = 0; index < (size_t)selectionRangeCount; index++)
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
    return m_scriptHandler->Check(content.GetBSTR(), reference.GetBSTR(), format, selectionRanges);
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
    if (urlStr.IsEmpty())
    {
        LOGE << "URL is empty";
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
        LOGE << "URL is not valid: " << urlStr.GetString();
    }
}


void CSidebarControl::FireSelectRanges(CString checkId, CString matches)
{
    IMatches* matchesObj = ConvertToMatches(checkId, matches);
    if (matchesObj != nullptr)
    {
        m_sidebar->SelectRanges(matchesObj);
        matchesObj->Release();
    }
    else
    {
        LOGE << "Fail to prepare matches" << matches.GetString();
    }
}


void CSidebarControl::FireReplaceRanges(CString checkId, CString matchesWithReplacement)
{
    IMatches* matchesObj = ConvertToMatches(checkId, matchesWithReplacement, TRUE);
    if (matchesObj != nullptr)
    {
        m_sidebar->ReplaceRanges(matchesObj);
        matchesObj->Release();
    }
    else
    {
        LOGE << "Fail to prepare matches" << matchesWithReplacement.GetString();
    }
}


IMatches* CSidebarControl::ConvertToMatches(CString checkId, CString matches, BOOL isReplacement)
{
    CComObject<CMatches>* matchesObj = nullptr;
    try {
        HRESULT hRes = CComObject<CMatches>::CreateInstance(&matchesObj);
        if (SUCCEEDED(hRes))
        {
            matchesObj->AddRef();
            matchesObj->SetCheckId(checkId.AllocSysString());

            WDocument matchesDom;
            CJsonUtil::Parse(matches, matchesDom);
            LOGD << "matches dom is ready";
            for (size_t i = 0; i < matchesDom.Size(); i++)
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
                if (isReplacement)
                {
                    replacement = matchesDom[i][_T("replacement")].GetString();
                }
                match->InitInstance(content.AllocSysString(), replacement.AllocSysString(), range, extractedRange);
                LOGD << "match added to matches list";
                matchesObj->Add(match);
            }
        }
    }
    catch (...)
    {
        LOGE << "Fail to prepare matches";
        if (matchesObj != nullptr)
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

    for (size_t i = 0; i < matches->GetCount(); i++)
    {
        _bstr_t checkId;
        matches->GetCheckId(checkId.GetAddress());
        CString domKey;
        domKey.Format(_T("/%d/checkId"), i);
        CJsonUtil::SetString(matchRanges, domKey, CString(checkId.GetBSTR()));
        IMatch* match = nullptr;
        matches->GetMatchAt(i, &match);
        IRange* range = nullptr;
        match->GetRange(&range);
        LONG start = 0, end = 0;
        range->GetStart(&start);
        range->GetEnd(&end);
        domKey.Format(_T("/%d/range/0"), i);
        CJsonUtil::SetInt(matchRanges, domKey, start);
        domKey.Format(_T("/%d/range/1"), i);
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

    if (CC_MAIN == category)
    {
        softwareComponentCategory = _T("MAIN");
    }
    else if (CC_DETAIL == category)
    {
        softwareComponentCategory = _T("DETAIL");
    }

    return softwareComponentCategory;
}

void CSidebarControl::RunAsync(std::function<void()> callback)
{
    auto* task = new std::function<void()>(callback);
    PostMessage(s_runAsyncWindowMessage, reinterpret_cast<WPARAM>(task), 0);
}

BOOL CSidebarControl::InitializeWebView()
{

    CloseWebView();
    m_dcompDevice = nullptr;


    HRESULT hRes = DCompositionCreateDevice2(nullptr, IID_PPV_ARGS(&m_dcompDevice));
    if (!SUCCEEDED(hRes))
    {
        LOGE << "Attempting to create WebView using DComp Visual is not supported.";
        return false;
    }


    // TODO: Create user directory for WebView2
    LPCWSTR subFolder = nullptr;
    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    options->put_AllowSingleSignOnUsingOSPrimaryAccount(FALSE);



    hRes = CreateCoreWebView2EnvironmentWithOptions(subFolder, nullptr, options.Get(), Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &CSidebarControl::OnCreateEnvironmentCompleted).Get());

    if (!SUCCEEDED(hRes))
    {
        if (hRes == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            LOGE << "Couldn't find WebView2 installation. Check if Edge and WebView2 runtime is installed";
        }
        else
        {
            LOGE << "Failed to create webview environment";
        }
    }
}

HRESULT CSidebarControl::DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv)
{
    HRESULT hr = E_FAIL;
    static decltype(::DCompositionCreateDevice2)* fnCreateDCompDevice2 = nullptr;
    if (fnCreateDCompDevice2 == nullptr)
    {
        HMODULE hmod = ::LoadLibraryEx(L"dcomp.dll", nullptr, 0);
        if (hmod != nullptr)
        {
            fnCreateDCompDevice2 = reinterpret_cast<decltype(::DCompositionCreateDevice2)*>(
                ::GetProcAddress(hmod, "DCompositionCreateDevice2"));
        }
    }
    if (fnCreateDCompDevice2 != nullptr)
    {
        hr = fnCreateDCompDevice2(renderingDevice, riid, ppv);
    }
    return hr;
}

void CSidebarControl::Eval(CString script)
{
    m_webView->ExecuteScript(script, Callback<ICoreWebView2ExecuteScriptCompletedHandler>
        (this, &CSidebarControl::ExecuteScriptResponse).Get());
}

HRESULT CSidebarControl::ExecuteScriptResponse(HRESULT error, LPCWSTR result)
{
    //TODO: Do something with the result
    return S_OK;
}



void CSidebarControl::CloseWebView(bool cleanupUserDataFolder)
{

    if (m_controller)
    {
        m_controller->Close();
        m_controller = nullptr;
        m_webView = nullptr;
    }
    m_webViewEnvironment = nullptr;
    if (cleanupUserDataFolder)
    {
        //Clean user data        
    }
}

HRESULT CSidebarControl::OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment)
{
    m_webViewEnvironment = environment;
    m_webViewEnvironment->CreateCoreWebView2Controller(this->GetSafeHwnd(), Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>
        (this, &CSidebarControl::OnCreateCoreWebView2ControllerCompleted).Get());

    return S_OK;
}

HRESULT CSidebarControl::OnCoreWebView2NavigationStarting(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args)
{
    return S_OK;
}

HRESULT CSidebarControl::OnCoreWebView2NavigationCompleted(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args)
{
    // Get the new scriptHandler
    HRESULT hRes = CComObject<CScriptHandler>::CreateInstance(&m_scriptHandler);
    if (!SUCCEEDED(hRes))
    {
        LOGE << "CreateInstance script handle instance failed";
        return S_FALSE;
    }

    m_scriptHandler->SetSidebarControl(this);

    VARIANT scriptingObjectAsVariant = {};
    scriptingObjectAsVariant.vt = VT_DISPATCH;
    scriptingObjectAsVariant.pdispVal = m_scriptHandler;

    m_webView->AddHostObjectToScript(L"bridge", &scriptingObjectAsVariant);

    m_scriptHandler->OnAfterObjectSet();

    return S_OK;
}

HRESULT CSidebarControl::OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller)
{
    if (result == S_OK)
    {
        m_controller = controller;

        Microsoft::WRL::ComPtr<ICoreWebView2> coreWebView2;
        m_controller->get_CoreWebView2(&coreWebView2);
        m_webView = coreWebView2.Get();;

        // Add Handlers for WebView2 events
        m_webView->add_NavigationCompleted(Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>
            (this, &CSidebarControl::OnCoreWebView2NavigationCompleted).Get(), nullptr);

        m_webView->add_NavigationStarting(Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>
            (this, &CSidebarControl::OnCoreWebView2NavigationStarting).Get(), nullptr);

        NewComponent<ViewComponent>(this, m_dcompDevice.Get(), m_creationModeId == IDM_CREATION_MODE_TARGET_DCOMP);

        if (!m_webView)
        {
            return S_FALSE;
        }

        // Disable some WebView2 features
        Microsoft::WRL::ComPtr<ICoreWebView2Settings> webViewSettings;
        m_webView->get_Settings(&webViewSettings);

        //TODO: Uncomment the following lines after dev work is completed.
        //webViewSettings->put_AreDefaultContextMenusEnabled(FALSE);
        //webViewSettings->put_IsStatusBarEnabled(FALSE);

        Microsoft::WRL::ComPtr<ICoreWebView2Settings3> webViewSettings3;
        webViewSettings->QueryInterface(IID_ICoreWebView2Settings3, (VOID **)&webViewSettings);
        if (webViewSettings3)
        {
            webViewSettings3->put_AreBrowserAcceleratorKeysEnabled(FALSE);
        }
        else
        {
            LOGW << "Browser accelerator keys can't be disabled.";
        }

        CString startPageURL = GetStartPageURL();
        HRESULT hresult = m_webView->Navigate(startPageURL);

        if (hresult == S_OK)
        {
            m_label.ShowWindow(SW_HIDE);
            LOGI << "Successfully navigated to start page";
            ResizeEverything();
        }
    }
    else
    {
        LOGE << "Failed to create webview2";
    }
    return S_OK;
}

void CSidebarControl::ResizeEverything()
{
    RECT availableBounds = { 0 };
    GetClientRect(&availableBounds);

    if (auto view = GetComponent<ViewComponent>())
    {
        view->SetBounds(availableBounds);
    }
}
