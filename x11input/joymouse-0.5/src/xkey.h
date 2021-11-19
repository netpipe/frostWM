#ifndef _X_KEY_INCLUDED_
#define _X_KEY_INCLUDED_

#ifdef HAVE_LIBX11

#include <X11/Xlib.h>

void InitX11();
KeyCode GetKeyCode(const char* s);
void SendKey(KeyCode keycode,int press);

#endif


#endif
