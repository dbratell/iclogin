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

// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iclogin.h"
#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int CLog::LOG_ALL = 5000;
const int CLog::LOG_DUMP = 50;
const int CLog::LOG_INFO = 20;
const int CLog::LOG_WARNING = 10;
const int CLog::LOG_ERROR = 5;
const int CLog::LOG_NONE = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
: m_log_to_file(false), m_loglevel(CLog::LOG_INFO)
{

}

CLog::~CLog()
{

}

void CLog::SetLogLevel(int new_log_level)
{
	m_loglevel = new_log_level;
}

bool CLog::SetLogFile(const CString &filename)
{
	CSingleLock lock(&m_mutex, true);
	if(m_logfile.m_hFile != CFile::hFileNull)
	{
		try
		{
			m_logfile.Close();
		} catch(...)
		{
		}

		m_log_to_file = false;
	}

	if(filename.IsEmpty())
		return true;

	if(m_logfile.Open(filename, 
		CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite |
		CFile::shareDenyWrite))
	{
		// Closed automagically in the destructor
		try
		{
			m_logfile.SeekToEnd();
		}
		catch (...)
		{
			TRACE("Couldn't move to end of logfile!\n");
			return false;
		}

		m_log_to_file = true;
		return true;
	}
	return false;
}


void CLog::Log(const CString &text, int log_level /* = LOG_INFO */)
{
	if(m_loglevel < log_level)
		return;

	CSingleLock lock(&m_mutex, true);

	CTime now = CTime::GetCurrentTime();
	
//	CString timestring = now.Format("%Y-%m-%d %H:%M:%S %Z: ");
	CString timestring = now.Format("%Y-%m-%d %H:%M:%S  ");
	
	if(m_log_to_file)
	{
		try {
			m_logfile.WriteString(timestring+text+_T("\n"));
			m_logfile.Flush();
		} catch(...) {}
	}

	TRACE(timestring+text+_T("\n"));
}