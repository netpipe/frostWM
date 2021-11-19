/***************************************************************************
 *   Copyright (C) 2004 by Sascha Hlusiak                                  *
 *   Spam84@gmx.de                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "joymouse.h"

#ifdef HAVE_LIBX11

#include <stdio.h>
#include <stdlib.h>
#include <X11/extensions/XTest.h>
#include "xkey.h"


static Display* display=NULL;


/**
 * Sends a key to the window of X11 that has the focus
 **/
void SendKey(KeyCode keycode,int press)
{
	if (display==NULL)return;
	XTestFakeKeyEvent(display,keycode,press?True:False,CurrentTime);

	XFlush(display);
}

KeyCode GetKeyCode(const char* s)
{
	KeySym sym=XStringToKeysym(s);
	if (display==NULL)return NoSymbol;
	if (sym==0)return NoSymbol;
	return XKeysymToKeycode(display,sym);
}


/**
 * Initializes the X11 lib
 */
void InitX11()
{
	display=XOpenDisplay(NULL);
	if (display==NULL)
	{
		printf("Could not open display. Sending keys disabled.\n");
		return;
	}

}

#endif
