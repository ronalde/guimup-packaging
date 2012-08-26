/*
 *  gm_songinfo.h
 *  GUIMUP struct for song data
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

#ifndef GM_SONGINFO_H
#define GM_SONGINFO_H

#include <list>
#include <glibmm/ustring.h>
    using Glib::ustring;

class gm_songInfo
{
public:
    gm_songInfo();
	virtual ~gm_songInfo();
    
    bool nosong;// NULL song
    int songNr; // pos in list
    int songID; // ID in list
    double time;
    int type;
    ustring name;
    ustring artist;
    ustring title;
    ustring album;
    ustring year;
    ustring track;
    ustring genre;
    ustring comment;
	ustring file_mpd;
	ustring file_dir;
	ustring file_name;

	void reset();
};

typedef std::list <gm_songInfo> gm_songList;

#endif // GM_SONGINFO_H
