/*
 *  gm_commandlist.h
 *  GUIMUP struct to pass commands to mpd
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

#ifndef GM_COMMAND_H
#define GM_COMMAND_H


#define CMD_DEF 0 // default (none)
#define CMD_DEL 1 // delete
#define CMD_ADD 2 // apend
#define CMD_INS 3 // insert at moveTo
#define CMD_MOV 4 // move to moveTo
#define CMD_DPL 5 // delete playlist

#include <stdlib.h>
#include <list>
#include <glibmm/ustring.h>
    using Glib::ustring;

typedef struct gm_cmdStruct
{
    int cmd;
    unsigned int songnr;
    unsigned int songid;
    unsigned int moveTo;
    ustring file;
    /*  below: used by the playlist when the
        datatree sends a commandlist    */
    int time;
    ustring artist;
    ustring album;
    ustring title;
	ustring track;
	ustring genre;
} gm_cmdStruct;


typedef std::list<gm_cmdStruct> gm_commandList;

#endif // GM_COMMAND_H
