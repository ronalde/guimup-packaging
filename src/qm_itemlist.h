/*
*  qm_itemlist.h
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

#ifndef qm_listitemInfo_H
#define qm_listitemInfo_H

#include <QString>
#include <stdlib.h>
#include <list>

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
    TP_DIRECTORY,
    TP_PLISTITEM, 	// no drag&drop
    TP_COMMENT,  	// no drag&drop
    TP_NOITEM
};

class qm_listitemInfo
{
public:
    qm_listitemInfo();

    int type;
    int time;
    QString disc;
    QString artist;
    QString album;
    QString title;
    QString file;
    QString track;
    QString name;
    QString year;
    QString moddate;
    QString genre;
    QString sorter;

    // Override the < operator for alist.sort();
    bool operator < (const qm_listitemInfo& litem)
    {
        return sorter < litem.sorter;
    }
};


typedef std::list <qm_listitemInfo> qm_itemList;

#endif // qm_listitemInfo_H
