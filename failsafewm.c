/***************************************************************************
                         failsafewm.c  -  description
                         ---------------------
    begin                : Sun Dec 25 18:12:15 CEST 2001
    copyright            : (C) 2001 by Robert Sperling
    email                : sperling@small-window-manager.de
    version              : v0.0.2
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "failsafewm.h"

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
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("Tab")),
           Mod1Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("F2")),
           Mod1Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("F3")),
           Mod1Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("F4")),
           Mod1Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);
  XGrabKey(display,XKeysymToKeycode(display,XStringToKeysym("F5")),
           Mod1Mask,rootWindow,True,GrabModeAsync,GrabModeAsync);	   
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
  if (event->keycode == XKeysymToKeycode(display, XStringToKeysym("Tab")) 
      && event->state == Mod1Mask) 
  { XCirculateSubwindowsUp(display,rootWindow);
  }/* end if Alt-Tab */

  if (event->keycode == XKeysymToKeycode(display, XStringToKeysym("F2")) 
      && event->state == Mod1Mask) 
  { pointerMode=POINTERMOVEMODE;
    XGrabPointer(display, focusedWindow, False, 
                 PointerMotionMask | ButtonPressMask, GrabModeAsync, 
		 GrabModeAsync, None, None, CurrentTime); 
  }/* end if Alt-F2 */
  if (event->keycode == XKeysymToKeycode(display, XStringToKeysym("F3")) 
      && event->state == Mod1Mask) 
  { pointerMode=POINTERRESIZEMODE;
    XGrabPointer(display, focusedWindow, False, 
                 PointerMotionMask | ButtonPressMask, GrabModeAsync, 
		 GrabModeAsync, None, None, CurrentTime); 
    XGetGeometry(display,focusedWindow, &rootWindow, &dummy, &dummy, 
                 &sizeX, &sizeY, &dummy, &dummy);
  }/* end if Alt-F3 */
  if (event->keycode==XKeysymToKeycode(display,XStringToKeysym("F4")) 
      && event->state == Mod1Mask)		
  { sendExitClient(focusedWindow);
  }/* end if Alt-F4 */         
  if (event->keycode==XKeysymToKeycode(display,XStringToKeysym("F5")) 
      && event->state == Mod1Mask)		
  { XRaiseWindow(display,focusedWindow);
  }/* end if Alt-F5 */         

//  }/* end if rootWindow */
}/* end keyEventHandler */

/*************************************************************************/

/* buttonPressedEventHandler
   handles button presses after move/resize
*/
void buttonPressedEventHandler(XButtonEvent *event)
{ while (XCheckTypedEvent(display,ButtonPress,(XEvent *) event));

  if (event->window == rootWindow) 
  { switch (event->button) 
    { case Button1: system(CONFDIR "/.failsafewm/button1" "&"); break;
      case Button2: system(CONFDIR "/.failsafewm/button2" "&"); break;
      case Button3: system(CONFDIR "/.failsafewm/button3" "&"); break; 
    }	
  } 
  else 
  { XUngrabPointer(display,CurrentTime);
    switch (event->button) 
    { case Button1: pointerMode=0;
           break;
      case Button2: 
           break;
      case Button3: 
	   break;
    }/* end switch button */
  }/* end else if rootwin */
}/* end buttonPressEventHandler */

/*************************************************************************/

/* mouseMotionEventHandler
   handles mouse movement while moveing or resizing clients
*/

void mouseMotionEventHandler(XMotionEvent *event)
{ 

  if (motionStartingX<0) motionStartingX=event->x=event->x_root;
  if (motionStartingY<0) motionStartingY=event->y=event->y_root;     

  while (XCheckTypedEvent (display, MotionNotify, (XEvent *) event));

  if (pointerMode==POINTERMOVEMODE)
  { XMoveWindow(display, focusedWindow, event->x_root,event->y_root);
  }
  if (pointerMode==POINTERRESIZEMODE)
  { XResizeWindow(display, focusedWindow, 
                  sizeX+event->x_root-motionStartingX,
		  sizeY+event->y_root-motionStartingY);
  }
}/* end mouseMotionEventHandler */


/***************************************************************************/

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

/***************************************************************************/

void failsafewmEventLoop()
{ XEvent event;

  while (!exitfailsafewm) 
  { 
      XNextEvent(display, &event); 
      switch (event.type) 
      { 
        case ButtonPress  : buttonPressedEventHandler  	(&event.xbutton); 
             break;
        case MotionNotify : mouseMotionEventHandler    	(&event.xmotion);    
             break; 
	case KeyPress  	  : keyEventHandler  		(&event.xkey); 
             break;
      }
  }
}/* end failsafewmEventLoop */
