/*
 *  gm_library.cc
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

#include "gm_library.h"
#include "gm_library_pix.h"

gm_library::gm_library()
{
	// for the soup:
	g_thread_init (NULL);
	g_type_init ();
	
    init_widgets();
    init_signals();
    init_vars();
}

void gm_library::set_plistPath(ustring plpath)
{
	theDatatree.set_plistPath(plpath);
}

// called by player when connected
void gm_library::connect_server(ustring host, int port, ustring pwd)
{
	if (!mpdCom.mpd_connect(host, port, pwd))
		cout << "Could not connect \"library\"" << endl;
}

// called by player when disconnected
void gm_library::disconnect_server()
{
	mpdCom.mpd_disconnect();
}

// called by player when fontsize is set
void gm_library::set_listfont(ustring font)
{
	thePlaylist.set_listfont(font);
	theDatatree.set_listfont(font);
}


void gm_library::init_vars()
{
	stream_error = "";
	tr_config = NULL;
	b_db_updating = false;
	b_no_cb_feedback = false;
    plistlength = 0;
}


void gm_library::init_widgets()
{
	Glib::RefPtr<Gdk::Pixbuf> pxb;
	Gtk::TreeRow row;
	
    // window properties
    set_title("Guimup Library");
    set_border_width(4);
	
    std::list< Glib::RefPtr<Gdk::Pixbuf> > window_icons;
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon16, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon24, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon32, false);
    window_icons.push_back(pxb);
    set_icon_list(window_icons);

	// widget properties
	lb_dbstats.modify_font((Pango::FontDescription)"9");
	lb_dbstats.set_markup("Library statistics");
	lb_plstats.modify_font((Pango::FontDescription)"9");
	lb_plstats.set_markup("Playlist statistics");
	
	lb_dbase.set_markup("<b> Library </b>");
	lb_dbase.set_size_request(-1, 26);	
	lb_dbase.set_alignment(0.5, 0.5);
	
	lb_plist.set_markup("<b> Playlist </b>");
	lb_plist.set_size_request(-1, 26);	
	lb_plist.set_alignment(0.5, 0.5);
	
	fr_left_outer.set_label_widget(lb_dbase);
	fr_left_outer.set_label_align (Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	fr_right_outer.set_label_widget(lb_plist);
	fr_right_outer.set_label_align (Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	
	fr_left_main.set_shadow_type(Gtk::SHADOW_IN);
	vb_left.set_spacing(0);

	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_update, false);
	img_bt_update.set(pxb);
	bt_update.set_image(img_bt_update);
	bt_update.set_size_request(110,-1);
	bt_update.set_label(" Update");
	bt_update.set_tooltip_text("Update the library");
	
	////// start setup datamode combo //////
		cb_datamode.set_tooltip_text("Library mode");
		cb_datamode.set_size_request(110,-1);	
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_datamode.set_model(comboTreeModel);
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_artist;
		row[cbColums.col_name] = " Artists ";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_artist, false);
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_album;
		row[cbColums.col_name] = " Albums ";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_album, false);
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_plist;
		row[cbColums.col_name] = " Playlists ";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_playlist, false);
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_folder;
		row[cbColums.col_name] = " Folders ";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_folder, false);
		row[cbColums.col_icon] = pxb;

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_search;
		row[cbColums.col_name] = " Search ";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_search, false);
		row[cbColums.col_icon] = pxb;

		cb_datamode.pack_start(cbColums.col_icon, false);
		cb_datamode.pack_start(cbColums.col_name, true);
		cb_datamode.set_active(0);
		cb_datamode.set_focus_on_click(false);
	
		dataModeID = ID_artist;
	////// end setup datamode combo //////

	////// start setup search combo //////
	cb_searchmode.set_tooltip_text("Search mode");
		cb_searchmode.set_size_request(110,-1);
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_searchmode.set_model(comboTreeModel);

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Artist ";
		row[cbColums.col_id] = ID_artist;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Album ";
		row[cbColums.col_id] = ID_album;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Title ";
		row[cbColums.col_id] = ID_title;

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Genre ";
		row[cbColums.col_id] = ID_genre;

		cb_searchmode.pack_start(cbColums.col_name, true);
		cb_searchmode.set_active(0);
		cb_searchmode.set_focus_on_click(false);
		searchModeID = ID_artist;
	////// end setup search combo //////
	
	////// start setup select combo ////
	cb_selectmode.set_tooltip_text("Select mode");
		cb_selectmode.set_size_request(110,-1);
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_selectmode.set_model(comboTreeModel);

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Artist";
		row[cbColums.col_id] = ID_artist;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Album";
		row[cbColums.col_id] = ID_album;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Title";
		row[cbColums.col_id] = ID_title;

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " Genre";
		row[cbColums.col_id] = ID_genre;

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = " All columns";
		row[cbColums.col_id] = ID_all;

		cb_selectmode.pack_start(cbColums.col_name, true);
		cb_selectmode.set_active(4);
		cb_selectmode.set_focus_on_click(false);
		selectModeID = ID_all;	
	////// end setup select combo ////
	
		vb_right.set_spacing(0);	
		fr_right_main.set_shadow_type(Gtk::SHADOW_IN);
	
		et_searchtext.set_size_request(100,-1);
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_pointleft, false);
		img_bt_search.set(pxb);
		bt_search.set_image(img_bt_search);
		bt_search.set_label("Search");
		bt_search.set_size_request(110,-1);
		bt_search.set_tooltip_text("Start search");
	
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_strmbutn, false);
		img_bt_stream.set(pxb);
		bt_stream.set_image(img_bt_stream);
		bt_stream.set_label(" Add stream");
		bt_stream.set_size_request(110,-1);
		bt_stream.set_tooltip_text("Add url for streaming media");

		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_redcross, false);
		img_bt_clear.set(pxb);
		bt_clear.set_image(img_bt_clear);
		bt_clear.set_label(" Clear");
		bt_clear.set_size_request(110,-1);
		bt_clear.set_tooltip_text("Remove all items");
	
		et_selecttext.set_size_request(100,-1);
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_pointleft, false);
		img_bt_select.set(pxb);
		bt_select.set_image(img_bt_select);
		bt_select.set_label("Select");
		bt_select.set_size_request(110,-1);
		bt_select.set_tooltip_text("Start select");
	
    // put widgets together
    add(hp_Paned);
		hp_Paned.pack1(fr_left_outer);
		fr_left_outer.add(vb_left);	
	
			hb_l_lowerup.set_border_width(4);	
	        vb_left.pack_start(hb_l_lowerup, false, false, 0);
				hb_l_lowerup.pack_start(cb_datamode, false, false, 0);
				hb_l_lowerup.pack_end(bt_update, false, false, 0);
	
	        vb_left.pack_start(hb_datatree, true, true, 0);
				hb_datatree.pack_start(fr_left_main,true, true, 4);
				fr_left_main.add(theDatatree);

		    vb_left.pack_start(hb_left_stats, false, false, 0);
				hb_left_stats.pack_end(fr_left_stats, true, true, 4);
				fr_left_stats.add(lb_dbstats);
	
			hb_l_lower.set_border_width(4);
	        vb_left.pack_end(hb_l_lower, false, false, 0);
				hb_l_lower.pack_start(cb_searchmode, false, false, 0);
				hb_l_lower.pack_start(et_searchtext, true, true, 4);
				hb_l_lower.pack_end(bt_search, false, false, 0);

		hp_Paned.pack2(fr_right_outer, true, false);
		fr_right_outer.add(vb_right);
	
			hb_r_lowerup.set_border_width(4);
	        vb_right.pack_start(hb_r_lowerup, false, false, 0);
				hb_r_lowerup.pack_start(bt_stream, false, false, 4);
				hb_r_lowerup.pack_end(bt_clear, false, false, 0);

	        vb_right.pack_start(hb_playlist, true, true, 0);
				hb_playlist.pack_start(fr_right_main,true, true, 4);
				fr_right_main.add(thePlaylist);
	
			vb_right.pack_start(hb_right_stats, false, false, 0);
				hb_right_stats.pack_start(fr_right_stats, true, true, 4);
				fr_right_stats.add(lb_plstats);
	
			hb_r_lower.set_border_width(4);
	        vb_right.pack_end(hb_r_lower, false, false, 0);
				hb_r_lower.pack_start(cb_selectmode, false, false, 0);
				hb_r_lower.pack_start(et_selecttext, true, true, 4);
				hb_r_lower.pack_end(bt_select, false, false, 0);

	show_all_children();
}


void gm_library::init_signals()
{
	// the datatree
	theDatatree.signal_cmdList.connect(sigc::mem_fun(*this, &gm_library::on_dt_cmdlist));
	theDatatree.signal_mode.connect(sigc::mem_fun(*this, &gm_library::on_dtmenu_signal));	
    // the mpdCom
    mpdCom.signal_songList.connect(sigc::mem_fun(*this, &gm_library::on_new_playlist));
    mpdCom.signal_stats.connect(sigc::mem_fun(*this, &gm_library::on_new_stats));
    mpdCom.signal_update_ready.connect(sigc::mem_fun(*this, &gm_library::on_dbupdate_ready));
	mpdCom.signal_songchange.connect(sigc::mem_fun(*this, &gm_library::on_songchange));
	mpdCom.signal_connected.connect(sigc::mem_fun(*this, &gm_library::on_connectsignal));
	// widgets
    bt_update.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_upd));
	cb_datamode.signal_changed().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_lmd));
	bt_search.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_src));
	et_searchtext.signal_activate().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_sed));
    bt_clear.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_clr));
    bt_stream.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_str));
	bt_select.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_sel));
	et_selecttext.signal_activate().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_sld));
	thePlaylist.signal_plist_saved.connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_library::on_signal), ID_sav));
}


void gm_library::on_dtmenu_signal(int sigID)
{
	switch (sigID)
	{
		case ID_artist:
		{
			if (cb_datamode.get_active_row_number() != 0)
			{
				theDatatree.set_menu_del_item(false);
				cb_datamode.set_active(0);
			}
			break;
		}
		case ID_album:
		{
			if (cb_datamode.get_active_row_number() != 1)
			{
				theDatatree.set_menu_del_item(false);
				cb_datamode.set_active(1);
			}
			break;
		}
		case ID_plist:
		{
			if (cb_datamode.get_active_row_number() != 2)
			{
				theDatatree.set_menu_del_item(true);
				cb_datamode.set_active(2);
			}
			break;
		}
		case ID_folder:
		{
			if (cb_datamode.get_active_row_number() != 3)
			{
				theDatatree.set_menu_del_item(false);
				cb_datamode.set_active(3);
			}
			break;
		}
		case ID_search:
		{
			if (cb_datamode.get_active_row_number() != 4)
			{
				theDatatree.set_menu_del_item(false);
				cb_datamode.set_active(4);
			}
			break;
		}
		default:
		break;
	} // end switch (sigID)
}


void gm_library::on_dt_cmdlist(gm_commandList cmdlist)
{
/*  cmdlist is filled in the datatree (on_dtdrag_data_get)
	then signalled to here, where the list is passed on to 
	thePlaylist, where we add the 'droppos' and execute in
	on_pldata_received.  */
	thePlaylist.set_cmdlist(cmdlist);
}


// Signal handler.
void gm_library::on_signal(int sigID)
{
    switch (sigID)
    {
		case ID_upd: // update button
		{
			if (b_db_updating)
			break;
			bt_update.set_sensitive(false);
			b_db_updating = true;
			mpdCom.data_update();
			break;
		}
		
		case ID_sav:// playlist signals list was saved
		{
			if (dataModeID == ID_plist)
				theDatatree.get_playlists();
			break;
		}
		
		case ID_src: // search button
		case ID_sed: // 'enter' on search text entry
		{
		
			if ( cb_datamode.get_active_row_number() != 4)
			{
				b_no_cb_feedback = true;
				cb_datamode.set_active(4);
			}
			
			if (b_db_updating)
				break;
			
			ustring findThis = et_searchtext.get_text();
			
			Gtk::TreeModel::iterator iter = cb_searchmode.get_active();
			if(iter)
			{
				Gtk::TreeModel::Row row = *iter;
				if(row)
				  searchModeID = row[cbColums.col_id];
			}
			else
			{
				std::cout << "Search: invalid mode" << std::endl;
				break;
			}
			theDatatree.search_for(findThis, searchModeID);
			bt_search.set_sensitive(true);
			break;
		}
		
		case ID_sel: // select button
		case ID_sld: // 'enter' on select text entry
		{
			// search the playlist
			ustring findthis = et_selecttext.get_text();
			Gtk::TreeModel::iterator iter = cb_selectmode.get_active();
			if(iter)
			{
				Gtk::TreeModel::Row row = *iter;
				if(row)
				  selectModeID = row[cbColums.col_id];
			}
			else
			{
				std::cout << "Select: invalid mode" << std::endl;
				break;
			}
			
			thePlaylist.quick_select(findthis, selectModeID);
			break;
		}
		
		case ID_str: // add stream button
		{
			ustring input;
			bool check_ok = false;
			bool new_try = string_from_input("<b>Enter a direct URL to a m3u, pls or binary file </b>", input); // false upon 'cancel', true upon 'ok'
			if (new_try)
				check_ok = process_url(input);

			while (!check_ok && new_try)
			{
				new_try = string_from_input(stream_error, input);
				if (new_try)
					check_ok = process_url(input);
			}
			break;
		}
		
		case ID_clr: // clear button
		{
			thePlaylist.clear_list();
			break;
		}
		
		case ID_lmd: // dbase mode combo
		{
			if (b_no_cb_feedback)
			{
				b_no_cb_feedback = false;
				break;
			}
			
			Gtk::TreeModel::iterator iter = cb_datamode.get_active();
			if(iter)
			{
				Gtk::TreeModel::Row row = *iter;
				if(row)
				  dataModeID = row[cbColums.col_id];
			}
			
			switch (dataModeID)
			{
				case ID_artist:
				{
					theDatatree.set_menu_del_item(false);
					theDatatree.get_artists();
					break;
				}
				case ID_album:
				{
					theDatatree.set_menu_del_item(false);
					theDatatree.get_albums("*_*");
					break;
				}
				case ID_plist:
				{
					theDatatree.get_playlists();
					theDatatree.set_menu_del_item(true);
					break;
				}
				case ID_folder:
				{
					theDatatree.set_menu_del_item(false);
					theDatatree.get_folders("");
					break;
				}
				case ID_search:
				{
					theDatatree.set_menu_del_item(false);
					// fake search button clicked:
					on_signal(ID_src);
					break;
				}
				default:
					break;
			} // end switch (dataModeID)
			
			break;
		}
		
        default:
            break;
    }
}


void gm_library::set_config(gm_Config *cfg)
{
	tr_config = cfg;

}


void gm_library::set_trSettings()
{
	if (tr_config == NULL)
		return;
		
	move(tr_config->libraryWindow_Xpos, tr_config->libraryWindow_Ypos);
	resize(tr_config->libraryWindow_W, tr_config->libraryWindow_H);
	hp_Paned.set_position (tr_config->libraryWindow_panePos);
	cb_datamode.set_active(tr_config->library_DBmode);
	
	if (!tr_config->show_ToolTips)
	{
		cb_datamode.set_has_tooltip(false);
		cb_searchmode.set_has_tooltip(false);
		bt_search.set_has_tooltip(false);
		bt_stream.set_has_tooltip(false);
		bt_update.set_has_tooltip(false);
		bt_clear.set_has_tooltip(false);
	}
}


void gm_library::set_random(bool rand)
{
	thePlaylist.set_random(rand);
}
void gm_library::set_repeat(bool rept)
{
	thePlaylist.set_repeat(rept);
}
void gm_library::set_consume(bool cons)
{
	thePlaylist.set_consume(cons);
}
void gm_library::set_single(bool sing)
{
	thePlaylist.set_single(sing);
}


void gm_library::get_trSettings()
{
	if (tr_config == NULL)
		return;
	
	tr_config->libraryWindow_panePos = hp_Paned.get_position();
	get_size(tr_config->libraryWindow_W, tr_config->libraryWindow_H);
	tr_config->library_DBmode = cb_datamode.get_active_row_number();
	// don't remember 'search' mode
	if (tr_config->library_DBmode == 4)
			tr_config->library_DBmode = 0;
}


void gm_library::on_dbupdate_ready()
{
	bt_update.set_sensitive(true);
	b_db_updating = false;
	// reload the datatree
	on_signal(ID_lmd);
}


void gm_library::on_new_stats(int artists, int albums, int songs)
{
	if (artists == -1)
	{
		lb_dbstats.set_markup("No statistics available");
		return;
	}
	
	lb_dbstats.set_markup("Artists: " + into_string(artists) +
						   "   Albums: "  + into_string(albums)  +
						   "   Songs: "   + into_string(songs));
}


void gm_library::on_songchange(int songID, int status)
{
	// pass it on to the playlist
	thePlaylist.on_songchange(songID, status);
}


void gm_library::on_connectsignal(bool connected)
{
	if (connected)
	{
		thePlaylist.set_mpdcom( &mpdCom );
		theDatatree.set_mpdcom( &mpdCom );
	}
	else
	{
		// playlist is cleared via mpdCom
		thePlaylist.set_mpdcom( NULL );
		theDatatree.set_mpdcom( NULL );
	}
	// fill the datatree (or show "not connected")
	on_signal(ID_lmd);
}


void gm_library::on_new_playlist(gm_songList playlist)
{
	plistlength = 0;
	pltotaltime = 0;
	
  	std::list<songInfo>::iterator iter;
	for (iter = playlist.begin(); iter != playlist.end(); ++iter)
	{
		if (!(*iter).nosong)
		{
			plistlength++;
			pltotaltime += (*iter).time;
		}
	}

	lb_plstats.set_markup("Tracks: " + into_string(plistlength) +
						   "      Time: "   + into_time(pltotaltime));
	
	thePlaylist.set_playlist(playlist);
	playlist.clear();
}


ustring gm_library::into_time(int time)
{
    ustring formattedTime = "";
	int hrs, mins, secs;
	
	hrs = (int) (time / 3600);
	if (hrs > 0)
	{
		formattedTime += into_string(hrs) + ":";
		mins = (time % 3600)/60;
		if(mins < 10)
    		formattedTime += "0";
	}
	else
    	mins = (int)(time / 60);
	
    formattedTime += into_string(mins) + ":";
	
    secs = time % 60;
    if(secs < 10)
    formattedTime += "0";
    formattedTime += into_string(secs);
    return formattedTime;
}


std::string gm_library::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}


// window calls this when closed by clicking [x]
bool gm_library::on_delete_event(GdkEventAny* event)
{
	get_position(tr_config->libraryWindow_Xpos,tr_config->libraryWindow_Ypos);
	this->hide();
	signal_hide.emit();
	return true; // ignore event
}


bool  gm_library::string_from_input(ustring message, ustring &input)
{
	Gtk::Dialog dlg("Add stream", true, true);
	dlg.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dlg.add_button("OK", Gtk::RESPONSE_OK);
	dlg.set_default_response (Gtk::RESPONSE_CANCEL);
	dlg.set_has_separator(false);
	
	Gtk::VBox vbox(false, 20);
	Gtk::Entry entry;
	Gtk::Label label("", 0.0, 0.5);
	// label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	label.set_line_wrap(true);
	label.set_use_markup(true);
	label.set_markup(message);

	dlg.get_vbox()->pack_start(vbox);
	vbox.set_border_width(10);
	vbox.pack_start(label);
	vbox.pack_start(entry);
	vbox.show();
	label.show();
	entry.show();

	dlg.set_size_request(350, -1);

	if (dlg.run() == Gtk::RESPONSE_OK)
	{
		input = entry.get_text();
		return true;
	}
	else
		return false;
}



bool gm_library::process_url(ustring input)
{
	// url must make sense
	if ((input.substr(0,5)).lowercase() != "http:" || input.length() < 10)
	{
		stream_error = "<b>Not a valid URL.</b> Try again or cancel ";
		return false;
	}
	
	// remove '/' at the end, if present
	if (input.rfind("/") == input.length()-1)
		input = input.substr(0, input.length()-1);
	
	ustring xtension = (input.substr(input.rfind(".")+1, input.length()) ).lowercase();
	
	if ( xtension == "xspf" || xtension == "asx"	|| xtension == "asp"	|| xtension == "ram" 
		        			|| xtension == "shtml"  || xtension == "htm"	||  xtension == "html" 
		        			|| xtension == "xml"	|| xtension == "txt"	||  xtension == "txt" )
	{
		stream_error = "<b>Can't handle " + xtension + " files.</b> Try again or cancel ";
		return false;
	}
	
	// url points directly to an audio file
	if (xtension == "mp2" || xtension == "mp3" ||xtension == "mp4" || xtension == "ogg")
	{
		std::list<ustring> url_list;
		url_list.push_back(input);
		thePlaylist.set_stream(url_list);		
		cout << "Stream: assuming binary" << endl;
		return true;
	}
	
	SoupURI *proxy = NULL;
	bool synchronous = FALSE;
	int opt;

	base = input.data(); // the url
	base_uri = soup_uri_new (base);
	if (!base_uri)
	{
		stream_error = "<b>Could not parse the URL.</b> Try again or cancel ";
		return false;
	}
	
	// not a playlist: add as binary
	if (xtension != "m3u" && xtension != "pls")
	{
		std::list<ustring> url_list;
		url_list.push_back(input);
		thePlaylist.set_stream(url_list);		
		cout << "Stream: assuming binary" << endl;
		return true;		
	}

	fetched_urls = g_hash_table_new (g_str_hash, g_str_equal);

	if (synchronous)
	{
		session = soup_session_sync_new_with_options (
			SOUP_SESSION_SSL_CA_FILE, NULL,
			SOUP_SESSION_USER_AGENT, "get ",
			NULL);
	} 
	else 
	{
		session = soup_session_async_new_with_options (
			SOUP_SESSION_SSL_CA_FILE, NULL,
			SOUP_SESSION_USER_AGENT, "get ",
			NULL);
	}

	// if (!synchronous) //////////
		// loop

	bool succes = get_url (base);

	//if (!synchronous) ///////////
		// stop loop

	soup_uri_free (base_uri);
	
	return succes;
}


bool gm_library::get_url (const char *url)
{
	char *url_to_get, *slash, *name;
	SoupMessage *msg;
	int fd, i;
	SoupURI *uri;
	GPtrArray *hrefs;
	const char *header;

	if (strncmp (url, base, strlen (base)) != 0)
	{
		stream_error = "<b>Download error.</b> Try again or cancel ";
		return false;
	}
	
	if (strchr (url, '?') && strcmp (url, base) != 0)
	{
		stream_error = "<b>Download error.</b> Try again or cancel ";
		return false;
	}

	url_to_get = g_strdup (url);

	if (g_hash_table_lookup (fetched_urls, url_to_get))
	{
		stream_error = "<b>Download error.</b> Try again or cancel ";
		return false;
	}
	
	g_hash_table_insert (fetched_urls, url_to_get, url_to_get);

	msg = soup_message_new (SOUP_METHOD_GET, url_to_get);
	soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);

	soup_session_send_message (session, msg);

	name = soup_message_get_uri (msg)->path;
	if (strncmp (base_uri->path, name, strlen (base_uri->path)) != 0)
	{
		stream_error = "<b>Download error.</b> Try again or cancel ";
		return false;
	}

	name += strlen (base_uri->path);
	if (*name == '/')
		name++;

	if (SOUP_STATUS_IS_REDIRECTION (msg->status_code))
	{
		header = soup_message_headers_get_one (msg->response_headers, "Location");
		if (header)
		{
			return get_url (header);
		}
	}


	if (!SOUP_STATUS_IS_SUCCESSFUL (msg->status_code))
	{
		stream_error = "<b>Download failed.</b> Try again or cancel ";
		return false;
	}
	else
		cout << "Stream: retrieved " << url << " OK" << endl;

	const char *s_line = msg->response_body->data;
	if (!strncasecmp(s_line, "[playlist]", 10)) // pls file
		return parse_pls_file(msg->response_body->data, msg->response_body->length);
	else if (!strncasecmp(s_line, "#EXTM3U", 7)) // extended m3u file
		return parse_extm3u_file(msg->response_body->data, msg->response_body->length);
/*   else if (!strncasecmp(msg->response_body->data, "<?xml", 5)) // xspf file
       return parse_xspf_file(msg->response_body->data, msg->response_body->length); */
	else  // non-extended m3u file
		return parse_extm3u_file(msg->response_body->data, msg->response_body->length);
}


/* bool gm_library::parse_xspf_file(const char *data, int size)
{
	int i = 0;
	int count  = 0;
	std::list<ustring> url_list;
	gchar **tokens = g_regex_split_simple("\n", data, G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_ANY);
	if (tokens)
	{
		for (i = 0; tokens[i]; i++)
		{
			gchar *track = g_strstr_len(tokens[i], strlen(tokens[i]),"<track>");
			if (track != NULL)
			{
				gchar *url = g_strstr_len(tokens[i], strlen(tokens[i]),"http:");
				if (url != NULL)
				{
					ustring urlu = url;
					url_list.push_back(urlu.substr(0, urlu.rfind("</location>")));
					count ++;
				}
			}
		}
		g_strfreev(tokens);
	}
	
	if (count == 0)
	{
		stream_error = "<b>No stream found in xspf file.</b> Try again or cancel ";
		return false;
	}
	else
	{
		thePlaylist.set_stream(url_list);
		return true;
	}
}
*/

bool gm_library::parse_pls_file(const char *data, int size)
{
	int i = 0;
	int count  = 0;
	std::list<ustring> url_list;
	
	gchar **tokens = g_regex_split_simple("(\r\n|\n|\r)", data, G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_ANY);
	if (tokens)
	{
		for (i = 0; tokens[i]; i++)
		{
			// url lines begin with 'File#='
			if (!strncmp(tokens[i], "File", 4)) 
			{
				int del = 0;
				// split the string at '=' 
				for (del = 3; tokens[i][del] != '\0' && tokens[i][del] != '='; del++) ;
				// if the url behind '=' starts with http:// add it
				if (tokens[i][del] == '=' && strncmp(&tokens[i][del + 1], "http://", 7) == 0)
				{
					ustring url = &(tokens[i][del+1]);
					// remove return at the end
					int pos = url.length();
					int i = url.rfind("\0");
					if (i < pos);
						pos = i;
					i =  url.rfind("\n");
					if (i < pos);
						pos = i;
					i = url.rfind("\r");
					if (i < pos);
						pos = i;				 
					url_list.push_back(url.substr(0, pos));
					count++;
				}
			}
		}
		g_strfreev(tokens);
	}
	if (count == 0)
	{
		stream_error = "<b>No stream found in pls file.</b> Try again or cancel ";
		return false;
	}
	else
	{
		thePlaylist.set_stream(url_list);
		return true;
	}
}

bool gm_library::parse_extm3u_file(const char *data, int size)
{
	int i = 0;
	int count  = 0;
	std::list<ustring> url_list;
	
	gchar **tokens = g_regex_split_simple("(\r\n|\n|\r)", data, G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_ANY);
	if (tokens)
	{
		for (i = 0; tokens[i]; i++)
		{
			// simply add any line strarting with "http"//"
			if (!strncmp(tokens[i], "http://", 7))
			{
				ustring url = tokens[i];
				// remove return at the end
				int pos = url.length();
				int i = url.rfind("\0");
				if (i < pos);
					pos = i;
				i =  url.rfind("\n");
				if (i < pos);
					pos = i;
				i = url.rfind("\r");
				if (i < pos);
					pos = i;				 
				url = url.substr(0, pos);
				url_list.push_back(url.substr(0, pos));
				count++;
			}
		}
		g_strfreev(tokens);
	}
	if (count == 0)
	{
		stream_error = "<b>No stream found in m3u file.</b> Try again or cancel ";
		return false;
	}
	else
	{
		thePlaylist.set_stream(url_list);
		return true;
	}
}


gm_library::~gm_library()
{
}
 
