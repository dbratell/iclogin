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

const char* const LOGINHOSTKEY = _T("LoginHost");
const char* const USERNAMEKEY = _T("Username");
const char* const PASSWORDKEY = _T("Password");
const char* const LOGINATINTERVALKEY = _T("LoginAtInterval");
const char* const LOGININTERVALKEY = _T("LoginInterval");
const char* const LOGINFAILEDINTERVALKEY = _T("LoginFailedInterval");
const char* const UPDATETIMERSINTERVALKEY = _T("UpdateTimersInterval");
const char* const LOGINATSTARTUPKEY = _T("LoginAtStartup");
const char* const HIDEPASSWORDKEY = _T("HidePassword");
const char* const STARTHIDDENKEY = _T("StartHidden");
const char* const ISCONFIGUREDKEY = _T("IsConfigured");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfiguration::CConfiguration()
{

}

CConfiguration::~CConfiguration()
{

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
	return GetIntData(LOGININTERVALKEY, 60);
}

void CConfiguration::SetLoginFailedInterval(int interval)
{
	SetIntData(LOGININTERVALKEY, interval);
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



int CConfiguration::GetIntData(const CString& key, int default_data)
{
	int rv = default_data; // Default;
	CRegKey regkey1;
	if(regkey1.Create(HKEY_LOCAL_MACHINE, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Create(regkey1, _T("Bratell")) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, _T("IC Login")) == ERROR_SUCCESS)
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
		if(regkey2.Create(regkey1, _T("Bratell")) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, _T("IC Login")) == ERROR_SUCCESS)
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
		if(regkey2.Create(regkey1, _T("Bratell")) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, _T("IC Login")) == ERROR_SUCCESS)
			{
				DWORD data_size;
				if(regkey3.QueryValue(NULL, key, &data_size) == ERROR_SUCCESS)
				{
					if(data_size > 0)
					{
						char *buffer = new char[data_size];
						if(buffer)
						{
							if(regkey3.QueryValue(buffer, key, &data_size) == ERROR_SUCCESS)
							{
								rv = buffer;
							}
						}
						delete[] buffer;
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
		if(regkey2.Create(regkey1, _T("Bratell")) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Create(regkey2, _T("IC Login")) == ERROR_SUCCESS)
			{
				if(regkey3.SetValue(value, key) != ERROR_SUCCESS)
				{
					ASSERT(false);
				}
			}
		}
	}
}