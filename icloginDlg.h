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

// icloginDlg.h : header file
//

#if !defined(AFX_ICLOGINDLG_H__904D94FD_A822_4E02_9267_9C616E22EEA8__INCLUDED_)
#define AFX_ICLOGINDLG_H__904D94FD_A822_4E02_9267_9C616E22EEA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TrayIcon.h"

#define LOGINTIMER 1284
#define UPDATETIMER 1433
#define ROTATEICONTIMER 1543
#define RESTARTDHCPTIMER 2313

/////////////////////////////////////////////////////////////////////////////
// CIcloginDlg dialog
class CIcloginDlg : public CDialog
{
// Construction and destruction
public:
	CIcloginDlg(CWnd* pParent = NULL, bool running_as_service=false);	// standard constructor
	~CIcloginDlg();

// Dialog Data
	//{{AFX_DATA(CIcloginDlg)
	enum { IDD = IDD_ICLOGIN_DIALOG };
	CStatic	m_loggedouttime;
	CStatic	m_loggedintime;
	CStatic	m_statusicon;
	CButton	m_configurebutton;
	CStatic	m_messagetext2;
	CButton	m_quitbutton;
	CButton	m_logoutbutton;
	CButton	m_loginbutton;
	CStatic	m_messagetext;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcloginDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
//	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	public:
	virtual BOOL Create(CWnd *pParentWnd = NULL);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CIcloginDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoginbutton();
	afx_msg void OnLogoutbutton();
	afx_msg LRESULT OnLoginStarted(WPARAM, LPARAM);
	afx_msg LRESULT OnLoginSucceeded(WPARAM, LPARAM);
	afx_msg LRESULT OnLoginFailed(WPARAM, LPARAM);
	afx_msg LRESULT OnLoginAlreadyLogin(WPARAM, LPARAM);
	afx_msg LRESULT OnLogoutStarted(WPARAM, LPARAM);
	afx_msg LRESULT OnLogoutFailed(WPARAM, LPARAM);
	afx_msg LRESULT OnLogoutSucceeded(WPARAM, LPARAM);
	afx_msg LRESULT OnRestartDHCPStarted(WPARAM, LPARAM);
	afx_msg LRESULT OnRestartDHCPFailed(WPARAM, LPARAM);
	afx_msg LRESULT OnRestartDHCPSucceeded(WPARAM, LPARAM);
	afx_msg LRESULT OnLoginThreadStarted(WPARAM, LPARAM);
	afx_msg LRESULT OnLoginThreadTerminated(WPARAM, LPARAM);
	afx_msg LRESULT OnLogoutThreadStarted(WPARAM, LPARAM);
	afx_msg LRESULT OnLogoutThreadTerminated(WPARAM, LPARAM);
	afx_msg LRESULT OnRestartDHCPThreadStarted(WPARAM, LPARAM);
	afx_msg LRESULT OnRestartDHCPThreadTerminated(WPARAM, LPARAM);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnResolvingName(WPARAM wparam, LPARAM);
	afx_msg LRESULT OnResolvedName(WPARAM wparam, LPARAM);
	afx_msg LRESULT OnConnectingServer(WPARAM wparam, LPARAM);
	afx_msg LRESULT OnConnectedServer(WPARAM wparam, LPARAM);
	afx_msg LRESULT OnSendingRequest(WPARAM, LPARAM);
	afx_msg LRESULT OnSentRequest(WPARAM, LPARAM);
	afx_msg LRESULT OnReceivingResponse(WPARAM, LPARAM);
	afx_msg LRESULT OnReceivedResponse(WPARAM, LPARAM);
	afx_msg LRESULT OnClosingConnection(WPARAM, LPARAM);
	afx_msg LRESULT OnClosedConnection(WPARAM, LPARAM);
	afx_msg void OnConfigurebutton();
	afx_msg LRESULT OnTrayIcon(WPARAM wparam, LPARAM lparam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAboutDialog();
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnRestartDHCP();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

private:
	void DisplayMessage(UINT message_id);
	void SetupLoginTimer();
	void SetLoginStatus(int status);
	void UpdateTimers();
	void SetupLocalizedText();
	CString GetLocalIpNumber();
	void SetDialogTitle();
	void RotateIcons();
	void LoadIcons();
	void MinimizeMemoryUsage(bool reallyminimize = false);
	void OnThreadTerminated();
	void OnThreadStarted();


// Attributes
	UINT m_logintimer;
	UINT m_updatetimer;
	UINT m_restartdhcptimer;
	int m_currentstatus;  // -1 not logged in, 0 unknown, 1 logged in.
	int m_expectedstatus;
	bool m_failedlastlogin;
	bool m_running_as_service;

	CTimeSpan m_loggedintimespan, m_loggedouttimespan;
	CTime m_laststatustime;
	bool m_everlogin, m_everlogout;
	CTime m_lastlogintime, m_lastlogouttime;

	CTrayIcon *m_trayicon;
	bool m_oktoclose; // true when it's ok for the user to close the whole app

	int m_thread_counter;
	CMutex m_thread_counter_mutex;
	int m_icon_rotation_state;
	HICON m_happy_icon[4];
	HICON m_question_icon[4];
	HICON m_sad_icon[4];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICLOGINDLG_H__904D94FD_A822_4E02_9267_9C616E22EEA8__INCLUDED_)
