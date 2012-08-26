/*
*  gm_songinfo.cpp
*  GUIMUP struct for song data
*  (c) 2008-2012 Johan Spee
*
*  This file is part of Guimup
*
*  QUIMUP is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  QUIMUP is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see http://www.gnu.org/licenses/.
*/


#include "gm_songinfo.h"

gm_songInfo::gm_songInfo()
{
	reset();
}


void gm_songInfo::reset()
{
    nosong = true;
    songNr = -1;
    songID = -1;
    time = -1;
    type = -1;
	name = "";
    artist = "";
    title = "";
    album = "";
    year = "";
    track = "";
    genre = "";
    comment = "";
	file_mpd = "";
	file_dir = "";
	file_name = "";
}

gm_songInfo::~gm_songInfo()
{
}
