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

typedef HINSTANCE (__stdcall *ShellExecuteFunc)(HWND hwnd, LPCTSTR lpVerb, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);

void CAboutDialog::OnAppurl() 
{
	// This is the only use of shell.dll which is a large dll. Better to
	// open it dynamically, since people don't care about working set when
	// judging a program.

	HINSTANCE shell32dll = LoadLibrary(_T("shell32.dll"));
	if(!shell32dll)
	{
		return;
	}
	ShellExecuteFunc func;
#ifdef _UNICODE
	func = (ShellExecuteFunc)GetProcAddress(shell32dll, "ShellExecuteW");
#else
	func = (ShellExecuteFunc)GetProcAddress(shell32dll, "ShellExecuteA");
#endif
	if(func)
	{
		func(*this, NULL, IC_APPURL, NULL, "", SW_RESTORE);
	}

	FreeLibrary(shell32dll);
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

	_TCHAR letter = m_credits.GetAt(m_scrolloffset);
	++m_scrolloffset;
	if(CharWidth(letter < 6))
	{
		++m_scrolloffset;
	}

	if(m_scrolloffset >= length)
	{
		m_scrolloffset = m_scrolloffset % length;
	}

	CString mess; 
	mess.Format("%c is %d wide.\n", letter, CharWidth(letter));
	TRACE(mess);
}

inline int CAboutDialog::CharWidth(_TCHAR letter)
{
	int rv=8;
	CDC *dc = GetDC();
	if(!dc)
	{
		ASSERT(false);
		return 8;
	}
	
	int width;
	if(dc->GetCharWidth(letter, letter, &width))
	{
		rv = width;
	}

	ReleaseDC(dc);
	return rv;
}
