#pragma once
class CCOMInstanceMngr
{
public:
    CCOMInstanceMngr(void);
    ~CCOMInstanceMngr(void);
protected:
    BOOL m_isInstanceCreated;
public:
    BOOL Instantiate(void);
};

