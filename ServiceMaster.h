// ServiceMaster.h: interface for the CServiceMaster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEMASTER_H__585CFD3A_7B7F_49FA_9498_D289FF657AA0__INCLUDED_)
#define AFX_SERVICEMASTER_H__585CFD3A_7B7F_49FA_9498_D289FF657AA0__INCLUDED_

#ifdef ICLOGIN_SERVICE

#include <winsvc.h>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

VOID WINAPI ServiceHandler(DWORD dwCtrlCode);
void WINAPI ICLoginServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);

class CServiceMaster  
{
public:
	CServiceMaster();
	virtual ~CServiceMaster();

	static bool InstallService();
	static bool RemoveService();
	static bool IsInstalled();
	bool RegisterUsAsRunningService();
	static bool SetServiceFunctions();
	bool SetStatus(int status);

private:
	// Attributes
//	SC_HANDLE m_scm;
	SERVICE_STATUS_HANDLE m_ssh;
//  SERVICE_STATUS m_servicestatus;
private:
	static bool SetServiceDescription(SC_HANDLE servicehandle, LPTSTR description);

};

#endif // ICLOGIN_SERVICE
#endif // !defined(AFX_SERVICEMASTER_H__585CFD3A_7B7F_49FA_9498_D289FF657AA0__INCLUDED_)
