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

// TrayIcon.h: interface for the CTrayIcon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAYICON_H__944C8858_5F53_4E59_8203_D8FA0178B2F7__INCLUDED_)
#define AFX_TRAYICON_H__944C8858_5F53_4E59_8203_D8FA0178B2F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTrayIcon  
{
public:
	CTrayIcon();
	virtual ~CTrayIcon();

	void Init(CWnd *ownerwindow);

	LRESULT HandleMessage(WPARAM wparam, LPARAM lparam);
	void SetToolTip(const CString& new_tooltip);
	void SetLogin(const CTime &logintime);
	void SetLogout(const CTime &logouttime);

protected:
	void ModifyIcon() const;
	BOOL Shell_NotifyIcon_Proxy(DWORD dwMessage, PNOTIFYICONDATA pnid) const;

	// Attributes
private:
	CWnd *m_wnd;
	HWND m_hwnd;
	bool m_inited;
	HICON m_hiconneutral, m_hiconplus, m_hiconminus, m_hcurrenticon;
	CString m_tooltip;
	int m_loginstatus;
};

#endif // !defined(AFX_TRAYICON_H__944C8858_5F53_4E59_8203_D8FA0178B2F7__INCLUDED_)
