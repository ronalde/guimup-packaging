/*
 *  gm_tracks.h
 *  GUIMUP tracks (database & playlist) window
 *  (c) 2008 Johan Spee
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

#ifndef GM_TRACKS_H
#define GM_TRACKS_H

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

#include "gm_tracks_mpdcom.h"
#include "gm_songlist.h"
#include "gm_tracks_playlist.h"
#include "gm_tracks_datatree.h"
#include "gm_modeid.h"

enum
{
    // ID's for widgets
    ID_upd, 	// update button
    ID_src, 	// search button
    ID_sed, 	// search editable
    ID_shf, 	// shuffle button
    ID_prg, 	// puge button
    ID_clr, 	// clear button
    ID_sav, 	// save button
    ID_dbm, 	// database mode combo
    ID_srm  	// search mode combo
};


struct tr_settings
{
    int pane_pos;
    int x_pos;
    int y_pos;
    int w_width;
    int w_height;
	int db_mode;
	bool b_ttips;
};


class gm_Tracks : public Gtk::Window
{
public:
	gm_Tracks();
	virtual ~gm_Tracks();

	void set_trSettings(tr_settings);
	tr_settings get_trSettings();
	void connect_server(ustring, int, ustring);
	void disconnect_server();
	void set_listfont(ustring font);
	void set_plistPath(ustring);

protected:


private:

//	combo boxes
	class comboColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

	comboColumns()
	{ add(col_id); add(col_name); }

	Gtk::TreeModelColumn<int> col_id;
	Gtk::TreeModelColumn<Glib::ustring> col_name;
	};

	comboColumns cbColums;
	Glib::RefPtr<Gtk::ListStore> comboTreeModel;

//	widgets
    Gtk::HPaned hp_Paned;
        Gtk::Frame  fr_left_main;
        Gtk::VBox   vb_left;
            Gtk::HBox   hb_l_upper;
                Gtk::Button bt_update;
                Gtk::Label  lb_dbase;
                Gtk::ComboBox   cb_datamode;
            gm_Datatree theDatatree;
            Gtk::HBox   hb_l_lower;
                Gtk::ComboBox   cb_searchmode;
                Gtk::Entry  et_searchtext;
                Gtk::Button bt_search;
            Gtk::Frame  fr_left_stats;
                Gtk::Label lb_dbstats;
        Gtk::Frame  fr_right_main;
        Gtk::VBox   vb_right;
            Gtk::HBox   hb_r_upper;
                Gtk::Button bt_purge;
                Gtk::Label  lb_plist;
                Gtk::Button bt_clear;
            gm_Playlist   thePlaylist;
            Gtk::HBox   hb_r_lower;
                Gtk::Button bt_shuffle;
                Gtk::Alignment al_r_lower;
                Gtk::Button bt_save;
            Gtk::Frame  fr_right_stats;
                Gtk::Label lb_plstats;

//	variables
    tracks_mpdCom mpdCom;
    bool b_db_updating;
    int
        plistlength,
        pltotaltime,
        dataModeID,
        searchModeID;
			
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
    std::string into_string(int);
    ustring into_time(int);
};

#endif //  GM_TRACKS_H
