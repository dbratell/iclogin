// AboutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "iclogin.h"
#include "AboutDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SCROLLWIDTH 30

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog


CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDialog::IDD, pParent), 	
	m_scrolloffset(0)

{
	//{{AFX_DATA_INIT(CAboutDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_credits.LoadString(IDS_CREDITS);
	m_credits += _T("       ");
}


void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDialog)
	DDX_Control(pDX, IDC_SCROLLTEXT, m_scrolltext);
	DDX_Control(pDX, IDC_APPURL, m_appurl);
	DDX_Control(pDX, IDC_VERSIONLABEL, m_versionlabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
	//{{AFX_MSG_MAP(CAboutDialog)
	ON_BN_CLICKED(IDC_APPURL, OnAppurl)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SCROLLTIMER  4342
/////////////////////////////////////////////////////////////////////////////
// CAboutDialog message handlers

BOOL CAboutDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_versionlabel.SetWindowText(IC_VERSIONSTRING);
	m_appurl.SetWindowText(IC_APPURL);
		// TODO: Add extra initialization here
	
	SetScrollText();
	SetTimer(SCROLLTIMER, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDialog::OnAppurl() 
{
	ShellExecute(*this, NULL, IC_APPURL, NULL, "", SW_RESTORE);
}


/**
 * CREDITS: 
 * Jonas Svensson (testare, idéspruta)
 * Andréas Bratell (testare, idévulkan)
 * Marcus Comstedt (vars script gav insikt i hur nya inloggninssystemet fungerade)
 * Calle Englund (dito)
 */
void CAboutDialog::OnDestroy() 
{
	KillTimer(SCROLLTIMER);

	CDialog::OnDestroy();
}


void CAboutDialog::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case SCROLLTIMER:
		++m_scrolloffset;
		SetScrollText();
		break;
	default:
		CDialog::OnTimer(nIDEvent);
	}
}

void CAboutDialog::SetScrollText()
{
	int length = m_credits.GetLength();
	if(length == 0)
	{
		ASSERT(false);
		m_scrolltext.SetWindowText(_T(""));
		return;
	}

	if(m_scrolloffset > length)
	{
		m_scrolloffset = m_scrolloffset % length;
	}

	CString toshow = m_credits.Right(length-m_scrolloffset);
	while(toshow.GetLength() < SCROLLWIDTH)
	{
		toshow += m_credits;
	}
	if(toshow.GetLength() > SCROLLWIDTH) // XXX Magic
	{
		toshow = toshow.Left(SCROLLWIDTH);
	}

	m_scrolltext.SetWindowText(toshow);
}
