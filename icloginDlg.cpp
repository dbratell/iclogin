// icloginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iclogin.h"
#include "icloginDlg.h"
#include "ComHemConnection.h"
#include "Configuration.h"
#include "ConfigurationDialog.h"
#include "AboutDialog.h"
#include "icmessages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IC_COMMON_WINDOW_MESSAGE _T("ICLOGIN_MESSAGE")


UINT g_common_message = 0;


/////////////////////////////////////////////////////////////////////////////
// CIcloginDlg dialog

CIcloginDlg::CIcloginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIcloginDlg::IDD, pParent),
	m_logintimer(0), m_loggedintimespan(0), m_loggedouttimespan(0), 
	m_failedlastlogin(false), m_startingup(true), m_currentstatus(0),
	m_expectedstatus(0), m_everlogin(false), m_everlogout(false)
{
	//{{AFX_DATA_INIT(CIcloginDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_common_message = RegisterWindowMessage(IC_COMMON_WINDOW_MESSAGE);
}

void CIcloginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcloginDlg)
	DDX_Control(pDX, IDC_LOGGEDOUTTIME, m_loggedouttime);
	DDX_Control(pDX, IDC_LOGGEDINTIME, m_loggedintime);
	DDX_Control(pDX, IDC_STATUSICON, m_statusicon);
	DDX_Control(pDX, IDC_CONFIGUREBUTTON, m_configurebutton);
	DDX_Control(pDX, IDC_MESSAGETEXT2, m_messagetext2);
	DDX_Control(pDX, IDOK, m_quitbutton);
	DDX_Control(pDX, IDC_LOGOUTBUTTON, m_logoutbutton);
	DDX_Control(pDX, IDC_LOGINBUTTON, m_loginbutton);
	DDX_Control(pDX, IDC_MESSAGETEXT, m_messagetext);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIcloginDlg, CDialog)
	//{{AFX_MSG_MAP(CIcloginDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOGINBUTTON, OnLoginbutton)
	ON_BN_CLICKED(IDC_LOGOUTBUTTON, OnLogoutbutton)
	ON_MESSAGE(IC_LOGINSTARTED, OnLoginStarted)
	ON_MESSAGE(IC_LOGINSUCCEEDED, OnLoginSucceeded)
	ON_MESSAGE(IC_LOGINFAILED, OnLoginFailed)
	ON_MESSAGE(IC_LOGINALREADYLOGIN, OnLoginAlreadyLogin)
	ON_MESSAGE(IC_LOGOUTSTARTED, OnLogoutStarted)
	ON_MESSAGE(IC_LOGOUTFAILED, OnLogoutFailed)
	ON_MESSAGE(IC_LOGOUTSUCCEEDED, OnLogoutSucceeded)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(IC_RESOLVINGNAME, OnResolvingName)
	ON_MESSAGE(IC_RESOLVEDNAME, OnResolvedName)
	ON_MESSAGE(IC_CONNECTINGSERVER, OnConnectingServer)
	ON_MESSAGE(IC_CONNECTEDSERVER, OnConnectedServer)
	ON_MESSAGE(IC_SENDINGREQUEST, OnSendingRequest)
	ON_MESSAGE(IC_SENTREQUEST, OnSentRequest)
	ON_MESSAGE(IC_RECEIVINGRESPONSE, OnReceivingResponse)
	ON_MESSAGE(IC_RECEIVEDRESPONSE, OnReceivedResponse)
	ON_MESSAGE(IC_CLOSINGCONNECTION, OnClosingConnection)
	ON_MESSAGE(IC_CLOSEDCONNECTION, OnClosedConnection)
	ON_BN_CLICKED(IDC_CONFIGUREBUTTON, OnConfigurebutton)
	ON_MESSAGE(IC_TRAYICON, OnTrayIcon)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(g_common_message, OnCommonMessage)
	ON_COMMAND(IDC_ABOUT, OnAboutDialog)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcloginDlg message handlers

BOOL CIcloginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(FindAnotherInstance())
	{
		::PostQuitMessage(0);
		return true;
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetupLocalizedText();

	// Configure it if this is the first time.
	if(!CConfiguration::GetIsConfigured())
	{
		g_log.Log("First start.");
		CConfigurationDialog config_dialog;
		if(config_dialog.DoModal() != IDOK)
		{
			// Without configuration, it's not much we can do.
			g_log.Log("No configuration. Have to quit.");
			EndDialog(IDCANCEL);
			return true;
		}
		
		CConfiguration::SetIsConfigured(true);
	}

	if(CConfiguration::GetStartHidden())
	{
//		SetWindowPos(NULL, 0,0,0,0, SWP_HIDEWINDOW | SWP_NOZORDER | 
//			SWP_NOMOVE  | SWP_NOSIZE );
//		ModifyStyle(WS_VISIBLE, 0);
	}

	m_trayicon.Init(this);

	EnableToolTips(true);
	
	SetLoginStatus(0);

	if(CConfiguration::GetLoginAtStartup() && !CConfiguration::GetUsername().IsEmpty())
		StartLoginThread(this);

	SetupLoginTimer();
	m_updatetimer = SetTimer(UPDATETIMER, 
			CConfiguration::GetUpdateTimersInterval()*1000, 
			NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CIcloginDlg::SetupLocalizedText()
{
	CString buttontext;
	buttontext.LoadString(IDS_LOGINBUTTONTEXT);
	m_loginbutton.SetWindowText(buttontext);
	buttontext.LoadString(IDS_LOGOUTBUTTONTEXT);
	m_logoutbutton.SetWindowText(buttontext);
	buttontext.LoadString(IDS_QUITBUTTONTEXT);
	m_quitbutton.SetWindowText(buttontext);
	buttontext.LoadString(IDS_CONFIGUREBUTTONTEXT);
	m_configurebutton.SetWindowText(buttontext);
	m_messagetext.SetWindowText(_T(""));
	m_messagetext2.SetWindowText(_T(""));

	CString window_title;
	window_title.LoadString(IDS_DIALOGTITLE);
	SetWindowText(window_title);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIcloginDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIcloginDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CIcloginDlg::OnLoginbutton() 
{
	// TODO: Add your control notification handler code here
	StartLoginThread(this);
}

void CIcloginDlg::OnLogoutbutton() 
{
	// TODO: Add your control notification handler code here
	StartLogoutThread(this);
}

LRESULT CIcloginDlg::OnLoginStarted(WPARAM, LPARAM)
{
	DisplayMessage(IDS_LOGINSTARTED);
	return 0;
}

LRESULT CIcloginDlg::OnLoginSucceeded(WPARAM, LPARAM)
{
	g_log.Log("Login succeded.");
	m_failedlastlogin = false;
	DisplayMessage(IDS_LOGINSUCCEEDED);
	m_messagetext2.SetWindowText(_T(""));
	SetLoginStatus(1);
	return 0;
}

LRESULT CIcloginDlg::OnLoginFailed(WPARAM, LPARAM)
{
	g_log.Log("Login failed.");
	m_failedlastlogin = true;
	DisplayMessage(IDS_LOGINFAILED);
	m_messagetext2.SetWindowText(_T(""));
	SetLoginStatus(-1);
	SetupLoginTimer();
	return 0;
}

LRESULT CIcloginDlg::OnLoginAlreadyLogin(WPARAM, LPARAM)
{
	m_failedlastlogin = false;
	DisplayMessage(IDS_LOGINALREADYLOGIN);
	// DisplayMessage(IDS_LOGINSUCCEEDED);
	m_messagetext2.SetWindowText(_T(""));
	SetLoginStatus(1);
	return 0;
}

LRESULT CIcloginDlg::OnLogoutStarted(WPARAM, LPARAM)
{
	DisplayMessage(IDS_LOGOUTSTARTED);
	return 0;
}

LRESULT CIcloginDlg::OnLogoutFailed(WPARAM, LPARAM)
{
	g_log.Log("Logout failed.");
	DisplayMessage(IDS_LOGOUTFAILED);
	m_messagetext2.SetWindowText(_T(""));
	SetLoginStatus(0);
	return 0;
}

LRESULT CIcloginDlg::OnLogoutSucceeded(WPARAM, LPARAM)
{
	g_log.Log("Logout succeded.");
	m_failedlastlogin = false;
	DisplayMessage(IDS_LOGOUTSUCCEEDED);
	m_messagetext2.SetWindowText(_T(""));
	SetLoginStatus(-1);
	return 0;
}

void CIcloginDlg::DisplayMessage(UINT message_id)
{
	CString message;
	message.LoadString(message_id);
	m_messagetext.SetWindowText(message);
}

void CIcloginDlg::SetupLoginTimer()
{
	if(m_logintimer != 0)
		KillTimer(m_logintimer);

	if(m_failedlastlogin)
	{
		m_logintimer = SetTimer(LOGINTIMER, 
			CConfiguration::GetLoginFailedInterval()*1000, 
			NULL);
#ifdef _DEBUG
		if(m_logintimer == 0)
		{
			TRACE("Out of timers!\n");
		}
#endif
	} 
	else if(CConfiguration::GetLoginAtInterval())
	{
		m_logintimer = SetTimer(LOGINTIMER, 
			CConfiguration::GetLoginInterval()*1000, 
			NULL);
#ifdef _DEBUG
		if(m_logintimer == 0)
		{
			TRACE("Out of timers!\n");
		}
#endif
	}
}


void CIcloginDlg::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case LOGINTIMER:
		StartLoginThread(this);
		break;
	case UPDATETIMER:
		UpdateTimers();
		break;
	default:
		CDialog::OnTimer(nIDEvent);
	}
}

void CIcloginDlg::OnDestroy() 
{
	if(m_logintimer != 0)
		KillTimer(m_logintimer);

	CDialog::OnDestroy();
}


LRESULT CIcloginDlg::OnResolvingName(WPARAM wparam, LPARAM)
{
	CString resolving;
	resolving.LoadString(IDS_RESOLVING);
	resolving += _T(" ");
	resolving += (LPCSTR)wparam;

	m_messagetext2.SetWindowText(resolving);
	return 0;
}

LRESULT CIcloginDlg::OnResolvedName(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	return 0;
}

LRESULT CIcloginDlg::OnConnectingServer(WPARAM wparam, LPARAM)
{
	CString connecting;
	connecting.LoadString(IDS_CONNECTING);
#if 0 // It's no SOCKADDR! It's a string
	SOCKADDR *sa = reinterpret_cast<SOCKADDR *>(wparam);
	ASSERT(sa->sa_family == AF_INET);
	SOCKADDR_IN *sai = reinterpret_cast<SOCKADDR_IN *>(sa);
	IN_ADDR *addr = &(sai->sin_addr);
	connecting += " ";
	connecting += (int)addr->S_un.S_un_b.s_b1;
	connecting += ".";
	connecting += (int)addr->S_un.S_un_b.s_b2;
	connecting += ".";
	connecting += (int)addr->S_un.S_un_b.s_b3;
	connecting += ".";
	connecting += (int)addr->S_un.S_un_b.s_b4;
#endif
	connecting += " ";
	connecting += (LPCSTR)wparam;

	m_messagetext2.SetWindowText(connecting);
	return 0;
}


LRESULT CIcloginDlg::OnConnectedServer(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	return 0;
}


LRESULT CIcloginDlg::OnSendingRequest(WPARAM, LPARAM)
{
	CString sending;
	sending.LoadString(IDS_SENDINGREQUEST);

	m_messagetext2.SetWindowText(sending);
	return 0;
}


LRESULT CIcloginDlg::OnSentRequest(WPARAM, LPARAM)
{
	CString sending;
	sending.LoadString(IDS_SENTREQUEST);

	m_messagetext2.SetWindowText(sending);
	return 0;
}


LRESULT CIcloginDlg::OnReceivingResponse(WPARAM, LPARAM)
{
	CString got;
	got.LoadString(IDS_RECEIVINGRESPONSE);

	m_messagetext2.SetWindowText(got);
	return 0;
}


LRESULT CIcloginDlg::OnReceivedResponse(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	return 0;
}


LRESULT CIcloginDlg::OnClosingConnection(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	return 0;
}


LRESULT CIcloginDlg::OnClosedConnection(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	return 0;
}


void CIcloginDlg::OnConfigurebutton() 
{
	CConfigurationDialog dialog;
	dialog.DoModal();

	SetupLoginTimer();
}

// IC_TRAYICON
// These notifications are sent when a mouse or event occurs in the bounding 
// rectangle of the icon or the icon is selected or activated with the 
// keyboard. The wParam parameter of the message contains the identifier of 
// the taskbar icon in which the event occurred. The lParam parameter holds 
// the mouse or keyboard message associated with the event. For example, when 
// the mouse cursor moves over a taskbar icon, lParam is set to 
// WM_MOUSEMOVE. See the Taskbar guide chapter for further discussion. 
LRESULT CIcloginDlg::OnTrayIcon(WPARAM wparam, LPARAM lparam)
{
	return m_trayicon.HandleMessage(wparam, lparam);
	switch(lparam)
	{
	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_SHOWNORMAL);
		SetForegroundWindow();

		TRACE("(left)Doubleclick\n");
		break;
	case WM_LBUTTONDOWN:
		TRACE("(left)Klicka på du bara...");
		break;
	case WM_LBUTTONUP:
		TRACE("upp(left)\n");
		break;
	case WM_RBUTTONUP:
		TRACE("upp(right)\n");
		break;
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		{
			CMenu menu;
			CPoint point;
			GetCursorPos(&point);
			if (menu.LoadMenu(IDR_POPUP_MENU))
			{
				CMenu* pPopup = menu.GetSubMenu(0);
				ASSERT(pPopup != NULL);
                SetForegroundWindow(); // Q135788
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, this); // use main window for cmds
			}
		}
		break;
	case WM_MOUSEMOVE:
		break;
	case NIN_KEYSELECT:
		ASSERT(false);
		break;
	case NIN_SELECT:
		ASSERT(false);
		break;
	default:
		TRACE1("Traymessage %ld!\n", lparam);
	}

	return 0;
}

void CIcloginDlg::OnSize(UINT nType, int cx, int cy) 
{
	TRACE("OnSize: type = ");
	switch(nType)
	{
	case SIZE_MINIMIZED: TRACE("SIZE_MINIMIZED"); break;
	case SIZE_MAXIMIZED: TRACE("SIZE_MAXIMIZED"); break;
	case SIZE_RESTORED   : TRACE("SIZE_RESTORED"); break;
	case SIZE_MAXHIDE   : TRACE("SIZE_MAXHIDE"); break;
	case SIZE_MAXSHOW: TRACE("SIZE_MAXSHOW"); break;
	default: TRACE1("%d", nType);
	}
	
	TRACE2(" cx=%d. cy=%d\n", cx, cy);

	if(m_startingup && nType == SIZE_RESTORED && CConfiguration::GetStartHidden())
	{
	    // To force ourself into hiding.
		// There must be a better way to do it!
        PostMessage(WM_SIZE, SIZE_MINIMIZED);
//		ShowWindow(SW_MINIMIZE); 
//      ShowWindow(SW_HIDE); 
		m_startingup = false;
	}
	else if(nType == SIZE_MINIMIZED)
	{
		ShowWindow(SW_HIDE);
	}
	else 
	{ 
		CDialog::OnSize(nType, cx, cy);
	}
}


/**
 * -1: Not logged in
 *  0: Unknown
 *  1: Logged in
 */
void CIcloginDlg::SetLoginStatus(int status)
{
	HICON icon = NULL;
	CString tooltip;

	switch(status)
	{
	case -1: // Not logged in
		icon = AfxGetApp()->LoadIcon(IDI_SAD);
		m_everlogout = true;
		m_lastlogouttime = CTime::GetCurrentTime();
		m_trayicon.SetLogout(m_lastlogouttime);
		break;
	case 0: // Unknown
		icon = AfxGetApp()->LoadIcon(IDI_QUESTIONICON);
		break;
	case 1: // Logged in
		icon = AfxGetApp()->LoadIcon(IDI_HAPPY);
		m_everlogin = true;
		m_lastlogintime = CTime::GetCurrentTime();
		m_trayicon.SetLogin(m_lastlogintime);
		break;
	default:
		ASSERT(false);
	}
	m_statusicon.SetIcon(icon);

	UpdateTimers(); // before updating status

	m_currentstatus = status;
}


void CIcloginDlg::UpdateTimers()
{
	CTime now = CTime::GetCurrentTime();
	switch(m_currentstatus)
	{
	case -1:
		m_loggedouttimespan += (now-m_laststatustime);
		break;
	case 1:
		m_loggedintimespan += (now-m_laststatustime);
		break;
	}
	m_laststatustime = now;

	char buffer[100];
	CString loggedinstr, loggedoutstr;
	long loggedin_seconds = m_loggedintimespan.GetTotalSeconds();
	long loggedout_seconds = m_loggedouttimespan.GetTotalSeconds();
	long total_seconds = loggedin_seconds + loggedout_seconds;
	loggedinstr.LoadString(IDS_LOGGEDINTIMEPREFIX);
	loggedinstr += m_loggedintimespan.Format(" %D:%H:%M:%S");
	if(total_seconds > 0)
	{
		sprintf(buffer, " (%0.1f%%)", ((double)loggedin_seconds/total_seconds)*100.0);
		loggedinstr += buffer;
	}

	loggedoutstr.LoadString(IDS_LOGGEDOUTTIMEPREFIX);
	loggedoutstr += m_loggedouttimespan.Format(" %D:%H:%M:%S");
	if(total_seconds > 0)
	{
		sprintf(buffer, " (%0.1f%%)", ((double)loggedout_seconds/total_seconds)*100.0);
		loggedoutstr += buffer;
	}

	m_loggedintime.SetWindowText(loggedinstr);
	m_loggedouttime.SetWindowText(loggedoutstr);
}


LRESULT CIcloginDlg::OnCommonMessage(WPARAM wparam, LPARAM)
{
	if((HWND)wparam == GetSafeHwnd())
	{
		// It's us checking if there are someone around.
		return 0;
	}
	// Someone is signalling that they are starting.
	// Send answer (which will make them quit). Se Find_other_instance.
	::PostMessage((HWND)wparam, g_common_message, 0, (LPARAM)GetSafeHwnd());
	return 1;
}


/**
 * Tries to locate another instance by sending the common message to
 * all windows. Only the other instance now how to interpret and answer
 * it. We send a message with our HWND as WPARAM. We then listen for a
 * message with a HWND as LPARAM and 0 as WPARAM and brings that window 
 * to the foreground.
 */
BOOL CIcloginDlg::FindAnotherInstance()
{
	HWND hwnd = GetSafeHwnd();
	if(!g_common_message)
	{ 
		AfxMessageBox(IDS_NOCOMMONMESSAGE, MB_SYSTEMMODAL| MB_OK | MB_ICONSTOP);
		return false;
	}

	// Check if the program is already running.
//	DWORD result;
//	LRESULT rv=SendMessageTimeout(HWND_BROADCAST, g_common_message, 
//		(WPARAM)hwnd, (LPARAM)hwnd, 0, 100000000, &result);
	LRESULT rv=::SendMessage(HWND_BROADCAST, g_common_message, 
		(WPARAM)hwnd, (LPARAM)0);
	if(!rv)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		::MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}

	// During half a second, check for message from someone
	MSG msg;
	for(int i=0; i<5; i++)
	{
		Sleep(100);
		if(PeekMessage(
			&msg,         // message information
			hwnd,           // handle to window
			g_common_message,  // first message
			g_common_message,  // last message
			PM_REMOVE))      // removal options
		{
			// Got a message from someone. We are not alone!
			if(msg.message == g_common_message) // Could also be a WM_QUIT
			{
				if(msg.wParam == 0)
				{
					g_log.Log("Detecting already running instance. ");
					HWND other_instance = (HWND)msg.lParam;
					// But since the user wants one of us, let's pop up the other one.
					// It can't do it self since only certain windows are allowed 
					// to do ShowWindow on Windows 2000 and Windows 98.
					::ShowWindow(other_instance, SW_SHOWNORMAL);
					::SetForegroundWindow(other_instance);
				}
			}
			return true;
		}
	}

	// No answer. We're probably alone. 
	return false;
}

void CIcloginDlg::OnAboutDialog() 
{
	CAboutDialog dialog;

	dialog.DoModal();
}

//Notification handler
BOOL CIcloginDlg::OnToolTipNotify(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   CString strTipText;
   UINT nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   switch(nID)
   {
   case IDC_LOGINBUTTON:
	   strTipText.LoadString(IDS_LOGINBUTTONTOOLTIP);
	   if(m_everlogin)
	   {
		   strTipText += _T(" (");
		   strTipText += m_lastlogintime.Format(IDS_LASTLOGOUTTIME);
		   strTipText += _T(")");
	   }
	   break;
   case IDC_LOGOUTBUTTON:
	   strTipText.LoadString(IDS_LOGOUTBUTTONTOOLTIP);
	   if(m_everlogout)
	   {
		   strTipText += _T(" (");
		   strTipText += m_lastlogouttime.Format(IDS_LASTLOGOUTTIME);
		   strTipText += _T(")");
	   }
	   break;
   case IDC_CONFIGUREBUTTON:
	   strTipText.LoadString(IDS_CONFIGUREBUTTONTOOLTIP);
	   break;
   default: // No tooltip
	   ;
   }

   if (pNMHDR->code == TTN_NEEDTEXTA)
      lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
   else
      _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
   *pResult = 0;

   return TRUE;    // message was handled
}
