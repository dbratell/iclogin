// ConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "iclogin.h"
#include "ConfigurationDialog.h"
#include "Configuration.h"

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
	//}}AFX_MSG_MAP
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
	char buffer[20];
	int interval = CConfiguration::GetLoginInterval()/60;
	if(interval < 1) interval = 1;
	_itoa(interval, buffer, 10);
	m_loginatintervalfield.SetWindowText(buffer);
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

	// Save data
	CConfiguration::SetHidePassword(m_hidepassword.GetCheck() != 0);

	CConfiguration::SetLoginAtStartup(m_loginatstartup.GetCheck() != 0);

	CConfiguration::SetLoginAtInterval(m_loginatinterval.GetCheck() != 0);

	CConfiguration::SetStartHidden(m_starthidden.GetCheck() != 0);

	m_loginatintervalfield.GetWindowText(data);
	if(m_loginatinterval.GetCheck())
		CConfiguration::SetLoginInterval(atoi(data)*60);

	m_usernamefield.GetWindowText(data);
	CConfiguration::SetUsername(data);

	m_passwordfield.GetWindowText(data);
	CConfiguration::SetPassword(data);

	CDialog::OnOK();
}

void CConfigurationDialog::OnKillfocusLoginintervalfield() 
{
	CString data;
	m_loginatintervalfield.GetWindowText(data);
	if(data.IsEmpty())
	{
		char buffer[20];
		int interval = CConfiguration::GetLoginInterval()/60;
		if(interval < 1) interval = 1;
		_itoa(interval, buffer, 10);
		m_loginatintervalfield.SetWindowText(buffer);
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


