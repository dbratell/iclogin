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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
: m_log_to_file(false)
{

}

CLog::~CLog()
{

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


void CLog::Log(const CString &text)
{
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