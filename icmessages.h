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

#define IC_LOGINSTARTED (WM_USER+1)
#define IC_LOGINSUCCEEDED (WM_USER+2)
#define IC_LOGINFAILED (WM_USER+3)
#define IC_LOGINALREADYLOGIN (WM_USER+4)
#define IC_LOGINABORTED (WM_USER+5)

#define IC_LOGOUTSTARTED (WM_USER+24)
#define IC_LOGOUTSUCCEEDED (WM_USER+25)
#define IC_LOGOUTFAILED (WM_USER+26)
#define IC_LOGOUTABORTED (WM_USER+27)

#define IC_LOGINTHREADSTARTED (WM_USER+47)
#define IC_LOGINTHREADTERMINATED (WM_USER+48)
#define IC_LOGOUTTHREADSTARTED (WM_USER+49)
#define IC_LOGOUTTHREADTERMINATED (WM_USER+50)
#define IC_RESTARTDHCPTHREADSTARTED (WM_USER+51)
#define IC_RESTARTDHCPTHREADTERMINATED (WM_USER+52)

#define IC_RESOLVINGNAME (WM_USER+70)
#define IC_RESOLVEDNAME (WM_USER+71)
#define IC_CONNECTINGSERVER (WM_USER+72)
#define IC_CONNECTEDSERVER (WM_USER+73)
#define IC_SENDINGREQUEST (WM_USER+74)
#define IC_SENTREQUEST (WM_USER+75)
#define IC_RECEIVINGRESPONSE (WM_USER+76)
#define IC_RECEIVEDRESPONSE (WM_USER+77)
#define IC_CLOSINGCONNECTION (WM_USER+78)
#define IC_CLOSEDCONNECTION (WM_USER+79)
#define IC_RESTARTDHCPSTARTED (WM_USER+80)
#define IC_RESTARTDHCPFAILED (WM_USER+81)
#define IC_RESTARTDHCPSUCCEEDED (WM_USER+82)

#define IC_TRAYICON (WM_USER+90)

