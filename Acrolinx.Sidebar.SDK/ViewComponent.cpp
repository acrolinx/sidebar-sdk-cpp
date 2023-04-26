/* Copyright 2023-present Acrolinx GmbH */

#pragma once
#include "stdafx.h"
#include "ViewComponent.h"
#include <sstream>
#include <windowsx.h>
#include <WinUser.h>

using namespace Microsoft::WRL;
ViewComponent::ViewComponent(
	CSidebarControl* appWindow,
    IDCompositionDevice* dcompDevice,
    bool isDcompTargetMode)
    : m_appWindow(appWindow), m_controller(appWindow->GetWebViewController()),
    m_webView(appWindow->GetWebView()), m_dcompDevice(dcompDevice),
    m_isDcompTargetMode(isDcompTargetMode)
{

    m_controller->add_ZoomFactorChanged(
        Callback<ICoreWebView2ZoomFactorChangedEventHandler>(
            [this](ICoreWebView2Controller* sender, IUnknown* args) -> HRESULT {
                double zoomFactor;
                sender->get_ZoomFactor(&zoomFactor);
                return S_OK;
            })
        .Get(),
                &m_zoomFactorChangedToken);

    ResizeWebView();
}

void ViewComponent::SetBounds(RECT bounds)
{
    m_webViewBounds = bounds;
    ResizeWebView();
}

void ViewComponent::ResizeWebView()
{
    SIZE webViewSize = {
            LONG((m_webViewBounds.right - m_webViewBounds.left) * m_webViewRatio * m_webViewScale),
            LONG((m_webViewBounds.bottom - m_webViewBounds.top) * m_webViewRatio * m_webViewScale) };

    RECT desiredBounds = m_webViewBounds;
    desiredBounds.bottom = LONG(
        webViewSize.cy + m_webViewBounds.top);
    desiredBounds.right = LONG(
        webViewSize.cx + m_webViewBounds.left);

    m_controller->put_Bounds(desiredBounds);
}

ViewComponent::~ViewComponent()
{
    m_controller->remove_ZoomFactorChanged(m_zoomFactorChangedToken);
}
