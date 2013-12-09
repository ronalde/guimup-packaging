/*
*  qm_songinfo.cpp
*  QUIMUP struct for song data
*  © 2008-2013 Johan Spee
*
*  This file is part of Quimup
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


#include "qm_songinfo.h"

qm_songInfo::qm_songInfo()
{
    reset();
}

void qm_songInfo::reset()
{
    nosong = true;
    songNr = -1;
    songID = -1;
    time = -1;
    type = -1;
    disc = "";
    file = "";
    name = "";
    artist = "";
    title = "";
    album = "";
    date = "";
    track = "";
    disc = "";
    genre = "";
    comment = "";
}

qm_songInfo::~qm_songInfo()
{
}
