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
#include "FileUtil.h"
#include "RegistryUtil.h"


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
    ACROASSERT(!GetClientSignature().IsEmpty(),
        "You do not have specified a client signature. Please ask Acrolinx for a client signature and set the client signature via acrolinxSidebar.SetClientSignature().");
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
    CString startPageURL = CString(_T(""));
    CString extractPath = FileUtil::ExtractEmbeddedStartPage();

    if (extractPath[0] == '\0')
    {
        return startPageURL;
    }

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

    startPageURL.Append(_T("https://"));
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
    this->SetWindowPos(NULL, 0, 0, width, height, SWP_NOZORDER);
    ResizeWebView();
    AdjustZoomFactor();
}


void CSidebarControl::AdjustZoomFactor()
{
    if (m_controller == NULL)
    {
        return;
    }

    try
    {
        CDC* pdc = GetDC();
        HDC screen = pdc->GetSafeHdc();

        double horizontalPPI = GetDeviceCaps(screen, LOGPIXELSX);
        double scalingFactor = horizontalPPI / 96;

        RECT bounds;
        m_controller->get_Bounds(&bounds);

        int width = bounds.right - bounds.left;

        if (width > 0)
        {
            m_controller->put_ZoomFactor(width / (300 * scalingFactor));
        }
    }
    catch (...)
    {
        LOGE << "Unable to set zoom to sidebar";
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

BOOL CSidebarControl::InitializeWebView()
{
    BOOL intializeStatus = true;
    CloseWebView();
    m_dcompDevice = nullptr;

    HRESULT hRes = DCompositionCreateDevice2(nullptr, IID_PPV_ARGS(&m_dcompDevice));
    if (!SUCCEEDED(hRes))
    {
        LOGE << "Attempting to create WebView using DComp Visual is not supported.";
        intializeStatus = false;
        return intializeStatus;
    }

    LPCWSTR subFolder = nullptr;
    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    options->put_AllowSingleSignOnUsingOSPrimaryAccount(FALSE);

    CString userDataFolder = FileUtil::GetWebViewUserDataDirectory();

    hRes = CreateCoreWebView2EnvironmentWithOptions(subFolder, userDataFolder, options.Get(),
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>
        (this, &CSidebarControl::OnCreateEnvironmentCompleted).Get());

    // Error codes - https://learn.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/webview2-idl?view=webview2-1.0.1722.45#createcorewebview2environmentwithoptions
    if (!SUCCEEDED(hRes))
    {
        if (hRes == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            LOGE << "Could not find Edge installation.";
            intializeStatus = false;
        }
        else if (hRes == HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
        {
            LOGE << "Edge Application path used in browserExecutableFolder.";
            intializeStatus = false;
        }
        else if (hRes == HRESULT_FROM_WIN32(ERROR_INVALID_STATE))
        {
            LOGE << "Specified options do not match the options of the WebViews that are currently running in the shared browser process.";
            intializeStatus = false;
        }
        else if (hRes == HRESULT_FROM_WIN32(ERROR_DISK_FULL))
        {
            LOGE << "Too many previous WebView2 Runtime versions exist.";
            intializeStatus = false;
        }
        else if (hRes == HRESULT_FROM_WIN32(ERROR_PRODUCT_UNINSTALLED))
        {
            LOGE << "WebView2 Runtime uninstalled.";
            intializeStatus = false;
        }
        else if (hRes == HRESULT_FROM_WIN32(ERROR_FILE_EXISTS))
        {
            LOGW << "User data folder cannot be created because a file with the same name already exists.";
        }
        else if(hRes == E_ACCESSDENIED)
        {
            LOGE << "Unable to create user data folder, Access Denied.";
            intializeStatus = false;
        }
        else if (hRes == E_FAIL)
        {
            LOGE << "Edge runtime unable to start.";
            intializeStatus = false;
        }
        else if (hRes == CO_E_NOTINITIALIZED)
        {
            LOGE << "CoInitializeEx was not called.";
            intializeStatus = false;
        }
        else if (hRes == RPC_E_CHANGED_MODE)
        {
            LOGE << "CoInitializeEx was previously called with COINIT_MULTITHREADED.";
            intializeStatus = false;
        }
        else
        {
            LOGE << "Failed to create webview environment";
            intializeStatus = false;
        }
    }

    return intializeStatus;
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
    HRESULT hRes = m_webView->ExecuteScript(script, Callback<ICoreWebView2ExecuteScriptCompletedHandler>
        (this, &CSidebarControl::ExecuteScriptResponse).Get());
    if (FAILED(hRes))
    {
        LOGE << "Script execution failed";
        LOGD << "Failed script: " << script;
    }
}

HRESULT CSidebarControl::ExecuteScriptResponse(HRESULT error, LPCWSTR result)
{
    if (FAILED(error))
    {
        LOGE << "Script execution failed" << result;
    }
    return error;
}

void CSidebarControl::CloseWebView()
{
    if (m_controller)
    {
        m_controller->Close();
        m_controller = nullptr;
        m_webView = nullptr;
    }
    m_webViewEnvironment = nullptr;
}

HRESULT CSidebarControl::OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment)
{
    m_webViewEnvironment = environment;
    m_webViewEnvironment->CreateCoreWebView2Controller(this->GetSafeHwnd(),
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>
        (this, &CSidebarControl::OnCreateCoreWebView2ControllerCompleted).Get());

    return S_OK;
}

HRESULT CSidebarControl::OnCoreWebView2NavigationStarting(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args)
{
    LOGI << "WebView Navigation Started";
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

        ComPtr<ICoreWebView2> coreWebView2;
        m_controller->get_CoreWebView2(&coreWebView2);
        m_webView = coreWebView2.Get();;

        // Add Handlers for WebView2 events
        m_webView->add_NavigationCompleted(Callback<ICoreWebView2NavigationCompletedEventHandler>
            (this, &CSidebarControl::OnCoreWebView2NavigationCompleted).Get(), nullptr);

        m_webView->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>
            (this, &CSidebarControl::OnCoreWebView2NavigationStarting).Get(), nullptr);

        NewComponent<ViewComponent>(this);

        if (!m_webView)
        {
            return S_FALSE;
        }

        // Disable some WebView2 features
        ComPtr<ICoreWebView2Settings> webViewSettings;
        m_webView->get_Settings(&webViewSettings);

        DWORD enableContextMenu(0);
        RegistryUtil::GetDWORDRegHKCU(L"", L"EnableContextMenu", enableContextMenu);

        webViewSettings->put_AreDefaultContextMenusEnabled(enableContextMenu);
        webViewSettings->put_IsStatusBarEnabled(FALSE);

        ComPtr<ICoreWebView2Settings3> webViewSettings3;
        webViewSettings->QueryInterface(IID_ICoreWebView2Settings3, (VOID **)&webViewSettings);
        if (webViewSettings3)
        {
            // WebView version 1.0.864.35
            webViewSettings3->put_AreBrowserAcceleratorKeysEnabled(FALSE);
        }
        else
        {
            LOGW << "Browser accelerator keys can't be disabled.";
        }

        CString startPageURL = GetStartPageURL();

        if (startPageURL[0] == '\0')
        {
            m_label.SetWindowTextW(_T("Couldn't load start page. Check logs for errors."));
            LOGE << "Startpage URL empty.";
            return S_FALSE;
        }

        HRESULT hresult = m_webView->Navigate(startPageURL);
        if (hresult == S_OK)
        {
            m_label.ShowWindow(SW_HIDE);
            LOGI << "Successfully navigated to start page";
            ResizeWebView();
        }
    }
    else
    {
        LOGE << "Failed to create webview2";
    }
    return S_OK;
}

void CSidebarControl::ResizeWebView()
{
    RECT availableBounds = { 0 };
    GetClientRect(&availableBounds);

    if (auto view = GetComponent<ViewComponent>())
    {
        view->SetBounds(availableBounds);
    }

    AdjustZoomFactor();

}
