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

// ICInternetSession.h: interface for the CICInternetSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICINTERNETSESSION_H__05573B5A_55A7_4B66_ADDF_FA666C1B528F__INCLUDED_)
#define AFX_ICINTERNETSESSION_H__05573B5A_55A7_4B66_ADDF_FA666C1B528F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CICInternetSession : public CInternetSession  
{
public:
	CICInternetSession(CWnd *parent);
	virtual ~CICInternetSession();

	// Override
	virtual void OnStatusCallback( DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength );

	// Attributes
private:
	CWnd *m_wnd;

};

#endif // !defined(AFX_ICINTERNETSESSION_H__05573B5A_55A7_4B66_ADDF_FA666C1B528F__INCLUDED_)
