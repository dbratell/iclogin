#if !defined(AFX_CONFIGURATIONDIALOG_H__973E90AE_53C4_4B50_8F10_B432EBFF7C18__INCLUDED_)
#define AFX_CONFIGURATIONDIALOG_H__973E90AE_53C4_4B50_8F10_B432EBFF7C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigurationDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigurationDialog dialog

class CConfigurationDialog : public CDialog
{
// Construction
public:
	CConfigurationDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigurationDialog)
	enum { IDD = IDD_CONFIGURATION };
	CButton	m_restartdhcp;
	CButton	m_startupmethod;
	CButton	m_servicepopuperror;
	CButton	m_noautostart;
	CButton	m_logtofile;
	CButton	m_cleanregistrybutton;
	CButton	m_starthidden;
	CStatic	m_loginatintervalunit;
	CEdit	m_loginatintervalfield;
	CButton	m_loginatinterval;
	CButton	m_loginatstartup;
	CButton	m_hidepassword;
	CEdit	m_usernamefield;
	CEdit	m_passwordfield;
	CStatic	m_usernamelabel;
	CStatic	m_passwordlabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigurationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigurationDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHidepassword();
	virtual void OnOK();
	afx_msg void OnKillfocusLoginintervalfield();
	afx_msg void OnCleanregistrybutton();
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStartupMethod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATIONDIALOG_H__973E90AE_53C4_4B50_8F10_B432EBFF7C18__INCLUDED_)
