// ServiceMaster.cpp: implementation of the CServiceMaster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iclogin.h"
#include "ServiceMaster.h"
#include "Configuration.h"
#include "Iclogindlg.h"
#include "InstanceChecker.h"

#ifdef ICLOGIN_SERVICE

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceMaster::CServiceMaster()
: m_ssh(0), m_wnd(NULL)
{
}


CServiceMaster::~CServiceMaster()
{
}


bool CServiceMaster::InstallService()
{
	SC_HANDLE servicehandle, scm;
	scm = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, 
		SC_MANAGER_CREATE_SERVICE);
	if(!scm)
		return false;
	CString servicename = CConfiguration::GetServiceName();
	CString servicedisplayname = CConfiguration::GetServiceLongName();
	CString ourpath;
	BOOL rv = GetModuleFileName(NULL, ourpath.GetBuffer(1000), 1000);
	ourpath.ReleaseBuffer();
	if(!rv)
	{
		CloseServiceHandle(scm);
		return false;
	}
	CString ourpathquoted;
	ourpathquoted.Format("\"%s\" -service", ourpath);

	servicehandle = CreateService(
		scm,
		servicename,
		servicedisplayname,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,  // Maybe SERVICE_ERROR_IGNORE?
		ourpathquoted,
		NULL,
		NULL,
		NULL,
		NULL, // LocalSystem account
		NULL);
	if(!servicehandle)
	{
		TRACE("Couldn't install service: ");
		switch(GetLastError())
		{
		case ERROR_SERVICE_EXISTS:
			TRACE("It already exist\n");
			break;
		case ERROR_INVALID_PARAMETER:
			TRACE("Invalid parameter\n");
			break;
		case ERROR_DUP_NAME:
			TRACE("A service with that name already exists\n");
			break;
		default:
			TRACE("Unknown Reason\n");
		}

		CloseServiceHandle(scm);
		return false;
	}
	
	SetServiceDescription(servicehandle, 
		_T("IC Logins logs you in and keeps you logged in to Internet Cable"));

	VERIFY(CloseServiceHandle(servicehandle));
	VERIFY(CloseServiceHandle(scm));
	return true;
}

typedef BOOL (__stdcall *ChangeServiceConfig2Func)(SC_HANDLE hService, DWORD dwInfoLevel, LPVOID lpInfo);

bool CServiceMaster::SetServiceDescription(SC_HANDLE servicehandle, LPTSTR description)
{
	if(!IsWindows2000())
	{
		return false;
	}

	HINSTANCE advapi32dll = LoadLibrary(_T("advapi32.dll"));
	if(!advapi32dll)
	{
		return false;
	}

	ChangeServiceConfig2Func func;
#ifdef _UNICODE
	func = (ChangeServiceConfig2Func)GetProcAddress(advapi32dll, "ChangeServiceConfig2W");
#else
	func = (ChangeServiceConfig2Func)GetProcAddress(advapi32dll, "ChangeServiceConfig2A");
#endif
	if(!func)
	{
		return false;
	}
	SERVICE_DESCRIPTION sd;
	sd.lpDescription = description;
	
	return func(servicehandle, SERVICE_CONFIG_DESCRIPTION, &sd) != FALSE;

	FreeLibrary(advapi32dll);

}


bool CServiceMaster::RemoveService()
{
	SC_HANDLE servicehandle, scm;
	scm = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, 
		SC_MANAGER_CREATE_SERVICE);
	if(!scm)
		return false;

	servicehandle = OpenService(scm,
		CConfiguration::GetServiceName(),
		STANDARD_RIGHTS_REQUIRED);
	if(!servicehandle)
	{
		VERIFY(CloseServiceHandle(scm));
		return false;
	}

	BOOL rv = DeleteService(servicehandle);
#ifdef _DEBUG
	if(!rv)
	{
		switch(GetLastError())
		{
		case ERROR_ACCESS_DENIED:
			TRACE("Error deleting service: ACCESS DENIED\n");
			break;
		case ERROR_INVALID_HANDLE:
			TRACE("Error deleting service: INVALID_HANDLE\n");
			break;
		case ERROR_SERVICE_MARKED_FOR_DELETE:
			TRACE("Error deleting service: Already marked for deletion\n");
			break;
		default:
			TRACE("Error deleting service: Unknown reason.\n");
			break;
		}
	}
#endif
	if(!rv && GetLastError()==ERROR_SERVICE_MARKED_FOR_DELETE)
	{
		rv = TRUE;
	}

	VERIFY(CloseServiceHandle(scm));
	CloseServiceHandle(servicehandle);
	return rv != FALSE;
}


bool CServiceMaster::IsInstalled()
{
	SC_HANDLE servicehandle, scm;
	scm = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, 
		SC_MANAGER_CREATE_SERVICE);
	if(!scm)
		return false;

	servicehandle = OpenService(scm,
		CConfiguration::GetServiceName(),
		READ_CONTROL);

	if(!servicehandle)
	{
		if(GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			VERIFY(CloseServiceHandle(scm));
			return false;
		}

		ASSERT(false);
		// Else return false.
		VERIFY(CloseServiceHandle(scm));
		return false;
	}

	VERIFY(CloseServiceHandle(scm));
	CloseServiceHandle(servicehandle);
	return true;
}


bool CServiceMaster::RegisterUsAsRunningService()
{
	g_log.Log("Entering RegisterUsAsRunningService()", CLog::LOG_DUMP);

	g_log.Log("RegisterServiceCtrlHandler", CLog::LOG_DUMP);
	m_ssh = RegisterServiceCtrlHandler(
		CConfiguration::GetServiceName(), 
		ServiceHandler);

	if(!m_ssh)
	{
		g_log.Log("failed", CLog::LOG_DUMP);
		switch(GetLastError())
		{
		case ERROR_INVALID_NAME:
			g_log.Log("error_invalid_name", CLog::LOG_DUMP);
			break;
		case ERROR_SERVICE_DOES_NOT_EXIST:
			g_log.Log("ERROR_SERVICE_DOES_NOT_EXIST", CLog::LOG_DUMP);
			break;
		default:
			CString error;
			error.Format("Error %d", GetLastError());
			g_log.Log(error, CLog::LOG_DUMP);
		}
		return false;
	}
	g_log.Log("succeeded", CLog::LOG_DUMP);

	SetStatus(SERVICE_RUNNING);
	g_log.Log("Leaving RegisterUsAsRunningService()", CLog::LOG_DUMP);
	return true;
}

bool CServiceMaster::SetStatus(int status)
{
	SERVICE_STATUS servicestatus;
	
	servicestatus.dwCurrentState       = status; 
	servicestatus.dwCheckPoint         = 0; 
	servicestatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	servicestatus.dwWaitHint           = 2000;  // 2000 ms
	servicestatus.dwWin32ExitCode      = 0; 
	servicestatus.dwServiceSpecificExitCode = 0; 
	servicestatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | 
		SERVICE_ACCEPT_SHUTDOWN;

	switch(status)
	{
	case SERVICE_START_PENDING:
	case SERVICE_STOP_PENDING:
	case SERVICE_STOPPED:
		servicestatus.dwControlsAccepted = 0;
	}

	BOOL rv = SetServiceStatus (m_ssh, &servicestatus); 
	if(!rv)
	{
		CString mess;
		switch(GetLastError())
		{
		case ERROR_INVALID_HANDLE:
			mess = "SetServiceStatus failed: ERROR_INVALID_HANDLE";
			break;
		case ERROR_INVALID_DATA:
			mess = "SetServiceStatus failed: ERROR_INVALID_DATA";
			break;
		default:
			mess.Format("SetServiceStatus failed: error %d", GetLastError());
		} 
		g_log.Log(mess, CLog::LOG_ERROR);

		return false;
	}

	g_log.Log("SetServiceStatus succeeded", CLog::LOG_DUMP);
	
	return true; 
}

bool CServiceMaster::SetServiceFunctions()
{
	g_log.Log("Entering SetServiceFunctions()", CLog::LOG_DUMP);
    SERVICE_TABLE_ENTRY   dispatchtable[] = 
    { 
        { "IC Login Service",  ICLoginServiceMain      }, 
        { NULL,              NULL          } 
	};
	// The string is ignored since we are running in an own process

	if(!StartServiceCtrlDispatcher(dispatchtable))
	{
		CString mess;
		mess.Format("StartServiceCtrlDispatcher failed with code %d", GetLastError());
		g_log.Log(mess, CLog::LOG_ERROR);
	}

	g_log.Log("Leaving SetServiceFunctions()", CLog::LOG_DUMP);
	return true;
}


bool CServiceMaster::IsRunning()
{
	if(!IsWindowsNT())
		return false;

	SC_HANDLE servicehandle, scm;
	scm = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, 
		SC_MANAGER_CREATE_SERVICE);
	if(!scm)
		return false;

	servicehandle = OpenService(scm,
		CConfiguration::GetServiceName(),
		READ_CONTROL | SERVICE_QUERY_STATUS);

	if(!servicehandle)
	{
		VERIFY(CloseServiceHandle(scm));
		return false;
	}

	bool return_value = false;
	SERVICE_STATUS ss;
	if(QueryServiceStatus(servicehandle, &ss))
	{
		if(ss.dwCurrentState != SERVICE_STOPPED)
		{
			return_value = true;
		}
	}

	VERIFY(CloseServiceHandle(scm));
	VERIFY(CloseServiceHandle(servicehandle));
	return return_value;
}



//SERVICE_STATUS_HANDLE CServiceMaster::GetServiceStatusHandle()
//{
//}

CServiceMaster& CServiceMaster::GetServiceMaster()
{
	static CServiceMaster sm;
	return sm;
}



void WINAPI ICLoginServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	g_log.Log("Entering ICLoginServiceMain()", CLog::LOG_DUMP);
	CString message;
	message.Format("dwArgc = %d", dwArgc);
	g_log.Log(message, CLog::LOG_DUMP);
	for(unsigned int i=0; i<dwArgc; i++)
	{
		message.Format("String %d: '%s'", i, lpszArgv[i]);
		g_log.Log(message, CLog::LOG_DUMP);
	}

	CServiceMaster &sm = CServiceMaster::GetServiceMaster();
	sm.RegisterUsAsRunningService(); // Will set the status as running.

	CIcloginDlg *mainwnd = new CIcloginDlg(NULL, true); // It destroys itself.

	if(mainwnd)
	{
		mainwnd->Create();
		if (!CInstanceChecker::AlreadyRunning())
				CInstanceChecker::SetOurselfAsRunning();
		if(CConfiguration::GetStartHidden())
		{
			mainwnd->ShowWindow(SW_HIDE);
		}
		else
		{
			mainwnd->ShowWindow(SW_SHOW);
		}
		
		sm.SetWnd(mainwnd);

		// Enter main message loop
		MSG current_message;
		for(;;)
		{
			int rv = ::GetMessage(&current_message, NULL, NULL, NULL);
			if(!rv)
			{
				// WM_QUIT
				g_log.Log("Service got quitrequest", CLog::LOG_INFO);
				break;
			}
			if(rv == -1)
			{
				TRACE("ERROR IN MESSAGELOOP!!!!\n");
				break;
			}

//	if (m_msgCur.message != WM_KICKIDLE && !PreTranslateMessage(&m_msgCur))
			if(!::IsWindow(*mainwnd) || !mainwnd->PreTranslateMessage(&current_message))
			{
				::TranslateMessage(&current_message);
				::DispatchMessage(&current_message);
			}
		}

		// The only way we can quit is if the systen sends a STOP request
		// and in that case the dialog is ended which in turn causes
		// PostQuitMessage() to be called, so the window should really
		// have been killed here.
//		if(::IsWindow(*mainwnd))
//		{
//			mainwnd->EndDialog(0);
//		}
	}

	sm.SetStatus(SERVICE_STOPPED);

	g_log.Log("Leaving ICLoginServiceMain()", CLog::LOG_DUMP);
}


VOID WINAPI ServiceHandler(DWORD dwCtrlCode)
{
	CServiceMaster &sm = CServiceMaster::GetServiceMaster();

	CString message;
	CWnd *wnd = sm.GetWnd(); 
	switch(dwCtrlCode)
	{
	case SERVICE_CONTROL_STOP:
		sm.SetStatus(SERVICE_STOP_PENDING);
		message = "They wants us to stop. :-)";
		wnd->PostMessage(WM_COMMAND, IDOK, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		sm.SetStatus(SERVICE_RUNNING);
		message = "They wants us to pause.";
		break;
	case SERVICE_CONTROL_CONTINUE:
		sm.SetStatus(SERVICE_RUNNING);
		message = "They wants us to resume.";
		break;
	case SERVICE_CONTROL_INTERROGATE:
		sm.SetStatus(SERVICE_RUNNING);
		message = "They wants us to tell them what we're doing.";
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		sm.SetStatus(SERVICE_STOP_PENDING);
		message = "System shutting down.";
		wnd->PostMessage(WM_COMMAND, IDOK, NULL);
		break;
	default:
		sm.SetStatus(SERVICE_RUNNING);
		message.Format("Code: %d", dwCtrlCode);
	}
	g_log.Log(message, CLog::LOG_DUMP);
}


#endif // ICLOGIN_SERVICE