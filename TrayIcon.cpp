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

// TrayIcon.cpp: implementation of the CTrayIcon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iclogin.h"
#include "TrayIcon.h"
#include "icmessages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TRAYICONID 1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrayIcon::CTrayIcon()
: m_inited(false), m_tooltip(_T("IC Login")), m_loginstatus(0), 
	m_wnd(NULL), m_hwnd(0), m_hiconneutral(0), m_hiconplus(0),
	m_hiconminus(0), m_hcurrenticon(0)
{
}


CTrayIcon::~CTrayIcon()
{
	if(!m_inited)
	{
		return;
	}

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hwnd;
	nid.uID = TRAYICONID;
	nid.uFlags = NIF_TIP;
	Shell_NotifyIcon_Proxy(NIM_DELETE, &nid);
}



void CTrayIcon::Init(CWnd *ownerwindow)
{
	m_wnd = ownerwindow;
	m_hwnd = m_wnd->GetSafeHwnd();

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hiconneutral = AfxGetApp()->LoadIcon(IDI_TRAYICON);
	m_hiconplus = AfxGetApp()->LoadIcon(IDI_TRAYPLUS);
	m_hiconminus = AfxGetApp()->LoadIcon(IDI_TRAYMINUS);
	m_hcurrenticon = m_hiconneutral;

	// Add tray icon
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hwnd;
	nid.uID = TRAYICONID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = IC_TRAYICON;
	nid.hIcon = m_hcurrenticon;
	strcpy(nid.szTip, _T("IC Login"));
	Shell_NotifyIcon_Proxy(NIM_ADD, &nid);

	m_inited = true;
}


LRESULT CTrayIcon::HandleMessage(WPARAM, LPARAM lparam)
{
	if(!m_inited)
	{
		ASSERT(false);
		return 0;
	}

	switch(lparam)
	{
	case WM_LBUTTONDBLCLK:
		if(m_wnd->ShowWindow(SW_SHOWNORMAL))
		{
			// The window was already visible. Let's hide it
			m_wnd->ShowWindow(SW_HIDE);
		}
		else
		{
			m_wnd->SetForegroundWindow();
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
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
                m_wnd->SetForegroundWindow(); // Q135788
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, m_wnd); // use main window for cmds
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

void CTrayIcon::ModifyIcon() const
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hwnd;
	nid.uID = TRAYICONID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = IC_TRAYICON;
	nid.hIcon = m_hcurrenticon; 
	strcpy(nid.szTip, m_tooltip);
	if(!Shell_NotifyIcon_Proxy(NIM_MODIFY, &nid))
	{
		// The shell may have crashed. Let's add the icon instead.
		Shell_NotifyIcon_Proxy(NIM_ADD, &nid);
	}

}

void CTrayIcon::SetToolTip(const CString& new_tooltip)
{
	m_tooltip = new_tooltip;
	ModifyIcon();
}

void CTrayIcon::SetLogin(const CTime &logintime)
{
	m_loginstatus = 1;
	CString tooltip;
	tooltip.LoadString(IDS_LASTLOGINTIME);
	tooltip = logintime.Format(tooltip);
	m_hcurrenticon = m_hiconplus;
	SetToolTip(tooltip); // Will modify the icon also.
}


void CTrayIcon::SetLogout(const CTime &logouttime)
{
	m_loginstatus = -1;
	CString tooltip;
	tooltip.LoadString(IDS_LASTLOGOUTTIME);
	tooltip = logouttime.Format(tooltip);
	m_hcurrenticon = m_hiconminus;
	SetToolTip(tooltip); // Will modify the icon also.
}


typedef BOOL (__stdcall *Shell_NotifyIcon_Func)(DWORD dwMessage, PNOTIFYICONDATA pnid);

BOOL CTrayIcon::Shell_NotifyIcon_Proxy(DWORD dwMessage, PNOTIFYICONDATA pnid) const
{
	HINSTANCE shell32dll = LoadLibrary(_T("shell32.dll"));
	if(!shell32dll)
	{
		return FALSE;
	}
	Shell_NotifyIcon_Func func;
#ifdef _UNICODE
	func = (Shell_NotifyIcon_Func)GetProcAddress(shell32dll, "Shell_NotifyIconW");
#else
	func = (Shell_NotifyIcon_Func)GetProcAddress(shell32dll, "Shell_NotifyIconA");
#endif
	BOOL return_value = FALSE;
	if(func)
	{
		return_value = func(dwMessage, pnid);
	}

	FreeLibrary(shell32dll);

	return return_value;
}
