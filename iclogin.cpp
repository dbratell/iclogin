/**
 * This file is a part of IC Login (c) Daniel Bratell 2000
 *
 * That the source code is publicized on Internet does not mean that
 * it's free to use. You are allowed to be inspired by it, but if you
 * do patches of your own you must submit them to the original author.
 *
 * Also, you mustn't redistribute the source code. If you want to
 * make other people see it, point them to 
 * 
 * http://www.lysator.liu.se/~bratell/iclogin/
 *
 * Original Author: Daniel Bratell <bratell@lysator.liu.se>
 */

// iclogin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "iclogin.h"
#include "icloginDlg.h"
#include "Configuration.h"
#include "InstanceChecker.h"
#include "ServiceMaster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const _TCHAR* const IC_VERSIONSTRING = _T("version 1.0.1pre");
const _TCHAR* const IC_APPURL = _T("http://www.lysator.liu.se/~bratell/iclogin/");


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
		// Can't log this since the other instance holds
		// on to the log file.
		CInstanceChecker::ActivateOtherInstance();
		return FALSE;
	} 

	g_log.SetLogLevel(CConfiguration::GetLogLevel());
	if(CConfiguration::GetLogToFile())
	{
		g_log.SetLogFile(CConfiguration::GetLogFile());
	}

	g_log.Log("IC Login started");
	

	// Update registry
	// There is only two versions right now. 0 and 1.
	if(CConfiguration::GetRegistryConfigurationVersion() < 1)
	{
		// Update password
		CString password = CConfiguration::GetPassword();
		CConfiguration::SetRegistryConfigurationVersionToCurrent();
		CConfiguration::SetPassword(password);
	}


#ifdef ICLOGIN_SERVICE
	if(IsWindowsNT())
	{
		CString commandline(m_lpCmdLine);
		if(commandline == _T("-install"))
		{
			CServiceMaster::RemoveService();
			CServiceMaster::InstallService();
			return false;
		}
		else if(commandline == _T("-remove"))
		{
			CServiceMaster::RemoveService();
			return false;
		}
		else if(commandline == _T("-service"))
		{
			g_log.Log("Run as service", CLog::LOG_DUMP);
			StartAsService();
			return false;
	//		AfxMessageBox("Run as service!");
		}
	}

	// We are not started as a service
	if(CServiceMaster::IsRunning())
	{
		AfxMessageBox(IDS_SERVICERUNNING);
		return false;
	}


#endif
	

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

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


void CIcloginApp::StartAsService()
{
	g_log.Log("Entering StartAsService()", CLog::LOG_DUMP);
	CServiceMaster::SetServiceFunctions();
	g_log.Log("Leaving StartAsService()", CLog::LOG_DUMP);
	return;
}


/**
 * To see if there are such a thing as "services"
 */
bool IsWindowsNT()
{
	static bool cached = false;
	static bool isnt;

	if(!cached) 
	{
		OSVERSIONINFO osvi;
		memset(&osvi, 0 , sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof (osvi);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			 return false;

		isnt = osvi.dwPlatformId == VER_PLATFORM_WIN32_NT;
		cached = true;
	}
	return isnt;
}

/**
 * If Windows 2000 (or better)
 */
bool IsWindows2000()
{
	static bool cached = false;
	static bool is2000;

	if(!cached) 
	{
		OSVERSIONINFO osvi;
		memset(&osvi, 0 , sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof (osvi);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			 return false;

		is2000 = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT &&
			osvi.dwMajorVersion>=5);
		cached = true;
	}
	return is2000;
}



