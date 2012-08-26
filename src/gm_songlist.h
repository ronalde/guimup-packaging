/*
 *  gm_songlist.h
 *  GUIMUP struct for song data
 *  (c) 2008 Johan Spee
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

struct songInfo
{
    bool nosong;// NULL song
    int songNr; // list pos
	int songID; // list ID
	int time;
    ustring file;
    ustring name;
    ustring artist;
    ustring title;
    ustring album;
    ustring date;
    ustring track;
    ustring disc;
    ustring genre;
    ustring performer;
    ustring composer;
    ustring comment;
};

typedef std::list <songInfo> gm_songList;

#endif // GM_SONGINFO_H
