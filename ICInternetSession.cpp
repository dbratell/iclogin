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

// ICInternetSession.cpp: implementation of the CICInternetSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iclogin.h"
#include "icmessages.h"
#include "ICInternetSession.h"
#include "Configuration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const _TCHAR* const USERAGENT = "Mozilla/4.75 [en] (Windows NT 5.0; U)";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CICInternetSession::CICInternetSession(CWnd *parent)
: CInternetSession(USERAGENT, 1, 
				   CConfiguration::GetNeverUseProxy()?
						INTERNET_OPEN_TYPE_DIRECT :
						INTERNET_OPEN_TYPE_PRECONFIG),
				m_wnd(parent)
{
	EnableStatusCallback();
}

CICInternetSession::~CICInternetSession()
{
	Close();
}

void CICInternetSession::OnStatusCallback( DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength )
{
	CInternetSession::OnStatusCallback(dwContext, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
	switch(dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		// Looking up the IP address of the name contained in lpvStatusInformation. 
		m_wnd->SendMessage(IC_RESOLVINGNAME, (WPARAM)lpvStatusInformation);
		break;
	case INTERNET_STATUS_NAME_RESOLVED:
		// Successfully found the IP address of the name contained in lpvStatusInformation. 
		m_wnd->SendMessage(IC_RESOLVEDNAME, (WPARAM)lpvStatusInformation);
		break;
	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		// Connecting to the socket address (SOCKADDR) pointed to by lpvStatusInformation. 
		m_wnd->SendMessage(IC_CONNECTINGSERVER, (WPARAM)lpvStatusInformation);
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		// Successfully connected to the socket address (SOCKADDR) pointed to by lpvStatusInformation. 
		m_wnd->SendMessage(IC_CONNECTEDSERVER, (WPARAM)lpvStatusInformation);
		break;
	case INTERNET_STATUS_SENDING_REQUEST:
		// Sending the information request to the server. The lpvStatusInformation parameter is NULL. 
		m_wnd->SendMessage(IC_SENDINGREQUEST);
		break;
	case INTERNET_STATUS_REQUEST_SENT:
		// Successfully sent the information request to the server. The lpvStatusInformation parameter is NULL. 
		m_wnd->SendMessage(IC_SENTREQUEST);
		break;
	case INTERNET_STATUS_RECEIVING_RESPONSE: 
		// Waiting for the server to respond to a request. The lpvStatusInformation parameter is NULL. 
		m_wnd->SendMessage(IC_RECEIVINGRESPONSE);
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED: 
		// Successfully received a response from the server. The lpvStatusInformation parameter is NULL. 
		m_wnd->SendMessage(IC_RECEIVEDRESPONSE);
		break;
	case INTERNET_STATUS_CLOSING_CONNECTION: 
		// Closing the connection to the server. The lpvStatusInformation parameter is NULL. 
		m_wnd->SendMessage(IC_CLOSINGCONNECTION);
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED: 
		// Successfully closed the connection to the server. The lpvStatusInformation parameter is NULL. 
		m_wnd->SendMessage(IC_CLOSEDCONNECTION);
		break;
	case INTERNET_STATUS_HANDLE_CREATED: 
		// Used by the Win32 API function InternetConnect to indicate that it has created the new handle. This lets the application call the Win32 function InternetCloseHandle from another thread if the connect is taking too long. See the ActiveX SDK for more information about these functions. 
		break;
	case INTERNET_STATUS_HANDLE_CLOSING: 
		// Successfully terminated this handle value. 
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE: 
		// Successfully completed the asynchronous operation. See the CInternetSession constructor for details on INTERNET_FLAG_ASYNC.The lpvStatusInformation parameter points at an INTERNET_ASYNC_RESULT structure, and dwStatusInformationLength contains the final completion status of the asynchronous function. If this is ERROR_INTERNET_EXTENDED_ERROR, the application can retrieve the server error information by using the Win32 function InternetGetLastResponseInfo. See the ActiveX SDK for more information about this function.
		break;
	default:
		// Unknown status
		;
	}
}
