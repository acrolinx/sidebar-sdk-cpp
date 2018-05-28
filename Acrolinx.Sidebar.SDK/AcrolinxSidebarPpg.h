/* Copyright (c) 2018 Acrolinx GmbH */

#pragma once

// AcrolinxSidebarPpg.h : Declaration of the CAcrolinxSidebarPropPage property page class.


// CAcrolinxSidebarPropPage : See AcrolinxSidebarPpg.cpp for implementation.
class CAcrolinxSidebarPropPage : public COlePropertyPage
{
    DECLARE_DYNCREATE(CAcrolinxSidebarPropPage)
    DECLARE_OLECREATE_EX(CAcrolinxSidebarPropPage)

// Constructor
public:
    CAcrolinxSidebarPropPage();

    // Dialog Data
    enum { IDD = IDD_PROPPAGE_ACROLINXSIDEBARSDK };

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
    DECLARE_MESSAGE_MAP()
};

