/*
 *  gm_browser.cc
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

#include "gm_browser.h"
#include "gm_browser_pix.h"

gm_browser::gm_browser()
{
	// for the soup:
	g_thread_init (NULL);
	g_type_init ();
	
	thePlaylist = new gm_Playlist;
	theLibrary = new gm_libraryView;
    init_widgets();
    init_signals();
    init_vars();
}

// called by player
void gm_browser::set_lib_fixed()
{
	theLibrary->set_columns_fixed();
}

// called by player
void gm_browser::set_plist_fixed()
{
	thePlaylist->set_columns_fixed();
}

// called by player
void gm_browser::set_listfont(Pango::FontDescription pFont)
{
	thePlaylist->set_listfont(pFont);
	theLibrary->set_listfont(pFont);
}

// called by player
void gm_browser::set_mark_played()
{
	thePlaylist->set_menu_purge();
}


void gm_browser::init_vars()
{
	stream_error = "";
	config = NULL;
	mpdCom = NULL;
	b_no_cb_feedback = false;
	b_connected  = false;
    plistlength = 0;
	b_random = false;
	b_repeat = false;
	b_single = false;
	b_consume = false;
	b_skip_status = false;
}


void gm_browser::init_widgets()
{
	Glib::RefPtr<Gdk::Pixbuf> pxb;
	Gtk::TreeRow row;
	
    // window properties
    set_title("Guimup Library");
    set_border_width(4);

    std::vector< Glib::RefPtr<Gdk::Pixbuf> > window_icons;
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon16, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon24, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon32, false);
    window_icons.push_back(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon48, false);
    window_icons.push_back(pxb);
    set_icon_list(window_icons);

	lb_dbstats.set_markup("Library statistics");
	lb_plstats.set_markup("Playlist statistics");
	
	lb_lib.set_markup("<b> Library </b>");
	lb_lib.set_size_request(-1, 26);	
	lb_lib.set_alignment(0.5, 0);
	
	lb_plist.set_markup("<b> Playlist </b>");
	lb_plist.set_size_request(-1, 26);	
	lb_plist.set_alignment(0.5, 0);
	
	fr_left_outer.set_label_widget(lb_lib);
	fr_left_outer.set_label_align (Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	fr_right_outer.set_label_widget(lb_plist);
	fr_right_outer.set_label_align (Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	
	fr_left_main.set_shadow_type(Gtk::SHADOW_IN);
	gr_left.set_row_spacing(2);
	gr_left.set_row_homogeneous(false);
	gr_left.set_column_homogeneous(false);
	
	theLibrary->set_vexpand(true);
	theLibrary->set_hexpand(true);
	cb_datamode.set_hexpand(false);
	bt_update.set_hexpand(false);
	lb_spacer_left.set_hexpand(true);
	cb_searchmode.set_hexpand(false);
	et_searchtext.set_hexpand(true);
	bt_search.set_hexpand(false);
	
	// datamode combo ->
		cb_datamode.set_tooltip_text("Library mode");
		cb_datamode.set_size_request(100,-1);	
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_datamode.set_model(comboTreeModel);
		// 0
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_ARTIST;
		row[cbColums.col_name] = "Artists";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_artist, false);
		row[cbColums.col_icon] = pxb;
		// 1
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_ALBUM;
		row[cbColums.col_name] = "Albums";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_album, false);
		row[cbColums.col_icon] = pxb;
		// 2
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_BYDATE;
		row[cbColums.col_name] = "Timestamp";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_album_tstamp, false);
		row[cbColums.col_icon] = pxb;
		// 3
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_BYEAR;
		row[cbColums.col_name] = "Year";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_album_year, false);
		row[cbColums.col_icon] = pxb;
		// 4
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_GENRE;
		row[cbColums.col_name] = "Genres";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_genre, false);
		row[cbColums.col_icon] = pxb;
		// 5
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_PLIST;
		row[cbColums.col_name] = "Playlists";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_playlist, false);
		row[cbColums.col_icon] = pxb;
		// 6
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_FOLDER;
		row[cbColums.col_name] = "Folders";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_folder, false);
		row[cbColums.col_icon] = pxb;
		// 7
		row = *(comboTreeModel->append());
		row[cbColums.col_id] = LB_ID_SEARCH;
		row[cbColums.col_name] = "Searched";
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_search, false);
		row[cbColums.col_icon] = pxb;

		cb_datamode.pack_start(cbColums.col_icon, false);
		cb_datamode.pack_start(cbColums.col_name, true);
		cb_datamode.set_active(0);
		cb_datamode.set_focus_on_click(false);
	// <- datamode combo

	// search combo ->
		cb_searchmode.set_tooltip_text("Search mode");
		cb_searchmode.set_size_request(100,-1);
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_searchmode.set_model(comboTreeModel);

		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_dot_small, false);
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "Artist ";
		row[cbColums.col_id] = SR_ID_ARTIST;
		row[cbColums.col_icon] = pxb;

	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "Album ";
		row[cbColums.col_id] = SR_ID_ALBUM;
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "Title ";
		row[cbColums.col_id] = SR_ID_TITLE;
		row[cbColums.col_icon] = pxb;

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "Genre ";
		row[cbColums.col_id] = SR_ID_GENRE;
		row[cbColums.col_icon] = pxb;
	
		cb_searchmode.pack_start(cbColums.col_icon, false);
		cb_searchmode.pack_start(cbColums.col_name, true);
		cb_searchmode.set_active(0);
		cb_searchmode.set_focus_on_click(false);
	// <- search combo
	
	// select combo ->
		cb_selectmode.set_tooltip_text("Select mode");
		cb_selectmode.set_size_request(100,-1);
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_selectmode.set_model(comboTreeModel);

		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_dot_small, false);

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "Artist";
		row[cbColums.col_id] = SL_ID_ARTIST;
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "Album";
		row[cbColums.col_id] = SL_ID_ALBUM;
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "Title";
		row[cbColums.col_id] = SL_ID_TITLE;
		row[cbColums.col_icon] = pxb;

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "All Columns";
		row[cbColums.col_id] = SL_ID_ALL;
		row[cbColums.col_icon] = pxb;
	
		cb_selectmode.pack_start(cbColums.col_icon, false);
		cb_selectmode.pack_start(cbColums.col_name, true);
		cb_selectmode.set_active(4);
		cb_selectmode.set_focus_on_click(false);
	// <- select combo
	
		int button_height; int nix;
	 	cb_selectmode.get_preferred_height(button_height, nix); 
	
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_update, false);
		img_bt_update.set(pxb);
		bt_update.set_image(img_bt_update);
		bt_update.set_size_request(100, -1);
		bt_update.set_label(" Update");
		bt_update.set_tooltip_text("Update the library database");
	
		gr_right.set_row_spacing(2);
		gr_right.set_row_homogeneous(false);
		gr_right.set_column_homogeneous(false);
	
		thePlaylist->set_vexpand(true);
		thePlaylist->set_hexpand(true);
		cb_selectmode.set_hexpand(false);
		et_selecttext.set_hexpand(true);
		bt_select.set_hexpand(false);
		bt_stream.set_hexpand(false);
		lb_spacer_right.set_hexpand(true);
		bt_options.set_hexpand(false);

	
		fr_right_main.set_shadow_type(Gtk::SHADOW_IN);
	
		et_searchtext.set_size_request(60, -1);
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_pointleft, false);
		img_bt_search.set(pxb);
		bt_search.set_image(img_bt_search);
		bt_search.set_label("Search");
		bt_search.set_size_request(100, button_height);
		bt_search.set_tooltip_text("Start search");
	
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_strmbutn, false);
		img_bt_stream.set(pxb);
		bt_stream.set_image(img_bt_stream);
		bt_stream.set_label(" Stream");
		bt_stream.set_size_request(100, button_height);
		bt_stream.set_tooltip_text("Add streaming media");

		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_options, false);
		img_bt_options.set(pxb);
		bt_options.set_image(img_bt_options);
		bt_options.set_label(" Options");
		bt_options.set_size_request(100, button_height);
		bt_options.set_tooltip_text("Playback options");
	
		et_selecttext.set_size_request(60,-1);
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_pointleft, false);
		img_bt_select.set(pxb);
		bt_select.set_image(img_bt_select);
		bt_select.set_label("Select");
		bt_select.set_size_request(100, button_height);
		bt_select.set_tooltip_text("Make selection");
	
    // put widgets together
    	add(hp_Paned);
		hp_Paned.pack1(fr_left_outer, true, false);
		fr_left_outer.add(gr_left);	
	
		gr_left.attach(cb_datamode,0,0,1,1);
		gr_left.attach(lb_spacer_left,1,0,1,1);
		gr_left.attach(bt_update,2,0,1,1);
		gr_left.attach(fr_left_main,0,1,3,1);
			fr_left_main.add(*theLibrary);
		gr_left.attach(fr_left_stats,0,3,3,1);
			fr_left_stats.add(lb_dbstats);
		gr_left.attach(cb_searchmode,0,4,1,1);
		gr_left.attach(et_searchtext,1,4,1,1);
		gr_left.attach(bt_search,2,4,1,1);

		hp_Paned.pack2(fr_right_outer, true, false);
		fr_right_outer.add(gr_right);

		gr_right.attach(bt_stream,0,0,1,1);
		gr_right.attach(lb_spacer_right,1,0,1,1);
		gr_right.attach(bt_options,2,0,1,1);
		gr_right.attach(fr_right_main,0,1,3,1);
			fr_right_main.add(*thePlaylist);
		gr_right.attach(fr_right_stats,0,3,3,1);
			fr_right_stats.add(lb_plstats);
		gr_right.attach(cb_selectmode,0,4,1,1);
		gr_right.attach(et_selecttext,1,4,1,1);
		gr_right.attach(bt_select,2,4,1,1);

	create_options_menu();
	
	show_all_children();
}


void gm_browser::init_signals()
{
	thePlaylist->signal_drop_received.connect(sigc::mem_fun(*theLibrary, &gm_libraryView::on_plist_dropreceived));
    bt_update.signal_clicked().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_upd));
	cb_datamode.signal_changed().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_lmd));
	bt_search.signal_clicked().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_src));
	et_searchtext.signal_activate().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_sed));
    bt_stream.signal_clicked().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_str));
	bt_select.signal_clicked().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_sel));
	et_selecttext.signal_activate().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_sld));
	thePlaylist->signal_plist_saved.connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_browser::on_signal), ID_sav));
	bt_options.signal_button_release_event().connect(sigc::mem_fun(*this, &gm_browser::on_options_clicked),false);
}


bool gm_browser::on_options_clicked(GdkEventButton* event)
{
	plistMenu->popup(event->button, event->time);
	return true;  // stop here
}

void gm_browser::on_rescan_started()
{
	bt_update.set_sensitive(false);
}

// Signal handler.
void gm_browser::on_signal(int sigID)
{
	switch (sigID)
    {
		case ID_upd: // update button
		{
			if (!b_connected)
			break;
			bt_update.set_sensitive(false);
			mpdCom->update_all();
			break;
		}
		
		case ID_sav:// playlist signals list was saved
		{
			if (cb_datamode.get_active_row_number() == LB_ID_PLIST)
				theLibrary->get_playlists();
			break;
		}
		
		case ID_src: // search button
		case ID_sed: // 'enter' on search text entry
		{
		
			if ( cb_datamode.get_active_row_number() != LB_ID_SEARCH)
			{
				b_no_cb_feedback = true;
					cb_datamode.set_active(LB_ID_SEARCH);
				b_no_cb_feedback = false;
				config->browser_LibraryMode = LB_ID_SEARCH;
			}
			
			config->browser_SearchString = et_searchtext.get_text();
			
			Gtk::TreeModel::iterator iter = cb_searchmode.get_active();
			if(iter)
			{
				bt_search.set_sensitive(false);
				theLibrary->search_for(config->browser_SearchString, (*iter)[cbColums.col_id]);
				bt_search.set_sensitive(true);
			}
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
				thePlaylist->quick_select(findthis, row[cbColums.col_id]);
			}
			else
			{
				std::cout << "Select: invalid mode" << std::endl;
			}
			break;
		}
		
		case ID_str: // stream button
		{
			ustring input;
			bool check_ok = false;
			bool new_try = string_from_input("Enter a URL to a playlist or a binary stream:", input); // false upon 'cancel', true upon 'ok'
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
		
		case ID_lmd: // lib mode combo
		{
			if (b_no_cb_feedback)
				break;
			
			Gtk::TreeModel::iterator iter = cb_datamode.get_active();
			if(iter)
			{
				Gtk::TreeModel::Row row = *iter;
				if(row)
					config->browser_LibraryMode = row[cbColums.col_id];
			}
			
			// load
			switch (config->browser_LibraryMode)
			{
			
				case LB_ID_ARTIST:
				{
					theLibrary->get_artists("~0~");
					break;
				}
				case LB_ID_ALBUM:
				{
					theLibrary->get_albums("~0~", false);
					break;
				}
				case LB_ID_BYDATE:
				{
					theLibrary->get_albums("~0~", true);
					break;
				}
				case LB_ID_BYEAR:
				{
					theLibrary->get_albums_year();
					break;
				}	
				case LB_ID_PLIST:
				{
					theLibrary->get_playlists();
					break;
				}
				case LB_ID_GENRE:
				{
					theLibrary->get_genres("~0~");
					break;
				}
				case LB_ID_FOLDER:
				{
					theLibrary->get_folders("~0~");
					break;
				}
				case LB_ID_SEARCH:
				{
					// signal search button click:
					on_signal(ID_src);
					break;
				}
				default:
					break;
			} // <- switch (browser_LibraryMode)
		} // <- case ID_lmd:
        default:
            break;
	} // <- switch (sigID)
}


void gm_browser::get_configs()
{
	if (get_visible())
		get_position(config->browserWindow_Xpos,config->browserWindow_Ypos);
	
	config->browserWindow_panePos = hp_Paned.get_position();
	get_size(config->browserWindow_W, config->browserWindow_H);
	config->browser_LibraryMode = cb_datamode.get_active_row_number();
	config->browser_SearchMode = cb_searchmode.get_active_row_number();
	config->browser_SelectMode = cb_selectmode.get_active_row_number();

	thePlaylist->get_configs();
	theLibrary->get_configs();
}


void gm_browser::set_tooltips()
{
	cb_datamode.set_has_tooltip(config->show_ToolTips);
	cb_searchmode.set_has_tooltip(config->show_ToolTips);
	cb_selectmode.set_has_tooltip(config->show_ToolTips);
	bt_search.set_has_tooltip(config->show_ToolTips);
	bt_select.set_has_tooltip(config->show_ToolTips);
	bt_stream.set_has_tooltip(config->show_ToolTips);
	bt_update.set_has_tooltip(config->show_ToolTips);
	bt_options.set_has_tooltip(config->show_ToolTips);	
}


void gm_browser::set_config(gm_Config *cfg)
{
	config = cfg;
	
	if (config == NULL)
		return;
	
	thePlaylist->set_config(cfg);
	theLibrary->set_config(cfg);
		
	b_use_trayicon = config->use_TrayIcon;
	move(config->browserWindow_Xpos, config->browserWindow_Ypos);
	resize(config->browserWindow_W, config->browserWindow_H);
	hp_Paned.set_position (config->browserWindow_panePos);
	b_no_cb_feedback = true;
		cb_datamode.set_active(config->browser_LibraryMode);
	b_no_cb_feedback = false;
	cb_searchmode.set_active(config->browser_SearchMode);
	et_searchtext.set_text(config->browser_SearchString);
	cb_selectmode.set_active(config->browser_SelectMode);
	
	set_tooltips();
}


void gm_browser::set_mpdCom(gm_mpdCom *com)
{
	mpdCom = com;

	thePlaylist->set_mpdcom(com);
	theLibrary->set_mpdcom(com);

	if (com != NULL)
	{
		mpdCom->sgnl_plistupdate.connect(sigc::mem_fun(*this, &gm_browser::on_new_playlist));
		mpdCom->sgnl_dbase_statistics.connect(sigc::mem_fun(*this, &gm_browser::on_new_statistics));
		mpdCom->sgnl_update_ready.connect(sigc::mem_fun(*this, &gm_browser::on_dbupdate_ready));
		mpdCom->sgnl_connected.connect(sigc::mem_fun(*this, &gm_browser::set_connected));
		mpdCom->sgnl_statusInfo.connect(sigc::mem_fun(*this, &gm_browser::on_newStatus));
		mpdCom->sgnl_rescan_started.connect(sigc::mem_fun(*this, &gm_browser::on_rescan_started));
	}
}


void gm_browser::set_random()
{
	if (!b_connected || cmi_random->get_active() == b_random || !config->mpd_random_allowed)
		return;
	
	if (cmi_random->get_active())
	{
		b_random = true;
		mpdCom->set_random(true);
	}
	else
	{
		b_random = false;
		mpdCom->set_random(false);
	}
}


void gm_browser::set_repeat()
{
	if (!b_connected || cmi_repeat->get_active() == b_repeat || !config->mpd_repeat_allowed)
		return;
	
	if (cmi_repeat->get_active())
	{
		b_repeat = true;
		mpdCom->set_repeat(true);
	}
	else
	{
		b_repeat = false;
		mpdCom->set_repeat(false);
	}
}


void gm_browser::set_consume()
{
	// block feedback
	if (!b_connected || cmi_consume->get_active() == b_consume || !config->mpd_consume_allowed)
		return;
	
	if (cmi_consume->get_active())
	{
		b_consume = true;
		mpdCom->set_consume(true);
		thePlaylist->set_consume(true);
	}
	else
	{
		b_consume = false;
		mpdCom->set_consume(false);
		thePlaylist->set_consume(false);
	}
}


void gm_browser::set_single()
{
	if (!b_connected || cmi_single->get_active() == b_single || !config->mpd_single_allowed)
		return;
	
	if (cmi_single->get_active())
	{
		b_single = true;
		mpdCom->set_single(true);
	}
	else
	{
		b_single = false;
		mpdCom->set_single(false);
	}
}


void gm_browser::on_dbupdate_ready()
{
	if (config->mpd_update_allowed)
			bt_update.set_sensitive(true);
	else
		bt_update.set_sensitive(false);
	
	// reload the theLibrary
	switch (config->browser_LibraryMode)
	{
		case LB_ID_ARTIST:
		{
			theLibrary->get_artists("~0~");
			break;
		}
		case LB_ID_ALBUM:
		{
			theLibrary->get_albums("~0~", false);
			break;
		}
		case LB_ID_BYDATE:
		{
			theLibrary->get_albums("~0~", true);
			break;
		}
		case LB_ID_BYEAR:
		{
			theLibrary->get_albums_year();
			break;
		}	
		case LB_ID_PLIST:
		{
			theLibrary->get_playlists();

			break;
		}
		case LB_ID_GENRE:
		{
			theLibrary->get_genres("~0~");
			break;
		}
		case LB_ID_FOLDER:
		{
			theLibrary->get_folders("~0~");
			break;
		}
		case LB_ID_SEARCH:
		{
			// signal search button click:
			on_signal(ID_src);
			break;
		}
		default:
			break;
	} // <- switch
}


void gm_browser::on_new_statistics(int artists, int albums, int songs)
{
	if (artists == -1)
	{
		lb_dbstats.set_markup("No statistics available");
		return;
	}
	
	lb_dbstats.set_markup("<i>Artists</i> " + into_string(artists) +
						   "  -  <i>Albums</i> "  + into_string(albums)  +
						   "  -  <i>Songs</i> "   + into_string(songs));
}


void gm_browser::set_connected(bool bconn)
{
	b_connected  = bconn;
	if (b_connected)
	{
		thePlaylist->set_connected(true);
		theLibrary->set_connected(true);
	
		mpdCom->get_statistics();

		if (config->mpd_update_allowed)
			bt_update.set_sensitive(true);
		else
			bt_update.set_sensitive(false);

		if (config->mpd_repeat_allowed)
		{
			if (config->mpd_single_allowed )
				cmi_repeat->set_label("Repeat Single/List");
			else
				cmi_repeat->set_label("Repeat");
			cmi_repeat->set_sensitive(true);
		}
		else
			cmi_repeat->set_sensitive(false);

		if (config->mpd_random_allowed)
			cmi_random->set_sensitive(true);
		else
			cmi_random->set_sensitive(false);

		if (config->mpd_consume_allowed)
			cmi_consume->set_sensitive(true);
		else
			cmi_consume->set_sensitive(false);

		if (config->mpd_single_allowed)
			cmi_single->set_sensitive(true);
		else
			cmi_single->set_sensitive(false);
	}
	else
	{
		bt_update.set_sensitive(false);
		cmi_random->set_sensitive(false);
		cmi_repeat->set_sensitive(false);
		cmi_consume->set_sensitive(false);
		cmi_single->set_sensitive(false);
		thePlaylist->set_connected(false);
		theLibrary->set_connected(false);
		on_new_statistics(-1, 0, 0);
	}
	
	// fill the library (or show "not connected")
	switch (config->browser_LibraryMode)
	{
		case LB_ID_ARTIST:
		{
			theLibrary->get_artists("~0~");
			break;
		}
		case LB_ID_ALBUM:
		{
			theLibrary->get_albums("~0~", false);
			break;
		}
		case LB_ID_BYDATE:
		{
			theLibrary->get_albums("~0~", true);
			break;
		}
		case LB_ID_BYEAR:
		{
			theLibrary->get_albums_year();
			break;
		}	
		case LB_ID_PLIST:
		{
			theLibrary->get_playlists();
			break;
		}
		case LB_ID_GENRE:
		{
			theLibrary->get_genres("~0~");
			break;
		}
		case LB_ID_FOLDER:
		{
			theLibrary->get_folders("~0~");
			break;
		}
		case LB_ID_SEARCH:
		{
			// signal search button click:
			on_signal(ID_src);
		}
		default:
			break;
	} // <- switch
}


void gm_browser::on_new_playlist(gm_songList playlist, int ID)
{
	plistlength = 0;
	pltotaltime = 0;
	
  	std::list<gm_songInfo>::iterator iter;
	for (iter = playlist.begin(); iter != playlist.end(); ++iter)
	{
		if (!iter->nosong)
		{
			plistlength++;
			pltotaltime += iter->time;
		}
	}

	lb_plstats.set_markup("<i>Tracks</i> " + into_string(plistlength) + 
	                      "   -   <i>Time</i> "   + into_time(pltotaltime));
}


void gm_browser::on_newStatus( mpd_status * sInfo )
{
	if (b_skip_status)
	{
		b_skip_status = false;
		return;
	}
	else
		b_skip_status = true;
	
    if ( mpd_status_get_repeat(sInfo) != b_repeat)
    {
        b_repeat = mpd_status_get_repeat(sInfo);
		cmi_repeat->set_active(b_repeat);
    }
    
    if (mpd_status_get_random(sInfo) != b_random)
    {
     	b_random = mpd_status_get_random(sInfo);
		cmi_random->set_active(b_random);
    }
    
    if (mpd_status_get_single(sInfo) != b_single)
    {
        b_single = mpd_status_get_single(sInfo);
		cmi_single->set_active(b_single);
    }
    
    if (mpd_status_get_consume(sInfo) != b_consume)
    {
        b_consume = mpd_status_get_consume(sInfo);
		cmi_consume->set_active(b_consume);
		thePlaylist->set_consume(b_consume);
    }
}


ustring gm_browser::into_time(int time)
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


std::string gm_browser::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}


bool gm_browser::on_delete_event(GdkEventAny* event)
{
	get_configs();
	return false; // pass on
}


bool gm_browser::string_from_input(ustring message, ustring &input)
{
	Gtk::Dialog dlg("Connect Stream", NULL);
	dlg.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dlg.add_button("OK", Gtk::RESPONSE_OK);
	dlg.set_default_response (Gtk::RESPONSE_CANCEL);
	
	Gtk::Grid content;
	content.set_row_spacing(10);
	content.set_orientation(Gtk::ORIENTATION_VERTICAL);
	Gtk::Entry entry;
	entry.set_hexpand(true);
	Gtk::Label label("", 0.0, 0.5);
	label.set_line_wrap(true);
	label.set_use_markup(true);
	label.set_markup(message);

	dlg.get_content_area()->pack_start(content);
	content.set_border_width(10);
	content.add(label);
	content.add(entry);
	content.show();
	label.show();
	entry.show();

	dlg.set_size_request(300, -1);

	if (dlg.run() == Gtk::RESPONSE_OK)
	{
		input = entry.get_text();
		return true;
	}
	else
		return false;
}


bool gm_browser::process_url(ustring input)
{
	// url must make sense
	if (!str_has_prefix(input.lowercase(),"http:"))
	{
		stream_error = "Not a valid URL. Try again or cancel ";
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
		stream_error = "Can't handle " + xtension + " files. Try again or cancel ";
		return false;
	}
	
	// url points directly to an audio file
	if (xtension == "mp2" || xtension == "mp3" ||xtension == "mp4" || xtension == "ogg")
	{
		std::list<ustring> url_list;
		url_list.push_back(input);
		thePlaylist->set_stream(url_list);		
		cout << "Stream: assuming binary" << endl;
		return true;
	}
	
	SoupURI *proxy = NULL;
	bool synchronous = FALSE;
	int opt;

	base = (char*)input.data(); // the url
	base_uri = soup_uri_new (base);
	if (!base_uri)
	{
		stream_error = "Could not parse the URL. Try again or cancel ";
		return false;
	}
	
	// not a playlist: add as binary
	if (xtension != "m3u" && xtension != "pls")
	{
		std::list<ustring> url_list;
		url_list.push_back(input);
		thePlaylist->set_stream(url_list);		
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

	//if (!synchronous)
		//loop;

	bool succes = get_url (base);

	//if (!synchronous)
		//stop loop;

	soup_uri_free (base_uri);
	
	return succes; 
}


bool gm_browser::get_url (const char *url)
{
	char *url_to_get, *slash, *name;
	SoupMessage *msg;
	int fd, i;
	SoupURI *uri;
	GPtrArray *hrefs;
	const char *header;

	if (strncmp (url, base, strlen (base)) != 0)
	{
		stream_error = "Download error. Try again or cancel ";
		return false;
	}
	
	if (strchr (url, '?') && strcmp (url, base) != 0)
	{
		stream_error = "Download error. Try again or cancel ";
		return false;
	}

	url_to_get = g_strdup (url);

	if (g_hash_table_lookup (fetched_urls, url_to_get))
	{
		stream_error = "Download error. Try again or cancel ";
		return false;
	}
	
	g_hash_table_insert (fetched_urls, url_to_get, url_to_get);

	msg = soup_message_new (SOUP_METHOD_GET, url_to_get);
	soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);

	soup_session_send_message (session, msg);

	name = soup_message_get_uri (msg)->path;
	if (strncmp (base_uri->path, name, strlen (base_uri->path)) != 0)
	{
		stream_error = "Download error. Try again or cancel ";
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
		stream_error = "Download failed. Try again or cancel ";
		return false;
	}
	else
		cout << "Stream: retrieved " << url << " OK" << endl;

	const char *s_line = msg->response_body->data;
	if (!strncasecmp(s_line, "[playlist]", 10)) // pls file
		return parse_pls_file(msg->response_body->data, msg->response_body->length);
	else if (!strncasecmp(s_line, "#EXTM3U", 7)) // extended m3u file
		return parse_extm3u_file(msg->response_body->data, msg->response_body->length);
    else if (!strncasecmp(msg->response_body->data, "<?xml", 5)) // xspf file
       return parse_xspf_file(msg->response_body->data, msg->response_body->length);
	else  // non-extended m3u file?
		return parse_extm3u_file(msg->response_body->data, msg->response_body->length);
}


bool gm_browser::parse_xspf_file(const char *data, int size)
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
		stream_error = "No stream found in xspf file. Try again or cancel ";
		return false;
	}
	else
	{
		thePlaylist->set_stream(url_list);
		return true;
	}
}


bool gm_browser::parse_pls_file(const char *data, int size)
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
		stream_error = "No stream found in pls file. Try again or cancel ";
		return false;
	}
	else
	{
		thePlaylist->set_stream(url_list);
		return true;
	}
}

bool gm_browser::parse_extm3u_file(const char *data, int size)
{
	int i = 0;
	int count  = 0;
	std::list<ustring> url_list;
	
	gchar **tokens = g_regex_split_simple("(\r\n|\n|\r)", data, G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_ANY);
	if (tokens)
	{
		for (i = 0; tokens[i]; i++)
		{
			// simply add any line strarting with "http://"
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
		stream_error = "No stream found in m3u file. Try again or cancel ";
		return false;
	}
	else
	{
		thePlaylist->set_stream(url_list);
		return true;
	}
}

void gm_browser::create_options_menu()
{
	plistMenu = new Gtk::Menu();

	cmi_random = new Gtk::CheckMenuItem("Random Playback");
	plistMenu->add(*cmi_random);
	cmi_random->signal_activate().connect(sigc::mem_fun(*this, &gm_browser::set_random));
	
	cmi_repeat = new Gtk::CheckMenuItem("Repeat List/Single");
	plistMenu->add(*cmi_repeat);
	cmi_repeat->signal_activate().connect(sigc::mem_fun(*this, &gm_browser::set_repeat));

	cmi_single = new Gtk::CheckMenuItem("Single Mode");
	plistMenu->add(*cmi_single);
	cmi_single->signal_activate().connect(sigc::mem_fun(*this, &gm_browser::set_single));

	cmi_consume = new Gtk::CheckMenuItem("Consume Mode");
	plistMenu->add(*cmi_consume);
	cmi_consume->signal_activate().connect(sigc::mem_fun(*this, &gm_browser::set_consume));
	
	plistMenu->show_all();
}


void gm_browser::set_songInfo(gm_songInfo *si)
{
	thePlaylist->set_songInfo(si);
}


gm_browser::~gm_browser()
{
}
 
