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

// Configuration.h: interface for the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGURATION_H__073303F5_1322_4D8A_AF60_B1FC723CB584__INCLUDED_)
#define AFX_CONFIGURATION_H__073303F5_1322_4D8A_AF60_B1FC723CB584__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConfiguration  
{
public:
	static const int GetLoginInterval();
	static void SetLoginInterval(int interval);
	static const bool GetLoginAtInterval();
	static void SetLoginAtInterval(bool do_it);
	static const CString GetLoginHost();
	static void SetLoginHost(const CString &host);
	static const bool GetLoginAtStartup();
	static void SetLoginAtStartup(bool do_it);
	static const CString GetUsername();
	static void SetUsername(const CString &name);
	static const CString GetPassword();
	static void SetPassword(const CString &password);
	static const bool GetHidePassword();
	static void SetHidePassword(bool do_it);
	static const int GetLoginFailedInterval();
	static void SetLoginFailedInterval(int interval);
	static const int GetUpdateTimersInterval();
	static void SetUpdateTimersInterval(int interval);
	static const bool GetStartHidden();
	static void SetStartHidden(bool do_it);
	static const bool GetIsConfigured();
	static void SetIsConfigured(bool do_it);
	static const bool GetAutoStart();
	static void SetAutoStart(bool do_it);
	static const bool GetMinimizeMemoryUsage();
	static void SetMinimizeMemoryUsage(bool do_it);
	static const bool GetRestartDHCPWhenNeeded();
	static void SetRestartDHCPWhenNeeded(bool do_it);
	static const int GetLogoutTimeBeforeRestartDHCP();
	static void SetLogoutTimeBeforeRestartDHCP(const int time);
	static const bool GetLogToFile();
	static void SetLogToFile(bool do_it);
	static const CString GetLogFile();
	static void SetLogFile(const CString &filename);
	static const int GetLogLevel();
	static void SetLogLevel(int loglevel);
#ifdef ICLOGIN_SERVICE
	static const CString GetServiceName();
	static void SetServiceName(const CString &name);
	static const CString GetServiceLongName();
	static void SetServiceLongName(const CString &name);
	static const bool GetVisibleAsService();
	static void SetVisibleAsService(bool be_it);
	static const bool GetRunAsService();
	static void SetRunAsService(bool do_it);
	static const bool GetServicePopupError();
	static void SetServicePopupError(bool do_it);
#endif

	static void CleanRegistry();


private:
	CConfiguration();
	virtual ~CConfiguration();
	static int GetIntData(const CString& key, int default_data);
	static void SetIntData(const CString& key, int value);
	static CString GetStringData(const CString& key, const CString &default_data);
	static void SetStringData(const CString& key, const CString& value);
};

#endif // !defined(AFX_CONFIGURATION_H__073303F5_1322_4D8A_AF60_B1FC723CB584__INCLUDED_)
