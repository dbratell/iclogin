// ComHemConnection.h: interface for the CComHemConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMHEMCONNECTION_H__CACAA417_389C_4D7F_980B_0234C659200F__INCLUDED_)
#define AFX_COMHEMCONNECTION_H__CACAA417_389C_4D7F_980B_0234C659200F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CComHemConnection  
{
public:
	CComHemConnection(CWnd *parent_window);
	virtual ~CComHemConnection();

	bool Login() const;
	bool Logout() const;
	bool Is_loggined() const;
	CWnd *CComHemConnection::GetWindow() const;

private:
	static int ExtractTimestamp(const CString& webpage, CString& timestamp);
	static void GetUrl(CInternetSession& internet_session, 
						   const CString& host, const CString & file, 
						   CString& data);
	static void VisitUrl(CInternetSession& internet_session, 
						   const CString& host, const CString & file);
	static bool PostLogin(CInternetSession &is, 
		const CString &timestamp,
		const CString &login_page);
	static bool	Login_old_way(CInternetSession &internet_session);
	static bool	Login_new_way(CInternetSession &internet_session);


	// Attributes
	CWnd *m_parent_window;
	static int m_login_method;
};


void StartLoginThread(CWnd *parent);
void StartLogoutThread(CWnd *parent);
UINT AFX_CDECL LoginThread( LPVOID pParam );
UINT AFX_CDECL LogoutThread( LPVOID pParam );


#endif // !defined(AFX_COMHEMCONNECTION_H__CACAA417_389C_4D7F_980B_0234C659200F__INCLUDED_)
