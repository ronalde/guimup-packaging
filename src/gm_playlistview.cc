/*
 *  gm_playlistview.cc
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

#include "gm_playlistview.h"
#include "gm_playlistview_pix.h"

gm_Playlist::gm_Playlist()
{
	b_streamplaying = false;
	b_nosong = false;
	b_consume = false;
	b_allow_newplist = true;
	current_status = -10;
	current_songID = -10;
	mpdCom = NULL;
	current_sInfo = NULL;
	config = NULL;
	b_connected = false;
	plistLength = 0;
	// pango
	context = Glib::wrap(gdk_pango_context_get());
	layout = Pango::Layout::create(context);
	// grafix
    pxb_tr_track = Gdk::Pixbuf::create_from_inline(-1, tr_track, false);
	pxb_tr_trplayed = Gdk::Pixbuf::create_from_inline(-1, tr_track_played, false);
    pxb_tr_trackx = Gdk::Pixbuf::create_from_inline(-1, tr_trackx, false);
	pxb_tr_trxplayed = Gdk::Pixbuf::create_from_inline(-1, tr_trackx_played, false);
    pxb_tr_stream = Gdk::Pixbuf::create_from_inline(-1, tr_stream, false);
	pxb_tr_strplayed = Gdk::Pixbuf::create_from_inline(-1, tr_stream_played, false);	
    pxb_tr_idle = Gdk::Pixbuf::create_from_inline(-1, tr_wait, false);
    pxb_tr_play = Gdk::Pixbuf::create_from_inline(-1, tr_play, false);
    pxb_tr_stop = Gdk::Pixbuf::create_from_inline(-1, tr_stop, false);
    pxb_tr_pause = Gdk::Pixbuf::create_from_inline(-1, tr_pause, false);
    pxb_tr_playlist = Gdk::Pixbuf::create_from_inline(-1, tr_playlist, false);
	pxb_drag_songs = Gdk::Pixbuf::create_from_inline(-1, drag_songs, false);
	// scrollbars
	set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	add(plTreeView);
	plTreeView.set_rules_hint(true);
	theSelection = plTreeView.get_selection();
	theSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	// columns
	Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );	
		// number
	column1.set_widget(col1_label);
	column1.get_widget()->show_all();
	col1_label.set_text(" # ");
	column1.pack_start(*pRenderer);
	column1.add_attribute(pRenderer->property_text(), plistCols.col_nr);
	column1.add_attribute(pRenderer->property_foreground(), plistCols.col_color);
	column1.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	column2.set_resizable(false);
	column1.set_fixed_width(24); // set_nrColumn_width()
	plTreeView.append_column (column1);
		// icon
	column2.pack_start(plistCols.col_icon);
	column2.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	column2.set_fixed_width(20);
	column2.set_resizable(false);
	plTreeView.append_column (column2);
		// artist / see also set_config()
	column3.set_widget(col3_label);
	column3.get_widget()->show_all();
	col3_label.set_text("Artist ");
	column3.pack_start(*pRenderer);
	column3.add_attribute(pRenderer->property_text(), plistCols.col_artist);
	column3.add_attribute(pRenderer->property_foreground(), plistCols.col_color);
	column3.set_resizable(true);
	plTreeView.append_column (column3);
		// title / see also set_config()
	column4.set_widget(col4_label);
	column4.get_widget()->show_all();
	col4_label.set_text("Title ");
	column4.pack_start(*pRenderer);
	column4.add_attribute(pRenderer->property_text(), plistCols.col_title);
	column4.add_attribute(pRenderer->property_foreground(), plistCols.col_color);
	column4.set_resizable(true);
	plTreeView.append_column (column4);
		// time
	column5.set_widget(col5_label);
	column5.get_widget()->show_all();
	col5_label.set_text("m:s");
	col5_label.set_property("xalign", 1.0);
	column5.pack_start(*pRenderer);
	column5.add_attribute(pRenderer->property_text(), plistCols.col_time);
	column5.add_attribute(pRenderer->property_foreground(), plistCols.col_color);
	column5.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	column5.set_fixed_width(40); // see set_listfont()
	column5.set_resizable(false);
	plTreeView.append_column (column5);	
		// album & tracknr / see also set_config()
	column6.set_widget(col6_label);
	column6.get_widget()->show_all();
	col6_label.set_markup("Album : nr ");
	column6.pack_start(*pRenderer);
	column6.add_attribute(pRenderer->property_text(), plistCols.col_album);
	column6.add_attribute(pRenderer->property_foreground(), plistCols.col_color);
	column6.set_resizable(true);
	plTreeView.append_column (column6);
	
	// mouse offset
	int y_offset = pRenderer->property_rise();
	int height = pRenderer->property_size(); 
	mouse_offset = y_offset + height + 6;// add 6 for header borders
	plTreeView.set_mouse_offset(mouse_offset);
	
	create_rClickmenu();
	
	plListStore = Gtk::ListStore::create(plistCols);
	plTreeView.set_model(plListStore);
		
	// D&D targets. See also set_connected()
	DnDlist.push_back(Gtk::TargetEntry("STRING", Gtk::TARGET_SAME_APP,0));
    DnDlist.push_back(Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP,1));
	DnDlist.push_back(Gtk::TargetEntry("text/uri-list", Gtk::TARGET_OTHER_APP,0 ));
	
	// double click
	plTreeView.signal_row_activated().connect(sigc::mem_fun(*this, &gm_Playlist::on_row_activated) );
	// spacebar pressed
	plTreeView.signal_key_pressed.connect(sigc::mem_fun(*this, &gm_Playlist::on_acckey_pressed) );
	// del pressed
	plTreeView.signal_key_press_event().connect(sigc::mem_fun(*this, &gm_Playlist::on_key_pressed) );
	// mouse-click
	plTreeView.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Playlist::on_button_pressed), false);
	// drag & drop
	plTreeView.signal_drag_data_received().connect(sigc::mem_fun(*this, &gm_Playlist::on_pldata_received) );
	plTreeView.signal_drag_data_get().connect(sigc::mem_fun(*this, &gm_Playlist::on_pldrag_data_get));
	plTreeView.signal_drag_motion().connect(sigc::mem_fun(*this, &gm_Playlist::on_pldrag_motion));

	show_all_children();
}


void gm_Playlist::create_rClickmenu()
{
	Glib::RefPtr<Gdk::Pixbuf> pxb;

	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_prgbutn, false);
	img_men_purge.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, tb_shuffle, false);
	img_men_shuffle.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_clrbutn, false);
	img_men_delete.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_redcross, false);
	img_men_clear.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_save, false);
	img_men_save.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_saveselect, false);
	img_men_saveselect.set(pxb);

	rClickMenu = new Gtk::Menu();
	
	imi_clear = new Gtk::ImageMenuItem(img_men_clear,"Clear list");
	rClickMenu->add(*imi_clear);
	imi_clear->signal_activate().connect(sigc::mem_fun(*this, &gm_Playlist::on_menu_clear));
	
	imi_rmove = new Gtk::ImageMenuItem(img_men_delete,"Remove selected");
	rClickMenu->add(*imi_rmove);
	imi_rmove->signal_activate().connect(sigc::mem_fun(*this, &gm_Playlist::on_del_selected));

	imi_purge = new Gtk::ImageMenuItem(img_men_purge,"Purge played");
	rClickMenu->add(*imi_purge);
	imi_purge->signal_activate().connect(sigc::mem_fun(*this, &gm_Playlist::on_menu_purge));

	imi_shuffle = new Gtk::ImageMenuItem(img_men_shuffle,"Shuffle list");
	rClickMenu->add(*imi_shuffle);
	imi_shuffle->signal_activate().connect(sigc::mem_fun(*this, &gm_Playlist::on_menu_shuffle));
	
	imi_save = new Gtk::ImageMenuItem(img_men_save,"Save list");
	rClickMenu->add(*imi_save);
	imi_save->signal_activate().connect(sigc::mem_fun(*this, &gm_Playlist::on_menu_savelist));

	imi_selsave = new Gtk::ImageMenuItem(img_men_saveselect,"Save selected");
	rClickMenu->add(*imi_selsave);
	imi_selsave->signal_activate().connect(sigc::mem_fun(*this, &gm_Playlist::on_menu_saveselected));
	
	rClickMenu->show_all();
}


void gm_Playlist::on_menu_saveselected()
{
    if (plistLength == 0 || !b_connected)
        return;
        
	std::vector<tm_Path> selpaths = theSelection->get_selected_rows();

    if (selpaths.size() == 0)
		return;
	
    if (selpaths.size() == plistLength)
    {
        on_menu_savelist();
        return;
    }

	bool b_songx = false;
    std::vector<ustring> tobeSaved;
	std::vector<tm_Path>::iterator path;
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		tm_Iterator iter = plListStore->get_iter(*path);
		if (iter)
		{
			if ((*iter)[plistCols.col_type] != TP_SONGX)
        		tobeSaved.push_back((*iter)[plistCols.col_file]);
			else
				b_songx = true;
		}
    }
	
	if (tobeSaved.empty())
	{
		show_message("External songs cannot be saved as a playlist!");
		return;
	}

	if (b_songx)
		show_message("External songs will not be included!");	
	    
	ustring input;
	ustring plistpath;
	if ( string_from_input("Enter a name for the list:", input))
	{
		bool saved = false;
		
		if (!input.empty())
		{
			plistpath = config->MPD_Playlistpath + input + ".m3u";
			if (!Glib::file_test(plistpath.data(),Glib::FILE_TEST_EXISTS))
				saved = mpdCom->save_selection(tobeSaved, input);
		}
		
		bool new_try = true; // false upon 'cancel', true upon 'ok'
		
		while (!saved && new_try)
		{
			new_try = string_from_input("Could not save the list. Try a different name or cancel. ", input);
			if (new_try && !input.empty())
			{
				plistpath = config->MPD_Playlistpath + input + ".m3u";
				if (!Glib::file_test(plistpath.data(),Glib::FILE_TEST_EXISTS))
					saved = mpdCom->save_selection(tobeSaved, input);
			}
			else
				saved = false;
		}
		if (saved)
			 signal_plist_saved.emit();
	}	

}

bool gm_Playlist::on_pldrag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int dragx, int dragy, guint time)
{
	if (plistLength == 0)
		 return false;
	
	// scroll the drag down when the bottom is close
	if (dragy > (plTreeView.get_height() - 10) )
	{
		tm_Path path;
		Gtk::TreeViewDropPosition pos;
		if (plTreeView.get_dest_row_at_pos(dragx,  plTreeView.get_height() - 10, path, pos))
		{
			path.next();
			if (!path.empty())
				plTreeView.scroll_to_row(path);
		}
		return false;
	}
	
	// scroll the drag up when the top is close
	if (dragy < mouse_offset + 10)
	{
		tm_Path path;
		Gtk::TreeViewDropPosition pos;
		if (plTreeView.get_dest_row_at_pos(dragx, mouse_offset + 10, path, pos))
		{
			if (path.prev())
				plTreeView.scroll_to_row(path);
		}
		return false;
	}	
	
	// set the drop position
	tm_Path path;
	Gtk::TreeViewDropPosition pos;
	if (plTreeView.get_dest_row_at_pos(dragx, dragy, path, pos))
	{
		if (pos == Gtk::TREE_VIEW_DROP_INTO_OR_BEFORE || pos == Gtk::TREE_VIEW_DROP_BEFORE)			
			plTreeView.set_drag_dest_row (path, Gtk::TREE_VIEW_DROP_BEFORE);
		if (pos == Gtk::TREE_VIEW_DROP_INTO_OR_AFTER || pos == Gtk::TREE_VIEW_DROP_AFTER)			
			plTreeView.set_drag_dest_row (path, Gtk::TREE_VIEW_DROP_AFTER);	
	}

	return false;
}


void gm_Playlist::set_nrColumn_width()
{
	int w, h;
	layout->set_text(into_string(plistLength));
	layout->get_pixel_size(w,h);
	if (w < 16)
		w = 16;
	column1.set_fixed_width(w + 8);
}

// called by player
void gm_Playlist::set_listfont(Pango::FontDescription pFont)
{
	layout->set_font_description(pFont);
	plTreeView.override_font(pFont);
	// set nr column width;
	set_nrColumn_width();
	// set time column width;
	int w, h;
	layout->set_text("00:00");
	layout->get_pixel_size(w,h);
	column5.set_fixed_width(w + 8);
}


void gm_Playlist::on_pldata_received( const Glib::RefPtr<Gdk::DragContext>& context, int dragx, int dragy,
        const Gtk::SelectionData& selection_data, guint, guint time)
{

	if (!b_connected || selection_data.get_length() < 0 || selection_data.get_format() != 8)
	{
		context->drag_finish(false, false, time);
		return;
	}
	
	ustring sender = selection_data.get_data_as_string();

	int droppos; 
	tm_Iterator droppositer;
	if (plistLength == 0)
	{
		if(sender == "FromLiBry")
		{
			signal_drop_received.emit(-1); // -1 : empty list
			context->drag_finish(false, false, time);
			return;
		}
		droppos = 0;
		droppositer = plListStore->children().begin();
	}
	else
	{		
		// get the drop position
		tm_Path path;
		Gtk::TreeViewDropPosition pos;
		if (plTreeView.get_dest_row_at_pos(dragx, dragy, path, pos))
		{
			tm_Iterator iter = plListStore->get_iter(path);
			if (iter)
			{
				int rownr = (*iter)[plistCols.col_nr];
				if (pos == Gtk::TREE_VIEW_DROP_BEFORE || pos == Gtk::TREE_VIEW_DROP_INTO_OR_BEFORE)
				{
					droppos = rownr-1;
					droppositer = iter;
				}
				else
				{
					droppos = (*iter)[plistCols.col_nr];
					if (droppos == plistLength)
						droppositer = plListStore->children().end();
					else
					{
						path.next();
						droppositer = plListStore->get_iter(path);
					}
				}
			}
		}
		else // dropped below the list: append
		{
			droppos = plistLength;
			droppositer = plListStore->children().end();
		}
	}
	if(sender == "FromLiBry") // dragged from libraryview
	{
		signal_drop_received.emit(droppos);
	}
	else
  	if(sender == "FromPList") // dragged within playlistview
  	{
		// move rows
		std::list<tm_RowRef> rowrefsList;
		std::vector<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
		std::vector<Gtk::TreeModel::Path>::iterator path;
		// move(iter) invalidates other iters: store rowrefs
		for(path = selpaths.begin(); path != selpaths.end(); ++path)
			rowrefsList.push_back(tm_RowRef(plListStore, *path));
		// nex create iter from rowrefs and move into position
		for (std::list<Gtk::TreeModel::RowReference>::iterator i =
					rowrefsList.begin(); i != rowrefsList.end(); i++)
		{
			Gtk::TreeModel::Path path = i->get_path();
			Gtk::TreeModel::iterator iter = plListStore->get_iter(path);
			plListStore->move(iter, droppositer);
		}		
		rowrefsList.clear();
		  
		// next sync the mpd playlist with the treeview
		gm_commandList movCommandList;
		int pos = 0;
		tm_Children children = plListStore->children();
		for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			Gtk::TreeModel::Row row = *iter;
			if (row[plistCols.col_nr]-1 != pos)
			{
				gm_cmdStruct newCommand;
				newCommand.cmd = CMD_MOV;
				newCommand.songid = row[plistCols.col_id];
				newCommand.moveTo = pos;
				movCommandList.push_back(newCommand);
			}
			pos++;
		}
		
		mpdCom->execute_cmds( movCommandList, false );
		movCommandList.clear();
		renumber_list();  
  	}
	else
	if (str_has_prefix(sender,"file://")) // dragged from external source
	{
		std::vector<Glib::ustring> urilist = selection_data.get_uris();
		on_external_drop(urilist, droppos);
	}
	                   
	context->drag_finish(false, false, time);
}


void  gm_Playlist::on_external_drop(std::vector<ustring> urilist, int droppos)
{
	if (urilist.empty() || !b_connected)
		return;

	if (!config->mpd_socket_conn)
	{
		cout << "Dropped files ignored (no socket connection)" << endl;
		return;
	}
	else
		cout << "Appending dropped files to playlist" << endl;

	int added = 0;
	int i = 0;
	int last = urilist.size();
	std::vector<ustring>::iterator url;
	for(url = urilist.begin(); url != urilist.end(); ++url)
    {
		/* filename_from_uri() replaces '%20' with ' ' 
		   AND removes 'file://' */
		ustring newurl = Glib::filename_from_uri(*url); 
		newurl = "file://" + newurl;
	    gm_cmdStruct newCommand;
        newCommand.cmd = CMD_INS;
        newCommand.moveTo = droppos + added;
		newCommand.file = newurl;
		i ++;
		if (i == last) // trigger playlist update
		   	added += mpdCom->execute_single_command( newCommand, true);
		else
			added += mpdCom->execute_single_command( newCommand, false);
    }
}


void gm_Playlist::on_pldrag_data_get( const Glib::RefPtr<Gdk::DragContext>&,
        Gtk::SelectionData& selection_data, guint, guint)
{
	// Drag only a message: 'FromPList'
	selection_data.set(selection_data.get_target(), 8, (const guchar*)"FromPList", 9);
}


bool gm_Playlist::on_button_pressed(GdkEventButton* event)
{
	if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
	{
		rClickMenu->popup(event->button, event->time);
		return true; 
	}
	else 
	   return false; // pass on
}


void gm_Playlist::on_menu_savelist()
{
    if (plistLength == 0 || !b_connected)
        return;

	int xcount = 0;

	tm_Children children = plListStore->children();
	for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		if ((*iter)[plistCols.col_type] == TP_SONGX)
			xcount++;
	}

	if (xcount == plistLength)
	{
		show_message("External songs cannot be saved as a playlist!");
		return;
	}

	if (xcount > 0)
		show_message("External songs will not be included!");
	
	ustring input;
	ustring plistpath;
	if ( string_from_input("Enter a name for the list. ", input))
	{
		bool saved = false;
		
		if (!input.empty())
		{
			plistpath = config->MPD_Playlistpath + input + ".m3u";
			if (!Glib::file_test(plistpath.data(),Glib::FILE_TEST_EXISTS))
					saved = mpdCom->save_playlist(input);
		}

		bool new_try = true; // false upon 'cancel', true upon 'ok'
		
		while (!saved && new_try)
		{
			new_try = string_from_input("Could not save the list. Try a different name or cancel. ", input);
			if (new_try && !input.empty())
			{
				plistpath = config->MPD_Playlistpath + input + ".m3u";
				if (!Glib::file_test(plistpath.data(),Glib::FILE_TEST_EXISTS))
					saved = mpdCom->save_playlist(input);
			}
			else
				saved = false;
		}
		if (saved)
			 signal_plist_saved.emit();
	}
}


bool gm_Playlist::string_from_input(ustring message, ustring &input)
{
	Gtk::Dialog dlg("Save Playlist", NULL);
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


void gm_Playlist::on_menu_clear()
{
	if (!b_connected || plistLength == 0)
		return;
	plListStore.clear();
	plTreeView.unset_model();	
	plListStore = Gtk::ListStore::create(plistCols);
	plTreeView.set_model(plListStore);	
	mpdCom->clear_list();
}


void gm_Playlist::on_menu_shuffle()
{
	if (!b_connected || plistLength == 0)
		return;
	mpdCom->shuffle_list();
}


void gm_Playlist::on_del_selected()
{
	if (!b_connected || plistLength == 0)
		return;
	
    std::list<tm_RowRef> rowrefsList;
	std::vector<tm_Path> selpaths = theSelection->get_selected_rows();
	if (selpaths.size() == 0)
		return;
	
	gm_commandList newCommandList;
	std::vector<tm_Path>::iterator path;
	// fill newCommandList & rowrefsList
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		tm_Row row = *(plListStore->get_iter(*path));
		gm_cmdStruct newCommand;
		newCommand.cmd = CMD_DEL;
		int songID = row[plistCols.col_id];
		newCommand.songid = songID;
		if (songID == current_songID)
		{
			current_songID = -1;
			current_status = -10;
		}
		// highest nrs first: push_front!
		newCommandList.push_front(newCommand);
		// erase(iter) invalidates other iters: store rowrefs
		rowrefsList.push_back(tm_RowRef(plListStore, *path));
    }

    // remove the rows
    for (std::list<tm_RowRef>::iterator i =
                rowrefsList.begin(); i != rowrefsList.end(); i++)
    {
		tm_Path path = i->get_path();
		tm_Iterator iter = plListStore->get_iter(path);
		if (iter)
		{
        	plListStore->erase(iter);
			plistLength--;
		}
   }		
	rowrefsList.clear();
	
	b_allow_newplist = false;
	// execute_cmds(true) triggers statistics update:	
	mpdCom->execute_cmds( newCommandList, true);
	newCommandList.clear();
	
	renumber_list();	
}

//re-number the plist and get ID's
/* 	NOTE:
	We call this after adding/deleting tracks in mpd AND 
    adding/deleting the same tracks to the playlist here.
	If the playlist is not first updated here get_IDlist()
	will execute BEFORE the statusloop has triggered a
	playlist update. Consequently The ID-list will not match
	the playlist and ID's will end up in the wrong tracks!	
*/ 
void gm_Playlist::renumber_list() 
{
	if (!b_connected)
		return;
	
	plistLength = 0;
	int pos = 0;
	gm_IDlist idlist = mpdCom->get_IDlist();
	tm_Children children = plListStore->children();
	bool b_idlist_shorter = false;
	
	for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		plistLength++;
		(*iter)[plistCols.col_nr] = plistLength;
		if (pos < idlist.size())
		{
			(*iter)[plistCols.col_id] = idlist.at(pos);
			pos++;
		}
		else
		{
			b_idlist_shorter = true;
			break;
		}
	}

	set_nrColumn_width();
	
	// playing it safe..
	if (idlist.size() > plistLength || b_idlist_shorter)
	{
		cout << "Error on \"renumber_list\"!" << endl;
    	mpdCom->get_playlist();
	}
}


bool gm_Playlist::on_key_pressed(GdkEventKey* event)
{
	if (event->keyval == GDK_KEY_Delete)
	{
		on_del_selected();
		return true; 
	}
	return false; // pass on
}


// custom action for space-bar
void gm_Playlist::on_acckey_pressed(GdkEventKey* event)
{
	if (event->keyval == GDK_KEY_space)
	{
		tm_Children children = plListStore->children();
		for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			if ((*iter)[plistCols.col_id] == current_songID)
			{
				tm_Path path = (plTreeView.get_model())->get_path(iter);
				plTreeView.scroll_to_row(path, 0.5);
				break;
			}
		}
	}
}


void gm_Playlist::on_row_activated(const tm_Path& path, Gtk::TreeViewColumn *tvcol)
{
	if (!b_connected)
		return;
	
	tm_Iterator iter = plListStore->get_iter(path);
	if(iter)
		mpdCom->play_this((*iter)[plistCols.col_id]);
 }


gm_IDlist gm_Playlist::get_played_IDs()
{
	gm_IDlist the_list;

	tm_Children children = plListStore->children();
	for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		if ((*iter)[plistCols.col_played])
			the_list.push_back((*iter)[plistCols.col_id]);
	}
	return the_list;
}


void gm_Playlist::on_new_playlist(gm_songList playlist, int changerID)
{
	if (!b_allow_newplist)
	{
		b_allow_newplist = true;
		return;
	}
	
    // prevent resetting by a stream
    if (b_streamplaying)
    {
        // current song has reset the list
        if (current_songID == changerID)
            return;
    }

	// reset current song
	current_songID = -10; 
	current_status = -1;
	
	gm_IDlist shadow_list;

	if (config->pList_mark_played && !b_consume)
		shadow_list= get_played_IDs();// ID-list of the 'played' status

	plListStore.clear();
	plTreeView.unset_model();	
	plListStore = Gtk::ListStore::create(plistCols);
	
	// fill the liststore
  	std::list<gm_songInfo>::iterator iter;
	plistLength = 0;
	for (iter = playlist.begin(); iter != playlist.end(); ++iter)
	{
		if (!iter->nosong)
		{
    		tm_Row row = *(plListStore->append());
			if (iter->type == TP_STREAM)
			{
    			row[plistCols.col_artist] = "Stream ";
   				row[plistCols.col_title] = "Info in player  ";
				row[plistCols.col_time] = " [»] ";
				row[plistCols.col_album] = iter->file_mpd;
			}
			else
			{
				if (iter->album.empty())
					iter->album = "?";
				if (iter->artist.empty())
					iter->artist = "?";
 				if (iter->title.empty())
					iter->title = "?";
 				if (iter->track.empty())
					iter->track = "?";
    			row[plistCols.col_artist] = iter->artist + " ";
    			row[plistCols.col_title] = iter->title + " ";
				row[plistCols.col_time] = into_time(iter->time) + " ";
				row[plistCols.col_album] = iter->album + " : " + iter->track;
			}
			row[plistCols.col_nr] = iter->songNr + 1;
			row[plistCols.col_id] = iter->songID;
			row[plistCols.col_type] = iter->type;

			bool b_played = false;
			
			if (config->pList_mark_played && !b_consume)
			{
				// check shadow list for 'played' status
				std::vector<int>::iterator sliter;
				for(sliter = shadow_list.begin(); sliter != shadow_list.end(); ++sliter)
				{
					if (*sliter == iter->songID)
					{
						b_played = true;
						shadow_list.erase(sliter); // ok because we 'break'
						break;
					}
				}
			}
			
			if ( iter->type == TP_SONGX )
			{
				if (config->pList_mark_played && b_played)
					row[plistCols.col_icon] = pxb_tr_trxplayed;
				else
					row[plistCols.col_icon] = pxb_tr_trackx;
			}
			else
			if ( iter->type == TP_STREAM )
			{
				if (config->pList_mark_played && b_played)
					row[plistCols.col_icon] = pxb_tr_strplayed;
				else
					row[plistCols.col_icon] = pxb_tr_stream;
			}
			else
			{
				if (config->pList_mark_played && b_played)
			    	row[plistCols.col_icon] = pxb_tr_trplayed;
				else
					row[plistCols.col_icon] = pxb_tr_track;
			}
			
			if (config->pList_mark_played && b_played)
				row[plistCols.col_color] = "#667580";
			else
				row[plistCols.col_color] = "#000000";
			
			row[plistCols.col_played] = b_played;
			row[plistCols.col_file] = iter->file_mpd;	
			plistLength++;
		}
	}	
	plTreeView.set_model(plListStore);
	playlist.clear();
	set_nrColumn_width();

	if (plistLength > 0)
		cout << "New playlist" << endl;
}


void gm_Playlist::on_newSong() 
{
	if (current_sInfo == NULL)
		return;
	
	int new_songID = current_sInfo->songID;	
	
	if (current_sInfo->nosong)
	{
		new_songID = -1;
		current_status = MPD_STATE_UNKNOWN;
		b_nosong = true;
	}
	else
		b_nosong = false;
	
	if (new_songID != current_songID)
	{
		tm_Children children = plListStore->children();
		// if no current song, set the focus on the first
		if (new_songID < 0)
		{
			tm_Children::iterator first = children.begin();
			if (first)
			{
				tm_Row row = *first;
				new_songID = row[plistCols.col_id];
			}
			else
			{	// empty list
				current_songID = -1;
				current_status = -1;
				return;
			}
		}

		
		for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			tm_Row row = *iter;

			// reset the current song
			if (row[plistCols.col_id] == current_songID)
			{
				if(row[plistCols.col_played] && !b_consume && config->pList_mark_played)
				{
					if (row[plistCols.col_type] == TP_STREAM)
						row[plistCols.col_icon] = pxb_tr_strplayed;
					else
					if (row[plistCols.col_type] == TP_SONGX)
						row[plistCols.col_icon] = pxb_tr_trxplayed;	
					else
						row[plistCols.col_icon] = pxb_tr_trplayed;
					row[plistCols.col_color] = "#667580";
				}
				else
				{
					row[plistCols.col_played] = false; // reset played status
					if (row[plistCols.col_type] == TP_STREAM)
						row[plistCols.col_icon] = pxb_tr_stream;
					else
					if (row[plistCols.col_type] == TP_SONGX)
						row[plistCols.col_icon] = pxb_tr_trackx;	
					else
						row[plistCols.col_icon] = pxb_tr_track;
					row[plistCols.col_color] = "#000000";
				}
			}

			// set the new song
			if (row[plistCols.col_id] == new_songID)
			{
				row[plistCols.col_color] = "#004070";
				
				b_streamplaying = (row[plistCols.col_type] == TP_STREAM);
				switch (current_status)
				{
					case MPD_STATE_PLAY:
					{
						if (config->pList_mark_played)
							row[plistCols.col_played] = true;
						row[plistCols.col_icon] = pxb_tr_play;
						break;
					}
					case MPD_STATE_STOP:
					{
						row[plistCols.col_icon] = pxb_tr_stop;
						break;
					}
					case MPD_STATE_PAUSE:
					{
						row[plistCols.col_icon] = pxb_tr_pause;
						break;
					}
					default: // MPD_STATUS_STATE_UNKNOWN
					{
						row[plistCols.col_icon] = pxb_tr_idle;
						break;
					}
				}
			}
		}
		current_songID = new_songID;
	}
}


void gm_Playlist::on_newStatus( mpd_status * sInfo )
{
	int newstatus = -10;
	
    if ((b_nosong && current_status != MPD_STATE_UNKNOWN))
        newstatus = MPD_STATE_UNKNOWN;
    else
        if (mpd_status_get_state(sInfo) != current_status)
            newstatus = mpd_status_get_state(sInfo);

	if (newstatus != -10)
	{
		tm_Children children = plListStore->children();
		for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			tm_Row row = *iter;
			// set the new status
			if (row[plistCols.col_id] == current_sInfo->songID)
			{
				switch (newstatus)
				{
					case MPD_STATE_PLAY:
					{
						row[plistCols.col_played] = true;
						row[plistCols.col_icon] = pxb_tr_play;
						break;
					}
					case MPD_STATE_STOP:
					{
						row[plistCols.col_icon] = pxb_tr_stop;
						break;
					}
					case MPD_STATE_PAUSE:
					{
						row[plistCols.col_icon] = pxb_tr_pause;
						break;
					}
					default:  // 0 (MPD_STATE_UNKNOWN), -1 (disconnected) etc.
					{
						row[plistCols.col_icon] = pxb_tr_idle;
						break;
					}
				}
			}
		}
		current_status = newstatus;
	}	
}


ustring gm_Playlist::into_time(int time)
{
    ustring formattedTime = "";
    int mins = (int)(time / 60);
    formattedTime += into_string(mins) + ":";
    int secs = time % 60;
    if(secs < 10)
	formattedTime += "0";
    formattedTime += into_string(secs);
	return formattedTime;
}


std::string gm_Playlist::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}

// called by browserWindow
void gm_Playlist::set_consume(bool cmode)
{
	b_consume = cmode;

	if (b_consume)
		imi_purge->hide();
	else
		imi_purge->show();
	if (plistLength != 0)
	{
	tm_Children children = plListStore->children();
		for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			if ((*iter)[plistCols.col_played] &&  (*iter)[plistCols.col_id] != (current_songID))
			{
				{
					if (b_consume) // hide "played" status
					{
						if ((*iter)[plistCols.col_type] == TP_STREAM)
							(*iter)[plistCols.col_icon] = pxb_tr_stream;
						else
						if ((*iter)[plistCols.col_type] == TP_SONGX)
							(*iter)[plistCols.col_icon] = pxb_tr_trackx;	
						else
							(*iter)[plistCols.col_icon] = pxb_tr_track;
						(*iter)[plistCols.col_color] = "#000000";
						(*iter)[plistCols.col_played] = false;
					}
					else // reset "played" status
					{ 
						if ((*iter)[plistCols.col_type] == TP_STREAM)
							(*iter)[plistCols.col_icon] = pxb_tr_strplayed;
						else
						if ((*iter)[plistCols.col_type] == TP_SONGX)
							(*iter)[plistCols.col_icon] = pxb_tr_trxplayed;	
						else
							(*iter)[plistCols.col_icon] = pxb_tr_trplayed;
						(*iter)[plistCols.col_color] = "#667580";	
					}
				}
			}
		}
	}
}


void gm_Playlist::on_menu_purge()
{
	if (!b_connected || plistLength == 0)
		return;
	gm_commandList newCommandList;
	std::list<tm_RowRef> rowrefsList;
	tm_Children children = plListStore->children();
	for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		if (    (*iter)[plistCols.col_played]
			&&  (*iter)[plistCols.col_id] != (current_songID))
		{
			gm_cmdStruct newCommand;
			newCommand.cmd = CMD_DEL;
			newCommand.songid = (*iter)[plistCols.col_id];
			// highest nrs first: push_front!
			newCommandList.push_front(newCommand);
			// erase(iter) invalidates other iters: store rowrefs
			tm_Path path = (plTreeView.get_model())->get_path(iter);
			rowrefsList.push_back(tm_RowRef(plListStore, path));
		}
	}

	// next remove
	for (std::list<tm_RowRef>::iterator i =
				rowrefsList.begin(); i != rowrefsList.end(); i++)
	{
		tm_Path path = i->get_path();
		tm_Iterator iter = plListStore->get_iter(path);
		if (iter)
		{
			plListStore->erase(iter);
			plistLength--;
		}
	}

	rowrefsList.clear();

	b_allow_newplist = false;
	// execute_cmds(true) triggers statistics update:			
	mpdCom->execute_cmds( newCommandList, true );
	newCommandList.clear();
	renumber_list();
}

// called by browserWindow
void gm_Playlist::set_stream(std::list<ustring> url_list)
{
	if (!b_connected)
		return;
	
	gm_commandList cmdlist;
	
  	std::list<ustring>::iterator it;
	for (it = url_list.begin(); it != url_list.end(); ++it)
	{
		// inform mpd
		gm_cmdStruct newCommand;
		newCommand.cmd = CMD_ADD;
		newCommand.file = *it;
		cmdlist.push_back(newCommand);
		// inform the playlist
		tm_Row row;
		row = *(plListStore->append());
		row[plistCols.col_icon] = pxb_tr_stream;	
		row[plistCols.col_type] = TP_STREAM;	
   		row[plistCols.col_artist] = "Stream ";
   		row[plistCols.col_title] = "Info in player  ";
		row[plistCols.col_time] = " [»] ";
		row[plistCols.col_album] = *it;
		row[plistCols.col_file] = *it;
		row[plistCols.col_color] = "#000000";
		row[plistCols.col_played] = false;
	}

	b_allow_newplist = false;
	// execute_cmds(true) triggers statistics update:	
	mpdCom->execute_cmds( cmdlist, true );
	cmdlist.clear();	
	
	renumber_list(); 
}


void gm_Playlist::quick_select(ustring findthis, int mode)
{
	if (!b_connected || plistLength == 0)
		return;

	bool match = false;
	findthis = findthis.lowercase();
	theSelection->unselect_all();
	tm_Children children = plListStore->children();
	tm_Children::iterator lastiter;
	for(tm_Children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		plistLength++;
		ustring str;
		int i;
		
		match = false;
		if (findthis.empty())
			match = true;
		
		if (!match && (mode == SL_ID_ALL || mode == SL_ID_TITLE))
		{
			str = (*iter)[plistCols.col_title];
			str = str.lowercase();
			i = str.find(findthis);
			if ( i >= 0 && i < str.length() )
			match = true;
		}
		
		if (!match && (mode == SL_ID_ALL || mode == SL_ID_ARTIST))
		{
			str = (*iter)[plistCols.col_artist];
			str = str.lowercase();
			i = str.find(findthis);
			if ( i >= 0 && i < str.length() )
			match = true;
		}
		
		if (!match && (mode == SL_ID_ALL || mode == SL_ID_ALBUM))
		{
			str = (*iter)[plistCols.col_album];
			str = str.lowercase();
			i = str.find(findthis);
			if ( i >= 0 && i < str.length() )
			match = true;	
		}
		
		if (match)
		{
			theSelection->select(*iter);
			lastiter = iter;
		}
	}
	
	if (lastiter)
	{
		tm_Path path = (plTreeView.get_model())->get_path(lastiter);
		plTreeView.scroll_to_row(path, 0.5);
	}
	plTreeView.grab_focus();
}


void gm_Playlist::set_mpdcom(gm_mpdCom *com)
{
	mpdCom = com;
	
	if (mpdCom != NULL)
	{
		mpdCom->sgnl_plistupdate.connect(sigc::mem_fun(*this, &gm_Playlist::on_new_playlist));
		mpdCom->sgnl_newSong.connect(sigc::mem_fun(*this, &gm_Playlist::on_newSong));
		mpdCom->sgnl_statusInfo.connect(sigc::mem_fun(*this, &gm_Playlist::on_newStatus));
	}
}


void gm_Playlist::set_connected(bool iscon)
{
	b_connected = iscon;
	if (b_connected)
	{
		plTreeView.drag_dest_set(DnDlist);
		plTreeView.drag_source_set(DnDlist);
		plTreeView.drag_source_set_icon(pxb_drag_songs);

		if (config->mpd_deleteid_allowed)
			imi_purge->set_sensitive(true);
		else
			imi_purge->set_sensitive(false);

		if (config->mpd_shuffle_allowed)
			imi_shuffle->set_sensitive(true);
		else
			imi_shuffle->set_sensitive(false);

		if (config->mpd_clear_allowed)
			imi_clear->set_sensitive(true);
		else
			imi_clear->set_sensitive(false);

		if (config->mpd_deleteid_allowed)
			imi_rmove->set_sensitive(true);
		else
			imi_rmove->set_sensitive(false);

		if (config->mpd_save_allowed)
			imi_save->set_sensitive(true);
		else
			imi_save->set_sensitive(false);

		if(config->mpd_save_allowed)
			imi_selsave->set_sensitive(true);
		else
			imi_selsave->set_sensitive(false);
	}
	else
	{
		imi_purge->set_sensitive(false);
		imi_shuffle->set_sensitive(false);
		imi_clear->set_sensitive(false);
		imi_rmove->set_sensitive(false);
		imi_save->set_sensitive(false);
		imi_selsave->set_sensitive(false);
		plTreeView.drag_source_unset();
		plTreeView.drag_dest_unset();
		// set empty list
		gm_songList playlist;
		on_new_playlist(playlist, -1); 
	}
}

void gm_Playlist::set_songInfo (gm_songInfo *si)
{
	current_sInfo = si;
}


void gm_Playlist::get_configs()
{
	if (config->pList_fixed_columns)
	{
		// when TRUE get_width() returns 0:
		plTreeView.set_fixed_height_mode(false);
		
		config->pList_artist_width = column3.get_width();
		config->pList_title_width  = column4.get_width();
		config->pList_albumn_width = column6.get_width();
	}
}


void gm_Playlist::set_columns_fixed()
{
	if (config->pList_fixed_columns)
	{
		int width;
		column3.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		width = config->pList_artist_width;
		if (width == 0) width = 100;
		column3.set_fixed_width(width);
		column3.set_resizable(true);
		
		column4.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		width = config->pList_title_width;
		if (width == 0) width = 180;		
		column4.set_fixed_width(width);
		column4.set_resizable(true);
		
		column6.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		width = config->pList_albumn_width;
		if (width == 0) width = 140;			
		column6.set_fixed_width(width);
		column6.set_resizable(true);
		
		plTreeView.set_fixed_height_mode(true);
	}
	else
	{
		plTreeView.set_fixed_height_mode(false);
		
		column3.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE );
		column3.set_resizable(true);
		column4.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE );
		column4.set_resizable(true);
		column6.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE );
		column6.set_resizable(true);
	}
}

void gm_Playlist::set_menu_purge()
{
	if (config->pList_mark_played)
		imi_purge->show();
	else
		imi_purge->hide();
}


void gm_Playlist::set_config(gm_Config *cfg)
{
	config = cfg;
	set_columns_fixed();
	set_menu_purge();
}

void gm_Playlist::show_message(ustring message)
{
	Gtk::Dialog dlg("Attention", true);
	dlg.add_button("OK", Gtk::RESPONSE_CANCEL);
	dlg.set_default_response (Gtk::RESPONSE_CANCEL);

	Gtk::Grid content;
	content.set_row_spacing(20);
	content.set_orientation(Gtk::ORIENTATION_VERTICAL);
	Gtk::Label label("", 0.0, 0.5);
	label.set_line_wrap(true);
	label.set_use_markup(true);
	label.set_markup(message);
	dlg.get_content_area()->pack_start(content);
	content.set_border_width(10);
	content.add(label);
	content.show();
	label.show();
	dlg.set_size_request(300, -1);

	dlg.run();
}


gm_Playlist::~gm_Playlist()
{
}
