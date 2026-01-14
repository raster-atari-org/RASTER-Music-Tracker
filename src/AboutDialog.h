#pragma once

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog used for App About

class CAboutDialog : public CDialog
{
public:
    CAboutDialog();

    // Dialog Data
        //{{AFX_DATA(CAboutDialog)
    enum { IDD = IDD_ABOUTBOX };
    CString	m_rmtversion;
    CString	m_rmtauthor;
    CString	m_rmtrepository;
    CString	m_credits;
    CString	m_about6502;
    CString	m_aboutpokey;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDialog)

    static void Show(const CString& about6502, const CString& aboutPokey);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDialog)
        // No message handlers
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnStnClickedRmtRepository();
};

