// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__5BA52E31_40C9_4848_B089_1C73CE6511EB__INCLUDED_)
#define AFX_LOG_H__5BA52E31_40C9_4848_B089_1C73CE6511EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLog  
{
public:
	CLog();
	virtual ~CLog();
	bool SetLogFile(const CString &filename);
	void Log(const CString &text);

	// Attributes
private:
	CStdioFile m_logfile;
	bool m_log_to_file;
	CMutex m_mutex;

};

#endif // !defined(AFX_LOG_H__5BA52E31_40C9_4848_B089_1C73CE6511EB__INCLUDED_)
