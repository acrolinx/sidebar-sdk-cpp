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
#ifdef USE_WEBVIEW2_WIN10
        winrtComp::Compositor wincompCompositor,
#endif
        bool isDCompTargetMode
    );

    bool HandleWindowMessage(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT* result) override;

    void SetBounds(RECT bounds);
    void SetScale(float scale);

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
    void ToggleVisibility();
    void SetSizeRatio(float ratio);
    void SetZoomFactor(float zoom);
    
    void SetTransform(TransformType transformType);
    void ShowWebViewBounds();
    void ShowWebViewZoom();

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

    bool OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);
    bool OnPointerMessage(UINT message, WPARAM wParam, LPARAM lParam);
    void TrackMouseEvents(DWORD mouseTrackingFlags);
    bool m_isTrackingMouse = false;
    bool m_isCapturingMouse = false;
    std::unordered_set<UINT> m_pointerIdsStartingInWebView;

    void BuildDCompTreeUsingVisual();
    void DestroyDCompVisualTree();

    Microsoft::WRL::ComPtr<IDCompositionDevice> m_dcompDevice;
    Microsoft::WRL::ComPtr<IDCompositionTarget> m_dcompHwndTarget;
    Microsoft::WRL::ComPtr<IDCompositionVisual> m_dcompRootVisual;
    Microsoft::WRL::ComPtr<IDCompositionVisual> m_dcompWebViewVisual;


    EventRegistrationToken m_cursorChangedToken = {};
};
