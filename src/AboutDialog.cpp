#include "StdAfx.h"
#include "AboutDialog.h"
#include "resource.h"
#include "Rmt.h"
#include "Shell.h"

extern CRmtApp g_app;

CAboutDialog::CAboutDialog() : CDialog(CAboutDialog::IDD)
{
    //{{AFX_DATA_INIT(CAboutDialog)
    m_rmtversion = _T("");
    m_rmtauthor = _T("");
    m_about6502 = _T("");
    m_aboutpokey = _T("");
    //}}AFX_DATA_INIT
}

void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDialog)
    DDX_Text(pDX, IDC_RMT_VERSION, m_rmtversion);
    DDX_Text(pDX, IDC_RMT_AUTHOR, m_rmtauthor);
    DDX_Text(pDX, IDC_RMT_REPOSITORY, m_rmtrepository);
    DDX_Text(pDX, IDC_ABOUT6502, m_about6502);
    DDX_Text(pDX, IDC_ABOUTPOKEY, m_aboutpokey);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
    //{{AFX_MSG_MAP(CAboutDialog)
        // No message handlers
    //}}AFX_MSG_MAP
    ON_STN_CLICKED(IDC_RMT_REPOSITORY, &CAboutDialog::OnStnClickedRmtRepository)
END_MESSAGE_MAP()

void CAboutDialog::Show(const CString& about6502, const CString& aboutPokey)
{
    CAboutDialog aboutDlg;
    aboutDlg.m_rmtversion = g_app.GetVersionAndBuild();
    aboutDlg.m_rmtauthor.LoadString(IDS_RMT_AUTHOR);
    aboutDlg.m_rmtrepository.LoadString(IDS_RMT_REPOSITORY);
    aboutDlg.m_aboutpokey = aboutPokey;
    aboutDlg.m_aboutpokey.Replace("\n", "\x0d\x0a");
    aboutDlg.m_about6502 = about6502;
    aboutDlg.m_about6502.Replace("\n", "\x0d\x0a");

    aboutDlg.DoModal();
}

void CAboutDialog::OnStnClickedRmtRepository()
{
    CShell::OpenFile(m_rmtrepository);
}


