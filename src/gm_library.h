/*
 *  gm_library.h
 *  GUIMUP library (database & playlist) window
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

#ifndef gm_library_H
#define gm_library_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/paned.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/entry.h>
#include <gtkmm/alignment.h>
#include <gtkmm/treeview.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <stdlib.h>
#include <iostream>
    using std::cout;
    using std::endl;
#include <libsoup/soup.h>
#include "gm_library_mpdcom.h"
#include "gm_songlist.h"
#include "gm_library_playlist.h"
#include "gm_library_datatree.h"
#include "gm_modeid.h"
#include "gm_config.h"

enum
{
    // ID's for widgets
    ID_src, 	// search
    ID_sed, 	// search edit
    ID_smd,  	// search mode
    ID_sel,		// quick select
    ID_sld,		// quick select edit
	ID_qmd,		// quick select mode
    ID_shf, 	// shuffle 
    ID_prg, 	// puge 
	ID_str,		// add stream
    ID_clr, 	// clear 
    ID_sav, 	// save 
    ID_lmd, 	// library mode 
    ID_upd  	// update 
};


class gm_library : public Gtk::Window
{
public:
	gm_library();
	virtual ~gm_library();

	void set_config(gm_Config *cfg);
	void set_trSettings();
	void get_trSettings();
	void connect_server(ustring, int, ustring);
	void disconnect_server();
	void set_listfont(ustring font);
	void set_plistPath(ustring);
	void set_repeat(bool);
	void set_random(bool);
	void set_single(bool);
	void set_consume(bool);

	sigc::signal<void> signal_hide;

protected:


private:

//	combo boxes
	class comboColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

	comboColumns()
	{ add(col_id);  add(col_name); add(col_icon);}
		Gtk::TreeModelColumn<int> col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > col_icon;
	};

	comboColumns cbColums;
	Glib::RefPtr<Gtk::ListStore> comboTreeModel;

//	widgets
    Gtk::HPaned hp_Paned;
        Gtk::Frame  fr_left_outer;
      		Gtk::Label  lb_dbase;
        Gtk::Frame  fr_left_main;
        Gtk::VBox   vb_left;
			Gtk::HBox hb_datatree;
            gm_Datatree theDatatree;
			Gtk::HBox hb_left_stats;
            Gtk::Frame  fr_left_stats;
                Gtk::Label lb_dbstats;
			Gtk::HBox   hb_l_lowerup;
                Gtk::Button bt_update;
                Gtk::ComboBox   cb_datamode;			
            Gtk::HBox   hb_l_lower;
                Gtk::ComboBox   cb_searchmode;
                Gtk::Entry  et_searchtext;
                Gtk::Button bt_search;

        Gtk::Frame  fr_right_outer;
            Gtk::Label  lb_plist;
        Gtk::Frame  fr_right_main;
        Gtk::VBox   vb_right;
			Gtk::HBox hb_right_stats;
			Gtk::HBox hb_playlist;
            gm_Playlist   thePlaylist;
            Gtk::Frame  fr_right_stats;
                Gtk::Label lb_plstats;
            Gtk::HBox   hb_r_lowerup;
                Gtk::Button bt_stream;
                Gtk::Button bt_clear;
            Gtk::HBox   hb_r_lower;
                Gtk::ComboBox   cb_selectmode;
				Gtk::Entry  et_selecttext;
				Gtk::Button bt_select;


//	variables
    library_mpdCom mpdCom;
	gm_Config *tr_config;
	// soup:
	SoupSession *session;
	const char *base;
	SoupURI *base_uri;
	GHashTable *fetched_urls;

    bool 
		b_db_updating,
		b_no_cb_feedback;
    int
        plistlength,
        pltotaltime,
        dataModeID,
        searchModeID,
		selectModeID;
    Gtk::Image
		img_bt_update,
		img_bt_search,
		img_bt_stream,
		img_bt_clear,
		img_bt_select;	
//	functions
    void init_widgets();
    void init_signals();
    void init_vars();
    void on_signal(int);
    void on_new_playlist(gm_songList);
    void on_new_stats(int, int, int);
    void on_songchange(int, int);			
    void on_dbupdate_ready();
	void on_connectsignal(bool);
	void on_dt_cmdlist(gm_commandList);
    bool on_delete_event(GdkEventAny*);
	void on_dtmenu_signal(int ID);
    std::string into_string(int);
    ustring into_time(int);
	bool string_from_input(ustring, ustring &);
	// Streams
	ustring stream_error;
	bool process_url(ustring);
	bool get_url (const char *url);
	bool parse_pls_file(const char *data, int size);
	bool parse_extm3u_file(const char *data, int size);
};

#endif //  gm_library_H
