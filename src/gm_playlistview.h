/*
 *  gm_playlistview.h
 *  GUIMUP playlist-view
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

#ifndef GM_PLAYLIST_H
#define GM_PLAYLIST_H

#include <gtkmm/main.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/treerowreference.h>
#include <gtkmm/liststore.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/imagemenuitem.h>
#include <list>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <sigc++/sigc++.h>
#include "gm_songinfo.h"
#include "gm_config.h"
#include "gm_commandlist.h"
#include "gm_itemlist.h"
#include "gm_modeid.h"
#include "gm_mpdcom.h"
#include "gm_treeview.h"

// for convenience
typedef Gtk::TreeModel::Children 	 tm_Children;
typedef Gtk::TreeModel::Path 		 tm_Path;
typedef Gtk::TreeModel::iterator 	 tm_Iterator;
typedef Gtk::TreeModel::RowReference tm_RowRef;
typedef Gtk::TreeModel::Row			 tm_Row;


class gm_Playlist: public Gtk::ScrolledWindow
{
public:
    gm_Playlist();
    virtual ~gm_Playlist();
//  functions
	void get_configs();
    void set_playlist(gm_songList);
    void set_mpdcom(gm_mpdCom*);
	void set_songInfo (gm_songInfo *);
	void set_config(gm_Config *);
	void set_connected(bool);
	void set_stream(std::list<ustring> url_list);
	void set_listfont(Pango::FontDescription);
	void quick_select(ustring, int);
	void set_consume(bool);
	void set_columns_fixed();
	void set_menu_purge();

//signals
	sigc::signal<void> signal_plist_saved;
	sigc::signal<void, int> signal_drop_received;
	
protected:

private:
//  functions
    ustring into_time(int);
    std::string into_string(int);
    void on_row_activated(const tm_Path&, Gtk::TreeViewColumn*);
    bool on_key_pressed(GdkEventKey*);
    bool on_button_pressed(GdkEventButton*);
	void on_acckey_pressed(GdkEventKey*);
    void on_del_selected();
	void renumber_list();
    bool string_from_input(ustring, ustring &);
	void on_pldata_received( const Glib::RefPtr<Gdk::DragContext>&, int, int, const Gtk::SelectionData&, guint, guint);
	void on_pldrag_data_get( const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData&, guint, guint);
	bool on_pldrag_motion(const Glib::RefPtr<Gdk::DragContext>&, int, int, guint);
	void on_new_playlist(gm_songList, int);
	void on_newStatus( mpd_status *);
	void on_newSong();
	void on_external_drop(std::vector<ustring>, int);
	void create_rClickmenu();
	void on_menu_shuffle();
	void on_menu_clear();
	void on_menu_purge();
	void on_menu_savelist();
	void on_menu_saveselected();
	void set_nrColumn_width();
	void show_message(ustring);
	gm_IDlist get_played_IDs();

//  variables
	bool 
		b_allow_newplist,
		b_streamplaying,
		b_connected,
		b_nosong,
		b_consume;

	std::vector<Gtk::TargetEntry> DnDlist;
    int current_status,
		current_songID,
		plistLength,
		mouse_offset;
	
	Gtk::Menu *rClickMenu;
	Gtk::ImageMenuItem
		*imi_purge,
		*imi_shuffle,
		*imi_clear,
		*imi_rmove,
		*imi_save,
		*imi_selsave;
	
	Glib::RefPtr<Pango::Context> context;
	Glib::RefPtr<Pango::Layout> layout;
	
	gm_commandList dtCommandList;
    gm_mpdCom *mpdCom;
	gm_songInfo *current_sInfo;
	gm_Config *config;
    Glib::RefPtr<Gdk::Pixbuf>
        pxb_tr_track,
        pxb_tr_trplayed,
		pxb_tr_trackx,
        pxb_tr_trxplayed,
		pxb_tr_stream,
        pxb_tr_strplayed,	
		pxb_tr_idle,
        pxb_tr_play,
        pxb_tr_stop,
        pxb_tr_pause,
		pxb_tr_playlist,
		pxb_drag_playlist;
	Gtk::Image 
		img_men_save,
		img_men_saveselect,
		img_men_clear,
		img_men_delete,
		img_men_purge,
		img_men_shuffle;

//	widgets
    class plistColumnRecord : public Gtk::TreeModelColumnRecord
    {
        public:

        plistColumnRecord()
        { 	add(col_nr);	add(col_icon);	add(col_artist);	add(col_title);
			add(col_time);	add(col_album);	add(col_id);		add(col_type);
			add(col_color);	add(col_file);	add(col_played);			
		}

		Gtk::TreeModelColumn<int> col_nr;							// 1 
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > col_icon; // 2 
		Gtk::TreeModelColumn<Glib::ustring> col_artist;				// 3 
		Gtk::TreeModelColumn<Glib::ustring> col_title;				// 4 
		Gtk::TreeModelColumn<Glib::ustring> col_time;				// 5 
		Gtk::TreeModelColumn<Glib::ustring> col_album;				// 6 
		Gtk::TreeModelColumn<int> col_id;							// hidden 
		Gtk::TreeModelColumn<int> col_type;							// hidden 
        Gtk::TreeModelColumn<Glib::ustring> col_color;				// hidden 
		Gtk::TreeModelColumn<Glib::ustring>	col_file;				// hidden 
        Gtk::TreeModelColumn<bool> col_played;						// hidden 
    };
	
    plistColumnRecord plistCols;
    Glib::RefPtr<Gtk::ListStore> plListStore;
    gm_treeview plTreeView;
	Gtk::TreeViewColumn column1, column2, column3,
						column4, column5, column6;
	Gtk::Label 	col1_label, /*col2_label,*/ col3_label,
				col4_label, col5_label, col6_label;
    Glib::RefPtr<Gtk::TreeSelection> theSelection;
};

#endif //  GM_PLAYLIST_H
