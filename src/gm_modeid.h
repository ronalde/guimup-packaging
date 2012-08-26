/*
 *  gm_modeid.h
 *  GUIMUP combo mode IDs
 *  (c) 2008-2012 Johan Spee
 *
 *  This file is part of Guimup
 *
 *  GUIMUP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GUIMUP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#ifndef GM_MODEID_H
#define GM_MODEID_H

enum
{
    // library modes
    LB_ID_ARTIST,
    LB_ID_ALBUM,
    LB_ID_BYDATE,
	LB_ID_BYEAR,
    LB_ID_GENRE,
    LB_ID_PLIST,
    LB_ID_FOLDER,
    LB_ID_SEARCH,
	LB_ID_SONGS // libraryview only
};

enum 
{	// search modes
    SR_ID_ARTIST,
    SR_ID_ALBUM,
    SR_ID_TITLE,
    SR_ID_GENRE
};

enum 
{	// select modes
    SL_ID_ARTIST,
    SL_ID_ALBUM,
    SL_ID_TITLE,
    SL_ID_ALL
};

enum
{	// lib context menu
	MN_ID_append,
	MN_ID_newlist,
	MN_ID_dellist
	
};
#endif // GM_MODEID_H
