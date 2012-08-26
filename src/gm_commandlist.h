/*
 *  gm_commandlist.h
 *  GUIMUP struct to pass commands to mpd
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

#ifndef GM_COMMAND_H
#define GM_COMMAND_H

enum
{
	CMD_DEF, // default (none)
	CMD_DEL, // delete
	CMD_ADD, // apend
	CMD_INS, // insert at moveTo
	CMD_MOV, // move to moveTo
	CMD_POS, // move pos to pos
	CMD_DPL  // delete playlist
};


//#include <stdlib.h>
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
} gm_cmdStruct;


typedef std::list<gm_cmdStruct> gm_commandList;

#endif // GM_COMMAND_H
