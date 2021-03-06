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
	CString page;
	try {
		
		if(m_login_method == LOGIN_METHOD_UNKNOWN)
		{
			DetectLoginMethod(internet_session);
		}

		if(FindSubString(page, "logga ut") > 0)
		{
			g_log.Log("Finding 'logga ut'. Must be logged in.", 
				CLog::LOG_DUMP);
			return_value = true;
		}
		else if(m_login_method == LOGIN_METHOD_OLD)
		{
			GetUrl(internet_session, CConfiguration::GetLoginHost(), 
				"/serviceSelection.php3", page);
			page.MakeLower();
			int kstart = FindSubString(page, "kabel-tv internet");
			if(kstart<0)
			{
				g_log.Log("Didn't find kabel-tv. Looking for \"telia broadband\".", CLog::LOG_DUMP);
				kstart = FindSubString(page, "telia broadband");
			}
			if(kstart>0 && FindSubString(page, "aktiv", kstart) != -1)
			{
				g_log.Log("Finding keywords.", CLog::LOG_DUMP);
				return_value = true;

#if 0
				// Extended logintest?
				if(CConfiguration::GetExtendedLoggedInTest())
				{
					// Do it every forth time we do a test.
					static counter = 0;
					counter = (counter + 1) % 4;
					if(!counter)
					{
						return_value = ExtendedLoggedInTest(internet_session);
					}
				}
#endif
			}
			else
			{
				g_log.Log("Missing keyword.", CLog::LOG_DUMP);
			}
		}
		else if(m_login_method == LOGIN_METHOD_NEW)
		{
			GetUrl(internet_session, CConfiguration::GetLoginHost(), 
				"/sd/init", page);
			page.MakeLower();
			int kstart = FindSubString(page, "pwdenter");
			if(kstart != -1)
			{
				g_log.Log("Finding 'pwdenter'. Not logged in.", 
					CLog::LOG_DUMP);
				return_value = false;
			}
			else
			{
				g_log.Log("No 'pwdenter'. Logged in.", CLog::LOG_DUMP);
				return_value = true;
#if 0
				// Extended logintest?
				static counter = 0;
				counter = (counter + 1) % 3;
				if(!counter)
				{
					return_value = ExtendedLoggedInTest(internet_session);
				}
#endif
			}
		}
		else if(m_login_method == LOGIN_METHOD_UNKNOWN)
		{
			return false;
		}
	} catch (CInternetException *cie) {cie->Delete(); } 

	return return_value;
}

#if 0
bool CComHemConnection::ExtendedLoggedInTest(CInternetSession &internet_session)
{
	static lasturl = -1;
	static char* urls[] = 
	{
		"www.svt.se",   // I
		"www.netscape.com", // II
		"www.telia.se",  // III
		"www.yahoo.se",  // IV
		"www.tv4.se",  // V
		"www.dn.se",  // VI
		"www.expressen.se",  // VII
		"www.altavista.com",  // VIII
		"www.cnn.com", // IX 
		"www.aftonbladet.se", // X
		"www.passagen.se", // XI
		"www.nb.se",  // XII
		"www.foreningssparbanken.se", // XIII
		"www.ppm.nu",  // XIV
		"www.altavista.com", // XV
		"www.lycos.com", // XVI
		"www.google.com", // XVII
		"www.disney.com", // XVIII
	};
	const static no_urls = 18;

	if(lasturl == -1)
	{
		CTime now = CTime::GetCurrentTime();
		srand(now.GetSecond()+now.GetMinute()*60+now.GetHour()*3600);
		lasturl = rand() % no_urls;
	}

	int tries = 0;

	while(tries<5)
	{
		try 
		{
			lasturl = (lasturl + 1) % no_urls;
			VisitUrl(internet_session, urls[lasturl], "/", true);
			return true;
		}
		catch(CInternetException *cie)
		{
			cie->Delete();
		}
		tries++;
	}
	
	// Did all tries, and none succeeded. We are not logged in after all.
	return false;
}
#endif

void CComHemConnection::DetectLoginMethod(
			CInternetSession &internet_session) const
{
	// Detect login method
	g_log.Log("Trying to detect login method.");
	CString page;
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
		else
		{
			g_log.Log("Couldn't detect login method.");
		}
	} catch (CInternetException *cie) {
		g_log.Log("Couldn't detect login method - failed to contact login server.");
		cie->Delete();
	} 
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
	DetectLoginMethod(internet_session);

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

bool CComHemConnection::RestartDHCP() const
{
	m_parent_window->PostMessage(IC_RESTARTDHCPSTARTED);
	g_log.Log("Beginning restart of DHCP...");
	DWORD ai_buffer_size = 0;
	DWORD ii_buffer_size = 0;

	if(GetAdaptersInfo_proxy(NULL, &ai_buffer_size)!=ERROR_BUFFER_OVERFLOW)
	{
		g_log.Log("Failed GetAdaptersInfo (too old OS?)...");
		return false;
	}

	PIP_ADAPTER_INFO ai = (PIP_ADAPTER_INFO)new char[ai_buffer_size];
	if(!ai)
	{
		g_log.Log("Out of memory!");
		return false;
	}

	if(GetAdaptersInfo_proxy(ai, &ai_buffer_size) != ERROR_SUCCESS)
	{
		g_log.Log("Failed GetAdaptersInfo...");
		delete ai;
		return false;
	}

	if(GetInterfaceInfo_proxy(NULL, &ii_buffer_size) != ERROR_INSUFFICIENT_BUFFER)
	{
		g_log.Log("Failed GetInterfaceInfo...");
		delete ai;
		return false;
	}

	PIP_INTERFACE_INFO ii = (PIP_INTERFACE_INFO)new char[ii_buffer_size];
	if(!ii)
	{
		g_log.Log("Out of memory!");
		delete ai;
		return false;
	}

	if(GetInterfaceInfo_proxy(ii, &ii_buffer_size) != ERROR_SUCCESS)
	{
		g_log.Log("Failed GetInterfaceInfo (2)...");
		delete ai;
		delete ii;
		return false;
	}

	PIP_ADAPTER_INFO current_adapter = ai;

	bool return_value = true;

	// Do it with all DHCP connections
	while(current_adapter)
	{
		if(!current_adapter->DhcpEnabled)
		{
			current_adapter = current_adapter->Next;
			continue;
		}

		// Get the INTERFACE_MAP that the functions want.
		PIP_ADAPTER_INDEX_MAP aim = NULL;
		for(int i = 0; i < ii->NumAdapters; i++)
		{
			if(ii->Adapter[i].Index == ai->Index)
			{
				aim = &(ii->Adapter[i]);
				break;
			}
		}

		if(!aim)
		{
			g_log.Log("Didn't find adapter in map.");
			m_parent_window->PostMessage(IC_RESTARTDHCPFAILED);
			current_adapter = current_adapter->Next;
			return_value = false;
			continue;
		}

		if(IpReleaseAddress_proxy(aim) != NO_ERROR)
		{
			g_log.Log("Failed DHCP release...");
			m_parent_window->PostMessage(IC_RESTARTDHCPFAILED);
			current_adapter = current_adapter->Next;
			return_value = false;
			continue;
		}

		if(IpRenewAddress_proxy(aim) != NO_ERROR)
		{
			// This is bad. Right now, there are no network at all.
			g_log.Log("Failed DHCP renew...");
			m_parent_window->PostMessage(IC_RESTARTDHCPFAILED);
			current_adapter = current_adapter->Next;
			return_value = false;
			continue;
		}

		m_parent_window->PostMessage(IC_RESTARTDHCPSUCCEEDED);
		current_adapter = current_adapter->Next;
		return_value = true;
	}

	delete ai;
	delete ii;
	return return_value;
}


bool CComHemConnection::LoginOldWay(CInternetSession &internet_session)
{
	g_log.Log("Loggin in with old method (FLS).");
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
		VisitUrl(internet_session, "www.comhem.se", "/ic");
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
	g_log.Log("Loggin in with new method. (ELAS)");
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
		// Open a dummy places.
		VisitUrl(internet_session, "www.comhem.se", "/ic");
	}
	catch(CInternetException *cie)
	{
		cie->Delete();
		try 
		{
			// Open another dummy place, if the first fail. I
			// don't know if this is enough.
			VisitUrl(internet_session, "www.telia.com", "/");
		}
		catch(CInternetException *cie2)
		{
			cie2->Delete();
			return false;
		}
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
						   const CString& host, const CString & file,
						   bool just_head /* = false */)
{
	CString dummy_data;
	GetUrl(internet_session, host, file, dummy_data, just_head);
}


/**
 * Throws CInternetException. Puts output in data.
 */
void CComHemConnection::GetUrl(CInternetSession& internet_session, 
						   const CString& host, const CString & file, 
						   CString& data, bool just_head /* = false */)
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
		int http_verb = 
			just_head?CHttpConnection::HTTP_VERB_HEAD:CHttpConnection::HTTP_VERB_GET;
		http_file = auto_ptr<CHttpFile>(http_connection->OpenRequest(
			http_verb, 
			file, NULL, 1, NULL, NULL, 
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
		err.Format("Got status code %d from server.", 
			statuscode);
		if(statuscode < 200)
		{
			err += " That's an error.";
		}
		else if(statuscode<400)
		{
			err += " That's status IC Login can't handle.";
		}
		else if(statuscode==404)
		{
			err += " That means that the page didn't exist.";
		}
		else if(statuscode<500)
		{
			err += " That's a usage error.";
		}
		else if(statuscode<600)
		{
			err += " That's a server error.";
		}
		else
		{
			err += " That's an unknown type of error.";
		}
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

	g_log.Log(data, CLog::LOG_ALL);

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
	postdata += URLEncode(CConfiguration::GetUsername()); 
	postdata += "&";
	postdata += "password=";
	postdata += URLEncode(CConfiguration::GetPassword());

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
		return (((int)res - (int)str1)/sizeof(_TCHAR))+start;
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

void StartRestartDHCPThread(CWnd *parent)
{
	CComHemConnection *conn = new CComHemConnection(parent);
	AfxBeginThread(RestartDHCPThread, conn);
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


UINT AFX_CDECL RestartDHCPThread( LPVOID pParam )
{
	// Take ownership of the input
	auto_ptr<CComHemConnection> 
		conn(reinterpret_cast<CComHemConnection *>(pParam));

	conn->GetWindow()->PostMessage(IC_RESTARTDHCPTHREADSTARTED);

	conn->RestartDHCP();

	conn->GetWindow()->PostMessage(IC_RESTARTDHCPTHREADTERMINATED);
	return 0;
}


CString CComHemConnection::URLEncode(const CString original)
{
	int length = original.GetLength();
	CString encoded_string;
	for(int i = 0; i < length; i++)
	{
		unsigned char the_char = original[i];
		if(_istalnum(the_char))
		{
			encoded_string += the_char;
		}
		else
		{
			CString coded_char;
			coded_char.Format("%%%x", the_char);
			encoded_string += coded_char;
		}
	}

	return encoded_string;
}

typedef DWORD (__stdcall *IpReleaseAddress_Func)(PIP_ADAPTER_INDEX_MAP AdapterInfo);

DWORD CComHemConnection::IpReleaseAddress_proxy(PIP_ADAPTER_INDEX_MAP AdapterInfo)
{
	HINSTANCE iphlpapidll = LoadLibrary(_T("iphlpapi.dll"));
	if(!iphlpapidll)
	{
		return ERROR_NOT_SUPPORTED;
	}
	IpReleaseAddress_Func func;
	func = (IpReleaseAddress_Func)GetProcAddress(iphlpapidll, "IpReleaseAddress");
	DWORD return_value = ERROR_NOT_SUPPORTED;
	if(func)
	{
		return_value = func(AdapterInfo);
	}

	FreeLibrary(iphlpapidll);

	return return_value;
}

typedef DWORD (__stdcall *IpRenewAddress_Func)(PIP_ADAPTER_INDEX_MAP AdapterInfo);

DWORD CComHemConnection::IpRenewAddress_proxy(PIP_ADAPTER_INDEX_MAP AdapterInfo)
{
	HINSTANCE iphlpapidll = LoadLibrary(_T("iphlpapi.dll"));
	if(!iphlpapidll)
	{
		return ERROR_NOT_SUPPORTED;
	}
	IpRenewAddress_Func func;
	func = (IpRenewAddress_Func)GetProcAddress(iphlpapidll, "IpRenewAddress");
	DWORD return_value = ERROR_NOT_SUPPORTED;
	if(func)
	{
		return_value = func(AdapterInfo);
	}

	FreeLibrary(iphlpapidll);

	return return_value;
}

typedef DWORD (__stdcall *GetAdaptersInfo_Func)(
			PIP_ADAPTER_INFO pAdapterInfo,    // buffer to receive data
			PULONG pOutBufLen                 // size of data returned
);

DWORD CComHemConnection::GetAdaptersInfo_proxy(
  PIP_ADAPTER_INFO pAdapterInfo,    // buffer to receive data
  PULONG pOutBufLen                 // size of data returned
)
{
	HINSTANCE iphlpapidll = LoadLibrary(_T("iphlpapi.dll"));
	if(!iphlpapidll)
	{
		return ERROR_NOT_SUPPORTED;
	}
	GetAdaptersInfo_Func func;
	func = (GetAdaptersInfo_Func)GetProcAddress(iphlpapidll, "GetAdaptersInfo");
	DWORD return_value = ERROR_NOT_SUPPORTED;
	if(func)
	{
		return_value = func(pAdapterInfo, pOutBufLen);
	}

	FreeLibrary(iphlpapidll);

	return return_value;
}

typedef DWORD (__stdcall *GetInterfaceInfo_Func)(
			PIP_INTERFACE_INFO pIfTable, // buffer to receive info
			PULONG dwOutBufLen // size of buffer 
);

DWORD CComHemConnection::GetInterfaceInfo_proxy(
  PIP_INTERFACE_INFO pIfTable,    // buffer to receive data
  PULONG dwOutBufLen                 // size of buffer
)
{
	HINSTANCE iphlpapidll = LoadLibrary(_T("iphlpapi.dll"));
	if(!iphlpapidll)
	{
		return ERROR_NOT_SUPPORTED;
	}
	GetInterfaceInfo_Func func;
	func = (GetInterfaceInfo_Func)GetProcAddress(iphlpapidll, "GetInterfaceInfo");
	DWORD return_value = ERROR_NOT_SUPPORTED;
	if(func)
	{
		return_value = func(pIfTable, dwOutBufLen);
	}

	FreeLibrary(iphlpapidll);

	return return_value;
}