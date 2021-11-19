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

#ifndef _JOYMOUSE_INCLUDED_
#define _JOYMOUSE_INCLUDED_
 
 
enum JOYSTICKTYPE{TYPE_NONE,TYPE_BYVALUE,TYPE_ACCELERATED};
enum MAPPING{	MAPPING_NONE=0,
		MAPPING_X,MAPPING_Y,
		MAPPING_ZX,MAPPING_ZY,
		MAPPING_BUTTON,MAPPING_KEY,
		MAPPING_SPEED_MULTIPLY,
		MAPPING_DISABLE,MAPPING_DISABLE_MOUSE,MAPPING_DISABLE_KEYS};


struct JOYSTICKDATA
{
	struct AXIS
	{
		int value;
		enum JOYSTICKTYPE type;
		enum MAPPING mapping;
	}axis[32];
	struct BUTTON
	{
		char pressed;
		int value;
		enum MAPPING mapping;
	}button[32];
};


#endif
