// ComHemConnection.cpp: implementation of the CComHemConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iclogin.h"
#include "Configuration.h"
#include "ComHemConnection.h"
#include "ICInternetSession.h"
#include "icmessages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// #define TRACE_ALL_OUTPUT

#define LOGIN_METHOD_UNKNOWN 0
#define LOGIN_METHOD_OLD 1
#define LOGIN_METHOD_NEW 2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComHemConnection::CComHemConnection(CWnd *parent_window)
: m_parent_window(parent_window)
{

}


CComHemConnection::~CComHemConnection()
{

}

// This variable will be used on many threads at once. It is not critical 
// that the value is correct as long as it is either the old or the
// new value that we get.
int CComHemConnection::m_login_method = LOGIN_METHOD_UNKNOWN;

/**
 * Extracts the timestamp. Returns 0 if everything ok. Something negative otherwise.
 */
int CComHemConnection::ExtractTimestamp(const CString& webpage, CString &timestamp)
{
	int formstart, formend;
	
	CString lowerpage = webpage;
	lowerpage.MakeLower();
	formstart = FindSubString(lowerpage, "<form");
	if(formstart == -1)
		return -1; // No form 

	formend = FindSubString(lowerpage, "</form>", formstart);
	if(formend == -1) {
		// strange html. Let go for now.
	}

	int inputindex = formstart;
	bool found_timestamp_input = false;
	while(!found_timestamp_input)
	{
		inputindex = FindSubString(lowerpage, "<input", inputindex);
		if(inputindex == -1)
			return -1;

		int inputend = FindSubString(lowerpage, '>', inputindex);
		if(inputend == -1)
			return -1;

		CString input = lowerpage.Mid(inputindex, inputend-inputindex);
		// TRACE(input);

		int name = FindSubString(input,"name=\"timestamp\"");
		if(name == -1)
			continue;

		// We found it!
		int value = FindSubString(input, "value=\"");
		if(value == -1)
			return -1;
	
		value += 7;
		int endingquote = FindSubString(input, '"', value);
		if(endingquote == -1)
			return -1;

		timestamp = input.Mid(value, endingquote-value);
		found_timestamp_input = true;
	}

	return 0;
}

bool CComHemConnection::Is_loggined() const
{
	CICInternetSession internet_session(m_parent_window);
	bool return_value = false;
	try {
		CString page;
		GetUrl(internet_session, CConfiguration::GetLoginHost(), 
			"/serviceSelection.php3", page);
		page.MakeLower();
		int kstart = FindSubString(page, "kabel-tv internet");
		if(kstart>0 && FindSubString(page, "aktiv", kstart) != -1)
		{
			g_log.Log("Finding keywords.", CLog::LOG_DUMP);
			return_value = true;
		}
		else
		{
			g_log.Log("Missing keyword.", CLog::LOG_DUMP);
		}
	} catch (CInternetException *cie) {cie->Delete(); } 

	return return_value;
}


bool CComHemConnection::Login() const
{
	m_parent_window->PostMessage(IC_LOGINSTARTED);
	g_log.Log("Starting login...");

	if(Is_loggined())
	{
		g_log.Log("Already logged in.");
		m_parent_window->PostMessage(IC_LOGINALREADYLOGIN);
		return true;
	}

	CICInternetSession internet_session(m_parent_window);
	CString page;
	// Detect login method
	if(m_login_method == LOGIN_METHOD_UNKNOWN)
	{
		g_log.Log("Trying to detect login method.");
		try {
			GetUrl(internet_session, CConfiguration::GetLoginHost(),
				"/", page);
			page.MakeLower();
			if(FindSubString(page, "login.php3") != -1)
			{
				m_login_method = LOGIN_METHOD_OLD;
				g_log.Log("Detecting 'old' login method.");
			}
			else if(FindSubString(page, "sd/") != -1)
			{
				m_login_method = LOGIN_METHOD_NEW;
				g_log.Log("Detecting 'new' login method.");
			}
		} catch (CInternetException *cie) {
			m_parent_window->PostMessage(IC_LOGINFAILED);
			cie->Delete();
			return false;
		} 
	}

	// Make a copy to avoid race conditions.
	int our_login_method = m_login_method; 
	if(our_login_method == LOGIN_METHOD_OLD)
	{
		if(!LoginOldWay(internet_session))
		{
			m_parent_window->PostMessage(IC_LOGINFAILED);
			return false;
		}
	}
	else if(our_login_method == LOGIN_METHOD_NEW)
	{
		if(!LoginNewWay(internet_session))
		{
			m_parent_window->PostMessage(IC_LOGINFAILED);
			return false;
		}
	}
	else if(our_login_method == LOGIN_METHOD_UNKNOWN)
	{
		// Try first old, then new
		if(!LoginOldWay(internet_session))
		{
			if(!LoginNewWay(internet_session))
			{
				m_parent_window->PostMessage(IC_LOGINFAILED);
				return false;
			}
			else
			{
				m_login_method = LOGIN_METHOD_NEW;
			}
		}
		else
		{
			m_login_method = LOGIN_METHOD_OLD;
		}
	}
	else
	{
		ASSERT(false); // What the...
	}

	m_parent_window->PostMessage(IC_LOGINSUCCEEDED);
	return true;
}

bool CComHemConnection::LoginOldWay(CInternetSession &internet_session)
{
	g_log.Log("Loggin in with old method.");
	CString page;
	try {
		GetUrl(internet_session, CConfiguration::GetLoginHost(),
			"/login.php3", page);
	} catch (CInternetException *cie) {
		cie->Delete();
		return false;
	} 

	page.MakeLower();
	if(FindSubString(page, "password") == -1 && FindSubString(page, "username") == -1)
	{
		// Something wrong. Fail!
		TRACE("Already logged in.\n");
		// ASSERT(false); // Why wasn't this trapped above?
		return false;
	}

	CString timestamp;
	int rv = ExtractTimestamp(page, timestamp);
	
	if(rv<0) timestamp.Empty();
	if(!PostLogin(internet_session, timestamp, _T("/userlookup.php3")))
	{
		return false;
	}


	try 
	{
		// Open two dummy places.
		VisitUrl(internet_session, CConfiguration::GetLoginHost(), "/serviceSelection.php3");
		VisitUrl(internet_session, "www.comhem.telia.se", "/ic");
	}
	catch(CInternetException *cie)
	{
		cie->Delete();
		return false;
	}

	return true;
}


bool CComHemConnection::LoginNewWay(CInternetSession &internet_session)
{
	g_log.Log("Loggin in with new method.");
	try {
		VisitUrl(internet_session, CConfiguration::GetLoginHost(), _T("/sd/init"));
	} catch (CInternetException *cie) {
		cie->Delete();
		return false;
	} 

	if(!PostLogin(internet_session, _T(""), _T("/sd/login")))
	{
		return false;
	}

	try 
	{
		// Open two dummy places.
		VisitUrl(internet_session, "www.comhem.telia.se", "/ic");
	}
	catch(CInternetException *cie)
	{
		cie->Delete();
		return false;
	}

	return true;
}



// Messy!
bool CComHemConnection::Logout() const
{
	g_log.Log("Loggin out.");

	int login_method = m_login_method; // Copy to avoid race conditions

	m_parent_window->PostMessage(IC_LOGOUTSTARTED);
	CICInternetSession internet_session(m_parent_window);

	if(login_method == LOGIN_METHOD_OLD)
	{
		try {
			VisitUrl(internet_session, "login1.telia.com", "/logout.php3?logout=1");
		} catch (CInternetException *cie) {
			m_parent_window->PostMessage(IC_LOGOUTFAILED);
			cie->Delete();
			return false;
		}
	}
	else if(login_method == LOGIN_METHOD_NEW)
	{
		try {
			VisitUrl(internet_session, "login1.telia.com", "/sd/logout?logout=1");
		} catch (CInternetException *cie) {
			m_parent_window->PostMessage(IC_LOGOUTFAILED);
			cie->Delete();
			return false;
		}
	}
	else // login_method == LOGIN_METHOD_UNKNOWN
	{
		try {
			VisitUrl(internet_session, "login1.telia.com", "/logout.php3?logout=1");
		} catch (CInternetException *oldcie) {
			oldcie->Delete();
			try {
				VisitUrl(internet_session, "login1.telia.com", "/sd/logout?logout=1");
			} catch (CInternetException *newcie) {
				m_parent_window->PostMessage(IC_LOGOUTFAILED);
				newcie->Delete();
				return false;
			}
			m_login_method = LOGIN_METHOD_NEW;
			m_parent_window->PostMessage(IC_LOGOUTSUCCEEDED);
			return true;
		}
		m_login_method = LOGIN_METHOD_OLD;
	}

	m_parent_window->PostMessage(IC_LOGOUTSUCCEEDED);
	return true;
}


/**
 * Throws CInternetException
 */
void CComHemConnection::VisitUrl(CInternetSession& internet_session, 
						   const CString& host, const CString & file)
{
	CString dummy_data;
	GetUrl(internet_session, host, file, dummy_data);
}


/**
 * Throws CInternetException. Puts output in data.
 */
void CComHemConnection::GetUrl(CInternetSession& internet_session, 
						   const CString& host, const CString & file, 
						   CString& data)
{
	auto_ptr<CHttpConnection> http_connection;
	auto_ptr<CHttpFile> http_file;

	g_log.Log("Going to http://"+host+file);

	try {
		http_connection = auto_ptr<CHttpConnection>(
			internet_session.GetHttpConnection(host));
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "GetHttpConnection", cie);
		throw;
	} 

	try {
		http_file = auto_ptr<CHttpFile>(http_connection->OpenRequest(
			CHttpConnection::HTTP_VERB_GET, file, NULL, 1, NULL, NULL, 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE));
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "OpenRequest", cie);
		http_connection->Close();
		throw;
	} 

	try {
		http_file->SendRequest();
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "SendRequest", cie);
		http_file->Abort();
		http_connection->Close();
		throw;
	}

	DWORD statuscode;
	http_file->QueryInfoStatusCode(statuscode);
	if(statuscode<200 || 299 < statuscode)
	{
		CString err;
		err.Format("Got status code %d from server. That's not good.", 
			statuscode);
		g_log.Log(err, CLog::LOG_INFO);
		http_file->Abort();
		http_connection->Close();
		throw new CInternetException(1);
	}

	try {
		CString line;
		while(http_file->ReadString(line))
		{
			data += line;
			line += '\n';
#ifdef TRACE_ALL_OUTPUT
			TRACE(line);
			TRACE("\n");
#endif
		}
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "ReadString", cie);
		http_file->Abort();
		http_connection->Close();
		throw;
	}

	http_file->Close();
	http_connection->Close();
}


bool CComHemConnection::PostLogin(CInternetSession &internet_session, 
								  const CString& timestamp, 
								  const CString& login_page)
{
	auto_ptr<CHttpConnection> http_connection;
	auto_ptr<CHttpFile> http_file;

	// Try to login
    CString strHeaders =
    	_T("Content-Type: application/x-www-form-urlencoded");
	// Build post data
	CString postdata;
    if(timestamp.IsEmpty())
    {
	   TRACE("Ingen timestamp\n");
	}
	else
	{
		postdata += "timestamp=";
		postdata += timestamp;
		postdata += "&";
	}
	postdata += "username=";
	postdata += CConfiguration::GetUsername(); // (should be url-encoded)
	postdata += "&";
	postdata += "password=";
	postdata += CConfiguration::GetPassword(); // (should be url-encoded)

	g_log.Log("Posting to http://"+CConfiguration::GetLoginHost()+login_page);
	try {
		http_connection = auto_ptr<CHttpConnection>(
			internet_session.GetHttpConnection(
				CConfiguration::GetLoginHost()));
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "GetHttpConnection", cie);
		cie->Delete();
		return false;
	} 

	try {
		http_file = auto_ptr<CHttpFile>(http_connection->OpenRequest(
			CHttpConnection::HTTP_VERB_POST, login_page, 
			NULL, 1, NULL, NULL, 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE));
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "OpenRequest", cie);
		http_connection->Close();
		cie->Delete();
		return false;
	} 
	
	try {
		TRACE1("Sending data: %s\n", postdata);
		http_file->SendRequest(strHeaders, strHeaders.GetLength(), (LPVOID)(LPCTSTR)postdata, postdata.GetLength());
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "SendRequest", cie);
		http_file->Abort();
		http_connection->Close();
		cie->Delete();
		return false;
	}

	DWORD statuscode;
	http_file->QueryInfoStatusCode(statuscode);
	if(statuscode<200 || 299 < statuscode)
	{
		CString err;
		err.Format("Got status code %d from server. That's not good.", 
			statuscode);
		g_log.Log(err, CLog::LOG_INFO);
		http_file->Abort();
		http_connection->Close();
		return false;
	}

	try {
		CString line;
		while(http_file->ReadString(line))
		{
#ifdef TRACE_ALL_OUTPUT
			TRACE(line);
			TRACE("\n");
#endif
		}
	} catch (CInternetException *cie) {
		LogInternetError(internet_session, "Read", cie);
		http_file->Abort();
		http_connection->Close();
		cie->Delete();
	}

	try {
		http_file->Close();
	} catch(CInternetException *cie) { delete cie; }

	http_connection->Close();

	return true;
}


CWnd *CComHemConnection::GetWindow() const
{
	return m_parent_window;
}


void CComHemConnection::LogInternetError(const CInternetSession &internet_session,
		const CString& operation, CInternetException *ie)
{
	DWORD extended_error;
	internet_session.QueryOption(INTERNET_OPTION_EXTENDED_ERROR, extended_error);
	CString errmess;
	ie->GetErrorMessage(errmess.GetBuffer(1000), 1000);
	errmess.ReleaseBuffer(); 
	errmess.TrimRight();
	CString output;
	output.Format(
		"Error in %s: '%s' Extended error: %d",
		operation, errmess, extended_error);
	g_log.Log(output, CLog::LOG_ERROR);
}

/**
 * This function replaces CString::Find which requires a new 
 * (1997 or something) mfc42.dll.
 */
int CComHemConnection::FindSubString(const CString &str, const CString &substr, int start /*=0*/)
{
	if(start>=str.GetLength())
	{
		return -1;
	}

	LPCTSTR str1 = (LPCTSTR)str;
	LPCTSTR substr1 = (LPCTSTR)substr;
	str1 += start;
	LPCTSTR res = strstr(str1, substr1);
	if(res)
	{
		return ((int)res - (int)str1)/sizeof(_TCHAR);
	}
	return -1;
}


void StartLoginThread(CWnd *parent)
{
	CComHemConnection *conn = new CComHemConnection(parent);
	AfxBeginThread(LoginThread, conn);
}


void StartLogoutThread(CWnd *parent)
{
	CComHemConnection *conn = new CComHemConnection(parent);
	AfxBeginThread(LogoutThread, conn);
}


UINT AFX_CDECL LoginThread( LPVOID pParam )
{
	// Take ownership of the input
	auto_ptr<CComHemConnection> 
		conn(reinterpret_cast<CComHemConnection *>(pParam));

	conn->GetWindow()->PostMessage(IC_LOGINTHREADSTARTED);

	conn->Login();

	conn->GetWindow()->PostMessage(IC_LOGINTHREADTERMINATED);
	return 0;
}


UINT AFX_CDECL LogoutThread( LPVOID pParam )
{
	// Take ownership of the input
	auto_ptr<CComHemConnection> 
		conn(reinterpret_cast<CComHemConnection *>(pParam));

	conn->GetWindow()->PostMessage(IC_LOGOUTTHREADSTARTED);

	conn->Logout();

	conn->GetWindow()->PostMessage(IC_LOGOUTTHREADTERMINATED);
	return 0;
}

