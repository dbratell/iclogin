// ConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "iclogin.h"
#include "ConfigurationDialog.h"
#include "Configuration.h"
#include "ServiceMaster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigurationDialog dialog


CConfigurationDialog::CConfigurationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigurationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigurationDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConfigurationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigurationDialog)
	DDX_Control(pDX, IDC_LOGTOFILE, m_logtofile);
	DDX_Control(pDX, IDC_AUTOSTART, m_autostart);
	DDX_Control(pDX, IDC_CLEANREGISTRYBUTTON, m_cleanregistrybutton);
	DDX_Control(pDX, IDC_STARTHIDDEN, m_starthidden);
	DDX_Control(pDX, IDC_LOGINATINTERVALUNIT, m_loginatintervalunit);
	DDX_Control(pDX, IDC_LOGININTERVALFIELD, m_loginatintervalfield);
	DDX_Control(pDX, IDC_LOGINATINTERVAL, m_loginatinterval);
	DDX_Control(pDX, IDC_LOGINATSTARTUPCHECKBOX, m_loginatstartup);
	DDX_Control(pDX, IDC_HIDEPASSWORD, m_hidepassword);
	DDX_Control(pDX, IDC_USERNAMEFIELD, m_usernamefield);
	DDX_Control(pDX, IDC_PASSWORDFIELD, m_passwordfield);
	DDX_Control(pDX, IDC_CONFIGUSERNAMELABEL, m_usernamelabel);
	DDX_Control(pDX, IDC_CONFIGPASSWORDLABEL, m_passwordlabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigurationDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigurationDialog)
	ON_BN_CLICKED(IDC_HIDEPASSWORD, OnHidepassword)
	ON_EN_KILLFOCUS(IDC_LOGININTERVALFIELD, OnKillfocusLoginintervalfield)
	ON_BN_CLICKED(IDC_CLEANREGISTRYBUTTON, OnCleanregistrybutton)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigurationDialog message handlers

BOOL CConfigurationDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CString data;
	data.LoadString(IDS_CONFIGDIALOGTITLE);
	SetWindowText(data);

	data.LoadString(IDS_USERNAMELABEL);
	m_usernamelabel.SetWindowText(data);
	m_usernamefield.SetWindowText(CConfiguration::GetUsername());

	data.LoadString(IDS_PASSWORDLABEL);
	m_passwordlabel.SetWindowText(data);
	m_passwordfield.SetWindowText(CConfiguration::GetPassword());

	data.LoadString(IDS_HIDEPASSWORDLABEL);
	m_hidepassword.SetWindowText(data);
	if(CConfiguration::GetHidePassword())
	{
		m_hidepassword.SetCheck(true);
	} 
	else
	{
		m_hidepassword.SetCheck(false);
	}
	OnHidepassword(); // Does the hiding

	data.LoadString(IDS_LOGINATSTARTUPLABEL);
	m_loginatstartup.SetWindowText(data);
	if(CConfiguration::GetLoginAtStartup())
	{
		m_loginatstartup.SetCheck(true);
	}
	else
	{
		m_loginatstartup.SetCheck(false);
	}

	data.LoadString(IDS_LOGINATINTERVALLABEL);
	m_loginatinterval.SetWindowText(data);
	CString number;
	int interval = CConfiguration::GetLoginInterval()/60;
	if(interval < 1) interval = 1;
	number.Format("%d", interval);
	m_loginatintervalfield.SetWindowText(number);
	if(CConfiguration::GetLoginAtInterval())
	{
		m_loginatinterval.SetCheck(true);
		m_loginatintervalfield.EnableWindow(true);
	}
	else
	{
		m_loginatstartup.SetCheck(false);
		m_loginatintervalfield.EnableWindow(false);
	}

	data.LoadString(IDS_LOGINATINTERVALUNIT);
	m_loginatintervalunit.SetWindowText(data);

	data.LoadString(IDS_STARTHIDDEN);
	m_starthidden.SetWindowText(data);
	if(CConfiguration::GetStartHidden())
	{
		m_starthidden.SetCheck(true);
	}
	else
	{
		m_starthidden.SetCheck(false);
	}

	data.LoadString(IDS_AUTOSTART);
	m_autostart.SetWindowText(data);
	if(CConfiguration::GetAutoStart())
	{
		m_autostart.SetCheck(true);
	}
	else
	{
		m_autostart.SetCheck(false);
	}

	data.LoadString(IDS_LOGTOFILE);
	m_logtofile.SetWindowText(data);
	if(CConfiguration::GetLogToFile())
	{
		m_logtofile.SetCheck(true);
	}
	else
	{
		m_logtofile.SetCheck(false);
	}


	data.LoadString(IDS_CLEANREGISTRYBUTTON);
	m_cleanregistrybutton.SetWindowText(data);

	EnableToolTips(true);
	
	if(!CConfiguration::GetIsConfigured())
	{
	//	ModifyStyleEx(0, WS_EX_TOPMOST);
		SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE  | SWP_NOSIZE );
	}


//	CEdit	m_loginatintervalfield;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigurationDialog::OnHidepassword() 
{
	if(m_hidepassword.GetCheck())
	{
		m_passwordfield.SetPasswordChar('*');
	} 
	else
	{
		m_passwordfield.SetPasswordChar(0);
	}
	m_passwordfield.Invalidate();
}

void CConfigurationDialog::OnOK() 
{
	CString data;

	// Check data
	m_loginatintervalfield.GetWindowText(data);
	if(m_loginatinterval.GetCheck() && 
		atoi(data) <= 0)
	{
		CString message;
		message.LoadString(IDS_INCORRECTLOGININTERVAL);
		MessageBox(message);
		return;
	}

	CString username;
	m_usernamefield.GetWindowText(username);
	username.TrimLeft();
	username.TrimRight();
	CString password;
	m_passwordfield.GetWindowText(password);
	password.TrimLeft();
	password.TrimRight();

	if(username.IsEmpty() || password.IsEmpty())
	{
		CString message;
		message.LoadString(IDS_NOUSERORPASSWORD);
		MessageBox(message);
		return;
	}

	// Save data
	CConfiguration::SetHidePassword(m_hidepassword.GetCheck() != 0);

	CConfiguration::SetLoginAtStartup(m_loginatstartup.GetCheck() != 0);

	CConfiguration::SetLoginAtInterval(m_loginatinterval.GetCheck() != 0);

	CConfiguration::SetStartHidden(m_starthidden.GetCheck() != 0);

	CConfiguration::SetAutoStart(m_autostart.GetCheck() != 0);

	if(m_logtofile.GetCheck() != 0 && !CConfiguration::GetLogToFile())
	{
		g_log.SetLogFile(CConfiguration::GetLogFile());
	}
	else if(m_logtofile.GetCheck() == 0 && CConfiguration::GetLogToFile())
	{
		g_log.SetLogFile("");
	}
	CConfiguration::SetLogToFile(m_logtofile.GetCheck() != 0);

	m_loginatintervalfield.GetWindowText(data);
	if(m_loginatinterval.GetCheck())
		CConfiguration::SetLoginInterval(atoi(data)*60);

	CConfiguration::SetUsername(username);

	CConfiguration::SetPassword(password);

	CDialog::OnOK();
}

void CConfigurationDialog::OnKillfocusLoginintervalfield() 
{
	CString data;
	m_loginatintervalfield.GetWindowText(data);
	if(data.IsEmpty())
	{
		CString number;
		int interval = CConfiguration::GetLoginInterval()/60;
		if(interval < 1) interval = 1;
		number.Format("%d", interval);
		m_loginatintervalfield.SetWindowText(number);
		return;
	}

	bool good_input = true;
	for(int i=0; i<data.GetLength(); i++)
	{
		if(!isdigit(data.GetAt(i)))
		{
			good_input = false;
			break;
		}
	}

	if(!good_input || atoi(data) <= 0)
	{
		CString message;
		message.LoadString(IDS_WRONGINTERVALVALUE);
		MessageBox(message);
		m_loginatintervalfield.SetFocus();
	}
}



void CConfigurationDialog::OnCleanregistrybutton() 
{
	CServiceMaster::RemoveService();
	CConfiguration::CleanRegistry();
	AfxMessageBox(IDS_CLEANREGISTRYINFORMATION);
	::PostQuitMessage(0);
	EndDialog(IDCANCEL);
}


//Notification handler
BOOL CConfigurationDialog::OnToolTipNotify(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
   UINT nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   pTTT->hinst = AfxGetResourceHandle();

   switch(nID)
   {
   case IDC_USERNAMEFIELD:
	   pTTT->lpszText = (LPTSTR)IDS_USERNAMEFIELDTOOLTIP;
	   break;
   case IDC_PASSWORDFIELD:
	   pTTT->lpszText = (LPTSTR)IDS_PASSWORDFIELDTOOLTIP;
	   break;
   case IDC_HIDEPASSWORD:
	   pTTT->lpszText = (LPTSTR)IDS_HIDEPASSWORDTOOLTIP;
	   break;
   case IDC_LOGINATSTARTUPCHECKBOX:
	   pTTT->lpszText = (LPTSTR)IDS_LOGINATSTARTUPCHECKBOXTOOLTIP;
	   break;
   case IDC_LOGINATINTERVAL:
	   pTTT->lpszText = (LPTSTR)IDS_LOGINATINTERVALTOOLTIP;
	   break;
   case IDC_LOGININTERVALFIELD:
	   if(m_loginatinterval.GetCheck()) // No tooltip if not active.
	   {
		   pTTT->lpszText = (LPTSTR)IDS_LOGININTERVALFIELDTOOLTIP;
	   }
	   else
	   {
		   pTTT->hinst = NULL;
		   strcpy(pTTT->lpszText, _T(""));
	   }
	   break;
   case IDC_STARTHIDDEN:
	   pTTT->lpszText = (LPTSTR)IDS_STARTHIDDENTOOLTIP;
	   break;
   case IDC_AUTOSTART:
	   pTTT->lpszText = (LPTSTR)IDS_AUTOSTARTTOOLTIP;
	   break;
   case IDC_CLEANREGISTRYBUTTON:
	   pTTT->lpszText = (LPTSTR)IDS_CLEANREGISTRYBUTTONTOOLTIP;
	   break;
   case IDOK:
	   pTTT->lpszText = (LPTSTR)IDS_CONFIGUREIDOKTOOLTIP;
	   break;
   case IDCANCEL:
	   pTTT->lpszText = (LPTSTR)IDS_CONFIGUREIDCANCELTOOLTIP;
	   break;
   case IDC_LOGTOFILE:
	   {
		   pTTT->hinst = NULL;
		   CString tooltip;
		   tooltip.LoadString(IDS_LOGTOFILETOOLTIP);
		   tooltip += " "+CConfiguration::GetLogFile();
		   strcpy(pTTT->lpszText, tooltip); // Less than 80 chars
	   }
	   break;
   default: // No tooltip
	   TRACE1("Why are there no tooltip for control %d?\n", nID);
   }

   *pResult = 0;

   return TRUE;    // message was handled
}
