/*
 *  gm_library_playlist.h
 *  GUIMUP library playlist view
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

#ifndef GM_PLAYLIST_H
#define GM_PLAYLIST_H

#include <gtkmm/main.h>
#include <gtkmm/scrolledwindow.h>
#include <gm_treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/treerowreference.h>
#include <gtkmm/liststore.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <list>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>
    using Glib::ustring;
#include "gm_library_mpdcom.h"
#include "gm_songlist.h"
#include "gm_commandlist.h"
#include "gm_itemlist.h"
#include "gm_modeid.h"

// for convenience
typedef Gtk::TreeModel::Children Tr_Ch;

class gm_Playlist: public Gtk::ScrolledWindow
{
public:
    gm_Playlist();
    virtual ~gm_Playlist();
//  functions
    void set_playlist(gm_songList);
    void set_mpdcom(library_mpdCom*);
    void purge_list();
	void save_list();
	void clear_list();
	void shuffle_list();
    void on_songchange(int songID, int status);
	void set_cmdlist(gm_commandList &cmdList);
	void set_stream(std::list<ustring> url_list);
	void set_listfont(ustring font);
	void quick_select(ustring, int);
	void set_repeat(bool);
	void set_random(bool);
	void set_consume(bool);
	void set_single(bool);
	bool on_pldrag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
//signals
	sigc::signal<void> signal_plist_saved;			
protected:


private:
//  functions
    ustring into_time(int);
    std::string into_string(int);
    void on_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
    bool on_key_pressed(GdkEventKey* event);
    bool on_button_pressed(GdkEventButton*);
	void on_acckey_pressed(GdkEventKey* event);
    void del_selected();
	void renumber_list();
	void set_DnDmode();
    bool string_from_input(ustring message, ustring &input);
	void on_pldata_received( const Glib::RefPtr<Gdk::DragContext>&, int, int, const Gtk::SelectionData&, guint, guint);
	void on_pldrag_data_get( const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData&, guint, guint);
	void toggle_random();
	void toggle_repeat();
	void toggle_single();
	void toggle_consume();

//  variables
	bool 
		b_dragadded,
		b_plistbusy,
		b_streamplaying,
		b_random_set_ext,
		b_repeat_set_ext,
		b_single_set_ext,
		b_consume_set_ext,
		b_consume,
		b_single,
		b_random,
		b_repeat;

	std::list<Gtk::TargetEntry> DnDlist;
    int current_status,
		current_songNum,
		current_songID,
		plistLength,
		mouse_offset;
    Gtk::Menu rClickMenu;
	gm_commandList dtCommandList;
    library_mpdCom *pl_mpdCom;
    Glib::RefPtr<Gdk::Pixbuf>
        pxb_tr_track,
        pxb_tr_trplayed,
        pxb_tr_wait,
        pxb_tr_play,
        pxb_tr_stop,
        pxb_tr_pause,
		pxb_tr_playlist;
	Gtk::Image 
		img_men_save,
		img_men_clear,
		img_men_delete,
		img_men_purge,
		img_men_shuffle;


//	widgets
    class plModelColumns : public Gtk::TreeModelColumnRecord
    {
        public:

        plModelColumns()
        { 	add(col_nr); 	 add(col_id); 	 add(col_icon); 
		 	add(col_artist); add(col_title); add(col_time);
			add(col_album);  add(col_genre); add(col_color);
			add(col_file); 	add(col_played); add(col_stream);	}

		Gtk::TreeModelColumn<int> col_nr;							/* 1 */
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > col_icon; /* 2 */
		Gtk::TreeModelColumn<Glib::ustring> col_artist;				/* 3 */
		Gtk::TreeModelColumn<Glib::ustring> col_title;				/* 4 */
		Gtk::TreeModelColumn<Glib::ustring> col_time;				/* 5 */ 
		Gtk::TreeModelColumn<Glib::ustring> col_album;				/* 6 */
		Gtk::TreeModelColumn<Glib::ustring> col_genre;				/* 7 */ 
		Gtk::TreeModelColumn<int> col_id;							/* hidden */
        Gtk::TreeModelColumn<Glib::ustring> col_color;				/* hidden */
		Gtk::TreeModelColumn<Glib::ustring>	col_file;				/* hidden */
        Gtk::TreeModelColumn<bool> col_played;						/* hidden */
		Gtk::TreeModelColumn<bool> col_stream;						/* hidden */
    };
    plModelColumns plColumns;
    Glib::RefPtr<Gtk::ListStore> plListStore;
    gm_treeview plTreeView;
	Gtk::TreeViewColumn column1, column2, column3,
						column4, column5, column6, column7;
	Gtk::Label 	col1_label, col2_label, col3_label,
				col4_label, col5_label, col6_label, col7_label;
	Gtk::HBox 	col1_box, col2_box, col3_box,
				col4_box, col5_box, col6_box, col7_box;
    Glib::RefPtr<Gtk::TreeSelection> theSelection;
};

#endif //  GM_PLAYLIST_H
