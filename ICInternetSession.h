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
