/*
 *  gm_tracks.cc
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

#include "gm_tracks.h"
#include "gm_tracks_pix.h"

gm_Tracks::gm_Tracks()
{
    init_widgets();
    init_signals();
    init_vars();
}

// called by player when connected
void gm_Tracks::connect_server(ustring host, int port, ustring pwd)
{
	if (!mpdCom.mpd_connect(host, port, pwd))
		cout << "Could not connect \"Tracks\"" << endl;
}

// called by player when disconnected
void gm_Tracks::disconnect_server()
{
	mpdCom.mpd_disconnect();
}

// called by player when fontsize is set
void gm_Tracks::set_listfont(ustring font)
{
	thePlaylist.set_listfont(font);
	theDatatree.set_listfont(font);
}


void gm_Tracks::init_vars()
{
	b_db_updating = false;
    plistlength = 0;
}


void gm_Tracks::init_widgets()
{
	Glib::RefPtr<Gdk::Pixbuf> pxb_pointer;
	Gtk::TreeRow row;
    const static Gdk::Color gmdark("#1E3C69");
	
    // window properties
    set_title("Guimup Library");
    set_border_width(4);
	pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_playlist, false);
	set_icon(pxb_pointer);

	// widget properties
	lb_dbstats.modify_font((Pango::FontDescription)"11");
	lb_dbstats.set_markup("Datdabase statistics");
	lb_plstats.modify_font((Pango::FontDescription)"11");
	lb_plstats.set_markup("Playlist statistics");

	lb_dbstats.modify_fg(Gtk::STATE_NORMAL, gmdark);
	lb_plstats.modify_fg(Gtk::STATE_NORMAL, gmdark);
	lb_dbase.modify_fg(Gtk::STATE_NORMAL, gmdark);
	lb_plist.modify_fg(Gtk::STATE_NORMAL, gmdark);
	
	vb_left.set_spacing(4);
	hb_l_upper.set_spacing(4);
		bt_update.set_size_request(80,-1);
		bt_update.set_label("Update");
		bt_update.set_tooltip_text("Update the database");
		lb_dbase.set_markup("<b>Database</b>");
		lb_dbase.set_alignment(0.5,0.5);
	////// start setup datamode combo //////
		cb_datamode.set_tooltip_text("Database mode");
		cb_datamode.set_size_request(90,-1);	
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_datamode.set_model(comboTreeModel);
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_artist;
		row[cbColums.col_name] = "Artists";

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_album;
		row[cbColums.col_name] = "Albums";

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_plist;
		row[cbColums.col_name] = "Playlists";
	
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_folder;
		row[cbColums.col_name] = "Folders";

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_search;
		row[cbColums.col_name] = "Search";

		cb_datamode.pack_start(cbColums.col_name);
		cb_datamode.set_active(0);
		cb_datamode.set_focus_on_click(false);
		dataModeID = ID_artist;
	////// end setup datamode combo //////
	fr_left_main.set_shadow_type(Gtk::SHADOW_IN);
	hb_l_lower.set_spacing(4);
	////// start setup search combo //////
		cb_searchmode.set_tooltip_text("Search mode");
		cb_searchmode.set_size_request(90,-1);
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_searchmode.set_model(comboTreeModel);

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_artist;
		row[cbColums.col_name] = "Artist";

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_album;
		row[cbColums.col_name] = "Album";

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_title;
		row[cbColums.col_name] = "Title";

		row = *(comboTreeModel->append());
		row[cbColums.col_id] = ID_genre;
		row[cbColums.col_name] = "Genre";

		cb_searchmode.pack_start(cbColums.col_name);
		cb_searchmode.set_active(0);
		cb_searchmode.set_focus_on_click(false);
		searchModeID = ID_artist;
	////// end setup search combo //////
		et_searchtext.set_size_request(80,-1);
		bt_search.set_size_request(80,-1);
		bt_search.set_label("Search");
		bt_search.set_tooltip_text("Start search");
	
	vb_right.set_spacing(4);
	hb_r_upper.set_spacing(4);
		bt_purge.set_size_request(80,-1);
		bt_purge.set_label("Purge");
		bt_purge.set_tooltip_text("Remove played items");
		lb_plist.set_markup("<b>Playlist</b>");
		lb_plist.set_alignment(0.5,0.5);
		bt_clear.set_size_request(80,-1);
		bt_clear.set_label("Clear");
		bt_clear.set_tooltip_text("Remove all items");
	fr_right_main.set_shadow_type(Gtk::SHADOW_IN);
	hb_r_lower.set_spacing(4);
		bt_shuffle.set_size_request(80,-1);
		bt_shuffle.set_label("Shuffle");
		bt_shuffle.set_tooltip_text("Randomize the playlist");
		al_r_lower.set_size_request(120,-1);
		bt_save.set_size_request(80,-1);
		bt_save.set_label("Save");
		bt_save.set_tooltip_text("Save this playlist");
	
    // put widgets together
    add(hp_Paned);
		hp_Paned.pack1(vb_left);
	        vb_left.pack_start(hb_l_upper, false, false, 0);
				hb_l_upper.pack_start(cb_datamode, false, false, 0);
				hb_l_upper.pack_start(lb_dbase, true, true, 0);
				hb_l_upper.pack_end(bt_update, false, false, 0);
	        vb_left.pack_start(fr_left_main, true, true, 0);// LIST
				fr_left_main.add(theDatatree);
	        vb_left.pack_start(hb_l_lower, false, false, 0);
				hb_l_lower.pack_start(cb_searchmode, false, false, 0);
				hb_l_lower.pack_start(et_searchtext, true, true, 0);
				hb_l_lower.pack_end(bt_search, false, false, 0);
		    vb_left.pack_end(fr_left_stats, false, false, 0);
				fr_left_stats.add(lb_dbstats);
	
		hp_Paned.pack2(vb_right, true, false);
	        vb_right.pack_start(hb_r_upper, false, false, 0);
				hb_r_upper.pack_start(bt_purge, false, false, 0);
				hb_r_upper.pack_start(lb_plist, true, true, 0);
				hb_r_upper.pack_end(bt_clear, false, false, 0);
	        vb_right.pack_start(fr_right_main, true, true, 0);// LIST
				fr_right_main.add(thePlaylist);
	        vb_right.pack_start(hb_r_lower, false, false, 0);
				hb_r_lower.pack_start(bt_shuffle, false, false, 0);
				hb_r_lower.pack_start(al_r_lower, true, true, 0);
				hb_r_lower.pack_end(bt_save, false, false, 0);
		    vb_right.pack_end(fr_right_stats, false, false, 0);
				fr_right_stats.add(lb_plstats);

	show_all_children();
}


void gm_Tracks::init_signals()
{
	// the datatree
	theDatatree.signal_cmdList.connect(sigc::mem_fun(*this, &gm_Tracks::on_dt_cmdlist));
    // the mpdCom
    mpdCom.signal_songList.connect(sigc::mem_fun(*this, &gm_Tracks::on_new_playlist));
    mpdCom.signal_stats.connect(sigc::mem_fun(*this, &gm_Tracks::on_new_stats));
    mpdCom.signal_update_ready.connect(sigc::mem_fun(*this, &gm_Tracks::on_dbupdate_ready));
	mpdCom.signal_songchange.connect(sigc::mem_fun(*this, &gm_Tracks::on_songchange));
	mpdCom.signal_connected.connect(sigc::mem_fun(*this, &gm_Tracks::on_connectsignal));
	
	// widgets
    bt_update.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_upd));
	cb_datamode.signal_changed().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_dbm));
	bt_search.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_src));
	et_searchtext.signal_activate().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_sed));
    bt_clear.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_clr));
    bt_purge.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_prg));
    bt_save.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_sav));
    bt_shuffle.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Tracks::on_signal), ID_shf));
}


void gm_Tracks::on_dt_cmdlist(gm_commandList cmdlist)
{
/*  cmdlist is filled in the datatree (on_dtdrag_data_get)
	then signalled to here, where the list is passed on to 
	thePlaylist, where we add the 'droppos' and execute in
	on_pldata_received.  */
	thePlaylist.set_cmdlist(cmdlist);
}


// Signal handler.
void gm_Tracks::on_signal(int sigID)
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
		
		case ID_src: // search button
		case ID_sed: // 'enter' on search text entry
		{
			if (b_db_updating)
			{
				cb_datamode.set_active(4);
				break;
			}
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
			
			cb_datamode.set_active(4);
			theDatatree.search_for(findThis, searchModeID);
			
			break;
		}

		case ID_shf: // shuffle button
		{
			thePlaylist.shuffle_list();
			break;
		}
		
		case ID_prg: // purge button
		{
			thePlaylist.purge_list();
			break;
		}
		
		case ID_clr: // clear button
		{
			thePlaylist.clear_list();
			break;
		}
		
		case ID_sav: // save button
		{
			thePlaylist.save_list();
			break;
		}
		
		case ID_dbm: // dbase mode combo
		{
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
					theDatatree.get_artists();
					break;
				}
				case ID_album:
				{
					theDatatree.get_albums("");
					break;
				}
				case ID_plist:
				{
					theDatatree.get_playlists();
					break;
				}
				case ID_folder:
				{
					theDatatree.get_folders("");
					break;
				}
				case ID_search:
				{
					if (b_db_updating)
					break;
					// search button clicked
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


void gm_Tracks::set_trSettings(tr_settings sets)
{
	move(sets.x_pos, sets.y_pos);
	resize(sets.w_width, sets.w_height);
	hp_Paned.set_position (sets.pane_pos);
	cb_datamode.set_active(sets.db_mode);
	if (!sets.b_ttips)
	{
		cb_datamode.set_has_tooltip(false);
		cb_searchmode.set_has_tooltip(false);
		bt_search.set_has_tooltip(false);
		bt_save.set_has_tooltip(false);
		bt_shuffle.set_has_tooltip(false);
		bt_purge.set_has_tooltip(false);
		bt_update.set_has_tooltip(false);
		bt_clear.set_has_tooltip(false);
	}
}


tr_settings gm_Tracks::get_trSettings()
{
	tr_settings sets;
	
	sets.pane_pos = hp_Paned.get_position();
	
	int x, y;
	
    get_position(x, y);
	sets.x_pos = x;
	sets.y_pos = y;
	
	get_size(x, y);
	sets.w_width = x;
	sets.w_height = y;
	
	sets.db_mode = cb_datamode.get_active_row_number();
	 // don't store 'search' mode
	if (sets.db_mode == 4)
		sets.db_mode = 0;
	
	return sets;
}


void gm_Tracks::on_dbupdate_ready()
{
	bt_update.set_sensitive(true);
	b_db_updating = false;
	// reload the datatree
	on_signal(ID_dbm);
}


void gm_Tracks::on_new_stats(int artists, int albums, int songs)
{
	if (artists == -1)
	{
		lb_dbstats.set_markup("No statistics available");
		return;
	}
	
	lb_dbstats.set_markup("Artists: " + into_string(artists) +
						   "   Albums: "  + into_string(albums)  +
						   "   Songs: "   + into_string(songs) );
}


void gm_Tracks::on_songchange(int songID, int status)
{
	// pass it on to the playlist
	thePlaylist.on_songchange(songID, status);
}


void gm_Tracks::on_connectsignal(bool connected)
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
	on_signal(ID_dbm);
}


void gm_Tracks::on_new_playlist(gm_songList playlist)
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
						   "      Time: "   + into_time(pltotaltime) );
	
	thePlaylist.set_playlist(playlist);
	playlist.clear();
}


bool gm_Tracks::on_delete_event(GdkEventAny* event)
{
   this->hide();
   return true;
}


ustring gm_Tracks::into_time(int time)
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

std::string gm_Tracks::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}


gm_Tracks::~gm_Tracks()
{

}

