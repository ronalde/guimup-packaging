/*
 *  gm_browser.h
 *  GUIMUP browser (library & playlist) window
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

#ifndef GM_BROWSER_H
#define GM_BROWSER_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/paned.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/entry.h>
#include <gtkmm/alignment.h>
#include <gtkmm/treeview.h>
#include <gtkmm/checkmenuitem.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <stdlib.h>
#include <iostream>
    using std::cout;
    using std::endl;
#include <libsoup/soup.h>
#include "gm_songinfo.h"
#include "gm_playlistview.h"
#include "gm_libraryview.h"
#include "gm_modeid.h"
#include "gm_config.h"
#include "gm_mpdcom.h"

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
    ID_lmd, 	// library mode combo
    ID_upd  	// update 
};


class gm_browser : public Gtk::Window
{
public:
	gm_browser();
	virtual ~gm_browser();

	void set_config(gm_Config *);
	void get_configs();
	void set_tooltips();
	void set_mpdCom(gm_mpdCom *);
	void set_songInfo(gm_songInfo *);
	void set_listfont(Pango::FontDescription);
	void set_lib_fixed();
	void set_plist_fixed();
	void set_mark_played();
	void set_connected(bool);
	
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

	Gtk::Menu *plistMenu;
	
//	widgets
    Gtk::Paned hp_Paned;
        Gtk::Frame fr_left_outer;
      	Gtk::Label lb_lib;
		Gtk::Grid gr_left;
        	Gtk::Frame fr_left_main;
			gm_libraryView *theLibrary; 
			Gtk::Frame fr_left_stats;
				Gtk::Label lb_dbstats;
			Gtk::Button bt_update;
			Gtk::Label lb_spacer_left;
			Gtk::ComboBox cb_datamode;			
			Gtk::ComboBox cb_searchmode;
			Gtk::Entry et_searchtext;
			Gtk::Button bt_search;

        Gtk::Frame fr_right_outer;
        Gtk::Label lb_plist;
        Gtk::Frame fr_right_main;
        Gtk::Grid gr_right;
        	gm_Playlist *thePlaylist; 
        	Gtk::Frame fr_right_stats;
        		Gtk::Label lb_plstats;
        	Gtk::Button bt_stream;
			Gtk::Label lb_spacer_right;
        	Gtk::Button bt_options;
        	Gtk::ComboBox cb_selectmode;
			Gtk::Entry et_selecttext;
			Gtk::Button bt_select;

//	variables
    gm_mpdCom *mpdCom;
	gm_Config *config;

    bool 
		b_connected,
		b_use_trayicon,
		b_no_cb_feedback,
		b_random,
		b_repeat,
		b_single,
		b_consume,
		b_skip_status;
    int
        plistlength,
        pltotaltime;
    Gtk::Image
		img_bt_update,
		img_bt_search,
		img_bt_stream,
		img_bt_options,
		img_bt_select;
	Gtk::CheckMenuItem
		*cmi_random,
		*cmi_repeat,
		*cmi_single,
		*cmi_consume;
	
//	functions
    void init_widgets();
    void init_signals();
    void init_vars();
	void create_options_menu();
    void on_signal(int);
    void on_new_statistics(int, int, int);
    void on_dbupdate_ready();
	void on_rescan_started();
    bool on_delete_event(GdkEventAny*);
	bool on_options_clicked(GdkEventButton*);
	void on_new_playlist(gm_songList, int);	
	void on_newStatus( mpd_status *);
	void set_repeat();
	void set_random();
	void set_single();
	void set_consume();
    std::string into_string(int);
    ustring into_time(int);
	bool string_from_input(ustring, ustring &);
	
	// stream soup
	SoupSession *session;
	char *base;
	SoupURI *base_uri;
	GHashTable *fetched_urls;
	ustring stream_error;
	bool process_url(ustring);
	bool get_url (const char *);
	bool parse_pls_file(const char *, int);
	bool parse_extm3u_file(const char *, int);
	bool parse_xspf_file(const char *, int);

};

#endif //  GM_BROWSER_H
