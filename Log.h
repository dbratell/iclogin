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
	void SetLogLevel(int new_log_level);
	bool SetLogFile(const CString &filename);
	void Log(const CString &text, int loglevel = CLog::LOG_INFO);

	// Attributes
private:
	CStdioFile m_logfile;
	bool m_log_to_file;
	CMutex m_mutex;
	int m_loglevel;

public:
	static const int LOG_ALL;
	static const int LOG_DUMP;
	static const int LOG_INFO;
	static const int LOG_WARNING;
	static const int LOG_ERROR;
	static const int LOG_NONE;

};

#endif // !defined(AFX_LOG_H__5BA52E31_40C9_4848_B089_1C73CE6511EB__INCLUDED_)
