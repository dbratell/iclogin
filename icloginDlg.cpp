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

/////////////////////////////////////////////////////////////////////////////
// CIcloginDlg dialog

CIcloginDlg::CIcloginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIcloginDlg::IDD, pParent),
	m_logintimer(0), m_loggedintimespan(0), m_loggedouttimespan(0), 
	m_failedlastlogin(false), m_currentstatus(0),
	m_expectedstatus(0), m_everlogin(false), m_everlogout(false),
	m_oktoclose(false)
{
	//{{AFX_DATA_INIT(CIcloginDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	ON_COMMAND(IDC_ABOUT, OnAboutDialog)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcloginDlg message handlers

BOOL CIcloginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetupLocalizedText();

	EnableToolTips(true);
	
	SetLoginStatus(0);

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

	SetDialogTitle();
}

void CIcloginDlg::SetDialogTitle() 
{
	CString window_title;
	window_title.LoadString(IDS_DIALOGTITLE);
	CString ip = GetLocalIpNumber();
	if(!ip.IsEmpty())
	{
		window_title += _T(" - ");
		window_title += ip;
	}
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
	SetupLoginTimer();
	StartLoginThread(this);
}

void CIcloginDlg::OnLogoutbutton() 
{
	if(m_logintimer != 0)
	{
		KillTimer(m_logintimer);
		m_logintimer = 0;
	}
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
	{
		KillTimer(m_logintimer);
		m_logintimer = 0;
	}

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
	{
		KillTimer(m_logintimer);
		m_logintimer = 0;
	}

	CDialog::OnDestroy();
}


LRESULT CIcloginDlg::OnResolvingName(WPARAM wparam, LPARAM)
{
	CString resolving;
	resolving.LoadString(IDS_RESOLVING);
	resolving += _T(" ");
	resolving += (LPCSTR)wparam;

	m_messagetext2.SetWindowText(resolving);
	g_log.Log(resolving, CLog::LOG_DUMP);
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
	g_log.Log(connecting, CLog::LOG_DUMP);
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
	g_log.Log(sending, CLog::LOG_DUMP);
	return 0;
}


LRESULT CIcloginDlg::OnSentRequest(WPARAM, LPARAM)
{
	CString sending;
	sending.LoadString(IDS_SENTREQUEST);

	m_messagetext2.SetWindowText(sending);
	g_log.Log(sending, CLog::LOG_DUMP);
	return 0;
}


LRESULT CIcloginDlg::OnReceivingResponse(WPARAM, LPARAM)
{
	CString got;
	got.LoadString(IDS_RECEIVINGRESPONSE);

	m_messagetext2.SetWindowText(got);
	g_log.Log(got, CLog::LOG_DUMP);
	return 0;
}


LRESULT CIcloginDlg::OnReceivedResponse(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	g_log.Log("Received response", CLog::LOG_DUMP);
	return 0;
}


LRESULT CIcloginDlg::OnClosingConnection(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	g_log.Log("Closing connection", CLog::LOG_DUMP);
	return 0;
}


LRESULT CIcloginDlg::OnClosedConnection(WPARAM, LPARAM)
{
	m_messagetext2.SetWindowText("");
	g_log.Log("Closed connection", CLog::LOG_DUMP);
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

	if(nType == SIZE_MINIMIZED)
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

	// Display new IP-number
	SetDialogTitle();
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

	CString buffer;
	CString loggedinstr, loggedoutstr;
	long loggedin_seconds = m_loggedintimespan.GetTotalSeconds();
	long loggedout_seconds = m_loggedouttimespan.GetTotalSeconds();
	long total_seconds = loggedin_seconds + loggedout_seconds;
	loggedinstr.LoadString(IDS_LOGGEDINTIMEPREFIX);
	loggedinstr += m_loggedintimespan.Format(" %D:%H:%M:%S");
	if(total_seconds > 0)
	{
		buffer.Format(" (%0.1f%%)", ((double)loggedin_seconds/total_seconds)*100.0);
		loggedinstr += buffer;
	}

	loggedoutstr.LoadString(IDS_LOGGEDOUTTIMEPREFIX);
	loggedoutstr += m_loggedouttimespan.Format(" %D:%H:%M:%S");
	if(total_seconds > 0)
	{
		buffer.Format(" (%0.1f%%)", ((double)loggedout_seconds/total_seconds)*100.0);
		loggedoutstr += buffer;
	}

	m_loggedintime.SetWindowText(loggedinstr);
	m_loggedouttime.SetWindowText(loggedoutstr);
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

void CIcloginDlg::PostNcDestroy() 
{
	g_log.Log("IC Login exited");
	delete this;
}

void CIcloginDlg::OnClose() 
{
	if (m_oktoclose)
	{
		// CleanUp ();
		DestroyWindow();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}

	// CDialog::OnClose();
}

//BOOL CIcloginDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
BOOL CIcloginDlg::Create(CWnd *pParentWnd /*= NULL */) 
{
	BOOL returnvalue = CDialog::Create(IDD, pParentWnd);
	// Configure it if this is the first time.
	if(!CConfiguration::GetIsConfigured())
	{
		ShowWindow(SW_HIDE);
		g_log.Log("First start.");
		CConfigurationDialog config_dialog;
		if(config_dialog.DoModal() != IDOK)
		{
			// Without configuration, it's not much we can do.
			g_log.Log("No configuration. Have to quit.");
			EndDialog(0);
			::PostQuitMessage(0);
			return false;
		}
		
		CConfiguration::SetIsConfigured(true);
		ShowWindow(SW_SHOW);
	}

	m_trayicon.Init(this);
	
	if(CConfiguration::GetLoginAtStartup() && !CConfiguration::GetUsername().IsEmpty())
	{
		StartLoginThread(this);
	}

	SetupLoginTimer();

	m_updatetimer = SetTimer(UPDATETIMER, 
			CConfiguration::GetUpdateTimersInterval()*1000, 
			NULL);

	return returnvalue;
}

/**
 * Called when the user choses exit from the menu.
 */
void CIcloginDlg::OnOK() 
{
	TRACE("OnOK\n");
	m_oktoclose = true;
	DestroyWindow();
}


CString CIcloginDlg::GetLocalIpNumber()
{
	char szHostName[128];

	BOOL bPrivateAdr = false; 
	// Private Address area 
	BOOL bClassA = false;     
	// Class A definition 
	BOOL bClassB = false;     
	// Class B definition 
	BOOL bClassC = false;     
	// Class C definition 
	BOOL bAutoNet = false;    
	// AutoNet definition 
	CString str;
	WORD wVersionRequested = MAKEWORD( 1, 1 );
	WSADATA wsaData;

	WSAStartup(wVersionRequested,  &wsaData);

	if(!gethostname(szHostName, sizeof(szHostName)))
	{
		// Get host adresses 
		struct hostent * pHost;
		int i;
		UINT ipb;
		pHost = gethostbyname(szHostName); 

		for(i = 0; 
	       pHost!= NULL && pHost->h_addr_list[i]!= NULL; 
		   i++ )
		{
			int j;
			str="";
			bClassA = bClassB = bClassC = false;
			for( j = 0; j < pHost->h_length; j++ )
			{
				CString addr;

				if( j > 0 )	str += ".";
				ipb = (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j];

				// Define the IP range for exclusion 
				if(j==0)
				{
					bClassA = (ipb == 10);
					if(bClassA) break; // Class A defined 
					bClassB = (ipb == 172); 
					bClassC = (ipb == 192);
					bAutoNet = (ipb == 169);
				}
				else if (j==1)
				{
					// Class B defined 
					bClassB = (bClassB && ipb >= 16 && ipb <= 31);
					if(bClassB) break;

					// Class C defined 
					bClassC = (bClassC && ipb == 168);
					if(bClassC) break;

					//AutoNet pasibility defined 
					bAutoNet = (bAutoNet && ipb == 254);
					if(bAutoNet) break;
				}

				addr.Format("%u", ipb );
				str += addr;
			}
			// If any address of Private Address  
			// area has been found bPrivateAdr = true 
			bPrivateAdr = bPrivateAdr || bClassA || bClassB || bClassC;

			// If any address of Internet Address area  
			// has been found returns TRUE 
			if (!bClassA && !bClassB && !bClassC && !bAutoNet && 
				str != "127.0.0.1" && !str.IsEmpty()) 
			{
				WSACleanup();
				return str;
			}
		}
	}

	if (bPrivateAdr)
	{
		// The system has IP address from Private Address 
		// area,only. Internet from the computer can be accessable 
		// via Proxy. If user turn on proxy connection flag, the 
		// function believe Internet accessable. 
		// return bProxyConnection;
		WSACleanup();
		return "";
	}
 
 	WSACleanup();
	return "";
}
