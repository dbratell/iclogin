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

#define ICLOGIN_SERVICE


#include "resource.h"		// main symbols
#include "Log.h"

extern const _TCHAR* const IC_VERSIONSTRING;
extern const _TCHAR* const IC_APPURL;
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
	void StartAsService();

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#ifdef ICLOGIN_SERVICE
bool IsWindowsNT();
bool IsWindows2000();
#endif

#endif // !defined(AFX_ICLOGIN_H__36C2C509_4A53_4078_BAA7_ACF3347747C1__INCLUDED_)
