/*
 *  qm_songinfo.h
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

#ifndef QM_SONGINFO_H
#define QM_SONGINFO_H

# include <QString>
# include <QList>

class qm_songInfo
{
public:
    qm_songInfo();
    virtual ~qm_songInfo();

    void reset();

    bool nosong;// NULL song
    int songNr; // pos in list
    int songID; // ID in list
    int time;
    int type;
    QString disc;
    QString file;
    QString name;
    QString artist;
    QString title;
    QString album;
    QString date;
    QString track;
    QString genre;
    QString comment;
};

typedef QList <qm_songInfo> qm_songinfo;

#endif // QM_SONGINFO_H
