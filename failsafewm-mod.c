#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <stdio.h>

#define true        	  1
#define false       	  0

/* X11 needed things */
static Display *	  display;
static char		 *displayOption = "";
static int		  screen;
static Window	  rootWindow;
Window  		  focusedWindow; 
int 			  dummy, sizeX, sizeY;

/* used atoms */
Atom                      xa_wm_protos;
Atom                      xa_wm_delete;
int			  exitfailsafewm = false;

void failsafewmQuitNicely();
void initfailsafewm();
void grabKeys();
void failsafewmEventLoop();
void keyEventHandler (XKeyEvent *event);
void sendExitClient(Window clientWindow);

/***************************************************************************
 *   Main - Methode                                                        *
 ***************************************************************************/

int main(int argc, char *argv[])
{ XSetWindowAttributes attr; 
  display = XOpenDisplay(displayOption); 

  if (!display) 
  { fprintf(stderr,"failsafewm: cannot open display %s, giving up\n", displayOption);
    exit(1);
  }
  screen = DefaultScreen(display);
  rootWindow = RootWindow(display, screen);

//  xa_wm_change_state = XInternAtom(display, "WM_CHANGE_STATE", False);
  xa_wm_protos       = XInternAtom(display, "WM_PROTOCOLS", False);
  xa_wm_delete       = XInternAtom(display, "WM_DELETE_WINDOW", False);

  grabKeys();
  failsafewmEventLoop();   
  exit(0); 
}// end main

/***************************************************************************/

/* grabKeys
   grabs keys only to root window
 
*/
void grabKeys() 
{
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("z")),
           Mod4Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("q")),
           Mod4Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("r")),
           Mod4Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("f")),
           Mod4Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("Left")),
           Mod4Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("Right")),
           Mod4Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);	   
}/* end grabKeys */        

/***************************************************************************/

/* keyEventHandler
   manages all incoming key events for failsafewm, keys are usually grabbed to 
   the rootWindow   
*/

void keyEventHandler (XKeyEvent *event)
{ int     dummyInt; 
  Window  dummyWin;

  XQueryPointer(display, rootWindow, &dummyWin, &focusedWindow, 
                &dummyInt, &dummyInt, &dummyInt, &dummyInt, &dummyInt); 
  if (event->keycode == XKeysymToKeycode(display, XStringToKeysym("z")) 
      && event->state == Mod4Mask) 
  { XCirculateSubwindowsUp(display,rootWindow);
	XSetInputFocus(display, PointerRoot, RevertToPointerRoot, CurrentTime);
  }/* end if Win-z */
  if (event->keycode==XKeysymToKeycode(display,XStringToKeysym("q")) 
      && event->state == Mod4Mask)		
  { sendExitClient(focusedWindow);
  }/* end if Win-q */         
  if (event->keycode==XKeysymToKeycode(display,XStringToKeysym("r")) 
      && event->state == Mod4Mask)		
  { system("dmenu_run");
  }/* end if Win-r */ 
  if (event->keycode==XKeysymToKeycode(display,XStringToKeysym("f")) 
      && event->state == Mod4Mask)		
  { XMoveResizeWindow(display, focusedWindow, 0, 0, 1920, 1080);
  }/* end if Win-f */
  if (event->keycode==XKeysymToKeycode(display,XStringToKeysym("Left")) 
      && event->state == Mod4Mask)		
  { XMoveResizeWindow(display, focusedWindow, 0, 0, 960, 1080);
  }/* end if Win-Left */
  if (event->keycode==XKeysymToKeycode(display,XStringToKeysym("Right")) 
      && event->state == Mod4Mask)		
  { XMoveResizeWindow(display, focusedWindow, 960, 0, 960, 1080);
  }/* end if Win-Right */                 

//  }/* end if rootWindow */
}/* end keyEventHandler */

/*************************************************************************/

/* sendExitClient
   sends the client a message to quit itself nicely, if it doesn`t listen to
   xa_wm_delete, we try to kill it as nicely as possible.
*/

void sendExitClient(Window clientWindow)
{ int count, maxcount, found = 0;
  Atom *protocols;

  if (XGetWMProtocols(display, clientWindow, &protocols, &maxcount))
  { for (count=0; count<maxcount; count++) 
    { if (protocols[count] == xa_wm_delete) found++;
    }
    if (protocols) XFree(protocols);
  }
  if (found) 
  { sendXMessage(clientWindow, xa_wm_protos, xa_wm_delete);
  }
  else 
  { XKillClient(display, clientWindow);
  }/* end else if found */
}/* end sendExitClient */

/*************************************************************************/

/* sendXMessage 
   sends a xwindow event to the window "givenWin" 
*/

int sendXMessage(Window givenWin, Atom xatom, long value)
{ XEvent event;
  event.type 			= ClientMessage;
  event.xclient.window 		= givenWin;
  event.xclient.message_type 	= xatom;
  event.xclient.format 		= 32;
  event.xclient.data.l[0] 	= value;
  event.xclient.data.l[1] 	= CurrentTime;

  return XSendEvent(display, givenWin, False, NoEventMask, &event);
}// end sendXmessage 

/***************************************************************************/

void failsafewmEventLoop()
{ XEvent event;

  while (!exitfailsafewm) 
  { 
      XNextEvent(display, &event); 
      switch (event.type) 
      { 
	case KeyPress  	  : keyEventHandler  		(&event.xkey); 
             break;
      }
  }
}/* end failsafewmEventLoop */
