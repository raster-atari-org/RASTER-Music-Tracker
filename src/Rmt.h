// Rmt.h : main header file for the RMT application
//
// The following style guides are applied in the source:
// 
// - Interface names start with "I", example "ISong" and are in camel case.
// - Class names start with "C", example "CSong" and are in camel case.
// - Structure type names start with "T", "TViewState" and are in camel case.
// - Enum type name start with an uppercase letter and are in camel case.

#include "StdAfx.h"

#if !defined(AFX_RMT_H__1709C745_06D0_11D7_BEB0_00600854AFCA__INCLUDED_)
#define AFX_RMT_H__1709C745_06D0_11D7_BEB0_00600854AFCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRmtApp:
//

class CRmtApp : public CWinApp
{
public:
    CRmtApp();

    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CRmtApp)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

    CString GetVersionAndBuild() const;
    void OpenOnlineHelp() const;


// Implementation
    //{{AFX_MSG(CRmtApp)

    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    afx_msg void OnHelpHelpTopics();
    afx_msg void OnHelpOnlineHelp();
    afx_msg void OnHelpAboutApp();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RMT_H__1709C745_06D0_11D7_BEB0_00600854AFCA__INCLUDED_)
