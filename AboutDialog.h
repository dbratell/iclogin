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

#if !defined(AFX_ABOUTDIALOG_H__E443F4A3_39C4_4ECC_99E1_28E8EC5B27E5__INCLUDED_)
#define AFX_ABOUTDIALOG_H__E443F4A3_39C4_4ECC_99E1_28E8EC5B27E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog

class CAboutDialog : public CDialog
{
// Construction
public:
	CAboutDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutDialog)
	enum { IDD = IDD_ABOUTDIALOG };
	CStatic	m_scrolltext;
	CButton	m_appurl;
	CStatic	m_versionlabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAboutDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAppurl();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Attributes
private:
	CString m_credits;
	int m_scrolloffset;

	// Implementation
private:
	void SetScrollText();
	int CharWidth(_TCHAR letter);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDIALOG_H__E443F4A3_39C4_4ECC_99E1_28E8EC5B27E5__INCLUDED_)
