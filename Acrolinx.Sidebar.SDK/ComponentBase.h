/* Copyright 2023-present Acrolinx GmbH */
#pragma once

#include "stdafx.h"

class ComponentBase
{
public:
    virtual bool HandleWindowMessage(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT* result)
    {
        return false;
    }
    virtual ~ComponentBase() { }
};
