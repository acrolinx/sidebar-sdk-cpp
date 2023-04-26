/* Copyright 2023-present Acrolinx GmbH */
#pragma once

#include "stdafx.h"
#include "SidebarControl.h"

class ViewComponent : public ComponentBase
{

public:
    ViewComponent(
		CSidebarControl* appWindow
    );

    void SetBounds(RECT bounds);

    ~ViewComponent() override;

private:

    void ResizeWebView();

	CSidebarControl* m_appWindow = nullptr;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    

    float m_webViewRatio = 1.0f;
    RECT m_webViewBounds = {};
    float m_webViewScale = 1.0f;
    EventRegistrationToken m_zoomFactorChangedToken = {};
};
