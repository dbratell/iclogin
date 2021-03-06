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