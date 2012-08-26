/*
 *  gm_itemlist.h
 *  GUIMUP struct for datatree item data
 *  (c) 2008-2009 Johan Spee
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

#ifndef GM_LISTITEM_H
#define GM_LISTITEM_H

#include <stdlib.h>
#include <list>
#include <glibmm/ustring.h>
    using Glib::ustring;

enum
{ 
// 	list item types
	TP_FOLDER,
	TP_ALBUM,
	TP_ARTIST,
	TP_SONG,
	TP_PLAYLIST,
	TP_PLISTITEM, 	// no drag&drop
	TP_GENRE, 		// no drag&drop
	TP_COMMENT, 	// no drag&drop
};

class listItem
{
public:
    int type;
	int time;
    ustring artist;
    ustring album;
    ustring title;
	ustring dirpath;
    ustring file;
	ustring track;
	ustring name;
	ustring date;
	ustring genre;
	ustring sorter;
	
	// Override the < operator for mylist.sort();
	bool operator < (const listItem& rhs)
	{
		return sorter < rhs.sorter;
	}
};



typedef std::list <listItem> gm_itemList;

#endif // GM_LISTITEM_H
