/*
 *  gm_tracks_datatree.h
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

#ifndef GM_DATATREE_H
#define GM_DATATREE_H

#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/treestore.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/image.h>
#include <gtkmm/messagedialog.h>
#include <list>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
    using std::cout;
    using std::endl;

#include "gm_tracks_mpdcom.h"
#include "gm_songlist.h"
#include "gm_itemlist.h"
#include "gm_commandlist.h"
#include "gm_modeid.h"

// for convenience
typedef Gtk::TreeModel::Children Tr_Ch;

class gm_Datatree : public Gtk::ScrolledWindow
{
public:
	gm_Datatree();
	virtual ~gm_Datatree();
// 	signal		
	sigc::signal<void, gm_commandList> signal_cmdList;

// 	functions
    void set_mpdcom(tracks_mpdCom*);
	void get_artists();
	void get_albums(ustring);
	void get_folders(ustring);
	void get_playlists();
	void search_for(ustring, int);
	void set_listfont(ustring font);			
protected:

private:

// 	widgets
    class dtModelcolumns : public Gtk::TreeModelColumnRecord
    {
        public:
        dtModelcolumns() { add(col_icon); add(col_name); add(col_listitem); add(col_haschildren);}
        Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > col_icon;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn<listItem> col_listitem;
		Gtk::TreeModelColumn<bool> col_haschildren;
    };
    dtModelcolumns dtColumns;
    Glib::RefPtr<Gtk::TreeStore> dtTreeStore;
    Gtk::TreeView dtTreeView;
    Glib::RefPtr<Gtk::TreeSelection> theSelection;

// 	variables
    ustring plistPath, searchString;
	std::list<Gtk::TargetEntry> DnDlist;
    tracks_mpdCom *dt_mpdCom; // pointer to the parent mpdCom
    Glib::RefPtr<Gdk::Pixbuf>
        pxb_tr_track,
        pxb_tr_album,
        pxb_tr_artist,
        pxb_tr_playlist,
		pxb_tr_folder,
		pxb_tr_check,
		pxb_tr_redcross,
		pxb_tr_trplayed;
	Gtk::Image col_header_img;
	Gtk::Label col_header_label;
	Gtk::HBox headerbox;
	Gtk::TreeViewColumn	viewcol_item;
    int dataModeID;
	bool b_called_by_row_expanded;
	gm_commandList dtCommandList;
			
// 	functions
	void sort_by_date(gm_itemList&);
	bool on_key_pressed(GdkEventKey* event);
	void on_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
	void on_row_expanded(const Gtk::TreeModel::iterator&, const Gtk::TreeModel::Path&);
	std::string into_string(int);
	void on_dtdrag_data_get( const Glib::RefPtr<Gdk::DragContext>&,
        Gtk::SelectionData& selection_data, guint, guint);
	void get_dir_items(ustring);
};

#endif //  GM_DATATREE_H
