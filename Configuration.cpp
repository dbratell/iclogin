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

// Configuration.cpp: implementation of the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iclogin.h"
#include "Configuration.h"
#include "ServiceMaster.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CONFIGURATIONVERSION 1

const _TCHAR* const CONFIGURATIONVERSIONKEY = _T("ConfigurationVersion");
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
const _TCHAR* const MINIMIZEMEMORYUSAGEKEY = _T("MinimizeMemoryUsage");
const _TCHAR* const RESTARTDHCPWHENNEEDEDKEY = _T("RestartDHCPWhenNeeded");
const _TCHAR* const LOGOUTTIMEBEFORERESTARTKEY = _T("LogoutTimeBeforeRestartDHCP");
const _TCHAR* const LOGTOFILEKEY = _T("LogToFile");
const _TCHAR* const LOGFILEKEY = _T("LogFile");
const _TCHAR* const LOGLEVELKEY = _T("LogLevel");
const _TCHAR* const NEVERUSEPROXYKEY = _T("NeverUseProxy");
const _TCHAR* const EXTENDEDLOGGEDINKEY = _T("ExtendedLoggedInTest");
#ifdef ICLOGIN_SERVICE
const _TCHAR* const SERVICENAMEKEY = _T("ServiceName");
const _TCHAR* const SERVICEDISPLAYNAMEKEY = _T("ServiceDisplayName");
const _TCHAR* const VISIBLEASSERVICE = _T("VisibleAsService");
const _TCHAR* const RUNASSERVICE = _T("RunAsService");
const _TCHAR* const SERVICEPOPUPERROR = _T("ServicePopupError");
#endif

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
			bool rv = RegEnumKey(regkey2, 0, dummy.GetBuffer(MAX_PATH+1), MAX_PATH+1) == 
				ERROR_NO_MORE_ITEMS;
			dummy.ReleaseBuffer();
			if(rv)
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


const int CConfiguration::GetRegistryConfigurationVersion()
{
	return GetIntData(CONFIGURATIONVERSIONKEY, 0);
}

void CConfiguration::SetRegistryConfigurationVersionToCurrent()
{
	SetIntData(CONFIGURATIONVERSIONKEY, CONFIGURATIONVERSION);
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
	return GetIntData(LOGINFAILEDINTERVALKEY, 30);
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
	SetIntData(LOGINATINTERVALKEY, do_it?1:0);
}

const bool CConfiguration::GetStartHidden()
{
	return (GetIntData(STARTHIDDENKEY, 1) != 0);
}


void CConfiguration::SetStartHidden(bool do_it)
{
	SetIntData(STARTHIDDENKEY, do_it?1:0);
}

const bool CConfiguration::GetIsConfigured()
{
	return (GetIntData(ISCONFIGUREDKEY, 0) != 0);
}


void CConfiguration::SetIsConfigured(bool do_it)
{
	SetIntData(ISCONFIGUREDKEY, do_it?1:0);
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
	SetIntData(LOGINATSTARTUPKEY, do_it?1:0);
}


const CString CConfiguration::GetUsername()
{
	return GetStringData(USERNAMEKEY, _T(""));
}


void CConfiguration::SetUsername(const CString &name)
{
	SetStringData(USERNAMEKEY, name);
}


/**
 * A little obscured. Just to make it unreadable by the casual eye.
 * This is by no means secure. I just liked playing with numbers
 * which is the reason it looks so complicated. (A XOR or 
 * Base64-encode would have done) 
 */
const CString CConfiguration::GetPassword()
{
	CString password = GetStringData(PASSWORDKEY, _T(""));
	if(GetRegistryConfigurationVersion()<1)
	{
		return password;
	}

	// Must be a multiple of three
	if(password.GetLength() % 3)
	{
		return "";
	}

	if(password.GetLength() > 90)
	{
		// Could cause trouble
		return "";
	}

	CString decoded_password;
	for(int i = 0; i < password.GetLength()/3; i++)
	{
		// First three bits
		unsigned char first = (unsigned char)(password[3*i]-i);
		if(first<'a' || first>('a'+7))
		{
			return "";
		}

		// Second two bits
		unsigned char second = (unsigned char)(password[3*i+1]-i);
		if(second<'g' || second > ('g'+3))
		{
			return "";
		}

		// The last three bits
		unsigned char third = (unsigned char)(password[3*i+2]-i);
		if(third<'b' || third > ('b'+7))
		{
			return "";
		}

		unsigned char decoded_char = 
			(unsigned char)((first-'a')<<5 | 
			((second-'g')<<3) | 
			(third-'b'));
			
		decoded_password += decoded_char;
	}

	return decoded_password;
}


/**
 * A little obscured. Just to make it unreadable by the casual eye.
 * This is by no means secure.
 */
void CConfiguration::SetPassword(const CString &password)
{

    // Easy way to "see" password with this test. If the password
	// was encrypted this would be a security hole.
	if(GetRegistryConfigurationVersion()<1)
	{
		SetStringData(PASSWORDKEY, password);
	}
	
	CString obscured_password;

	for(int i = 0; i < password.GetLength(); i++)
	{
		char letter = password[i];
		obscured_password += (unsigned char)(((letter & 0xe0) >> 5) + 'a'+i);
		obscured_password += (unsigned char)(((letter & 0x18) >> 3) + 'g'+i);
		obscured_password += (unsigned char)((letter & 0x07) + 'b'+i);
	}

	SetStringData(PASSWORDKEY, obscured_password);
}



const bool CConfiguration::GetHidePassword()
{
	return (GetIntData(HIDEPASSWORDKEY, 0) != 0);
}


void CConfiguration::SetNeverUseProxy(const bool do_it)
{
	SetIntData(NEVERUSEPROXYKEY, do_it?1:0);
}


const bool CConfiguration::GetNeverUseProxy()
{
	return (GetIntData(NEVERUSEPROXYKEY, 0) != 0);
}


void CConfiguration::SetHidePassword(const bool do_it)
{
	SetIntData(HIDEPASSWORDKEY, do_it?1:0);
}


const bool CConfiguration::GetLogToFile()
{
	return (GetIntData(LOGTOFILEKEY, 0) != 0);
}


void CConfiguration::SetLogToFile(const bool do_it)
{
	SetIntData(LOGTOFILEKEY, do_it?1:0);
}


const bool CConfiguration::GetMinimizeMemoryUsage()
{
	return (GetIntData(MINIMIZEMEMORYUSAGEKEY, 1) != 0);
}


void CConfiguration::SetMinimizeMemoryUsage(const bool do_it)
{
	SetIntData(MINIMIZEMEMORYUSAGEKEY, do_it?1:0);
}

const bool CConfiguration::GetRestartDHCPWhenNeeded()
{
	return (GetIntData(RESTARTDHCPWHENNEEDEDKEY, 0) != 0);
}


void CConfiguration::SetRestartDHCPWhenNeeded(const bool do_it)
{
	SetIntData(RESTARTDHCPWHENNEEDEDKEY, do_it?1:0);
}

/**
 * Time in seconds
 */
const int CConfiguration::GetLogoutTimeBeforeRestartDHCP()
{
	return GetIntData(LOGOUTTIMEBEFORERESTARTKEY, 1800);
}


/**
 * Time in seconds
 */
void CConfiguration::SetLogoutTimeBeforeRestartDHCP(const int time)
{
	SetIntData(LOGOUTTIMEBEFORERESTARTKEY, time);
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

#ifdef ICLOGIN_SERVICE
const CString CConfiguration::GetServiceName()
{
	return GetStringData(SERVICENAMEKEY, _T("ICLogin"));
}


void CConfiguration::SetServiceName(const CString &name)
{
	SetStringData(SERVICENAMEKEY, name);
}


const CString CConfiguration::GetServiceLongName()
{
	return GetStringData(SERVICEDISPLAYNAMEKEY, _T("IC Login Service"));
}


void CConfiguration::SetServiceLongName(const CString &name)
{
	SetStringData(SERVICEDISPLAYNAMEKEY, name);
}

bool CConfiguration::GetExtendedLoggedInTest()
{
	return (GetIntData(EXTENDEDLOGGEDINKEY, 0) != 0);
}

void CConfiguration::SetExtendedLoggedInTest(bool do_it)
{
	SetIntData(EXTENDEDLOGGEDINKEY, do_it?1:0);
}


const bool CConfiguration::GetVisibleAsService()
{
	return (GetIntData(VISIBLEASSERVICE, 0) != 0);
}


void CConfiguration::SetVisibleAsService(const bool be_it)
{
	SetIntData(VISIBLEASSERVICE, be_it?1:0);
}


const bool CConfiguration::GetServicePopupError()
{
	return (GetIntData(SERVICEPOPUPERROR, 0) != 0);
}


void CConfiguration::SetServicePopupError(const bool do_it)
{
	SetIntData(SERVICEPOPUPERROR, do_it?1:0);
}


const bool CConfiguration::GetRunAsService()
{
	return (GetIntData(RUNASSERVICE, 0) != 0);
}


void CConfiguration::SetRunAsService(const bool do_it)
{
	CServiceMaster::RemoveService();
	if(do_it)
	{
		if(CServiceMaster::InstallService())
		{
			SetIntData(RUNASSERVICE, 1);
		}
	}
	else
	{
		SetIntData(RUNASSERVICE, 0);
	}
}


#endif // ICLOGIN_SERVICE

const bool CConfiguration::GetAutoStart()
{
	// Could also check the run key.
	return (GetIntData(AUTOSTARTKEY, 0) != 0);
}

void CConfiguration::SetAutoStart(const bool do_it)
{
	HKEY start_key = GetRegKey(LocalMachine);
	CRegKey regkey1;
	if(regkey1.Create(start_key, _T("Software")) == ERROR_SUCCESS)
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
							BOOL rv = GetModuleFileName(NULL, ourpath.GetBuffer(1000), 1000);
							ourpath.ReleaseBuffer();
							if(rv)
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


/**
 * Try to write, and if successable, retur true.
 */
bool CConfiguration::IsWritable()
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
				static const _TCHAR* key = _T("DummyToSeeIfWritable");
				if(regkey3.SetValue(4711, key) == ERROR_SUCCESS)
				{
					regkey3.DeleteValue(key);
					return true;
				}
			}
		}
	}
	return false;
}


int CConfiguration::GetIntData(const CString& key, 
							   int default_data, 
		RegistryPart registry_part /* =LocalMachine */)
{
	int rv = default_data; // Default;
	HKEY start_key = GetRegKey(registry_part);
	CRegKey regkey1;
	if(regkey1.Open(start_key, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Open(regkey1, COMPANYKEY) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Open(regkey2, APPKEY) == ERROR_SUCCESS)
			{
				DWORD data_in_reg;
				if(regkey3.QueryValue(data_in_reg, key) == ERROR_SUCCESS)
				{
					return data_in_reg;
				}
			}
		}
	}
	return rv; 
}



void CConfiguration::SetIntData(const CString& key, 
								int value, 
		RegistryPart registry_part /* =LocalMachine */)
{
	HKEY start_key = GetRegKey(registry_part);
	CRegKey regkey1;
	if(regkey1.Create(start_key, _T("Software")) == ERROR_SUCCESS)
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



CString CConfiguration::GetStringData(const CString& key, 
									  const CString& default_data, 
		RegistryPart registry_part /*=LocalMachine */)
{
	CString rv = default_data; // Default;
	HKEY start_key = GetRegKey(registry_part);
	CRegKey regkey1;
	if(regkey1.Open(start_key, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey regkey2;
		if(regkey2.Open(regkey1, COMPANYKEY) == ERROR_SUCCESS)
		{
			CRegKey regkey3;
			if(regkey3.Open(regkey2, APPKEY) == ERROR_SUCCESS)
			{
				DWORD data_size;
				if(regkey3.QueryValue(NULL, key, &data_size) == ERROR_SUCCESS)
				{
					if(data_size > 0)
					{
						regkey3.QueryValue(rv.GetBuffer(data_size), key, &data_size);
						rv.ReleaseBuffer();
					}
				}
			}
		}
	}
	return rv; 
}

void CConfiguration::SetStringData(const CString& key, 
								   const CString& value, 
		RegistryPart registry_part /*=LocalMachine*/)
{
	HKEY start_key = GetRegKey(registry_part);
	CRegKey regkey1;
	if(regkey1.Create(start_key, _T("Software")) == ERROR_SUCCESS)
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

inline HKEY CConfiguration::GetRegKey(RegistryPart registry_part)
{
	switch(registry_part)
	{
	case LocalMachine:
		return HKEY_LOCAL_MACHINE;
		break;
	default:
		ASSERT(registry_part == CurrentUser);
		return HKEY_CURRENT_USER;
	}
}
