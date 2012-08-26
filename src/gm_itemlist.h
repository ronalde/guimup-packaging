/*
 *  gm_itemlist.h
 *  GUIMUP struct for datatree item data
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

#ifndef GM_LISTITEM_H
#define GM_LISTITEM_H

//#include <stdlib.h>
#include <list>
#include <glibmm/ustring.h>
    using Glib::ustring;

enum
{ 
	//  list item types
	TP_ARTIST,
	TP_ALBUM,
	TP_GENRE,
	TP_SONG,
	TP_SONGX,		// external song
	TP_NOSONG,
	TP_STREAM,
	TP_PLAYLIST,
	TP_FOLDER,
	TP_PLISTITEM,
	TP_NOITEM
};

class gm_listItem
{
public:
	int type;
	int time;		// playtime
	int modtime; 	// last modified time_t
	ustring artist;
	ustring album;
	ustring title;
	ustring file;	// full mpd-path
	ustring track;
	ustring name;	// file name
	ustring year;
	ustring genre;
	ustring moddate;
	ustring sorter;
	
	gm_listItem()
	{ 
		b_sort_by_modtime = false;
		
		type = TP_NOITEM;
		time = 0;
		modtime = 0; 
		artist = "";
		album = "";
		title = "";
		file = "";
		track = "";
		name = "";
		year = "";
		genre = "";
		moddate = "";
		sorter = "";
	}
	
	virtual ~gm_listItem()
	{	}

	// Override the < operator for mylist.sort()
	bool operator < (gm_listItem& liter)
	{
		if (b_sort_by_modtime)
			return modtime < liter.modtime;
		else
			return sorter < liter.sorter;
	}

	void set_sort_modtime()
	{
		b_sort_by_modtime = true;
	}
	
private:
	bool b_sort_by_modtime;
};


typedef std::list<gm_listItem> gm_itemList;


#endif // GM_LISTITEM_H
