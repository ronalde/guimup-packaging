/*
 *  gm_libraryview.cc
 *  GUIMUP library-view
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

#include "gm_libraryview.h"
#include "gm_libraryview_pix.h"

gm_libraryView::gm_libraryView()
{
	b_connected = false;
	mpdCom = NULL;
	libModeID = 0;
	plist_droppos = 0;
	b_called_by_activated = false;
	b_called_by_clapsxpand = false;
    // get the grafix
    pxb_tr_track = Gdk::Pixbuf::create_from_inline(-1, tr_track, false);
    pxb_tr_album = Gdk::Pixbuf::create_from_inline(-1, tr_album, false);
    pxb_tr_artist = Gdk::Pixbuf::create_from_inline(-1, tr_artist, false);
    pxb_tr_playlist = Gdk::Pixbuf::create_from_inline(-1, tr_playlist, false);
    pxb_tr_folder = Gdk::Pixbuf::create_from_inline(-1, tr_folder, false);
	pxb_tr_check = Gdk::Pixbuf::create_from_inline(-1, tr_check, false);
	pxb_tr_redcross = Gdk::Pixbuf::create_from_inline(-1, tr_redcross, false);
	pxb_tr_track_played = Gdk::Pixbuf::create_from_inline(-1, tr_track_played, false);
	pxb_tr_trackx_played = Gdk::Pixbuf::create_from_inline(-1, tr_trackx_played, false);
	pxb_tr_stream_played = Gdk::Pixbuf::create_from_inline(-1, tr_stream_played, false);
	pxb_tr_genre = Gdk::Pixbuf::create_from_inline(-1, tr_genre, false);
	pxb_drag_playlist = Gdk::Pixbuf::create_from_inline(-1, drag_playlist, false);
	// context menu
	create_rClickmenu();
	// scrollbars
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	// the treeview
    add(libTreeView);
	libTreeView.set_rules_hint(true);
    theSelection = libTreeView.get_selection();
    theSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	
	// setup columns. See also set_config()
	// column0
	column0.pack_start(libCols.col0_icon, false);
	column0.set_resizable(true);
	Gtk::CellRendererText *pRenderer = Gtk::manage( new Gtk::CellRendererText() );
	column0.pack_start(*pRenderer);
	column0.add_attribute(pRenderer->property_text(), libCols.col0_Info);
	column0.add_attribute(pRenderer->property_foreground(), libCols.col0_color);
	libTreeView.append_column (column0);
	// header
	col0_headerbox.attach(col0_header_img, 0,0,1,1);
	col0_headerbox.attach(col0_header_label,1,0,1,1);	
	column0.set_widget(col0_headerbox);
	column0.get_widget()->show_all();
	// column1
	column1.set_resizable(true);
	column1.pack_start(*pRenderer);
	column1.add_attribute(pRenderer->property_text(), libCols.col1_Info);
	libTreeView.append_column (column1);
	// header
	column1.set_widget(col1_header_label);
	column1.get_widget()->show_all();
	
	// send the mouse offset to the treeview	
	int y_offset = pRenderer->property_rise();
	int height = pRenderer->property_size(); 
	mouse_offset = y_offset + height + 6;// add 6 for header borders
	libTreeView.set_mouse_offset(mouse_offset);
	
	// enable normal drag source (not the enable_model_drag_source() treeview type)
	DnDlist.push_back(Gtk::TargetEntry("STRING", Gtk::TARGET_SAME_APP, 0));
    DnDlist.push_back(Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP, 1));
	
	// signals
	libTreeView.signal_row_activated().connect(sigc::mem_fun(*this, &gm_libraryView::on_row_activated) );
	libTreeView.signal_row_expanded().connect(sigc::mem_fun(*this, &gm_libraryView::on_row_expanded) );
	libTreeView.signal_row_collapsed().connect(sigc::mem_fun(*this, &gm_libraryView::on_row_collapsed) );
	libTreeView.signal_drag_data_get().connect(sigc::mem_fun(*this, &gm_libraryView::on_dtdrag_data_get));
	// menu popup
	libTreeView.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_libraryView::on_button_pressed), false);
	// "smart select":
	theSelection->signal_changed().connect(sigc::mem_fun(*this, &gm_libraryView::on_selection_changed) );
	
	show_all_children();
}


void gm_libraryView::on_dtdrag_data_get(const Glib::RefPtr<Gdk::DragContext>&,
													Gtk::SelectionData& selection_data, guint, guint)
{
	if (!b_connected)
		return;
	// Drag only a message: 'FromLiBry'
  	selection_data.set(selection_data.get_target(), 8, (const guchar*)"FromLiBry", 9);	
}


void gm_libraryView::set_mpdcom(gm_mpdCom* com)
{
	mpdCom = com;
}


void gm_libraryView::set_connected(bool iscon)
{
	b_connected = iscon;
	if (b_connected)
	{
		libTreeView.drag_source_set(DnDlist);
		libTreeView.drag_source_set_icon(pxb_drag_playlist);
		
		/*if(config->mpd_rm_allowed) // FIXME ??
			imi_delplist->set_sensitive(true);
		else
			imi_delplist->set_sensitive(false);
		
		imi_newplist->set_sensitive(true);
		imi_append->set_sensitive(true);*/
		
		on_selection_changed();
	}
	else
	{
		/*imi_delplist->set_sensitive(false);
		imi_newplist->set_sensitive(false);
		imi_append->set_sensitive(false);*/
		libTreeView.drag_source_unset();
	}
	
	on_selection_changed();
}


void gm_libraryView::set_listfont(Pango::FontDescription pFont)
{
	libTreeView.override_font(pFont);
}

// 1st level items in genre mode
void gm_libraryView::get_genres(ustring searchfor)
{
	libModeID = LB_ID_GENRE;

	libTreeView.unset_model();
	libTreeStore.clear();
	libTreeStore = Gtk::TreeStore::create(libCols);
	libTreeView.set_model(libTreeStore);
	
	if (column1.get_visible())
		column1.set_visible(false);
		
	if (!b_connected)
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" Not connected ");
		return;
	}

	gm_itemList itemlist = mpdCom->get_genrelist(searchfor);

	if (itemlist.empty())
	{
		col0_header_img.set(pxb_tr_redcross);
		if (config->browser_LibraryMode = LB_ID_SEARCH)
			col0_header_label.set_text(" No genres found ");
		else
			col0_header_label.set_text(" No genres in database ");
		return;
	}
	else
		itemlist.sort();				  

    ustring genre = "~0~";
	int count = 0;
	libTreeView.unset_model();
  	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		// skip duplicates!
		if (genre == iter->genre)
           continue;
            
        genre = iter->genre;
		
		tm_Row row = *(libTreeStore->append());
		row[libCols.col0_icon] = pxb_tr_genre;
		if (iter->genre.empty())
			row[libCols.col0_Info] = "?";			
		else
			row[libCols.col0_Info] = iter->genre;
		row[libCols.col_listitem] = *iter;
		row[libCols.col_haschildren] = false;
		// add a dummy child
		libTreeStore->append(row.children());
		count ++;
	}
	libTreeView.set_model(libTreeStore);
	itemlist.clear();
	
	col0_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " Genres";
	col0_header_label.set_text(str);
}

// 1st level items in artist mode
void gm_libraryView::get_artists(ustring searchfor)
{
	libModeID = LB_ID_ARTIST;
	
	libTreeView.unset_model();
	libTreeStore.clear();
	libTreeStore = Gtk::TreeStore::create(libCols);
	libTreeView.set_model(libTreeStore);
	
	if (column1.get_visible())
		column1.set_visible(false);
		
	if (!b_connected)
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" Not connected ");
		return;
	}

	gm_itemList itemlist;
	if (searchfor == "~0~")
        itemlist = mpdCom->get_artistlist();
    else
        itemlist = mpdCom->get_artist_searchlist(searchfor);

	if (itemlist.empty())
	{
		col0_header_img.set(pxb_tr_redcross);
		if (config->browser_LibraryMode = LB_ID_SEARCH)
			col0_header_label.set_text(" No artists found ");
		else
			col0_header_label.set_text(" No artists in database ");
		return;
	}
	else
		itemlist.sort();				  

    ustring artist = "~0~";
	int count = 0;
	libTreeView.unset_model();
  	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		// skip duplicates!
		if (artist == iter->artist)
           continue;
            
        artist = iter->artist;
		
		tm_Row row = *(libTreeStore->append());
		row[libCols.col0_icon] = pxb_tr_artist;
		if (iter->artist.empty())
			row[libCols.col0_Info] = "?";			
		else
			row[libCols.col0_Info] = iter->artist;
		row[libCols.col_listitem] = *iter;
		row[libCols.col_haschildren] = false;
		// add a dummy child
		libTreeStore->append(row.children());
		count ++;
	}
	libTreeView.set_model(libTreeStore);
	itemlist.clear();
	
	col0_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " Artists";
	col0_header_label.set_text(str);
}

// 1st level items in year mode
void gm_libraryView::get_albums_year()
{
	libModeID = LB_ID_BYEAR;
	
	libTreeView.unset_model();
	libTreeStore.clear();
	libTreeStore = Gtk::TreeStore::create(libCols);
	libTreeView.set_model(libTreeStore);
	
	if (!column1.get_visible())
		column1.set_visible(true);
	
	if (!b_connected)
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" Not connected ");
		return;
	}

	gm_itemList itemlist = mpdCom->get_yearlist();

	if (itemlist.empty())
	{
		col0_header_img.set(pxb_tr_redcross);
		if (config->browser_LibraryMode = LB_ID_SEARCH)
			col0_header_label.set_text(" No albums found ");
		else
			col0_header_label.set_text(" No albums in database ");
		return;
	}
	else
		itemlist.sort();				  

    ustring artist = "~0~";
	ustring album =  "~0~";
	int count = 0;
	libTreeView.unset_model();
  	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		// skip duplicates!
		if (artist == iter->artist && album == iter->album)
           continue;
            
        artist = iter->artist;
		album = iter->album;
		
		tm_Row row = *(libTreeStore->append());
		row[libCols.col0_icon] = pxb_tr_album;
		ustring yr = "   ?   ";
		if (!iter->year.empty())
			yr = iter->year + "  ";
			
		if (iter->album.empty())
			row[libCols.col0_Info] = yr + "?";			
		else
			row[libCols.col0_Info] = yr + iter->album;

		if (iter->artist.empty())
			row[libCols.col1_Info] = "?";			
		else
			row[libCols.col1_Info] = iter->artist;
		
		row[libCols.col_listitem] = *iter;
		row[libCols.col_haschildren] = false;
		// add a dummy child
		libTreeStore->append(row.children());
		count ++;
	}
	libTreeView.set_model(libTreeStore);
	itemlist.clear();
	
	col0_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " Albums";
	col0_header_label.set_text(str);
	col1_header_label.set_text(" Artist");
}

// 1st level items in playlist mode
void gm_libraryView::get_playlists()
{
	libModeID = LB_ID_PLIST;
	
	libTreeView.unset_model();
	libTreeStore.clear();
	libTreeStore = Gtk::TreeStore::create(libCols);
	libTreeView.set_model(libTreeStore);

	if (column1.get_visible())
		column1.set_visible(false);
	
	if (!b_connected)
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" Not connected ");
		return;
	}
	
    gm_itemList itemlist = mpdCom->get_playlistlist();
	
	if (itemlist.empty())
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" No playslists found ");
		return;
	}
	else
		itemlist.sort();

	int count = 0;
	libTreeView.unset_model();
  	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		tm_Row row = *(libTreeStore->append());
		row[libCols.col0_icon] = pxb_tr_playlist;
		row[libCols.col0_Info] = iter->name;
		row[libCols.col_listitem] = *iter;
		row[libCols.col_haschildren] = false;
		// add a dummy child
		libTreeStore->append(row.children());
		count ++;
	}
	libTreeView.set_model(libTreeStore);
	itemlist.clear();
	
	col0_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " Playlists";
	col0_header_label.set_text(str);
}

// tree root items in folder mode
void gm_libraryView::get_folders(ustring path)
{
	libModeID = LB_ID_FOLDER;
	
	libTreeView.unset_model();
	libTreeStore.clear();
	libTreeStore = Gtk::TreeStore::create(libCols);
	libTreeView.set_model(libTreeStore);

	if (column1.get_visible())
		column1.set_visible(false);
		
	if (!b_connected)
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" Not connected ");
		return;
	}

	
    gm_itemList itemlist = mpdCom->get_folderlist(path);

	if (itemlist.empty())
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" No items in music folder ");
		return;
	}
	else
		itemlist.sort();	
	
	gm_itemList dirs_first_list;
  	std::list<gm_listItem>::iterator riter;
	for (riter = itemlist.begin(); riter != itemlist.end(); ++riter)
	{
		if (riter->type == TP_FOLDER)
			dirs_first_list.push_back(*riter); // dirs first
	}
	for (riter = itemlist.begin(); riter != itemlist.end(); ++riter)
	{
		if (riter->type != TP_FOLDER)
			dirs_first_list.push_back(*riter); // non-dirs next
	}
	
	itemlist.clear();
	
	int count = 0;
	libTreeView.unset_model();
  	std::list<gm_listItem>::iterator iter;
	for (iter = dirs_first_list.begin(); iter != dirs_first_list.end(); ++iter)
	{
		tm_Row row = *(libTreeStore->append());
		
		if ( iter->type == TP_FOLDER )
		{
			row[libCols.col0_icon] = pxb_tr_folder;
			row[libCols.col0_Info] = iter->name;
			row[libCols.col_listitem] = *iter;
			row[libCols.col_haschildren] = false;
			// add a dummy child
			libTreeStore->append(row.children());
			count ++;
		}
		
		if ( iter->type == TP_SONG )
		{
			row[libCols.col0_icon] = pxb_tr_track;
			row[libCols.col0_Info] = iter->name;
			row[libCols.col_listitem] = *iter;
			// No need to get children
			row[libCols.col_haschildren] = true;
			count ++;
		}
	}
	libTreeView.set_model(libTreeStore);
	dirs_first_list.clear();
		
	col0_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) +" Folders" ;
	col0_header_label.set_text(str);
}


std::string gm_libraryView::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}

void gm_libraryView::search_for(ustring searchfor, int search_mode)
{
	switch (search_mode)
	{
		case SR_ID_ARTIST:
		{
			get_artists(searchfor);
			break;
		}
		case SR_ID_ALBUM:
		{
			get_albums(searchfor, false);
 			break;
		}
		case SR_ID_TITLE:
		{
			get_songs(searchfor);
			break;
		}
		case SR_ID_GENRE:
		{
			get_genres(searchfor);
			break;
		}
		default:
			break;
	} // <- switch
}

void gm_libraryView::get_songs(ustring searchfor)
{
	libModeID = LB_ID_SONGS;
	
	libTreeView.unset_model();
	libTreeStore.clear();
	libTreeStore = Gtk::TreeStore::create(libCols);
	libTreeView.set_model(libTreeStore);
	
	if (!column1.get_visible())
		column1.set_visible(true);
		
	if (!b_connected)
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" Not connected ");
		return;
	}

	gm_itemList itemlist = mpdCom->get_song_searchlist(searchfor);

	if (itemlist.empty())
	{
		col0_header_img.set(pxb_tr_redcross);
		if (config->browser_LibraryMode = LB_ID_SEARCH)
			col0_header_label.set_text(" No titles found ");
		else
			col0_header_label.set_text(" No songs in database ");
		return;
	}
	else
		itemlist.sort();
	
	int count = 0;
	libTreeView.unset_model();	// see footnote
  	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		tm_Row row = *(libTreeStore->append()); 
				row[libCols.col0_icon] = pxb_tr_track;
		if (iter->title.empty())
			row[libCols.col0_Info] = iter->name;
		else
			row[libCols.col0_Info] = iter->title;
		if (iter->artist.empty())
			row[libCols.col1_Info] = "?";
		else
			row[libCols.col1_Info] = iter->artist;		
		row[libCols.col_listitem] = (*iter);
		row[libCols.col_haschildren] = true;
		count ++;
	}
	libTreeView.set_model(libTreeStore);
	itemlist.clear();

	col0_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " Songs";
	col0_header_label.set_text(str);
	col1_header_label.set_text(" Artist");
	
}


void gm_libraryView::on_row_collapsed(const tm_Iterator& iter, const tm_Path& path)
{
	if (b_called_by_activated)
		b_called_by_activated = false;
}


void gm_libraryView::on_row_expanded(const tm_Iterator& iter, const tm_Path& path)
{
	if (!b_called_by_activated)
	{
		// pass on to the double-click callback
		b_called_by_clapsxpand = true;	
		on_row_activated(path, NULL);
	}
	else
		// reset 
		b_called_by_activated = false;
	
	b_called_by_clapsxpand = false;
}

// expand or collapse (and add subitems when items are expanded for the 1st time)
void gm_libraryView::on_row_activated(const tm_Path& path, Gtk::TreeViewColumn* /*column*/ )
{
	if (libModeID == LB_ID_SONGS)
		return;
	
	tm_Iterator iter = libTreeStore->get_iter(path);
	if(iter)
	{
    	tm_Row row = *iter;
		gm_listItem litem = row[libCols.col_listitem];
		int type = litem.type;
		
		if (type == TP_SONG || type == TP_PLISTITEM)
			return;
		
		if (!b_connected)
		{
			// don't show the dummy
			libTreeView.collapse_row(path);
			return;
		}
		
		// if node was expanded before
		if 	(row[libCols.col_haschildren])
		{
			// if a double-click called this
			if (!b_called_by_clapsxpand)
			{
				if (libTreeView.row_expanded(path))
				{
					b_called_by_activated = true;
					libTreeView.collapse_row(path);
				}
				else
				{
					b_called_by_activated = true;
					libTreeView.expand_row(path, false);
				}
			}
			return;
		}
		
		// remove dummy child
		tm_Children children = row.children();
		tm_Children::iterator citer = children.begin();
		if (citer)
			libTreeStore->erase(citer);
		
		switch (type)
		{
			case TP_ARTIST:
			{
				if (expand_artist(row))
				{
					b_called_by_activated = true;
					libTreeView.expand_row(path, false);
				}
				break;
			}
					
			case TP_ALBUM:
			{
				if (expand_album(row))
				{
					b_called_by_activated = true;
					libTreeView.expand_row(path, false);
				}
				break;
			}
					
			case TP_PLAYLIST:
			{
				if (expand_playlist(row))
				{
					b_called_by_activated = true;				
					libTreeView.expand_row(path, false);
				}
				break;
			}
				
			case TP_GENRE:
			{
				if (expand_genre(row))
				{
					b_called_by_activated = true;
					libTreeView.expand_row(path, false);
				}
				break;
			}
				
			case TP_FOLDER:
			{
				if (expand_folder(row))
				{
					b_called_by_activated = true;
					libTreeView.expand_row(path, false);
				}
				break;
			}
			default:
				break;
		}
  	}
}


bool gm_libraryView::expand_playlist(tm_Row row)
{
	gm_listItem litem = row[libCols.col_listitem];

    gm_itemList itemlist = mpdCom->get_playlist_itemlist(litem.file);
  
    if (itemlist.empty())
	{
		row[libCols.col_haschildren] = true;
        return false;
	}
 
	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		tm_Row childrow = *(libTreeStore->append(row.children()));
		
		childrow[libCols.col0_color] = "#667580";
		
		if (iter->type == TP_NOITEM) // item is an U.P.O.
		{
			childrow[libCols.col0_icon] = pxb_tr_redcross;
			childrow[libCols.col0_Info] = iter->title;
		}
		if (iter->type == TP_STREAM)
		{
			childrow[libCols.col0_icon] = pxb_tr_stream_played;
			childrow[libCols.col0_Info] = iter->file;
		}
		else
		{
			if (iter->type == TP_SONGX)
				childrow[libCols.col0_icon] = pxb_tr_trackx_played;
			else
				childrow[libCols.col0_icon] = pxb_tr_track_played;
			ustring artist = "?";
			if (!iter->artist.empty())
				artist = iter->artist;
			if (!iter->title.empty())
				childrow[libCols.col0_Info] = artist + "  :  " + iter->title;
			else
				childrow[libCols.col0_Info] = iter->name;
		}
		iter->type = TP_PLISTITEM; // override type
		childrow[libCols.col_listitem] = (*iter);
	}	
	itemlist.clear();
	row[libCols.col_haschildren] = true;
	return true;
}


bool gm_libraryView::expand_genre(tm_Row row)
{
	gm_listItem litem = row[libCols.col_listitem];
	
	gm_itemList itemlist = mpdCom->get_genre_artistlist(litem.genre);
	
    if (itemlist.empty())
	{
		row[libCols.col_haschildren] = true;
        return false;
	}

    itemlist.sort();

	/* we add albums (as children) to the artists here, because the albums
	must match the genre and have already been selected accordingly */

	tm_Row artist_childrow;
	tm_Row album_childrow;	

    ustring current_artist = "~0~";
    ustring current_album = "~0~";
    std::list<gm_listItem>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if (iter->artist != current_artist)
        {
            // add artist
			current_artist = iter->artist;
			artist_childrow = *(libTreeStore->append(row.children()));
 			artist_childrow[libCols.col0_icon] = pxb_tr_artist;
			if (iter->artist.empty())
				artist_childrow[libCols.col0_Info] = "?";			
			else
				artist_childrow[libCols.col0_Info] = iter->artist;
			artist_childrow[libCols.col_listitem] = *iter;
			artist_childrow[libCols.col_haschildren] = true; // are added NOW:  

            // add first album to artist
			current_album = iter->album;
			album_childrow = *(libTreeStore->append(artist_childrow.children()));
			album_childrow[libCols.col0_icon] = pxb_tr_album;
			if (iter->album.empty())
				album_childrow[libCols.col0_Info] = "?";			
			else
				album_childrow[libCols.col0_Info] = iter->album;
			iter->type = TP_ALBUM;
			album_childrow[libCols.col_listitem] = *iter;
			album_childrow[libCols.col_haschildren] = false;
			// add a dummy child
			libTreeStore->append(album_childrow.children());
        }
        else // add more albums to artist
        {
            if (iter->album != current_album)
            {
				current_album = iter->album;
				album_childrow = *(libTreeStore->append(artist_childrow.children()));
				album_childrow[libCols.col0_icon] = pxb_tr_album;
				if (iter->album.empty())
					album_childrow[libCols.col0_Info] = "?";			
				else
					album_childrow[libCols.col0_Info] = iter->album;
				iter->type = TP_ALBUM;
				album_childrow[libCols.col_listitem] = *iter;
				album_childrow[libCols.col_haschildren] = false;  
				// add a dummy child
				libTreeStore->append(album_childrow.children());
            }
        }
    }
    itemlist.clear();
	row[libCols.col_haschildren] = true;
	return true;
}


bool gm_libraryView::expand_album(tm_Row row)
{
	gm_itemList itemlist;
	gm_listItem litem = row[libCols.col_listitem];

	if ( libModeID == LB_ID_GENRE)
        itemlist = mpdCom->get_songlist(litem.album, litem.artist, litem.genre);
    else
        itemlist = mpdCom->get_songlist(litem.album, litem.artist, "~0~");

	if (itemlist.empty())
	{
		row[libCols.col_haschildren] = true;
        return false;
	}

	itemlist.sort();
	
	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		tm_Row childrow = *(libTreeStore->append(row.children()));
		childrow[libCols.col0_icon] = pxb_tr_track;
		ustring track = "?";
		if (!iter->track.empty())
			track = iter->track;
		if (!iter->title.empty())
			childrow[libCols.col0_Info] = track + "  " + iter->title;
		else
			childrow[libCols.col0_Info] = iter->name;
		childrow[libCols.col_listitem] = (*iter);
		childrow[libCols.col_haschildren] = true;
	}
	
	itemlist.clear();
	row[libCols.col_haschildren] = true;
	return true;
}


bool gm_libraryView::expand_artist(tm_Row row)
{
	gm_listItem litem = row[libCols.col_listitem];
	
	gm_itemList itemlist = mpdCom->get_albums_for_artist(litem.artist);
	
	if (itemlist.empty())
	{
		row[libCols.col_haschildren] = true;
        return false;
	}

	std::list<gm_listItem>::iterator iter;
	// add the date to the albums 
	if (config->lib_sort_albums_byear)
	{
		int count = 0;
		for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
		{
			iter->year = mpdCom->get_album_year(iter->album, litem.artist);
			// sort (below) by date, next by title
			iter->sorter = iter->year + iter->album;
			count++;
		}
	}

	itemlist.sort();
	
	// add items
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		tm_Row childrow = *(libTreeStore->append(row.children()));
		childrow[libCols.col0_icon] = pxb_tr_album;
		if (config->lib_sort_albums_byear)
		{
			ustring yr = "   ?   ";
			if (!iter->year.empty())
				yr = iter->year + "  ";
			if (iter->album.empty())
				childrow[libCols.col0_Info] = yr + "?";			
			else
				childrow[libCols.col0_Info] = yr + iter->album;
		}
		else
		{
			if (iter->album.empty())
				childrow[libCols.col0_Info] = "?";
			else
				childrow[libCols.col0_Info] = iter->album;
		}
		childrow[libCols.col_listitem] = (*iter);
		childrow[libCols.col_haschildren] = false;
		// add a dummy child
		libTreeStore->append(childrow.children());
	}
	itemlist.clear();
	row[libCols.col_haschildren] = true;
	return true;
}


bool gm_libraryView::expand_folder(tm_Row row)
{
	gm_listItem litem = row[libCols.col_listitem];
	gm_itemList itemlist = mpdCom->get_folderlist(litem.file);
	
	if (itemlist.empty())
	{
		row[libCols.col_haschildren] = true;
        return false;
	}
	
	itemlist.sort();

	gm_itemList dirs_first_list;
  	std::list<gm_listItem>::iterator riter;
	for (riter = itemlist.begin(); riter != itemlist.end(); ++riter)
	{
		if (riter->type == TP_FOLDER)
			dirs_first_list.push_back(*riter); // dirs first
	}
	for (riter = itemlist.begin(); riter != itemlist.end(); ++riter)
	{
		if (riter->type != TP_FOLDER)
			dirs_first_list.push_back(*riter); // non-dirs after the dirs
	}
	
	itemlist.clear();
	std::list<gm_listItem>::iterator iter;
	for (iter = dirs_first_list.begin(); iter != dirs_first_list.end(); ++iter)
	{
		tm_Row childrow = *(libTreeStore->append(row.children()));
		if ( iter->type == TP_FOLDER )
		{
			childrow[libCols.col0_icon] = pxb_tr_folder;
			childrow[libCols.col0_Info] = iter->name;
			childrow[libCols.col_listitem] = (*iter);
			childrow[libCols.col_haschildren] = false;
			// add a dummy child
			libTreeStore->append(childrow.children());
		}
		if ( iter->type == TP_SONG )
		{
			childrow[libCols.col0_icon] = pxb_tr_track;
			childrow[libCols.col0_Info] = iter->name;
			childrow[libCols.col_listitem] = (*iter);
			// no need to get children
			childrow[libCols.col_haschildren] = true;
		}
	}
	dirs_first_list.clear();
	row[libCols.col_haschildren] = true;
	return true;
}


bool gm_libraryView::on_button_pressed(GdkEventButton* event)
{
	if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
	{
		rClickMenu->popup(event->button, event->time);
		return true; 
	}
	else
		return false; // pass on
}

/*  
	SMART SELECT: selecting an item deselects its selected 
    children recursively.
*/
void gm_libraryView::on_selection_changed()
{
	int count = theSelection->count_selected_rows();

	if (count == 0)
	{
		imi_delplist->set_sensitive(false);
		imi_newplist->set_sensitive(false);
		imi_append->set_sensitive(false);
		imi_fileman->set_sensitive(false);
		imi_tagedit->set_sensitive(false);
		imi_reload->set_sensitive(false);	
		return;
	}
	else
	{
		if (libModeID == LB_ID_PLIST && config->mpd_rm_allowed)
			imi_delplist->set_sensitive(true);
		else
			imi_delplist->set_sensitive(false);	
		imi_append->set_sensitive(true);
		imi_newplist->set_sensitive(true);
	}

	if ( count == 1 && config->mpd_local_features)
	{
		std::vector<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
		std::vector<Gtk::TreeModel::Path>::iterator path = selpaths.begin();
		Gtk::TreeModel::Row row = *(libTreeStore->get_iter(*path));
		gm_listItem lit = row[libCols.col_listitem];
	
		if ((lit.type == TP_SONG || lit.type == TP_FOLDER || lit.type == TP_PLAYLIST) && !(config->File_Manager).empty())
			imi_fileman->set_sensitive(true);
		else
			imi_fileman->set_sensitive(false);


		if (lit.type == TP_SONG || lit.type == TP_FOLDER)
		{
			if(!(config->Tag_Editor).empty() )
				imi_tagedit->set_sensitive(true);
			else
				imi_tagedit->set_sensitive(false);

			if (!b_connected  || (!config->mpd_rescan_allowed && !config->mpd_update_allowed))
				imi_reload->set_sensitive(false);
			else
				imi_reload->set_sensitive(true);
		}

		return;
	}
	else
	{
		imi_fileman->set_sensitive(false);
		imi_tagedit->set_sensitive(false);
		imi_reload->set_sensitive(false);
	}
	
	
	if ( count == 1 ) // no need for smart select
		return;

	std::vector<tm_Path> selpaths = theSelection->get_selected_rows();
	
	std::vector<tm_Path>::iterator path;
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		tm_Iterator iter = libTreeStore->get_iter(*path);
		if (iter)
		{
			gm_listItem litem = (*iter)[libCols.col_listitem];
			if (litem.type == TP_PLISTITEM)
			{
				// hack to unselect the last selected item
				if (count == 1)
					theSelection->unselect_all();
				else;
					theSelection->unselect(*iter);
			}
			else
				deselect_children(iter);
		}
    }
}


void gm_libraryView::deselect_children(tm_Children::iterator iter)
{
	tm_Row row = *iter;
	if 	(row[libCols.col_haschildren])
	{
		tm_Children children = row.children();
		tm_Children::iterator citer;
		for (citer =  children.begin(); citer != children.end(); ++citer)
		{
			if (theSelection->is_selected(*citer))
				theSelection->unselect(*citer);
			deselect_children(citer);
		}
		if (citer)
			libTreeStore->erase(citer);
	}
}

// tree root items in album mode
void gm_libraryView::get_albums(ustring searchfor, bool sort_by_date)
{
	libModeID = LB_ID_ALBUM;
	
	libTreeView.unset_model();
	libTreeStore.clear();
	libTreeStore = Gtk::TreeStore::create(libCols);
	libTreeView.set_model(libTreeStore);
	
	if (!column1.get_visible())
		column1.set_visible(true);
	
	if (!b_connected)
	{
		col0_header_img.set(pxb_tr_redcross);
		col0_header_label.set_text(" Not connected ");
		return;
	}

	gm_itemList itemlist;
	
    if (searchfor == "~0~")
        itemlist = mpdCom->get_albumlist(sort_by_date);
    else
        itemlist = mpdCom->get_album_searchlist(searchfor);

	if (itemlist.empty())
	{
		col0_header_img.set(pxb_tr_redcross);
		if (config->browser_LibraryMode = LB_ID_SEARCH)
			col0_header_label.set_text(" No albums found ");
		else
			col0_header_label.set_text(" No albums in database ");
		return;
	}

	itemlist.sort();
/* 
	bool bydate = (sort_by_date &&  searchfor == "~0~");
	if (bydate)
	{
		if (!column2.get_visible())
			column2.set_visible(true)
	}	
	and use 'if (bydate)' to set the value below
*/
	int count = 0;
	libTreeView.unset_model();	// see footnote
  	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		tm_Row row = *(libTreeStore->append()); 
				row[libCols.col0_icon] = pxb_tr_album;
		if (iter->album.empty())
			row[libCols.col0_Info] = "?";
		else
			row[libCols.col0_Info] = iter->album;
		if (iter->artist.empty())
			row[libCols.col1_Info] = "?";
		else
			row[libCols.col1_Info] = iter->artist;		

		row[libCols.col_listitem] = (*iter);

		// if (bydate)
		// row[libCols.col2_Info] =  iter->moddate;
	
		row[libCols.col_haschildren] = false;
		// add a dummy child
		libTreeStore->append(row.children());
		count ++;
	}
	libTreeView.set_model(libTreeStore);
	itemlist.clear();

	col0_header_img.set(pxb_tr_check);
	ustring str = " " + into_string(count) + " Albums";
	col0_header_label.set_text(str);
	col1_header_label.set_text(" Artist");
}

// called by browser when program shuts down
void gm_libraryView::get_configs()
{
	if (config->lib_fixed_columns)
	{
		if (column1.get_visible())
		{
			 // when TRUE get_width returns 0!
			libTreeView.set_fixed_height_mode(false);// TRUE: get_width returns 0!
			config->lib_column0_width = column0.get_width();
			config->lib_column1_width = column1.get_width();
		}
	}
}

void gm_libraryView::set_columns_fixed()
{
	if (config->lib_fixed_columns)
	{
		int width;
		column0.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		width = config->lib_column0_width;
		if (width == 0) width = 120;
		column0.set_fixed_width(width);
		column0.set_resizable(true);
		
		column1.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		width = config->lib_column1_width;
		if (width == 0) width = 120;		
		column1.set_fixed_width(width);
		column1.set_resizable(true);
		
		libTreeView.set_fixed_height_mode(true);
	}
	else
	{
		libTreeView.set_fixed_height_mode(false);
		
		column0.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE );
		column0.set_resizable(true);
		column1.set_sizing(Gtk::TREE_VIEW_COLUMN_AUTOSIZE );
		column1.set_resizable(true);
	}
}


void gm_libraryView::set_config(gm_Config *cfg)
{
	config = cfg;
	set_columns_fixed();
}


void gm_libraryView::on_plists_delete()
{
	if (!b_connected || libModeID != LB_ID_PLIST)
		return;
	
	int count = theSelection->count_selected_rows();

	if (count == 0)
		return;

	count = 0;
	
	gm_commandList newCommandList;
	std::vector<tm_Path> selpaths = theSelection->get_selected_rows();

	if (selpaths.empty())
		return;
	
	std::vector<Gtk::TreeModel::Path>::iterator path;
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		Gtk::TreeModel::Row row = *(libTreeStore->get_iter(*path));
		gm_listItem lit = row[libCols.col_listitem];
		if (lit.type == TP_PLAYLIST)
		{
			gm_cmdStruct newCommand;
			newCommand.cmd = CMD_DPL;
			newCommand.file = lit.file;
			newCommandList.push_front(newCommand);
			count++;
		}
	}
	
	if (count > 0)
	{
		// dialog
		Gtk::Dialog dlg("Delete Playlist", true);
		dlg.add_button("Cancel", Gtk::RESPONSE_CANCEL);
		dlg.add_button("OK", Gtk::RESPONSE_OK);
		dlg.set_default_response (Gtk::RESPONSE_CANCEL);
	
		Gtk::Grid content;
		content.set_row_spacing(10);
		content.set_orientation(Gtk::ORIENTATION_VERTICAL);
		Gtk::Label label("", 0.0, 0.5);
		label.set_line_wrap(true);
		label.set_use_markup(true);
		label.set_markup("<b>Attention</b>\nSelected playlists will be permanently deleted! ");

		dlg.get_content_area()->pack_start(content);
		content.set_border_width(10);
		content.add(label);
		content.show();
		label.show();

		dlg.set_size_request(300, -1);

		if (dlg.run() == Gtk::RESPONSE_OK)
		{
				mpdCom->execute_cmds( newCommandList, false );
				newCommandList.clear();
				get_playlists();
		}
		// <- dialog
	}
}


void gm_libraryView::create_rClickmenu()
{
	Glib::RefPtr<Gdk::Pixbuf> pxb;

	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_plistappend, false);
	img_men_plistappend.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, tr_plistnew, false);
	img_men_plistnew.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_redcross, false);
	img_men_plistdel.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_folder, false);
	img_men_fileman.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, menu_sub, false);
	img_men_submenu.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tag_edit, false);
	img_men_edit.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_update, false);
	img_men_reload.set(pxb);

	rClickMenu = new Gtk::Menu();
	
	imi_newplist = new Gtk::ImageMenuItem(img_men_plistnew,"Open in new playlist");
	rClickMenu->add(*imi_newplist);
	imi_newplist->signal_activate().connect(sigc::mem_fun(*this, &gm_libraryView::on_menu_plistnew));

	imi_append = new Gtk::ImageMenuItem(img_men_plistappend,"Append to playlist");
	rClickMenu->add(*imi_append);
	imi_append->signal_activate().connect(sigc::mem_fun(*this, &gm_libraryView::on_menu_plistappend));

	imi_delplist = new Gtk::ImageMenuItem(img_men_plistdel,"Delete playlist(s)");
	rClickMenu->add(*imi_delplist);
	imi_delplist->signal_activate().connect(sigc::mem_fun(*this, &gm_libraryView::on_plists_delete));
	
	imi_submenu = new Gtk::ImageMenuItem(img_men_submenu,"folder & file actions:");
	rClickMenu->add(*imi_submenu);	
	
	rClickSubMenu = new Gtk::Menu();
	
	imi_tagedit = new Gtk::ImageMenuItem(img_men_edit,"Tag editor");
	rClickSubMenu->add(*imi_tagedit);
	imi_tagedit->signal_activate().connect(sigc::mem_fun(*this, &gm_libraryView::on_menu_tageditor));
	
	imi_fileman = new Gtk::ImageMenuItem(img_men_fileman,"File manager");
	rClickSubMenu->add(*imi_fileman);
	imi_fileman->signal_activate().connect(sigc::mem_fun(*this, &gm_libraryView::on_menu_filemanager));

	imi_reload = new Gtk::ImageMenuItem(img_men_reload,"Rescan tags");
	rClickSubMenu->add(*imi_reload);
	imi_reload->signal_activate().connect(sigc::mem_fun(*this, &gm_libraryView::on_menu_reload));	

	imi_submenu->set_submenu(*rClickSubMenu);

	
	imi_delplist->set_sensitive(false);
	imi_newplist->set_sensitive(false);
	imi_append->set_sensitive(false);
	imi_fileman->set_sensitive(false);
	imi_tagedit->set_sensitive(false);
	imi_reload->set_sensitive(false);

	rClickSubMenu->show_all();
	rClickMenu->show_all();
}

void gm_libraryView::on_menu_plistnew()
{
    create_theModList(true, false);

    if (!thePlistModList.empty() && config->pList_new_startplaying && b_connected)
        mpdCom->play(true);
}


void gm_libraryView::on_menu_plistappend()
{
    create_theModList(false, false);
}


void gm_libraryView::on_menu_reload()
{
	if (!b_connected  || (!config->mpd_rescan_allowed && !config->mpd_update_allowed))
		return;
	
	std::vector<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
	std::vector<Gtk::TreeModel::Path>::iterator path = selpaths.begin();
	Gtk::TreeModel::Row row = *(libTreeStore->get_iter(*path));
	gm_listItem lit = row[libCols.col_listitem];

	if (lit.type == TP_FOLDER)
		mpdCom->rescan_folder(lit.file);
	else
	if (lit.type == TP_SONG)
		mpdCom->rescan_folder( (lit.file).substr(0, (lit.file).rfind("/")) );
	else
	return;
}


void gm_libraryView::on_menu_tageditor()
{
	if (!config->mpd_local_features)
		return;

	std::vector<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
	std::vector<Gtk::TreeModel::Path>::iterator path = selpaths.begin();
	Gtk::TreeModel::Row row = *(libTreeStore->get_iter(*path));
	gm_listItem lit = row[libCols.col_listitem];
	
	if (lit.type == TP_SONG)
	{
		ustring open_this = config->MPD_Musicpath + (lit.file).substr(0, (lit.file).rfind("/"));
		std::vector<std::string> argv;
		argv.push_back(config->Tag_Editor);
		argv.push_back(open_this);
		Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
	}
	else
	if (lit.type == TP_FOLDER)
	{
		ustring open_this = config->MPD_Musicpath + lit.file;
		std::vector<std::string> argv;
		argv.push_back(config->Tag_Editor);
		argv.push_back(open_this);
		Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
	}
	else
		return;
}


void gm_libraryView::on_menu_filemanager()
{
	if (!config->mpd_local_features)
		return;

	std::vector<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();
	std::vector<Gtk::TreeModel::Path>::iterator path = selpaths.begin();
	Gtk::TreeModel::Row row = *(libTreeStore->get_iter(*path));
	gm_listItem lit = row[libCols.col_listitem];
	
	if (lit.type == TP_SONG)
	{
		ustring open_this = config->MPD_Musicpath + (lit.file).substr(0, (lit.file).rfind("/"));
		std::vector<std::string> argv;
		argv.push_back(config->File_Manager);
		argv.push_back(open_this);
		Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
	}
	if (lit.type == TP_FOLDER)
	{
		ustring open_this = config->MPD_Musicpath + lit.file;
		std::vector<std::string> argv;
		argv.push_back(config->File_Manager);
		argv.push_back(open_this);
		Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
	}
	else
	if (lit.type == TP_PLAYLIST)
	{
		std::vector<std::string> argv;
		argv.push_back(config->File_Manager);
		argv.push_back(config->MPD_Playlistpath);
		Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));	
	}
	else
		return;
}


void gm_libraryView::create_theModList(bool b_newlist, bool b_insert)
{
    thePlistModList.clear();

	if (!b_connected)
		return;

	std::vector<Gtk::TreeModel::Path> selpaths = theSelection->get_selected_rows();

	if (selpaths.empty())
	    return;
	    
	std::vector<Gtk::TreeModel::Path>::iterator path;
	for(path = selpaths.begin(); path != selpaths.end(); ++path)
	{
		Gtk::TreeModel::Row row = *(libTreeStore->get_iter(*path));
		gm_listItem lit = row[libCols.col_listitem];
		switch (lit.type)
		{
			case TP_SONG:
			case TP_PLISTITEM:
			{
				gm_cmdStruct newCommand;
              	if (b_insert)
                    newCommand.cmd = CMD_INS;
                else
                    newCommand.cmd = CMD_ADD;
				newCommand.moveTo = plist_droppos;
				newCommand.file = lit.file;
				if (b_insert)
                	thePlistModList.push_front(newCommand);
                else
                    thePlistModList.push_back(newCommand);
				break;
			}
					
			case TP_ALBUM:
			{
                if ( libModeID == LB_ID_GENRE) 
                    append_toModlist(lit.album, lit.artist, lit.genre, b_insert);
                else
                    append_toModlist(lit.album, lit.artist, "~0~", b_insert);
                break;
			}
					
			case TP_ARTIST:
			{
				if ( libModeID == LB_ID_GENRE)
                    append_toModlist("~0~", lit.artist, lit.genre, b_insert);
                else
                    append_toModlist("~0~", lit.artist, "~0~", b_insert);
				break;
			}

			case TP_GENRE:
			{
				append_toModlist("~0~", "~0~", lit.genre, b_insert);
				break;
			}
				
			case TP_PLAYLIST:
			{
				append_toModlist_plist(lit.file, b_insert);
				break;
			}
			case TP_FOLDER:
			{
				append_toModlist_folder(lit.file, b_insert);
				break;
			}
				
			default:
				break;
		} // <- switch
	}

	selpaths.clear();

	if (!thePlistModList.empty())
    {
        if (b_newlist)
            mpdCom->clear_list();
            
        mpdCom->execute_cmds(thePlistModList, true);
        plist_droppos = 0;
    }
}


void gm_libraryView::append_toModlist(ustring album, ustring artist, ustring genre, bool b_insert)
{
	if (!b_connected)
		return;
	
    gm_itemList itemlist = mpdCom->get_songlist(album, artist, genre);
    
    if (itemlist.empty())
        return;
        
    itemlist.sort();
    
    std::list<gm_listItem>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        gm_cmdStruct newCommand;
        if (b_insert)
            newCommand.cmd = CMD_INS;
        else
            newCommand.cmd = CMD_ADD;
        newCommand.moveTo = plist_droppos;
        newCommand.file = iter->file;
        
        if (b_insert)
            thePlistModList.push_front(newCommand);
        else
            thePlistModList.push_back(newCommand);
    }
    
    itemlist.clear();
}


void gm_libraryView::append_toModlist_plist(ustring playlist, bool b_insert)
{
	if (!b_connected)
		return;
	
    gm_itemList itemlist = mpdCom->get_playlist_itemlist(playlist);
    
    if (itemlist.empty())
        return;
        
    std::list<gm_listItem>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        gm_cmdStruct newCommand;
        if (b_insert)
            newCommand.cmd = CMD_INS;
        else
            newCommand.cmd = CMD_ADD;
        newCommand.file = iter->file;
        newCommand.moveTo = plist_droppos;
        if (b_insert)
           thePlistModList.push_front(newCommand);
        else
           thePlistModList.push_back(newCommand);
    }
}


void gm_libraryView::append_toModlist_folder(ustring path, bool b_insert)
{
	if (!b_connected)
		return;
	
    gm_itemList itemlist = mpdCom->get_folderlist(path);
    itemlist.sort();
    
    std::list<gm_listItem>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if ( iter->type == TP_FOLDER )
            append_toModlist_folder(iter->file, b_insert); // recursively
        else
            if ( iter->type == TP_SONG  || iter->type == TP_STREAM )
            {
                gm_cmdStruct newCommand;
                if (b_insert)
                    newCommand.cmd = CMD_INS;
                else
                    newCommand.cmd = CMD_ADD;
                newCommand.moveTo = plist_droppos;
                newCommand.file =   iter->file;
                if (b_insert)
                    thePlistModList.push_front(newCommand);
                else
                    thePlistModList.push_back(newCommand);
            }
    }
    itemlist.clear();
}


void gm_libraryView::on_plist_dropreceived(int pos)
{
    if (pos == -1) // drop on empty playlist
    {
        plist_droppos = 0;
        create_theModList(true, false);
    }
    else
    {
        plist_droppos = pos;
        create_theModList(false, true); 
    }
}


gm_libraryView::~gm_libraryView()
{
}


/*  footnote:
 
	When a ListStore or TreeStore contains a large number of data rows, adding new rows
	can become very slow. There are a few things that you can do to mitigate this problem:

    [1] disconnect the TreeModel from its TreeView (TreeView updates for each added/removed row).
    [2] disable sorting
    [3] Limit the number of TreeRowReferences (they update their path with each addition).
    [4] Set the TreeView "fixed-height-mode" property to TRUE.
*/