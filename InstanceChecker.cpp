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

#include "stdafx.h"
#include "InstanceChecker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////// Implementation //////////////////////////////

//struct which is put into shared memory
struct CWindowInstance
{
  HWND hMainWnd;
};

//Class which be used as a static to ensure that we
//only close the file mapping at the very last chance
class _INSTANCE_DATA
{
public:
	_INSTANCE_DATA() { 	hInstanceData = NULL; }
	~_INSTANCE_DATA() { hInstanceData ? ::CloseHandle(hInstanceData) : NULL; }

private:
	HANDLE hInstanceData;
	friend class CInstanceChecker;
};

static _INSTANCE_DATA instanceData;

#if 0
CInstanceChecker::CInstanceChecker()
{
}

CInstanceChecker::~CInstanceChecker()
{
}
#endif

/** Track the first instance of our App.
 *  Call this after LoadFrame() in InitInstance(). Call PreviousInstanceRunning() first
 *  and only call this if it returns false.
 *
 *  @return TRUE on success, else FALSE - another instance is already running.
 */
bool CInstanceChecker::SetOurselfAsRunning()
{
	//Create the MMF
	int nMMFSize = sizeof(CWindowInstance);
	instanceData.hInstanceData = ::CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE,
		NULL, PAGE_READWRITE, 0, nMMFSize, MakeMMFFilename());
	if(!instanceData.hInstanceData) //Creating the MMF should work
	{
		return false;
	}

	//Open the MMF
	CWindowInstance* pInstanceData = (CWindowInstance*) ::MapViewOfFile(instanceData.hInstanceData, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, nMMFSize);
	ASSERT(pInstanceData != NULL);   //Opening the MMF should work
	ASSERT(AfxGetMainWnd() != NULL); //Did you forget to set up the mainfrm in InitInstance ?
	ASSERT(AfxGetMainWnd()->GetSafeHwnd() != NULL);
	pInstanceData->hMainWnd = AfxGetMainWnd()->GetSafeHwnd();

	::UnmapViewOfFile(pInstanceData);

	return true;
}

/** Returns true if a previous instance of the App is running.
 */
bool CInstanceChecker::AlreadyRunning()
{
	bool return_value = false;
	//Try to open the MMF first to see if we are the second instance
	HANDLE hPrevInstance = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MakeMMFFilename());

	if(hPrevInstance)
		return_value = true;

	::CloseHandle(hPrevInstance);

	return return_value;
}


CString CInstanceChecker::MakeMMFFilename()
{
	//MMF name is taken from MFC's AfxGetAppName
	CString appname(AfxGetAppName()); 
	ASSERT(!appname.IsEmpty());
	return appname+_T("_running");
}

/** Activate the Previous Instance of our Application.
 *  @note Call PreviousInstanceRunning() before calling this function.
 *  @return hWnd of the previous instance's MainFrame if successful, else NULL.
 */
HWND CInstanceChecker::ActivateOtherInstance()
{
	HANDLE hPrevInstance = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MakeMMFFilename());
	ASSERT(hPrevInstance != NULL); //Whats happened to my handle !
	// Open up the MMF
	int nMMFSize = sizeof(CWindowInstance);
	CWindowInstance* pInstanceData = (CWindowInstance*) ::MapViewOfFile(hPrevInstance, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, nMMFSize);
	if (pInstanceData != NULL) //Opening the MMF should work
	{
		//activate the old window
		HWND other_instance = pInstanceData->hMainWnd;
		::ShowWindow(other_instance, SW_SHOWNORMAL);
		::SetForegroundWindow(other_instance);

		//Unmap the MMF we were using
		::UnmapViewOfFile(pInstanceData);

		//Close the file handle now that we 
		::CloseHandle(hPrevInstance);
		return other_instance;
	}
	else
	{
		ASSERT(false);  //Somehow MapViewOfFile failed.
	}

	//Close the file handle now that we 
	::CloseHandle(hPrevInstance);

	return NULL;
}


