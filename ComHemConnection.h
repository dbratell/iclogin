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

// ComHemConnection.h: interface for the CComHemConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMHEMCONNECTION_H__CACAA417_389C_4D7F_980B_0234C659200F__INCLUDED_)
#define AFX_COMHEMCONNECTION_H__CACAA417_389C_4D7F_980B_0234C659200F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <iphlpapi.h>


class CComHemConnection  
{
public:
	CComHemConnection(CWnd *parent_window);
	virtual ~CComHemConnection();

	bool Login() const;
	bool Logout() const;
	bool Is_loggined() const;
	bool RestartDHCP() const;
	CWnd *CComHemConnection::GetWindow() const;

private:
	static CString URLEncode(const CString original);
	static int ExtractTimestamp(const CString& webpage, CString& timestamp);
	static void GetUrl(CInternetSession& internet_session, 
						   const CString& host, const CString & file, 
						   CString& data);
	static void VisitUrl(CInternetSession& internet_session, 
						   const CString& host, const CString & file);
	static bool PostLogin(CInternetSession &is, 
		const CString &timestamp,
		const CString &login_page);
	static bool	LoginOldWay(CInternetSession &internet_session);
	static bool	LoginNewWay(CInternetSession &internet_session);
	static void LogInternetError(const CInternetSession &internet_session,
		const CString& operation, CInternetException *ie);

	static int FindSubString(const CString &str, const CString &substr, int start=0);

	static DWORD IpReleaseAddress_proxy(PIP_ADAPTER_INDEX_MAP AdapterInfo);
	static DWORD IpRenewAddress_proxy(PIP_ADAPTER_INDEX_MAP AdapterInfo);
	static DWORD GetAdaptersInfo_proxy(PIP_ADAPTER_INFO pAdapterInfo,
										PULONG pOutBufLen);
	static DWORD GetInterfaceInfo_proxy(PIP_INTERFACE_INFO pIfTable,
										PULONG dwOutBufLen);
	// Attributes
	CWnd *m_parent_window;
	static int m_login_method;
};


void StartLoginThread(CWnd *parent);
void StartLogoutThread(CWnd *parent);
void StartRestartDHCPThread(CWnd *parent);
UINT AFX_CDECL LoginThread( LPVOID pParam );
UINT AFX_CDECL LogoutThread( LPVOID pParam );
UINT AFX_CDECL RestartDHCPThread( LPVOID pParam );


#endif // !defined(AFX_COMHEMCONNECTION_H__CACAA417_389C_4D7F_980B_0234C659200F__INCLUDED_)
