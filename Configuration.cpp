// Configuration.cpp: implementation of the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iclogin.h"
#include "Configuration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const _TCHAR* const LOGINHOSTKEY = _T("LoginHost");
const _TCHAR* const USERNAMEKEY = _T("Username");
const _TCHAR* const PASSWORDKEY = _T("Password");
const _TCHAR* const LOGINATINTERVALKEY = _T("LoginAtInterval");
const _TCHAR* const LOGININTERVALKEY = _T("LoginInterval");
const _TCHAR* const LOGINFAILEDINTERVALKEY = _T("LoginFailedInterval");
const _TCHAR* const UPDATETIMERSINTERVALKEY = _T("UpdateTimersInterval");
const _TCHAR* const LOGINATSTARTUPKEY = _T("LoginAtStartup");
const _TCHAR* const HIDEPASSWORDKEY = _T("HidePassword");
const _TCHAR* const STARTHIDDENKEY = _T("StartHidden");
const _TCHAR* const ISCONFIGUREDKEY = _T("IsConfigured");
const _TCHAR* const AUTOSTARTKEY = _T("AutoStart");
const _TCHAR* const LOGTOFILEKEY = _T("LogToFile");
const _TCHAR* const LOGFILEKEY = _T("LogFile");
const _TCHAR* const LOGLEVELKEY = _T("LogLevel");

const _TCHAR* const PROGRAMNAMEINRUN = _T("IC Login");

const _TCHAR* const COMPANYKEY = _T("Sarasas");
const _TCHAR* const APPKEY = _T("IC Login");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#if 0

CConfiguration::CConfiguration()
{

}

CConfiguration::~CConfiguration()
{

}

#endif

void  CConfiguration::CleanRegistry()
{
	SetAutoStart(false); // To remove us from the Run key.

	CRegKey regkey1;
	if(regkey1.Create(HKEY_LOCAL_MACHINE, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Create(regkey1, COMPANYKEY) == ERROR_SUCCESS)
		{
			regkey2.RecurseDeleteKey(APPKEY);
			// Should check if COMPANYKEY is empty and delete it too
			// in that case.

			CString dummy;
			if(RegEnumKey(regkey2, 0, dummy.GetBuffer(MAX_PATH+1), MAX_PATH+1) == 
				ERROR_NO_MORE_ITEMS)
			{
				if(regkey2.Close() == ERROR_SUCCESS)
				{
					// Delete the COMPANYKEY too
					regkey1.DeleteSubKey(COMPANYKEY);
				}
			}
		}
	}
}


/**
 * The time in seconds between two logins
 */
const int CConfiguration::GetLoginInterval()
{
	return GetIntData(LOGININTERVALKEY, 600);
}

void CConfiguration::SetLoginInterval(int interval)
{
	SetIntData(LOGININTERVALKEY, interval);
}

/**
 * The time in seconds between two logins if previous attempt failed
 */
const int CConfiguration::GetLoginFailedInterval()
{
	return GetIntData(LOGINFAILEDINTERVALKEY, 45);
}

void CConfiguration::SetLoginFailedInterval(int interval)
{
	SetIntData(LOGINFAILEDINTERVALKEY, interval);
}


/**
 * The time in seconds between two updates of the display on screen
 */
const int CConfiguration::GetUpdateTimersInterval()
{
	return GetIntData(UPDATETIMERSINTERVALKEY, 5);
}

void CConfiguration::SetUpdateTimersInterval(int interval)
{
	SetIntData(UPDATETIMERSINTERVALKEY, interval);
}

const bool CConfiguration::GetLoginAtInterval()
{
	return (GetIntData(LOGINATINTERVALKEY, 1) != 0);
}


void CConfiguration::SetLoginAtInterval(bool do_it)
{
	if(do_it)
		SetIntData(LOGINATINTERVALKEY, 1);
	else
		SetIntData(LOGINATINTERVALKEY, 0);
		
}

const bool CConfiguration::GetStartHidden()
{
	return (GetIntData(STARTHIDDENKEY, 1) != 0);
}


void CConfiguration::SetStartHidden(bool do_it)
{
	if(do_it)
		SetIntData(STARTHIDDENKEY, 1);
	else
		SetIntData(STARTHIDDENKEY, 0);
}

const bool CConfiguration::GetIsConfigured()
{
	return (GetIntData(ISCONFIGUREDKEY, 0) != 0);
}


void CConfiguration::SetIsConfigured(bool do_it)
{
	if(do_it)
		SetIntData(ISCONFIGUREDKEY, 1);
	else
		SetIntData(ISCONFIGUREDKEY, 0);
}


const CString CConfiguration::GetLoginHost()
{
	return GetStringData(LOGINHOSTKEY, _T("login1.telia.com"));
}


void CConfiguration::SetLoginHost(const CString &host)
{
	SetStringData(LOGINHOSTKEY, host);
}


const bool CConfiguration::GetLoginAtStartup()
{
	return (GetIntData(LOGINATSTARTUPKEY, 1) != 0);
}

void CConfiguration::SetLoginAtStartup(const bool do_it)
{
	if(do_it)
		SetIntData(LOGINATSTARTUPKEY, 1);
	else
		SetIntData(LOGINATSTARTUPKEY, 0);
}


const CString CConfiguration::GetUsername()
{
	return GetStringData(USERNAMEKEY, _T(""));
	// return "u13305151";
}


void CConfiguration::SetUsername(const CString &name)
{
	SetStringData(USERNAMEKEY, name);
}


const CString CConfiguration::GetPassword()
{
	return GetStringData(PASSWORDKEY, _T(""));
// 	return "Hv6YcfJD"; // password XXX (should be encoded)
}


void CConfiguration::SetPassword(const CString &password)
{
	SetStringData(PASSWORDKEY, password);
}



const bool CConfiguration::GetHidePassword()
{
	return (GetIntData(HIDEPASSWORDKEY, 0) != 0);
}


void CConfiguration::SetHidePassword(const bool do_it)
{
	if(do_it)
		SetIntData(HIDEPASSWORDKEY, 1);
	else
		SetIntData(HIDEPASSWORDKEY, 0);
}

const bool CConfiguration::GetLogToFile()
{
	return (GetIntData(LOGTOFILEKEY, 0) != 0);
}


void CConfiguration::SetLogToFile(const bool do_it)
{
	if(do_it)
		SetIntData(LOGTOFILEKEY, 1);
	else
		SetIntData(LOGTOFILEKEY, 0);
}


const CString CConfiguration::GetLogFile()
{
	return GetStringData(LOGFILEKEY, _T("iclogin.log"));
}


void CConfiguration::SetLogFile(const CString &filename)
{
	SetStringData(LOGFILEKEY, filename);
}

const int CConfiguration::GetLogLevel()
{
	return GetIntData(LOGLEVELKEY, CLog::LOG_INFO);
}

void CConfiguration::SetLogLevel(int loglevel)
{
	SetIntData(LOGLEVELKEY, loglevel);
}


const bool CConfiguration::GetAutoStart()
{
	// Could also check the run key.
	return (GetIntData(AUTOSTARTKEY, 0) != 0);
}

void CConfiguration::SetAutoStart(const bool do_it)
{
	CRegKey regkey1;
	if(regkey1.Create(HKEY_CURRENT_USER, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Create(regkey1, _T("Microsoft")) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, _T("Windows")) == ERROR_SUCCESS)
				{
				CRegKey regkey4;
				if(regkey4.Create(regkey3, _T("CurrentVersion")) == ERROR_SUCCESS)
				{
					CRegKey regkey5;
					if(regkey5.Create(regkey4, _T("Run")) == ERROR_SUCCESS)
					{
						if(do_it)
						{
							CString ourpath;
							if(GetModuleFileName(NULL, ourpath.GetBuffer(1000), 1000))
							{
								regkey5.SetValue(ourpath, PROGRAMNAMEINRUN);
							}

							SetIntData(AUTOSTARTKEY, 1);
						}
						else
						{
							SetIntData(AUTOSTARTKEY, 0);
							regkey5.DeleteValue(PROGRAMNAMEINRUN);
						}
					}
				}
			}
		}
	}
}



int CConfiguration::GetIntData(const CString& key, int default_data)
{
	int rv = default_data; // Default;
	CRegKey regkey1;
	if(regkey1.Create(HKEY_LOCAL_MACHINE, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Create(regkey1, COMPANYKEY) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, APPKEY) == ERROR_SUCCESS)
			{
				DWORD data_in_reg;
				if(regkey3.QueryValue(data_in_reg, key) == ERROR_SUCCESS)
				{
					rv = data_in_reg;
				}
			}
		}
	}
	return rv; 
}



void CConfiguration::SetIntData(const CString& key, int value)
{
	CRegKey regkey1;
	if(regkey1.Create(HKEY_LOCAL_MACHINE, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Create(regkey1, COMPANYKEY) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, APPKEY) == ERROR_SUCCESS)
			{
				if(regkey3.SetValue(value, key) != ERROR_SUCCESS)
				{
					ASSERT(false);
				}
			}
		}
	}
}



CString CConfiguration::GetStringData(const CString& key, const CString& default_data)
{
	CString rv = default_data; // Default;
	CRegKey regkey1;
	if(regkey1.Create(HKEY_LOCAL_MACHINE, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Create(regkey1, COMPANYKEY) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, APPKEY) == ERROR_SUCCESS)
			{
				DWORD data_size;
				if(regkey3.QueryValue(NULL, key, &data_size) == ERROR_SUCCESS)
				{
					if(data_size > 0)
					{
						regkey3.QueryValue(rv.GetBuffer(data_size), key, &data_size);
					}
				}
			}
		}
	}
	return rv; 
}

void CConfiguration::SetStringData(const CString& key, const CString& value)
{
	CRegKey regkey1;
	if(regkey1.Create(HKEY_LOCAL_MACHINE, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Create(regkey1, COMPANYKEY) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, APPKEY) == ERROR_SUCCESS)
			{
				if(regkey3.SetValue(value, key) != ERROR_SUCCESS)
				{
					ASSERT(false);
				}
			}
		}
	}
}