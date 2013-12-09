/*
*  qm_commandlist.h
*  QUIMUP struct to pass commands to MPD
*  © 2008-2009 Johan Spee
*
*  This file is part of Guimup
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

#ifndef QM_COMMAND_H
#define QM_COMMAND_H


#define CMD_DEF 0 // default (none)
#define CMD_DEL 1 // delete
#define CMD_ADD 2 // apend
#define CMD_INS 3 // insert at moveTo
#define CMD_MOV 4 // move ID to moveTo
#define CMD_POS 5 // move pos to moveTo
#define CMD_DPL 6 // delete playlist
#define CMD_SCN 7 // rescan
#define CMD_UPD 8 // update


#include <QList>
#include <QTreeWidgetItem>
#include <QString>

typedef struct qm_mpd_command
{
    int cmd;
    int songnr;
    int songid;
    int moveTo;
    QString file;
    /*  below: these are used by the playlist when
    the library sends a commandlist to add items  */
    int time;
    int type;
    QString artist;
    QString album;
    QString title;
    QString track;
}
qm_mpd_command;


typedef QList<qm_mpd_command> qm_commandList;

#endif // QM_COMMAND_H
