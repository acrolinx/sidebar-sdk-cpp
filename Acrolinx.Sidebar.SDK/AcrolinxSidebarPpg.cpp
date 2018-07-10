/* Copyright Acrolinx GmbH */

// AcrolinxSidebarPpg.cpp : Implementation of the CAcrolinxSidebarPropPage property page class.

#include "stdafx.h"
#include "Acrolinx.Sidebar.SDK.h"
#include "AcrolinxSidebarPpg.h"



IMPLEMENT_DYNCREATE(CAcrolinxSidebarPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CAcrolinxSidebarPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAcrolinxSidebarPropPage, "ACROSIDEBAR.AcrolinxSidebarPropPage.1",
    0x69eba0d6, 0x63d7, 0x4a58, 0xae, 0xb3, 0xb5, 0x3e, 0xc2, 0xc5, 0x65, 0xf9)



// CAcrolinxSidebarPropPage::CAcrolinxSidebarPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CAcrolinxSidebarPropPage

BOOL CAcrolinxSidebarPropPage::CAcrolinxSidebarPropPageFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
        m_clsid, IDS_ACROLINXSIDEBARSDK_PPG);
    else
        return AfxOleUnregisterClass(m_clsid, nullptr);
}



// CAcrolinxSidebarPropPage::CAcrolinxSidebarPropPage - Constructor

CAcrolinxSidebarPropPage::CAcrolinxSidebarPropPage() :
COlePropertyPage(IDD, IDS_ACROLINXSIDEBARSDK_PPG_CAPTION)
{
}



// CAcrolinxSidebarPropPage::DoDataExchange - Moves data between page and properties

void CAcrolinxSidebarPropPage::DoDataExchange(CDataExchange* pDX)
{
    DDP_PostProcessing(pDX);
}



// CAcrolinxSidebarPropPage message handlers
