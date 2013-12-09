/*
*  qm_itemlist.cpp
*  QUIMUP library-tree item data
*  © 2008-2013 Johan Spee
*
*  This file is part of Quimup
*
*  QUIMUP is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GUIMUP is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see http://www.gnu.org/licenses/.
*/


#include "qm_itemlist.h"

qm_listitemInfo::qm_listitemInfo()
{
    type = -1;
    time = -1; // MPD_SONG_NO_TIME
    artist = "";
    album = "";
    title = "";
    file = "";
    track = "";
    name = "";
    year = "";
    moddate = "";
    genre = "";
    sorter = "";
    disc = "";
}
