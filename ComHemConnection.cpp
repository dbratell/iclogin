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


/**
 * Extracts the timestamp. Returns 0 if everything ok. Something negative otherwise.
 */
int CComHemConnection::ExtractTimestamp(const CString& webpage, CString &timestamp)
{
	int formstart, formend;
	
	CString lowerpage = webpage;
	lowerpage.MakeLower();
	formstart = lowerpage.Find("<form");
	if(formstart == -1)
		return -1; // No form 

	formend = lowerpage.Find("</form>", formstart);
	if(formend == -1) {
		// strange html. Let go for now.
	}

	int inputindex = formstart;
	bool found_timestamp_input = false;
	while(!found_timestamp_input)
	{
		inputindex = lowerpage.Find("<input", inputindex);
		if(inputindex == -1)
			return -1;

		int inputend = lowerpage.Find('>', inputindex);
		if(inputend == -1)
			return -1;

		CString input = lowerpage.Mid(inputindex, inputend-inputindex);
		// TRACE(input);

		int name = input.Find("name=\"timestamp\"");
		if(name == -1)
			continue;

		// We found it!
		int value = input.Find("value=\"");
		if(value == -1)
			return -1;
	
		value += 7;
		int endingquote = input.Find('"', value);
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
		int kstart = page.Find("kabel-tv internet");
		if(kstart>0 && page.Find("aktiv", kstart) != -1)
		{
			return_value = true;
		}
	} catch (...) { } 

	internet_session.Close();
	return return_value;
}

bool CComHemConnection::Login() const
{
	m_parent_window->PostMessage(IC_LOGINSTARTED);

	if(Is_loggined())
	{
		m_parent_window->PostMessage(IC_LOGINALREADYLOGIN);
		return true;
	}

	CICInternetSession internet_session(m_parent_window);
	auto_ptr<CHttpConnection> http_connection;
	auto_ptr<CHttpFile> http_file;
	CString page;
	try {
		GetUrl(internet_session, CConfiguration::GetLoginHost(),
			"/login.php3", page);
		page.MakeLower();
	} catch (...) {
		m_parent_window->PostMessage(IC_LOGINFAILED);
		internet_session.Close();
		return false;
	} 

	if(page.Find("password") == -1 && page.Find("username") == -1)
	{
		// Something wrong. Fail!
		TRACE("Already logged in.\n");
		// ASSERT(false); // Why wasn't this trapped above?
		m_parent_window->PostMessage(IC_LOGINFAILED);
		return false;
	}

	CString timestamp;
	int rv = ExtractTimestamp(page, timestamp);
	
	if(rv<0) timestamp.Empty();
	if(!PostLogin(internet_session, timestamp))
	{
		internet_session.Close();
		m_parent_window->PostMessage(IC_LOGINFAILED);
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
		cie->ReportError();
		m_parent_window->PostMessage(IC_LOGINFAILED);
		internet_session.Close();
		return false;
	}

	internet_session.Close();

	m_parent_window->PostMessage(IC_LOGINSUCCEEDED);
	return true;
}


bool CComHemConnection::Logout() const
{
	
	m_parent_window->PostMessage(IC_LOGOUTSTARTED);
	CICInternetSession internet_session(m_parent_window);
	try {
		VisitUrl(internet_session, "login1.telia.com", "/logout.php3?logout=1");
	} catch (...) {
		m_parent_window->PostMessage(IC_LOGOUTFAILED);
		internet_session.Close();
		return false;
	}
	internet_session.Close();

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

	try {
		http_connection = auto_ptr<CHttpConnection>(
			internet_session.GetHttpConnection(host));
	} catch (...) {
		TRACE("Couldn't get connection from internet session.\n");
		throw;
	} 

	try {
		http_file = auto_ptr<CHttpFile>(http_connection->OpenRequest(
			CHttpConnection::HTTP_VERB_GET, file, NULL, 1, NULL, NULL, 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE));
	} catch (...) {
		TRACE("Couldn't get file from internet session.\n");
		http_connection->Close();
		throw;
	} 

	try {
		http_file->SendRequest();
	} catch (CInternetException *ie) {
		TRACE("Couldn't open connection (SendRequest)!\n");
		http_file->Abort();
		http_connection->Close();
		throw;
	}

	try {
		CString line;
		while(http_file->ReadString(line))
		{
			data += line;
			line += '\n';
			// TRACE(data);
			// TRACE("\n");
		}
	} catch (CInternetException ie) {
		http_file->Close();
		http_connection->Close();
		throw;
	}

	http_file->Close();
	http_connection->Close();
}


bool CComHemConnection::PostLogin(CInternetSession &internet_session, CString timestamp)
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

	try {
		http_connection = auto_ptr<CHttpConnection>(
			internet_session.GetHttpConnection(
				CConfiguration::GetLoginHost()));
	} catch (...) {
		return false;
	} 

	try {
		http_file = auto_ptr<CHttpFile>(http_connection->OpenRequest(
			CHttpConnection::HTTP_VERB_POST, "/userlookup.php3", 
			NULL, 1, NULL, NULL, 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE));
	} catch (...) {
		http_connection->Close();
		return false;
	} 
	
	bool good_request = true;
	try {
		good_request = (http_file->SendRequest(strHeaders, strHeaders.GetLength(), (LPVOID)(LPCTSTR)postdata, postdata.GetLength()) == TRUE);
		char buffer[4000];
		while(http_file->Read(buffer, sizeof(buffer)))
		{
//			TRACE(buffer);
	//		TRACE(data);
	//		TRACE("\n");
		}
	} catch (...) {
		good_request = false;
	}

	if(!good_request)
	{
		http_file->Abort();
	}
	else
	{
		http_file->Close();
	}

	http_connection->Close();

	return good_request;

}


CWnd *CComHemConnection::GetWindow() const
{
	return m_parent_window;
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

