// iclogin.h : main header file for the ICLOGIN application
//

#if !defined(AFX_ICLOGIN_H__36C2C509_4A53_4078_BAA7_ACF3347747C1__INCLUDED_)
#define AFX_ICLOGIN_H__36C2C509_4A53_4078_BAA7_ACF3347747C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Log.h"

extern const char* const IC_VERSIONSTRING;
extern CLog g_log;

/////////////////////////////////////////////////////////////////////////////
// CIcloginApp:
// See iclogin.cpp for the implementation of this class
//

class CIcloginApp : public CWinApp
{
public:
	CIcloginApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcloginApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	private:
	//{{AFX_MSG(CIcloginApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICLOGIN_H__36C2C509_4A53_4078_BAA7_ACF3347747C1__INCLUDED_)
