/*
 *  gm_tracks_playlist.cc
 *  GUIMUP tracks playlist view
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

#include "gm_tracks_playlist.h"
#include "gm_tracks_playlist_pix.h"

gm_Playlist::gm_Playlist()
{
	b_dragadded = false;
	b_plistbusy = false;
	b_streamplaying = false;
	current_status = -1;
	current_songNum = -1;
	current_songID = -1;
	pl_mpdCom = NULL;
	plistLength = 0;
	// get the grafix
    pxb_tr_track = Gdk::Pixbuf::create_from_inline(-1, tr_track, false);
    pxb_tr_wait = Gdk::Pixbuf::create_from_inline(-1, tr_wait, false);
    pxb_tr_play = Gdk::Pixbuf::create_from_inline(-1, tr_play, false);
    pxb_tr_stop = Gdk::Pixbuf::create_from_inline(-1, tr_stop, false);
    pxb_tr_pause = Gdk::Pixbuf::create_from_inline(-1, tr_pause, false);
    pxb_tr_trplayed = Gdk::Pixbuf::create_from_inline(-1, tr_trplayed, false);
    pxb_tr_playlist = Gdk::Pixbuf::create_from_inline(-1, tr_playlist, false);
	// scrollbars, if needed
	set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	// treeview: allow multiple selection
	theSelection = plTreeView.get_selection();
	theSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	add(plTreeView);
	
	// add the columns to the view
	Gtk::CellRendererText* pRenderer = Gtk::manage( new Gtk::CellRendererText() );
	col1_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	col1_box.pack_end(col1_label, true, true, 0);
	column1.set_widget(col1_box);
	column1.get_widget()->show_all();
	col1_label.set_text("#");
	column1.pack_start(*pRenderer);
	column1.add_attribute(pRenderer->property_text(), plColumns.col_nr);
	column1.add_attribute(pRenderer->property_foreground(), plColumns.col_color);
	column1.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE);
	plTreeView.append_column (column1);
	
	col2_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	col2_box.pack_end(col2_label, true, true, 0);
	column2.set_widget(col2_box);
	column2.get_widget()->show_all();
	col2_label.set_text(" ");
	column2.pack_start(plColumns.col_icon);
	column2.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE);
	plTreeView.append_column (column2);
	
	col3_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	col3_box.pack_end(col3_label, true, true, 0);
	column3.set_widget(col3_box);
	column3.get_widget()->show_all();
	col3_label.set_text("Artist");
	column3.pack_start(*pRenderer);
	column3.add_attribute(pRenderer->property_text(), plColumns.col_artist);
	column3.add_attribute(pRenderer->property_foreground(), plColumns.col_color);
	column3.set_resizable(true);
	plTreeView.append_column (column3);

	col4_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	col4_box.pack_end(col4_label, true, true, 0);
	column4.set_widget(col4_box);
	column4.get_widget()->show_all();
	col4_label.set_text("Title");
	column4.pack_start(*pRenderer);
	column4.add_attribute(pRenderer->property_text(), plColumns.col_title);
	column4.add_attribute(pRenderer->property_foreground(), plColumns.col_color);
	column4.set_resizable(true);
	plTreeView.append_column (column4);

	Gtk::CellRendererText* ptimeRenderer = Gtk::manage( new Gtk::CellRendererText() );
	// this is why 'time' gets a renderer of its own:
	ptimeRenderer->set_property("xalign", 1.0);
	col5_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	col5_box.pack_end(col5_label, true, true, 0);
	column5.set_widget(col5_box);
	column5.get_widget()->show_all();
	col5_label.set_text("Time");
	column5.pack_start(*ptimeRenderer);
	column5.add_attribute(ptimeRenderer->property_text(), plColumns.col_time);
	column5.add_attribute(ptimeRenderer->property_foreground(), plColumns.col_color);
	column5.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE);
	plTreeView.append_column (column5);	
	
	col6_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	col6_box.pack_end(col6_label, true, true, 0);
	column6.set_widget(col6_box);
	column6.get_widget()->show_all();
	col6_label.set_text("Album");
	column6.pack_start(*pRenderer);
	column6.add_attribute(pRenderer->property_text(), plColumns.col_album);
	column6.add_attribute(pRenderer->property_foreground(), plColumns.col_color);
	column6.set_resizable(true);
	plTreeView.append_column (column6);

	// right-click popup menu
	{ Gtk::Menu::MenuList& menulist = rClickMenu.items();
	menulist.push_back( Gtk::Menu_Helpers::MenuElem("Save list",
	  	sigc::mem_fun(*this, &gm_Playlist::save_list) ) );
	menulist.push_back( Gtk::Menu_Helpers::MenuElem("Clear list",
	  	sigc::mem_fun(*this, &gm_Playlist::clear_list) ) );
	menulist.push_back( Gtk::Menu_Helpers::MenuElem("Delete selected",
	  	sigc::mem_fun(*this, &gm_Playlist::del_selected) ) );
	menulist.push_back( Gtk::Menu_Helpers::MenuElem("Purge played",
		sigc::mem_fun(*this, &gm_Playlist::purge_list) ) );
	menulist.push_back( Gtk::Menu_Helpers::MenuElem("Shuffle",
		sigc::mem_fun(*this, &gm_Playlist::shuffle_list) ) );
	} rClickMenu.accelerate(*this);
	
	plListStore = Gtk::ListStore::create(plColumns);
	plTreeView.set_model(plListStore);
	
	// enable drag & drop
	DnDlist.push_back(Gtk::TargetEntry("STRING", Gtk::TARGET_SAME_APP,0));
    DnDlist.push_back(Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP,1));
	// simple dest mode, no source
	plTreeView.drag_dest_set(DnDlist);
	b_treeviewmode = false;

	// signal: double click on the treeview
	plTreeView.signal_row_activated().connect(sigc::mem_fun(*this,
              &gm_Playlist::on_row_activated) );
	// signal: del keypress on the treeview
	plTreeView.signal_key_press_event().connect(sigc::mem_fun(*this,
              &gm_Playlist::on_key_pressed) );
	// signal: right mouse-click on the treeview
	plTreeView.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Playlist::on_button_pressed), false);	
	// signal: drag & drop
	plTreeView.signal_drag_data_received().connect(sigc::mem_fun(*this,
              &gm_Playlist::on_pldata_received) );
	// signal: drag & drop
	plTreeView.signal_drag_data_get().connect(sigc::mem_fun(*this,
              &gm_Playlist::on_pldrag_data_get));
	
	show_all_children();
}

// called by player (via tracks) when fontsize is set
void gm_Playlist::set_listfont(ustring font)
{
	plTreeView.modify_font((Pango::FontDescription)font.data());
}


void gm_Playlist::on_pldata_received( const Glib::RefPtr<Gdk::DragContext>& context, int dragx, int dragy,
        const Gtk::SelectionData& selection_data, guint, guint time)
{
	if (b_plistbusy || pl_mpdCom == NULL)
	{
		context->drag_finish(false, false, time);
		return;
	}
	b_plistbusy = true;
	
	int droppos; // for mpd
	Gtk::TreeModel::iterator droppositer; // for the treeview 
	if (plistLength == 0)
	{
		droppos = 0;
		droppositer = plListStore->children().begin();
	}
	else
	{
		// get the drop position
		Gtk::TreeModel::Path path;
		Gtk::TreeViewDropPosition pos;
		if (plTreeView.get_dest_row_at_pos(dragx, dragy, path, pos))
		{
			Gtk::TreeModel::iterator iter = plListStore->get_iter(path);
			Gtk::TreeModel::Row row = *iter;
			int rownr = row[plColumns.col_nr];
			if (	pos == Gtk::TREE_VIEW_DROP_BEFORE
				 || pos == Gtk::TREE_VIEW_DROP_INTO_OR_BEFORE)
			{
				droppos = rownr-1;
				droppositer = iter;
			}
			else
			{
				droppos = row[plColumns.col_nr];
				if (droppos == plistLength)
					droppositer = plListStore->children().end();
				else
				{
					path.next();
					droppositer = plListStore->get_iter(path);
				}
			}
		}
		else // dropped below the list: append
		{
			droppos = plistLength;
			droppositer = plListStore->children().end();
		}
	}
	
	ustring sender = selection_data.get_data_as_string();
  	if(sender == "FromPList")
  	{
		cout << "D&d in playlist" << endl;
		gm_commandList movCommandList;
    	std::list<Gtk::TreeRowReference> rowrefsList;
		std::list<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
		std::list<Gtk::TreeModel::Path>::iterator path;
		// move(iter) invalidates other iters: store rowrefs
		for(path = selpaths.begin(); path != selpaths.end(); ++path)
			rowrefsList.push_back(Gtk::TreeRowReference(plListStore, *path));
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
		int pos = 0;
		Tr_Ch children = plListStore->children();
		for(Tr_Ch::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			Gtk::TreeModel::Row row = *iter;
			if (row[plColumns.col_nr]-1 != pos)
			{
				gm_cmdStruct aCommand;
				aCommand.cmd = CMD_MOV;
				aCommand.songid = row[plColumns.col_id];
				aCommand.moveTo = pos;
				movCommandList.push_back(aCommand);
			}
			pos++;
		}
		// generate new list numbers
		renumber_list();
		// execute last
		pl_mpdCom->execute_cmds( movCommandList, false );
		movCommandList.clear();
  	}
	
	if(sender == "FromDTree")
	{
	  	cout << "D&d from data tree to playlist" << endl;
/*  	dtCommandList is filled in the datatree (on_dtdrag_data_get)
		then signalled to tracks, which passes the list on to the
		playlist (set_cmdlist). Here we add the 'droppos' and execute
		the list.	*/
		gm_commandList dtReversedList;
		std::list<gm_cmdStruct>::iterator iter;
		int count = 0;
		for (iter = dtCommandList.begin(); iter != dtCommandList.end(); ++iter)
		{
			(*iter).moveTo = droppos;
			dtReversedList.push_front(*iter);
		}
		// dtReversedList is used to insert the items in the treeview
		for (iter = dtReversedList.begin(); iter != dtReversedList.end(); ++iter)
		{
			Gtk::TreeModel::Row row;
			row = *(plListStore->insert(droppositer));
			row[plColumns.col_icon] = pxb_tr_track;
    		row[plColumns.col_artist] = (*iter).artist + " ";
    		row[plColumns.col_album] = (*iter).album + " ";
    		row[plColumns.col_title] = (*iter).title + " ";
			if ((*iter).time < 0) // stream
			{
				row[plColumns.col_time] = "[»]  ";
				row[plColumns.col_stream] = true;
			}
			else
			{
				row[plColumns.col_time] = into_time((*iter).time) + " ";
				row[plColumns.col_stream] = false;
			}
			row[plColumns.col_color] = "#000000";
			row[plColumns.col_played] = false;
			row[plColumns.col_file] = (*iter).file;
		}
		dtReversedList.clear();
		// If there is no current song, set it
		if (plistLength == 0)
		{
			current_songNum = -1;
			current_songID = -1;
			current_status = -1;
		}
		// Get new song#s and plistLength
		renumber_list();
/* 		Set_DnDmode(), executed here, repeats the drop!
		( the list of items is actually added twice )
		So we execute_cmds 'true', which will trigger
		set_playlist(), where b_dragadded will make sure
		(only) set_DnDmode() is executed (this will also
		trigger updating the playlist statistics)	*/
		b_dragadded = true;
		// execute last
		pl_mpdCom->execute_cmds( dtCommandList, true );
		dtCommandList.clear();
		// Items are now in the mpd playlist: 
		// get the ID's so they can be moved around
		gm_songList IDlist = pl_mpdCom->get_IDlist();
		std::list<songInfo>::iterator iterID = IDlist.begin();
		Tr_Ch children = plListStore->children();
		for(Tr_Ch::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			if (iterID != IDlist.end())
			{
				Gtk::TreeModel::Row row = *iter;
				row[plColumns.col_id] = (*iterID).songID;
				iterID++;
			}
		}
		IDlist.clear();
	}
	context->drag_finish(false, false, time);
	b_plistbusy = false;
}


void gm_Playlist::on_pldrag_data_get( const Glib::RefPtr<Gdk::DragContext>&,
        Gtk::SelectionData& selection_data, guint, guint)
{
	// no real data are dragged: only a message: 'FromPList'
	selection_data.set(selection_data.get_target(), 8 /* 8 bits format */,
          (const guchar*)"FromPList", 9 /* length of FromPList in bytes */);
}


bool gm_Playlist::on_button_pressed(GdkEventButton* event)
{
	 if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
	{
		rClickMenu.popup(event->button, event->time);
		return true;  // buck stops here
	}
	else
		return false;
}


void gm_Playlist::save_list()
{
	if (pl_mpdCom == NULL)
		return;
	
	ustring input;
	
	if ( string_from_input("Please enter a name for the list", input))
	{
		bool saved = false;
		bool newtry = true;
		
		while (!saved && newtry)
		{
			saved = pl_mpdCom->save_list(input);
			if (!saved)
				newtry = string_from_input("Could not save the list list. Perhaps the name is already in use. Try again or cancel.", input);
		}
	}
}


bool gm_Playlist::string_from_input(ustring message, ustring &input)
{
	Gtk::Dialog dlg("Save Playlist", true, true);
	dlg.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	dlg.add_button("OK", Gtk::RESPONSE_OK);
	dlg.set_default_response (Gtk::RESPONSE_CANCEL);
	dlg.set_has_separator(false);
	
	Gtk::VBox vbox(false, 20);
	Gtk::Entry entry;
	Gtk::Label label("", 0.0, 0.5);
	label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	label.set_line_wrap(true);
	label.set_use_markup(true);
	label.set_markup("<b>" + message + "</b>");

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
		
		if(!input.empty())
			return true;
	}
	
	return false;	
}


void gm_Playlist::clear_list()
{
	if (pl_mpdCom == NULL || b_plistbusy)
		return;
	b_plistbusy = true;
	plListStore.clear();
	plTreeView.unset_model();	
	plListStore = Gtk::ListStore::create(plColumns);
	plTreeView.set_model(plListStore);	
	pl_mpdCom->clear_list();
	b_plistbusy = false;
}


void gm_Playlist::shuffle_list()
{
	if (pl_mpdCom == NULL || b_plistbusy)
		return;
	b_plistbusy = true;
		pl_mpdCom->shuffle_list();
	b_plistbusy = false;
}


void gm_Playlist::del_selected()
{
	if (pl_mpdCom == NULL || b_plistbusy)
		return;
	b_plistbusy = true;
	
	gm_commandList newCommandList;
    std::list<Gtk::TreeRowReference> rowrefsList;
	std::list<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
	std::list<Gtk::TreeModel::Path>::iterator path;
	// fill newCommandList & rowrefsList
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		Gtk::TreeModel::Row row = *(plListStore->get_iter(*path));
		gm_cmdStruct newCommand;
		newCommand.cmd = CMD_DEL;
		int songID = row[plColumns.col_id];
		newCommand.songid = songID;
		if (songID == current_songID)
		{
			current_songID = -1;
			current_songNum = -1;
			current_status = -1;
		}
		// highest nrs first: push_front!
		newCommandList.push_front(newCommand);
		// erase(iter) invalidates other iters: store rowrefs
		rowrefsList.push_back(Gtk::TreeRowReference(plListStore, *path));
    }
    // remove the rows from the treeview
    for (std::list<Gtk::TreeModel::RowReference>::iterator i =
                rowrefsList.begin(); i != rowrefsList.end(); i++)
    {
		Gtk::TreeModel::Path path = i->get_path();
		Gtk::TreeModel::iterator iter = plListStore->get_iter(path);
        plListStore->erase(iter);
		plistLength--;
    }		
	rowrefsList.clear();
	// get new numbers and plistLength
	renumber_list();
/*  We execute_cmds 'true', which will trigger set_playlist(),
	where b_dragadded will make sure (only) set_DnDmode() is
	executed. We could do that here, but this will also trigger
	updating the playlist statistics :)	*/
	b_dragadded = true;
	pl_mpdCom->execute_cmds( newCommandList, true );
	newCommandList.clear();
	b_plistbusy = false;
}

//renumber the plist and get new plistLength in the process
void gm_Playlist::renumber_list()
{
	plistLength = 0;
	Tr_Ch children = plListStore->children();
	for(Tr_Ch::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		plistLength++;
		Gtk::TreeModel::Row row = *iter;
		row[plColumns.col_nr] = plistLength;
	}
}


bool gm_Playlist::on_key_pressed(GdkEventKey* event)
{
	if (event->keyval == GDK_Delete)
	{
		del_selected();
	}
	return true;
}


void gm_Playlist::on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* /* column */)
{
	if (pl_mpdCom == NULL)
		return;
	
	Gtk::TreeModel::iterator iter = plListStore->get_iter(path);
	if(iter)
	{
    	Gtk::TreeModel::Row row = *iter;
    	pl_mpdCom->play_this(row[plColumns.col_nr]-1);
  	}
}

//  called from Tracks when the playlist is updated
void gm_Playlist::set_playlist(gm_songList playlist)
{
	if (b_dragadded)
	{
		set_DnDmode();
		b_dragadded = false;
		return;
	}

	//	prevent erasing the 'played' status of the songs
	if (b_streamplaying)
	{
		/* 	If the change is coming from the current ID this
			update must indeed have been triggered by the stream	*/	
		int IDchanged = -1;
		if (pl_mpdCom != NULL)
			IDchanged = pl_mpdCom->get_listChange();
		if (current_songID == IDchanged)
			return;
	}
	
	b_plistbusy = true;
	// reset the current song
	current_songID = -1;
	current_songNum = -1;
	current_status = -1;
	// reset the liststore
	if (plistLength == 0)
		plTreeView.drag_dest_unset();
	else
	{
		plTreeView.drag_source_unset();
		plTreeView.unset_rows_drag_dest();
	}
	plListStore.clear();
	plTreeView.unset_model();	
	plListStore = Gtk::ListStore::create(plColumns);
	plTreeView.set_model(plListStore);
	// fill the liststore
  	std::list<songInfo>::iterator iter;
	plistLength = 0;
	for (iter = playlist.begin(); iter != playlist.end(); ++iter)
	{
		if (!(*iter).nosong)
		{
    		Gtk::TreeModel::Row row = *(plListStore->append());
			ustring file = (*iter).file;
 			if (((file.substr(0,5)).lowercase()) == "http:")
			{
    			row[plColumns.col_artist] = "Stream ";
    			row[plColumns.col_title] = file.substr(file.rfind("/")+1, file.length()) + " ";
				row[plColumns.col_time] = "[»]  ";
				row[plColumns.col_album] = "Info in player ";
				row[plColumns.col_stream] = true;
			}
			else
			{
    			row[plColumns.col_artist] = (*iter).artist + " ";
    			row[plColumns.col_title] = (*iter).title + " ";
				row[plColumns.col_time] = into_time((*iter).time) + " ";
				row[plColumns.col_album] = (*iter).album;
				row[plColumns.col_stream] = false;
			}
			row[plColumns.col_nr] = (*iter).songNr + 1;
			row[plColumns.col_id] = (*iter).songID;
			row[plColumns.col_icon] = pxb_tr_track;
			row[plColumns.col_color] = "#000000";
			row[plColumns.col_played] = false;
			row[plColumns.col_file] = (*iter).file;
			plistLength++;
		}
	}
	playlist.clear();
	set_DnDmode();
	b_plistbusy = false;
}


void gm_Playlist::on_songchange(int songID, int status)
{
	if (songID != current_songID)
	{
		Tr_Ch children = plListStore->children();
		// if no current song, set the focus on the first
		if (songID <= 0)
		{
			Tr_Ch::iterator first = children.begin();
			if (first)
			{
				Gtk::TreeModel::Row row = *first;
				songID = row[plColumns.col_id];
			}
			else
			{	// empty list
				current_songID = -1;
				current_songNum = -1;
				current_status = -1;
				return;
			}
		}
		
		for(Tr_Ch::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			Gtk::TreeModel::Row row = *iter;
			// reset the current song
			if (row[plColumns.col_id] == current_songID)
			{
				if(row[plColumns.col_played])
				{
					row[plColumns.col_icon] = pxb_tr_trplayed;
					row[plColumns.col_color] = "#667580";
				}
				else
				{
					row[plColumns.col_icon] = pxb_tr_track;
					row[plColumns.col_color] = "#000000";
				}
			}
			
			// set the new song
			if (row[plColumns.col_id] == songID)
			{
				current_songNum = row[plColumns.col_nr] - 1;
				row[plColumns.col_color] = "#004070";
				b_streamplaying = row[plColumns.col_stream];
				switch (status)
				{
					case MPD_STATUS_STATE_PLAY:
					{
						row[plColumns.col_played] = true;
						row[plColumns.col_icon] = pxb_tr_play;
						break;
					}
					case MPD_STATUS_STATE_STOP:
					{
						row[plColumns.col_icon] = pxb_tr_stop;
						break;
					}
					case MPD_STATUS_STATE_PAUSE:
					{
						row[plColumns.col_icon] = pxb_tr_pause;
						break;
					}
					default: // MPD_STATUS_STATE_UNKNOWN
					{
						row[plColumns.col_icon] = pxb_tr_wait;
						break;
					}
				}
			}
		}
		current_songID = songID;
		current_status = status;
	}
	else // only adjust status
	if (status != current_status)
	{
		Tr_Ch children = plListStore->children();
		for(Tr_Ch::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			Gtk::TreeModel::Row row = *iter;
			// set the new status
			if (row[plColumns.col_id] == songID)
			{
				switch (status)
				{
					case MPD_STATUS_STATE_PLAY:
					{
						row[plColumns.col_played] = true;
						row[plColumns.col_icon] = pxb_tr_play;
						break;
					}
					case MPD_STATUS_STATE_STOP:
					{
						row[plColumns.col_icon] = pxb_tr_stop;
						break;
					}
					case MPD_STATUS_STATE_PAUSE:
					{
						row[plColumns.col_icon] = pxb_tr_pause;
						break;
					}
					default:
					{
						row[plColumns.col_icon] = pxb_tr_wait;
						break;
					}
				}
			}
		}
		current_status = status;
	}
}

// set a pointer to the parent's mmdCom
void gm_Playlist::set_mpdcom(tracks_mpdCom *com)
{
	pl_mpdCom = com;
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


void gm_Playlist::purge_list()
{
	if (pl_mpdCom == NULL || b_plistbusy)
		return;
	b_plistbusy = true;	
	gm_commandList newCommandList;
	theSelection->select_all();
	std::list<Gtk::TreeRowReference> rowrefsList;
	std::list<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
	std::list<Gtk::TreeModel::Path>::iterator path;
	// fill newCommandList & rowrefsList
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		Gtk::TreeModel::iterator iter = plListStore->get_iter(*path);
		Gtk::TreeModel::Row row = *iter;
		if (    row[plColumns.col_played]
			&&  row[plColumns.col_id] != (current_songID))
		{
			gm_cmdStruct newCommand;
			newCommand.cmd = CMD_DEL;
			int songID = row[plColumns.col_id];
			newCommand.songid = songID;
			// highest nrs first: push_front!
			newCommandList.push_front(newCommand);
			// erase(iter) invalidates other iters: store rowrefs
			rowrefsList.push_back(Gtk::TreeRowReference(plListStore, *path));
		}
	}
	// nex create iter from rowrefs and remove
	for (std::list<Gtk::TreeModel::RowReference>::iterator i =
				rowrefsList.begin(); i != rowrefsList.end(); i++)
	{
		Gtk::TreeModel::Path path = i->get_path();
		Gtk::TreeModel::iterator iter = plListStore->get_iter(path);
		plListStore->erase(iter);
		plistLength--;
	}		
	theSelection->unselect_all();
	rowrefsList.clear();
	// get new numbers and plistLength
	renumber_list();
/*  We execute_cmds 'true', which will trigger set_playlist(),
	where b_dragadded will make sure (only) set_DnDmode() is
	executed. We could do that here, but THIS will also trigger
	updating the playlist statistics :)	*/
	b_dragadded = true;
	pl_mpdCom->execute_cmds( newCommandList, true );
	newCommandList.clear();
	b_plistbusy = false;
}


void gm_Playlist::set_cmdlist(gm_commandList &cmdlist)
{
	dtCommandList = cmdlist;
}

/* 	Drags on an empty list result in an error when
	drag & drop is in 'treeview mode'. To work 
	around this we set d&d in 'treeview mode' with 
	enable_model_drag_dest(), or in 'simple mode'
	with drag_dest_set() when the list is empty. */
void gm_Playlist::set_DnDmode()
{
	if (!b_treeviewmode && plistLength > 0)
	{
		plTreeView.drag_dest_unset();
		// switch to treeview d&d mode
		plTreeView.enable_model_drag_dest(DnDlist);
		plTreeView.drag_source_set(DnDlist);
		plTreeView.drag_source_set_icon(pxb_tr_playlist);
		b_treeviewmode = true;
	}
	else
	if (b_treeviewmode && plistLength == 0)
	{
		plTreeView.drag_source_unset();
		plTreeView.unset_rows_drag_dest();
		// switch to simple dest mode, no source
		plTreeView.drag_dest_set(DnDlist);
		b_treeviewmode = false;
	}
	/*	plTreeView.drag_dest_set(DnDlist) in the constructor
	  	does not seem to work. So when set_playlist is called
		the first time, at startup, we must allow setting it
		here, when !b_treeviewmode plistLength is 0. It won't
		do any harm in other cases where this applies	*/
	else
	if (!b_treeviewmode && plistLength == 0)
	{
		plTreeView.drag_dest_set(DnDlist);
		b_treeviewmode = false;
	}
	else
	if (b_treeviewmode && plistLength > 0)
	{
		plTreeView.enable_model_drag_dest(DnDlist);
		plTreeView.drag_source_set(DnDlist);
		plTreeView.drag_source_set_icon(pxb_tr_playlist);
		b_treeviewmode = true;
	}
}

gm_Playlist::~gm_Playlist()
{
}
