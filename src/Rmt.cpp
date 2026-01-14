// Rmt.cpp : Defines the class behaviors for the application.
// originally made by Raster, 2002-2009
// reworked by VinsCool, 2021-2022
//

#include "StdAfx.h"
#include "Rmt.h"
#include "MainFrm.h"
#include "RmtDoc.h"
#include "RmtView.h"
#include "Song.h"
#include "SongExporterTest.h"
#include "AboutDialog.h"
#include "GuiHelpers.h" // For SendErrorMessage
#include "RmtCommandLineInfo.h"
#include "Global.h"
#include "Shell.h"

#include "RmtTest.h"


// Activate MFC memory leak detection.
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void SetProgramFolderPath(const CString& folderPath); // See Global.cpp
extern CStatusBar* g_statusBar; // See GuiHelpers.cpp

// Some information for the about box is supplied by components outside this file
extern CString g_about6502;
extern CAtari g_Atari;
extern CAtariTrackerDriver* g_AtariTrackerDriver;
extern CXPokey g_Pokey;
extern CSong g_Song;

/////////////////////////////////////////////////////////////////////////////
// CRmtApp

BEGIN_MESSAGE_MAP(CRmtApp, CWinApp)

    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
    ON_COMMAND(ID_HELP_HELP_TOPICS, &CRmtApp::OnHelpHelpTopics)
    ON_COMMAND(ID_HELP_ONLINE_HELP, &CRmtApp::OnHelpOnlineHelp)
    ON_COMMAND(ID_HELP_ABOUT_APP, &CRmtApp::OnHelpAboutApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRmtApp construction

CRmtApp::CRmtApp() :CWinApp("RMT")
{
    // Place all significant initialization in InitInstance.
}

/////////////////////////////////////////////////////////////////////////////
// This declaration ensures that there is excatly one app instance.
// This should be the only static variable in the solution.

CRmtApp g_app;

/////////////////////////////////////////////////////////////////////////////
// CRmtApp initialization

BOOL CRmtApp::InitInstance()
{
    // Set the registry key under which our settings are stored.
    // This is a standard AFX feafture.
    // The subtree has this structure:
    // - RMT (the app name specifed in the constructor)
    // - RMT/Frame: Main window position and size.
    // - RMT/Recent File List: MRU list of files.
    // - RMT/Settings: Not used.
    SetRegistryKey(_T("RASTER Music Tracker"));

    // Initialize the COM library on the current thread and identifies the concurrency model as single-thread apartmen
    CoInitialize(NULL);

    // Register the application's document templates. Document templates
    // serve as the connection between documents, frame windows and views.

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CRmtDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CRmtView));
    AddDocTemplate(pDocTemplate);

    // Load standard INI file contents including MRU)
    LoadStdProfileSettings();

    // Determine program folder as base folder for resources.
    CString fullPath;
    auto pathLen = ::GetModuleFileName(NULL, fullPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
    fullPath.ReleaseBuffer(pathLen); // Note that ReleaseBuffer doesn't need a +1 for the null byte.
    auto nPos = fullPath.ReverseFind('\\');
    if (nPos != -1) {
        fullPath = fullPath.Left(nPos + 1);
    }
    SetProgramFolderPath(fullPath);


    // INITIAL 6502 INITIALIZATION (DLL)
    if (!g_Atari.Init())
    {
        g_Atari.DeInit();
        exit(1);
    }


    g_tuning.Initialize(g_Song.IsNTSC());
    g_tuningRatios.Initialize();

    // Intitilaize the Atari computer.
    g_Atari.Init(g_Song.IsNTSC());

    // Initialize Atari RMT routines.
    g_AtariTrackerDriver = new CAtariTrackerDriver(g_Atari);
    g_AtariTrackerDriver->LoadRMTRoutines(g_trackerDriverVersion);
    g_AtariTrackerDriver->Init();


    g_Song.ClearSong(8);

    // Parse the command line for standard shell commands, DDE, file open.
    CRmtCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);


    if (cmdInfo.IsTestFileSpecified()) {
        CRmtTest test;
        test.RunFor(*this, cmdInfo.GetTestFilePath());
        return FALSE;
    }

    // Dispatch the standard commands specified on the command line.
    // Will return FALSE if the app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo))
    {
        return FALSE;
    }

    // The one and only window has been initialized, so show and update it.
    auto mainFrame = (CMainFrame*)GetMainWnd();
    g_statusBar = &mainFrame->m_wndStatusBar;
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    // Initialize the random number based on the current time.
    srand((unsigned int)time(NULL));


    // Dispatch additional interactive commands specified on the command line.
    switch (cmdInfo.m_nShellCommand) {
    case CCommandLineInfo::FileOpen:
        g_Song.FileOpen(cmdInfo.m_strFileName, FALSE);
        break;
    }

    // Dispatch additional automatic commands specified on the command line.
    if (cmdInfo.IsScriptFileSpecified()) {
        CFile scriptFile;
        if (!scriptFile.Open(cmdInfo.GetScriptFilePath(), CFile::modeRead)) {
            SendErrorMessage("Invalid Command Line Parameter", "The script file \"" + scriptFile.GetFilePath() + "\" specified via the command line switch /SCRIPT cannot be opened for reading.");
            return FALSE;
        }
        CSongExporterTest::Test(g_Song);
        return FALSE;
    }

    return TRUE;
}

CString CRmtApp::GetVersionAndBuild() const {
    CString version;
    CString result;

    version.LoadString(IDS_RMTVERSION);

    result.Format("%s (%s %s)", version, __DATE__, __TIME__);
    return result;
}

void CRmtApp::OpenOnlineHelp() const {
    CShell::OpenFile("https://html-preview.github.io/?url=https://github.com/raster-atari-org/RASTER-Music-Tracker/blob/1.35/doc//rmt_en.html");
}

/////////////////////////////////////////////////////////////////////////////
// CRmtApp message handlers



void CRmtApp::OnHelpHelpTopics()
{
    CShell::OpenFile(GetResourceFilePath(std::filesystem::path("docs"), "rmt_en.html"));
}

void CRmtApp::OnHelpOnlineHelp()
{

    OpenOnlineHelp();
}

void CRmtApp::OnHelpAboutApp()
{
    CAboutDialog::Show(g_about6502, g_Pokey.GetPokey()->GetAbout());
}
