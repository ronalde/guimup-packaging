/*
*  qm_playlistview.h
*  QUIMUP playlist listview
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


#ifndef QM_PLAYLIST_H
#define QM_PLAYLIST_H
#include <QApplication>
#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QKeyEvent>
#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QTime>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringList>
#include <QUrl>
#include "qm_songinfo.h"
#include "qm_mpdcom.h"
#include "qm_config.h"
#include "qm_commandlist.h"
#include "qm_browser_ids.h"

#define set_id(id)  setData(0, Qt::UserRole + 1, id)
#define set_state(state)  setData(0, Qt::UserRole + 2, state)
#define set_time(time)   setData(0, Qt::UserRole + 4, time)
#define set_file(file)   setData(0, Qt::UserRole + 5, file)
#define set_type(type)   setData(0, Qt::UserRole + 6, type)
#define set_pos(pos)   setData(0, Qt::UserRole + 7, pos)

#define get_id   data(0, Qt::UserRole + 1).toInt()
#define get_state  data(0, Qt::UserRole + 2).toInt()
#define get_time  data(0, Qt::UserRole + 4).toInt()
#define get_file  data(0, Qt::UserRole + 5).toString()
#define get_type  data(0, Qt::UserRole + 6).toInt()
#define get_pos   data(0, Qt::UserRole + 7).toInt()

#define STATE_NEW   0
#define STATE_PLAYING  1
#define STATE_PLAYED  2

class qm_plistview : public QTreeWidget
{
    Q_OBJECT

public:
    qm_plistview(QWidget * parent = 0);
    virtual ~qm_plistview();

public slots:
    void set_newsong(int);
    void set_status(int);
    void set_markplayed();
    void set_panel_maxmode(bool);
    void set_connected(qm_mpdCom*, bool);
    void set_config(qm_config*);
    void select_it(int);
    void dropEvent(QDropEvent *);
    void on_open_with_request(const QString&);
    void on_open_with_droprequest(QDropEvent*);
    void set_auto_columns();

protected slots:
    void columnResized(int, int, int);

private slots:
    void dragLeaveEvent(QDragLeaveEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void showLine_at(int);
    void on_mpd_newlist(qm_songinfo, int);
    void on_item_dclicked(QTreeWidgetItem *);
    void clear_it();
    void purge_it();
    void shuffle_it();
    void delete_it();
    void save_it();
    void save_selected();
    void startDrag(Qt::DropActions);

private:
    bool
        b_streamplaying,
        b_mpd_connected,
        b_waspurged,
        b_panel_max;

    QWidget *line;

    int
    current_songPos,
    current_songID,
    current_status,
    dropBeforeIndex;

    qm_mpdCom *mpdCom;
    QColor  col_default_fg,
    col_playing_fg,
    col_played_fg;
    QHeaderView *plisthview;
    qm_config *config;
    QMenu *context_menu;
    QString into_time(int);
    QAction *a_shuffle,
            *a_savelist,
            *a_savesel,
            *a_delsel,
            *a_clearlist;
    qm_IDlist get_played_IDs();
    void contextMenuEvent(QContextMenuEvent *);
    void keyPressEvent(QKeyEvent *);
    void set_playlist_stats(int, int);
    bool file_check(const QString &);
    void fix_header();

signals:
    void sgnl_plist_statistics(QString);
    void sgnl_plist_dropreceived(int);
};

#endif //  QM_PLAYLIST_H
