// iclogin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "iclogin.h"
#include "icloginDlg.h"
#include "Configuration.h"
#include "InstanceChecker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char* const IC_VERSIONSTRING = "version 0.3.4";

/////////////////////////////////////////////////////////////////////////////
// CIcloginApp

BEGIN_MESSAGE_MAP(CIcloginApp, CWinApp)
	//{{AFX_MSG_MAP(CIcloginApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcloginApp construction

CLog g_log;

CIcloginApp::CIcloginApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIcloginApp object

CIcloginApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIcloginApp initialization

BOOL CIcloginApp::InitInstance()
{
	if(CInstanceChecker::AlreadyRunning())
	{
//		AfxMessageBox(_T("Previous version detected, will now restore it"), MB_OK);
		CInstanceChecker::ActivateOtherInstance();
		return FALSE;
	}

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	if(CConfiguration::GetLogToFile())
	{
		g_log.SetLogFile(CConfiguration::GetLogFile());
	}

	g_log.Log("IC Login started");

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	m_pMainWnd = new CIcloginDlg; // It destroys itself.

	if(m_pMainWnd)
	{
		((CIcloginDlg *)m_pMainWnd)->Create();
		if (!CInstanceChecker::AlreadyRunning())
				CInstanceChecker::SetOurselfAsRunning();
		if(CConfiguration::GetStartHidden())
		{
			m_pMainWnd->ShowWindow(SW_HIDE);
		}
		else
		{
			m_pMainWnd->ShowWindow(SW_SHOW);
		}

		return TRUE; // Enter message loop
	}
	else
	{
		return FALSE; // No window so we don't want to enter message loop
	}
}


