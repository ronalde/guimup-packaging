/*
 *  gm_tracks_datatree.cc
 *  GUIMUP tracks data treeview
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

#include "gm_tracks_datatree.h"
#include "gm_tracks_datatree_pix.h"

gm_Datatree::gm_Datatree()
{
    dt_mpdCom = NULL;
    plistPath = "";
	dataModeID = -1;
	b_called_by_row_expanded = false;
    // get the grafix
    pxb_tr_track = Gdk::Pixbuf::create_from_inline(-1, tr_track, false);
    pxb_tr_album = Gdk::Pixbuf::create_from_inline(-1, tr_album, false);
    pxb_tr_artist = Gdk::Pixbuf::create_from_inline(-1, tr_artist, false);
    pxb_tr_playlist = Gdk::Pixbuf::create_from_inline(-1, tr_playlist, false);
    pxb_tr_folder = Gdk::Pixbuf::create_from_inline(-1, tr_folder, false);
	pxb_tr_check = Gdk::Pixbuf::create_from_inline(-1, tr_check, false);
	pxb_tr_redcross = Gdk::Pixbuf::create_from_inline(-1, tr_redcross, false);
	pxb_tr_trplayed = Gdk::Pixbuf::create_from_inline(-1, tr_trplayed, false);
	// scrollbars, if needed
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    // treeview: allow multiple selection
    theSelection = dtTreeView.get_selection();
    theSelection->set_mode(Gtk::SELECTION_MULTIPLE);
    // add the treeview
    add(dtTreeView);
	// setup columns
	viewcol_item.pack_start(dtColumns.col_icon, false);
	viewcol_item.pack_end(dtColumns.col_name, true);
	viewcol_item.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE);
	dtTreeView.append_column (viewcol_item);
	// column header
	col_header_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
	headerbox.pack_start(col_header_img, false, false, 0);
	headerbox.pack_end(col_header_label, true, true, 0);	
	viewcol_item.set_widget(headerbox);
	viewcol_item.get_widget()->show_all();
	// enable drag
	DnDlist.push_back(Gtk::TargetEntry("STRING", Gtk::TARGET_SAME_APP, 0));
    DnDlist.push_back(Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP, 1));
	/* normal drag source (not the enable_model_drag_source() treeview type) */
	dtTreeView.drag_source_set(DnDlist);
	dtTreeView.drag_source_set_icon(pxb_tr_playlist);
	// signals
	dtTreeView.signal_row_activated().connect(sigc::mem_fun(*this,
              &gm_Datatree::on_row_activated) );
	dtTreeView.signal_row_expanded().connect(sigc::mem_fun(*this,
              &gm_Datatree::on_row_expanded) );
	dtTreeView.signal_drag_data_get().connect(sigc::mem_fun(*this,
              &gm_Datatree::on_dtdrag_data_get));
		// signal: del keypress on the treeview
	dtTreeView.signal_key_press_event().connect(sigc::mem_fun(*this,
              &gm_Datatree::on_key_pressed) );
	
	show_all_children();
}

// called by player (via tracks) when fontsize is set
void gm_Datatree::set_listfont(ustring font)
{
	dtTreeView.modify_font((Pango::FontDescription)font.data());
}

// delete playlist(s)
bool gm_Datatree::on_key_pressed(GdkEventKey* event)
{
	if (dt_mpdCom == NULL)
		return true;
	
	if (dataModeID == ID_plist && event->keyval == GDK_Delete)
	{
		int counter = 0;
		gm_commandList newCommandList;
		std::list<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
		std::list<Gtk::TreeModel::Path>::iterator path;
		for(path = selpaths.begin(); path != selpaths.end(); ++path)
		{
			Gtk::TreeModel::Row row = *(dtTreeStore->get_iter(*path));
			listItem lit = row[dtColumns.col_listitem];
			if (lit.type == TP_PLAYLIST)
			{
				gm_cmdStruct newCommand;
				newCommand.cmd = CMD_DPL;
				newCommand.file = lit.dirpath;
				newCommandList.push_front(newCommand);
				counter++;
			}
		}
		
		if (counter > 0)
		{
			// dialog
			Gtk::MessageDialog dlg("Confirm delete", false /* use_markup */, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
  			dlg.set_secondary_text( "This will delete all selected playlists from your harddisk.");

  			int result = dlg.run();

  			switch(result)
  			{
    			case(Gtk::RESPONSE_OK):
    			{
					dt_mpdCom->execute_cmds( newCommandList, false );
					newCommandList.clear();
					get_playlists();
      				break;
    			}
    			case(Gtk::RESPONSE_CANCEL):
      				break;

				  default:
      				break;
  			}
			// end dialog
		}
	}
	return true;
}

void gm_Datatree::on_dtdrag_data_get(
        const Glib::RefPtr<Gdk::DragContext>&,
        Gtk::SelectionData& selection_data, guint, guint)
{
	// no real data are dragged: only a message: 'FromDTree'
  	selection_data.set(selection_data.get_target(), 8 /* 8 bits format */,
          (const guchar*)"FromDTree", 9 /* length of FromDTree in bytes */);		

/*  commandlist is filled with songs (listItem.file) here,
	then signalled to tracks, which passes the list on to
	thePlaylist, where we add the 'droppos' and execute in
	on_pldata_received.  */
	dtCommandList.clear();
	std::list<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
	std::list<Gtk::TreeModel::Path>::iterator path;
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		Gtk::TreeModel::Row row = *(dtTreeStore->get_iter(*path));
		listItem lit = row[dtColumns.col_listitem];
		switch (lit.type)
		{
			case TP_SONG:
			case TP_PLISTITEM:
			{
				gm_cmdStruct newCommand;
				newCommand.cmd = CMD_INS;
				newCommand.file = lit.file;
				newCommand.time = lit.time; // -1 is a stream
				newCommand.artist = lit.artist;
				newCommand.album = lit.album;
				newCommand.title = lit.title;				
				dtCommandList.push_front(newCommand);
				break;
			}
					
			case TP_ALBUM:
			{
				ustring album = lit.album;
				ustring artist = ""; 		// used in ID_album mode
				if (!lit.artist.empty())
					artist = lit.artist;	// used in ID_artist mode
				// get the songs for this album
				gm_itemList itemlist = dt_mpdCom->get_songlist(album, artist);
				
				std::list<listItem>::iterator songiter;
				for (songiter = itemlist.begin(); songiter != itemlist.end(); ++songiter)
				{
						gm_cmdStruct newCommand;
						newCommand.cmd = CMD_INS;
						newCommand.file = (*songiter).file;
						newCommand.artist = (*songiter).artist;
						newCommand.title = (*songiter).title;
						newCommand.time = (*songiter).time;
						newCommand.album = album;
						dtCommandList.push_front(newCommand);
				}
				itemlist.clear();
				break;
			}
					
			case TP_ARTIST:
			{
				ustring artist = lit.artist;
				// get the albums for this artist
				gm_itemList albumlist = dt_mpdCom->get_albumlist(artist);
				
				std::list<listItem>::iterator albiter;
				for (albiter = albumlist.begin(); albiter != albumlist.end(); ++albiter)
				{
					ustring album = (*albiter).album;
					// get the songs for this album
					gm_itemList songlist = dt_mpdCom->get_songlist(album, artist);
					std::list<listItem>::iterator songiter;
					for (songiter = songlist.begin(); songiter != songlist.end(); ++songiter)
					{
						gm_cmdStruct newCommand;
						newCommand.cmd = CMD_INS;
						newCommand.file = (*songiter).file;
						newCommand.artist = (*songiter).artist;
						newCommand.title = (*songiter).title;
						newCommand.time = (*songiter).time;
						newCommand.album = album;
						dtCommandList.push_front(newCommand);
					}
					songlist.clear();
				}
				albumlist.clear();
				break;
			}
			case TP_PLAYLIST:
			{
				// get the playlist file and read it
				if (plistPath.empty())
				{
					cout << "Could not get path for playlist" << endl;
					break;
				}
					
				ustring plistfile = plistPath + lit.dirpath;
				plistfile += ".m3u";

				std::string s_line;
				ustring file;
				std::ifstream pfile (plistfile.data());
				if (pfile.is_open())
				{
					while (! pfile.eof() )
					{
						getline (pfile, s_line);
						file = s_line;
						// skip empty lines and comments
						if (file.empty() || file.find("#") == 0)
							  continue;
						// remove tailing spaces and linefeeds
						while (file.rfind("\n") == file.length()-1 || file.rfind(" ") == file.length()-1)
							file = file.substr(0, file.length()-1);

						gm_cmdStruct newCommand;
						newCommand.cmd = CMD_INS;
						newCommand.file = file;
						// special treatment for streams
						if (((file.substr(0,5)).lowercase()) == "http:")
						{
							newCommand.artist = "Stream ";
							newCommand.title = file.substr(file.rfind("/")+1, file.length()) + " ";
							newCommand.time = -1;
							newCommand.album = "Info in player ";
						}
						else
						{
							listItem item = dt_mpdCom->get_item_from(file);
							newCommand.time = item.time;
							newCommand.artist = item.artist;
							newCommand.album = item.album;
							newCommand.title = item.title;				
						}
						dtCommandList.push_front(newCommand);
					}
					pfile.close();
				}
				else
					cout << "Could not open playlist: \"" << plistfile << "\"" << endl;
				break;
			}
			case TP_FOLDER:
			{
				get_dir_items(lit.dirpath);
				break;
			}

			default:
				break;
		}
	}
	signal_cmdList.emit(dtCommandList);
}

/*				
	This function finds items in a directory and next calls
	itself to find subitems if an item is a directory (etc).
	If a found item is a song it is added to dtCommandList.
*/
void gm_Datatree::get_dir_items(ustring dirpath)
{
	gm_itemList itemlist = dt_mpdCom->get_folderlist(dirpath);
	std::list<listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		if ( (*iter).type == TP_FOLDER )
		{
			get_dir_items((*iter).dirpath);
		}

		if ( (*iter).type == TP_SONG )
		{
			gm_cmdStruct newCommand;
			newCommand.cmd = CMD_INS;
			newCommand.file = (*iter).file;
			newCommand.time = (*iter).time;
			newCommand.artist = (*iter).artist;
			newCommand.album = (*iter).album;
			newCommand.title = (*iter).title;
			dtCommandList.push_front(newCommand);
		}
	}
	itemlist.clear();	
}

// tree root items in album mode
void gm_Datatree::get_albums(ustring artist)
{
	dataModeID = ID_album;
	
	dtTreeView.unset_model();
	dtTreeStore.clear();
	dtTreeStore = Gtk::TreeStore::create(dtColumns);
	dtTreeView.set_model(dtTreeStore);

	if (dt_mpdCom == NULL)
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" Not connected");
		return;
	}
	
    gm_itemList itemlist = dt_mpdCom->get_albumlist(artist);

	if (itemlist.empty())
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" No albums in database");
		return;
	}

	int count = 0;
  	std::list<listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *(dtTreeStore->append());
		row[dtColumns.col_icon] = pxb_tr_album;
		row[dtColumns.col_name] = (*iter).album;
		row[dtColumns.col_listitem] = (*iter);
		row[dtColumns.col_haschildren] = false;
		// add a dummy child to enable expanding
		Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
		count ++;
	}
	itemlist.clear();
	
	col_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " albums";
	col_header_label.set_text(str);
}

// tree root items in folder mode
void gm_Datatree::get_folders(ustring path)
{
	dataModeID = ID_folder;
	
	dtTreeView.unset_model();
	dtTreeStore.clear();
	dtTreeStore = Gtk::TreeStore::create(dtColumns);
	dtTreeView.set_model(dtTreeStore);

	if (dt_mpdCom == NULL)
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" Not connected");
		return;
	}
	
    gm_itemList itemlist = dt_mpdCom->get_folderlist(path);

	if (itemlist.empty())
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" No items in music folder");
		return;
	}

	int count = 0;
  	std::list<listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *(dtTreeStore->append());
		
		if ( (*iter).type == TP_FOLDER )
		{
			row[dtColumns.col_icon] = pxb_tr_folder;
			row[dtColumns.col_name] = (*iter).dirpath;
			row[dtColumns.col_listitem] = (*iter);
			row[dtColumns.col_haschildren] = false;
			// add a dummy child to enable expanding
			Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));

			count ++;
		}
		
		if ( (*iter).type == TP_SONG )
		{
			row[dtColumns.col_icon] = pxb_tr_track;
			row[dtColumns.col_name] = (*iter).file;
			row[dtColumns.col_listitem] = (*iter);
			// No need to get children
			row[dtColumns.col_haschildren] = true;
			count ++;
		}
	}
	itemlist.clear();
	
	col_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " items";
	col_header_label.set_text(str);
}


// tree root items in palylist mode
void gm_Datatree::get_playlists()
{
	dataModeID = ID_plist;
	
	dtTreeView.unset_model();
	dtTreeStore.clear();
	dtTreeStore = Gtk::TreeStore::create(dtColumns);
	dtTreeView.set_model(dtTreeStore);

	if (dt_mpdCom == NULL)
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" Not connected");
		return;
	}
	
    gm_itemList itemlist = dt_mpdCom->get_playlistlist();

	if (itemlist.empty())
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" No playslists found");
		return;
	}

	int count = 0;
  	std::list<listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *(dtTreeStore->append());
		row[dtColumns.col_icon] = pxb_tr_playlist;
		row[dtColumns.col_name] = (*iter).name;
		row[dtColumns.col_listitem] = (*iter);
		row[dtColumns.col_haschildren] = false;
		// add a dummy child to enable expanding
		Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
		count ++;
	}
	itemlist.clear();
	
	col_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " playlists";
	col_header_label.set_text(str);
}

// tree root items in artist mode
void gm_Datatree::get_artists()
{
	dataModeID = ID_artist;
	
	dtTreeView.unset_model();
	dtTreeStore.clear();
	dtTreeStore = Gtk::TreeStore::create(dtColumns);
	dtTreeView.set_model(dtTreeStore);

	if (dt_mpdCom == NULL)
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" Not connected");
		return;
	}
	
    gm_itemList itemlist = dt_mpdCom->get_artistlist();

	if (itemlist.empty())
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" No artists in database");
		return;
	}

	int count = 0;
  	std::list<listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *(dtTreeStore->append());
		row[dtColumns.col_icon] = pxb_tr_artist;
		row[dtColumns.col_name] = (*iter).artist;
		row[dtColumns.col_listitem] = (*iter);
		row[dtColumns.col_haschildren] = false;
		// add a dummy child to enable expanding
		Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
		count ++;
	}
	itemlist.clear();
	
	col_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " artists";
	col_header_label.set_text(str);
}

// set a pointer to the parent's mmdCom
void gm_Datatree::set_mpdcom(tracks_mpdCom *com)
{
    dt_mpdCom = com;
	
    if (dt_mpdCom != NULL)
        plistPath = dt_mpdCom->get_playlistPath();
}


void gm_Datatree::on_row_expanded(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path)
{
	// don't let on_row_activated() collapse what is expanded here
	b_called_by_row_expanded = true;
	// pass on to on_row_activated(): the double-click callback
	on_row_activated(path, NULL);
}

// Add subitems when items are expanded for the first time
void gm_Datatree::on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* /* column */)
{
	if (dataModeID == ID_title || dataModeID == ID_genre)
		return;
	
	Gtk::TreeModel::iterator iter = dtTreeStore->get_iter(path);
	if(iter)
	{
    	Gtk::TreeModel::Row row = *iter;
		listItem lit = row[dtColumns.col_listitem];
		int type = lit.type;
		
		if (type == TP_SONG || type == TP_PLISTITEM || type == TP_COMMENT)
			return;
		
		if (dt_mpdCom == NULL)
		{
			// don't show the dummy child
			dtTreeView.collapse_row(path);
			return;
		}
		
		// if node was expanded before
		if 	(row[dtColumns.col_haschildren])
		{
			// if a double-click called this
			if (!b_called_by_row_expanded)
			{
				if (dtTreeView.row_expanded(path))
					dtTreeView.collapse_row(path);
				else
					dtTreeView.expand_row(path, false);
			}
			else
				b_called_by_row_expanded = false;
			return;
		}
		
		b_called_by_row_expanded = false;
		
		// remove the dummy child
		Tr_Ch children = row.children();
		Tr_Ch::iterator citer = children.begin();
		if (citer)
			dtTreeStore->erase(citer);
		
		switch (type)
		{
			case TP_ARTIST:
			{
				// get the albums
				gm_itemList albumlist = dt_mpdCom->get_albumlist(lit.artist);
				// add the date to the albums
				std::list<listItem>::iterator iter;
				int count = 0;
				for (iter = albumlist.begin(); iter != albumlist.end(); ++iter)
				{
					(*iter).date = dt_mpdCom->get_date((*iter).album, lit.artist);
					count++;
				}
				// sort by date
				if (count > 1)
					sort_by_date(albumlist);
				// add items
				for (iter = albumlist.begin(); iter != albumlist.end(); ++iter)
				{
					Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
					childrow[dtColumns.col_icon] = pxb_tr_album;
					childrow[dtColumns.col_name] = (*iter).date + "  " + (*iter).album;
					childrow[dtColumns.col_listitem] = (*iter);
					childrow[dtColumns.col_haschildren] = false;
					// add a dummy child to enable expanding
					Gtk::TreeModel::Row subchildrow = *(dtTreeStore->append(childrow.children()));
				}
				albumlist.clear();
				row[dtColumns.col_haschildren] = true;
				dtTreeView.expand_row(path, false);
				break;
			}
					
			case TP_ALBUM:
			{
				if (dataModeID == ID_artist)
				{
					// get the songs fot this artist/album
					// set label without artist name
					gm_itemList itemlist = dt_mpdCom->get_songlist(lit.album, lit.artist);

					std::list<listItem>::iterator iter;
					for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
					{
						Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
						childrow[dtColumns.col_icon] = pxb_tr_track;
						ustring track = (*iter).track;
						if (track.empty())
							track = "##";
						else
						{
							// remove the x/xx format
							track =  track.substr(0, track.find("/"));
							// add leading zero, when required
							if (track.length() == 1)
								track = "0" + track;
						}
						ustring title = (*iter).title;
						if (title.empty())
							title = "???";
						childrow[dtColumns.col_name] = track + " " + title;
						childrow[dtColumns.col_listitem] = (*iter);
						childrow[dtColumns.col_haschildren] = true;
					}
					itemlist.clear();
					row[dtColumns.col_haschildren] = true;
					dtTreeView.expand_row(path, false);
					break;
				}
				if (dataModeID == ID_album)
				{
					// get the songs for this album (any artist)
					// set label WITH artist name
					gm_itemList itemlist = dt_mpdCom->get_songlist(lit.album, "");

					std::list<listItem>::iterator iter;
					for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
					{
						Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
						childrow[dtColumns.col_icon] = pxb_tr_track;
						ustring track = (*iter).track;
						if (track.empty())
							track = "##";
						else
						{
							// remove the x/xx format
							track =  track.substr(0, track.find("/"));
							// add leading zero, when required
							if (track.length() == 1)
								track = "0" + track;
						}
						ustring title = (*iter).title;
						if (title.empty())
							title = "???";
						ustring artist = (*iter).artist;
						if (artist.empty())
							artist = "???";
						childrow[dtColumns.col_name] = artist + " : " + track + " " + title;
						childrow[dtColumns.col_listitem] = (*iter);
						childrow[dtColumns.col_haschildren] = true;
					}
					itemlist.clear();
					row[dtColumns.col_haschildren] = true;
					dtTreeView.expand_row(path, false);
					break;
				}
			}
					
			case TP_PLAYLIST:
			{
				// get the playlist file and read it
				if (plistPath.empty())
				{
					cout << "Could not get path for playlist" << endl;
					break;
				}
					
				ustring plistfile = plistPath + lit.dirpath;
				plistfile += ".m3u";

				std::string s_line;
				ustring file;
				std::ifstream pfile (plistfile.data());
				if (pfile.is_open())
				{
					while (! pfile.eof() )
					{
						getline (pfile,s_line);
						file = s_line;
						// skip empty lines and comments
						if (file.empty() || file.find("#") == 0)
							  continue;
						// remove tailing spaces and linefeeds
						while (file.rfind("\n") == file.length()-1 || file.rfind(" ") == file.length()-1)
							file = file.substr(0, file.length()-1);
						Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
						childrow[dtColumns.col_icon] = pxb_tr_track;
						ustring filename = file.substr(file.rfind("/")+1, file.length());
						childrow[dtColumns.col_name] = filename;
						listItem newitem;
						newitem.type = TP_PLISTITEM;
						newitem.file = file;
						// special treatment for streams
						if (((file.substr(0,5)).lowercase()) == "http:")
						{
							newitem.artist = "Stream ";
							newitem.title = filename + " ";
							newitem.time = -1;
							newitem.album = "Info in player ";
						}
						else
						{
							listItem item = dt_mpdCom->get_item_from(file);
							newitem.time = item.time;
							newitem.artist = item.artist;
							newitem.album = item.album;
							newitem.title = item.title;
						}
						childrow[dtColumns.col_listitem] = newitem;
						childrow[dtColumns.col_haschildren] = true;
					}
					pfile.close();
				}
				else
					cout << "Could not open playlist: \"" << plistfile << "\"" << endl;
				row[dtColumns.col_haschildren] = true;
				dtTreeView.expand_row(path, false);
				break;
			}
	
			case TP_FOLDER:
			{
    			gm_itemList itemlist = dt_mpdCom->get_folderlist(lit.dirpath);
				std::list<listItem>::iterator iter;
				for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
				{
					Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
					if ( (*iter).type == TP_FOLDER )
					{
						childrow[dtColumns.col_icon] = pxb_tr_folder;
						childrow[dtColumns.col_name] = (*iter).name;
						childrow[dtColumns.col_listitem] = (*iter);
						childrow[dtColumns.col_haschildren] = false;
						// add a dummy child to enable expanding
						Gtk::TreeModel::Row subchildrow = *(dtTreeStore->append(childrow.children()));
					}
		
					if ( (*iter).type == TP_SONG )
					{
						childrow[dtColumns.col_icon] = pxb_tr_track;
						childrow[dtColumns.col_name] = (*iter).name;
						childrow[dtColumns.col_listitem] = (*iter);
						// no need to get children
						childrow[dtColumns.col_haschildren] = true;
					}
				}
				itemlist.clear();
				row[dtColumns.col_haschildren] = true;
				dtTreeView.expand_row(path, false);
				break;
			}
			default:
				break;
		}
  	}
}


void gm_Datatree::search_for(ustring findThis, int mode)
{
	
    dataModeID = mode;
	searchString = findThis; // stored for reload
	
	dtTreeView.unset_model();
	dtTreeStore.clear();
	dtTreeStore = Gtk::TreeStore::create(dtColumns);
	dtTreeView.set_model(dtTreeStore);
	
    if(dt_mpdCom != NULL && findThis.empty())
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" Nothing to search for");
		return;
	}	
	
	if (dt_mpdCom == NULL)
	{
		col_header_img.set(pxb_tr_redcross);
		col_header_label.set_text(" Not connected");
		return;
	}
	
    if (mode == ID_artist)
    {
		gm_itemList itemlist = dt_mpdCom->get_artistlist();

		if (itemlist.empty())
		{
			col_header_img.set(pxb_tr_redcross);
			col_header_label.set_text(" No artists in database");
			return;
		}

		int count = 0;
		std::list<listItem>::iterator iter;
		for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
		{
			ustring artist = (*iter).artist.casefold();
			ustring findstring = searchString.casefold();
			if (artist.find(findstring) < artist.length())
			{	
				Gtk::TreeModel::Row row = *(dtTreeStore->append());
				row[dtColumns.col_icon] = pxb_tr_artist;
				row[dtColumns.col_name] = (*iter).artist;
				row[dtColumns.col_listitem] = (*iter);
				row[dtColumns.col_haschildren] = false;
				// add a dummy child to enable expanding
				Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
				count ++;
			}
		}
		itemlist.clear();
		
		if (count > 0)
			col_header_img.set(pxb_tr_check);
		else
			col_header_img.set(pxb_tr_redcross);
		ustring str = " Search: " + into_string(count) + " artists";
		col_header_label.set_text(str);
        return;
    }

    if (mode == ID_album)
    {
		gm_itemList itemlist = dt_mpdCom->get_albumlist("");

		if (itemlist.empty())
		{
			col_header_img.set(pxb_tr_redcross);
			col_header_label.set_text(" No albums in database");
			return;
		}

		int count = 0;
		std::list<listItem>::iterator iter;
		for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
		{
			ustring album = (*iter).album.casefold();
			ustring findstring = searchString.casefold();
			if (album.find(findstring) < album.length())
			{	
				Gtk::TreeModel::Row row = *(dtTreeStore->append());
				row[dtColumns.col_icon] = pxb_tr_album;
				row[dtColumns.col_name] = (*iter).album;
				row[dtColumns.col_listitem] = (*iter);
				row[dtColumns.col_haschildren] = false;
				// add a dummy child to enable expanding
				Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
				count ++;
			}
		}
		itemlist.clear();
		
		if (count > 0)
			col_header_img.set(pxb_tr_check);
		else
			col_header_img.set(pxb_tr_redcross);
		ustring str = " Search: " + into_string(count) + " albums";
		col_header_label.set_text(str);
        return;
    }

    if (mode == ID_title)
    {
		gm_itemList itemlist = dt_mpdCom->get_songsbytitle(searchString);

		if (itemlist.empty())
		{
			col_header_img.set(pxb_tr_redcross);
			ustring str = " Search: 0 tracks";
			col_header_label.set_text(str);
			return;
		}

		int count = 0;
		std::list<listItem>::iterator iter;
		for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
		{
			Gtk::TreeModel::Row row = *(dtTreeStore->append());
			row[dtColumns.col_icon] = pxb_tr_track;
			ustring artist = (*iter).artist;
			if (artist.empty())
				artist = "???";
			ustring title = (*iter).title;
			if (title.empty())
				title = "???";
			row[dtColumns.col_name] = artist + " : " + title;
			row[dtColumns.col_listitem] = (*iter);
			row[dtColumns.col_haschildren] = true;
			count ++;
		}
		itemlist.clear();
		
		if (count > 0)
			col_header_img.set(pxb_tr_check);
		else
			col_header_img.set(pxb_tr_redcross);
		ustring str = " Search: " + into_string(count) + " tracks";
		col_header_label.set_text(str);
        return;
    }

    if (mode == ID_genre)
    {
		gm_itemList itemlist = dt_mpdCom->get_songsbygenre(searchString);

		if (itemlist.empty())
		{
			// show a list of available genres
			col_header_img.set(pxb_tr_redcross);
			ustring str = " Search: 0 tracks";
			col_header_label.set_text(str);
			gm_itemList genrelist = dt_mpdCom->get_genrelist();
			if (genrelist.empty())
				return;

			Gtk::TreeModel::Row row = *(dtTreeStore->append());
			listItem gItem;
			gItem.type = TP_COMMENT;
			row[dtColumns.col_listitem] = gItem;
			row[dtColumns.col_icon] = pxb_tr_check;
			row[dtColumns.col_haschildren] = true;
			row[dtColumns.col_name] = "Available genres:";
			
			std::list<listItem>::iterator iter;
			for (iter = genrelist.begin(); iter != genrelist.end(); ++iter)
			{
				Gtk::TreeModel::Row childrow = *(dtTreeStore->append(row.children()));
				childrow[dtColumns.col_name] = (*iter).name;
				childrow[dtColumns.col_icon] = pxb_tr_trplayed;
				childrow[dtColumns.col_listitem] = (*iter);
				childrow[dtColumns.col_haschildren] = true;
			}
			genrelist.clear();
			dtTreeView.expand_all();
			return;
		}

		int count = 0;
		std::list<listItem>::iterator iter;
		for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
		{
			Gtk::TreeModel::Row row = *(dtTreeStore->append());
			row[dtColumns.col_icon] = pxb_tr_track;
			ustring artist = (*iter).artist;
			if (artist.empty())
				artist = "???";
			ustring title = (*iter).title;
			if (title.empty())
				title = "???";
			row[dtColumns.col_name] = artist + " : " + title;
			row[dtColumns.col_listitem] = (*iter);
			row[dtColumns.col_haschildren] = true;
			count ++;
		}
		itemlist.clear();
		
		if (count > 0)
			col_header_img.set(pxb_tr_check);
		else
			col_header_img.set(pxb_tr_redcross);
		ustring str = " Search: " + into_string(count) + " tracks";
		col_header_label.set_text(str);
        return;
	}
}

// bubblesort the list by date
void gm_Datatree::sort_by_date(gm_itemList &sortlist)
{
	listItem swap;
	std::list<listItem>::iterator it_first;
	for (it_first = sortlist.begin(); it_first != (sortlist.end())--; ++it_first)
    {
		std::list<listItem>::iterator it_next;
		for (it_next = sortlist.begin()++; it_next != sortlist.end(); ++it_next)
		{
			if ((*it_next).date > (*it_first).date)
			{
				swap = *it_first;
				*it_first = *it_next;
				*it_next = swap;
			}
		}
	}	
}


std::string gm_Datatree::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}


gm_Datatree::~gm_Datatree()
{

}
