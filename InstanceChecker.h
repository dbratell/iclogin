#ifndef __SINGLEINSTANCE_H__
#define __SINGLEINSTANCE_H__

class CInstanceChecker 
{
private:
//Constructors / Destructors
  CInstanceChecker();
  ~CInstanceChecker();

public:
//General functions
  static bool SetOurselfAsRunning();
  static bool AlreadyRunning();
  static HWND ActivateOtherInstance(); 

protected:
  static CString MakeMMFFilename();
};


#endif //__SINGLEINSTANCE_H__