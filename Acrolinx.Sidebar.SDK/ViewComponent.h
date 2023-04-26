#pragma once

#include "stdafx.h"
#include "SidebarControl.h"
#include <dcomp.h>
#include <unordered_set>

// TODO: ViewComponent needs cleanup
class DCompTargetImpl;

class ViewComponent : public ComponentBase
{
    friend class DCompTargetImpl;

public:
    ViewComponent(
		CSidebarControl* appWindow,
        IDCompositionDevice* dcompDevice,
        bool isDCompTargetMode
    );

    void SetBounds(RECT bounds);

    ~ViewComponent() override;

private:
    enum class TransformType
    {
        kIdentity = 0,
        kScale2X,
        kRotate30Deg,
        kRotate60DegDiagonally
    };
    void ResizeWebView();

    //Browser* m_appWindow = nullptr;
	CSidebarControl* m_appWindow = nullptr;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2> m_webView;
    
    bool m_isDcompTargetMode;
    bool m_isVisible = true;
    float m_webViewRatio = 1.0f;
    float m_webViewZoomFactor = 1.0f;
    RECT m_webViewBounds = {};
    float m_webViewScale = 1.0f;
    bool m_useCursorId = false;
    EventRegistrationToken m_zoomFactorChangedToken = {};

    bool m_isTrackingMouse = false;
    bool m_isCapturingMouse = false;
    std::unordered_set<UINT> m_pointerIdsStartingInWebView;

    Microsoft::WRL::ComPtr<IDCompositionDevice> m_dcompDevice;
    Microsoft::WRL::ComPtr<IDCompositionTarget> m_dcompHwndTarget;
    Microsoft::WRL::ComPtr<IDCompositionVisual> m_dcompRootVisual;
    Microsoft::WRL::ComPtr<IDCompositionVisual> m_dcompWebViewVisual;


    EventRegistrationToken m_cursorChangedToken = {};
};
