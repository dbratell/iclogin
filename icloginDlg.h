// icloginDlg.h : header file
//

#if !defined(AFX_ICLOGINDLG_H__904D94FD_A822_4E02_9267_9C616E22EEA8__INCLUDED_)
#define AFX_ICLOGINDLG_H__904D94FD_A822_4E02_9267_9C616E22EEA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define LOGINTIMER 1284
#define UPDATETIMER 1433
#define TRAYICONID 1

/////////////////////////////////////////////////////////////////////////////
// CIcloginDlg dialog
class CIcloginDlg : public CDialog
{
// Construction
public:
	CIcloginDlg(CWnd* pParent = NULL);	// standard constructor

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
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

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
	afx_msg LRESULT OnCommonMessage(WPARAM wparam, LPARAM lparam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DisplayMessage(UINT message_id);
	void SetupLoginTimer();
	void SetLoginStatus(int status);
	BOOL Find_another_instance();
	void UpdateTimers();


// Attributes
	UINT m_logintimer;
	UINT m_updatetimer;
	int m_currentstatus;
	bool m_failedlastlogin;

	CTimeSpan m_loggedintimespan, m_loggedouttimespan;
	CTime m_laststatustime;

	bool m_startingup;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICLOGINDLG_H__904D94FD_A822_4E02_9267_9C616E22EEA8__INCLUDED_)
