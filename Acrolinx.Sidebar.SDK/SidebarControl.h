/* Copyright Acrolinx GmbH */

#pragma once
#include "afxwin.h"
#include "ComponentBase.h"
#include "WebBrowser.h"
#include "JsonUtil.h"
#include "Dokument.h"
#include "Matches.h"
#include "ScriptHandler.h"
#include "dcomp.h"
#include <functional>

class CAcrolinxSidebar;

// CSidebarControl dialog

class CSidebarControl : public CDialogEx
{
    DECLARE_DYNAMIC(CSidebarControl)

public:
    CSidebarControl(CWnd* pParent = nullptr);   // standard constructor
    virtual ~CSidebarControl();

    // Dialog Data
    enum { IDD = IDD_ACROLINX_SIDEBAR };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// WebView2 Stuff
	DWORD m_creationModeId = 0;
	Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
	Microsoft::WRL::ComPtr<ICoreWebView2> m_webView;
	Microsoft::WRL::ComPtr<IDCompositionDevice> m_dcompDevice;
	std::vector<std::unique_ptr<ComponentBase>> m_components;

	template <class ComponentType, class... Args> void NewComponent(Args&&... args);
	template <class ComponentType> ComponentType* GetComponent();

    DECLARE_MESSAGE_MAP()
private:
    CStatic m_label;
    CWebBrowser m_webBrowser;
    CString m_startPageSourceLocation;
    IAcrolinxStorage* m_acrolinxStorage;
    CString m_logFileLocation;
    WDocument m_initParameters;
    CAcrolinxSidebar* m_sidebar;
    int m_clientComponents;
    BOOL m_isMainCategorySet;
    CComObject<CScriptHandler>* m_scriptHandler;

private:
    CString GetStartPageURL(void);
    void SetDefaults(CString serverAddress);
    void ShowHideServerSelectorIfServerAddressParameterSet(CString serverAddress);
    void RegisterComponents(void);
    IMatches* ConvertToMatches(CString checkId, CString matches, BOOL isReplacement = false);
    CString GetSoftwareComponentCategoryAsString(Software_Component_Category category);
    void AdjustZoomFactor();
public:
    BOOL Create(CWnd* pParent);
    BOOL DestroyWindow();
    void Start(CString serverAddress = CString());
    void SetServerAddress(CString serverAddress);
    CString GetServerAddress(void);
    void SetStartPageSourceLocation(CString startPageDirPath);
    CString GetStartPageSourceLocation(void);
    void SetShowServerSelector(BOOL showServerSelector);
    BOOL GetShowServerSelector(void);
    void SetStorage(IAcrolinxStorage* storage);
    IAcrolinxStorage* GetStorage(void);
    CString RequestInit(void);
    void SetClientLocale(CString locale);
    void SetClientSignature(CString signature);
    void SetMinimumSidebarVersion(CString version);
    void SetReadOnlySuggestions(BOOL isReadOnly);
    void SetSupportCheckSelection(BOOL isCheckSelectionSupported);
    CString GetClientSignature(void);
    CString GetMinimumSidebarVersion(void);
    BOOL GetReadOnlySuggestions(void);
    BOOL GetSupportCheckSelection(void);
    CString GetClientLocale(void);
    void SetSidebar(CAcrolinxSidebar* sidebar);
    void FireInitFinished(void);
    void FireRequestCheck(CString checkOptions);
    void RegisterClientComponent(CString idStr, CString name, CString version, Software_Component_Category category);
    CString Check(CDokument* document);
    void FireChecked(CString checkResult);
    void OpenWindow(CString urlJson);
    void FireSelectRanges(CString checkId, CString matches);
    void FireReplaceRanges(CString checkId, CString matchesWithReplacement);
    void InvalidateRanges(CMatches* matches);
    void AdjustControlSize(long width = 300, long height = 540);

	// WebView2 Stuff
	void InitializeWebView();
	void CloseWebView(bool cleanupUserDataFolder = false);
	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
	HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);
	void RunAsync(std::function<void(void)> callback);
	void ResizeEverything();
	HRESULT DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv);


	ICoreWebView2Controller* GetWebViewController()
	{
		return m_controller.Get();
	}
	ICoreWebView2* GetWebView()
	{
		return m_webView.Get();
	}
	ICoreWebView2Environment* GetWebViewEnvironment()
	{
		return m_webViewEnvironment.Get();
	}
	HWND GetMainWindow()
	{
		return this->GetSafeHwnd();
	}

protected:
    DECLARE_EVENTSINK_MAP()
    void DocumentCompleteWebBrowser(LPDISPATCH pDisp, VARIANT* URL);
};

template <class ComponentType, class... Args> void CSidebarControl::NewComponent(Args&&... args)
{
	m_components.emplace_back(new ComponentType(std::forward<Args>(args)...));
}

template <class ComponentType> ComponentType* CSidebarControl::GetComponent()
{
	for (auto& component : m_components)
	{
		if (auto wanted = dynamic_cast<ComponentType*>(component.get()))
		{
			return wanted;
		}
	}
	return nullptr;
}
