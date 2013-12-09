/*
*  qm_libraryview.h
*  QUIMUP library treeview
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

#ifndef QM_LIBRARY_H
#define QM_LIBRARY_H

#include <QTreeWidget>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QKeyEvent>
#include <QMenu>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include "qm_songinfo.h"
#include "qm_mpdcom.h"
#include "qm_itemlist.h"
#include "qm_commandlist.h"
#include "qm_browser_ids.h"
#include "qm_config.h"

#define setdummy(dm)  setData(0, Qt::UserRole +  1, dm)
#define settype(tp)  setData(0, Qt::UserRole +  2, tp)
#define settime(tm)  setData(0, Qt::UserRole +  3, tm)
#define setartist(at)  setData(0, Qt::UserRole +  4, at)
#define setalbum(ab) setData(0, Qt::UserRole +  5, ab)
#define settitle(tt) setData(0, Qt::UserRole +  6, tt)
#define setfile(fl)  setData(0, Qt::UserRole +  8, fl)
#define settrack(tr)  setData(0, Qt::UserRole +  9, tr)
#define setname(nm)  setData(0, Qt::UserRole + 10, nm)
#define setgenre(gr)  setData(0, Qt::UserRole + 12, gr)

#define getdummy  data(0, Qt::UserRole +  1).toInt()
#define gettype   data(0, Qt::UserRole +  2).toInt()
#define gettime   data(0, Qt::UserRole +  3).toInt()
#define getartist   data(0, Qt::UserRole +  4).toString()
#define getalbum  data(0, Qt::UserRole +  5).toString()
#define gettitle  data(0, Qt::UserRole +  6).toString()
#define getfile   data(0, Qt::UserRole +  8).toString()
#define gettrack   data(0, Qt::UserRole +  9).toString()
#define getname   data(0, Qt::UserRole + 10).toString()
#define getgenre   data(0, Qt::UserRole + 12).toString()

class qm_libview : public QTreeWidget
{
	Q_OBJECT
	
public:
	qm_libview(QWidget * parent = 0);
	virtual ~qm_libview();
	
public slots:
	void set_connected(qm_mpdCom*, bool);
	void set_config(qm_config*);
	void reload_root();
	void on_plist_dropreceived(int);
	void set_panel_maxmode(bool);
	void set_auto_columns();
	
protected slots:
    void columnResized(int, int, int);
	
private slots:
	void on_item_expanded(QTreeWidgetItem *);
	void on_selection_changed();
	void on_append();
	void on_createnew();
	void on_delete();
	void on_opendir();
	void on_tagedit();
	void on_update();
	void startDrag(Qt::DropActions);
	void on_mpd_plistsaved();
	
private:
	bool b_mpd_connected;
	int plist_droppos,
		searched_mode_used,
		b_panel_max,
		column_count;
	qm_mpdCom *mpdCom;
	qm_config *config;
	QMenu *context_menu;
	QAction *a_delete;
	QAction *a_opendir;
	QAction *a_update;
	QAction *a_tagedit;
	QHeaderView *libhview;
	
	qm_commandList playlist_modification_list;
	void get_albums(QString searchfor = "*^*", bool sort_by_date = false);
	void get_artists(QString searchfor = "*^*");
	void get_genres(QString searchfor = "*^*");
	void get_songs(QString searchfor = "*^*");
	void get_playlists();
	void get_directories();
	void get_directories(QString); // search
	void get_albums_year();
	void expand_album(QTreeWidgetItem *);
	void expand_artist(QTreeWidgetItem *);
	void expand_playlist(QTreeWidgetItem *);
	void expand_directory(QTreeWidgetItem *);
	void expand_genre(QTreeWidgetItem *);
	void add_dummy_to(QTreeWidgetItem *);
	void take_dummy_from(QTreeWidgetItem *);
	void contextMenuEvent(QContextMenuEvent *);
	void add_songdata_toItem(QTreeWidgetItem *, qm_listitemInfo);
	void modify_playlist(bool, bool);
	void append_to_modlist(QString, QString, QString, bool);
	void append_to_modlist_dir(QString, bool);
	void append_to_modlist_plist(QString, bool);
	void set_column_count(int);
	void fix_header();
};

#endif //  QM_LIBRARY_H
