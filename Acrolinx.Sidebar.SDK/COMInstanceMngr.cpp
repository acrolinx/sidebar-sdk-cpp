#include "StdAfx.h"
#include "COMInstanceMngr.h"


CCOMInstanceMngr::CCOMInstanceMngr(void)
    : m_isInstanceCreated(FALSE)
{
}


CCOMInstanceMngr::~CCOMInstanceMngr(void)
{
}


BOOL CCOMInstanceMngr::Instantiate(void)
{
    if(m_isInstanceCreated)
    {
        ACROASSERT(m_isInstanceCreated == FALSE, "Instance already exists");
        return FALSE;
    }
    else
    {
        m_isInstanceCreated = TRUE;
        return TRUE;
    }
}
