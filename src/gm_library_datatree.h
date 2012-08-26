/*
 *  gm_library_datatree.h
 *  GUIMUP library data treeview
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

#ifndef GM_DATATREE_H
#define GM_DATATREE_H

#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
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
#include "gm_treeview.h"
#include "gm_library_mpdcom.h"
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
	sigc::signal<void, int> signal_mode;

// 	functions
    void set_mpdcom(library_mpdCom*);
	void get_artists();
	void get_albums(ustring);
	void get_folders(ustring);
	void get_playlists();
	void search_for(ustring, int);
	void set_listfont(ustring);
	void set_plistPath(ustring);
	void set_menu_del_item(bool);

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
    gm_treeview dtTreeView;
    Glib::RefPtr<Gtk::TreeSelection> theSelection;
    Gtk::Menu rClickMenu;
// 	variables
    ustring plistPath, searchString;
	std::list<Gtk::TargetEntry> DnDlist;
    library_mpdCom *dt_mpdCom; // pointer to the parent mpdCom
    Glib::RefPtr<Gdk::Pixbuf>
        pxb_tr_track,
        pxb_tr_album,
        pxb_tr_artist,
        pxb_tr_playlist,
		pxb_tr_folder,
		pxb_tr_check,
		pxb_tr_redcross,
		pxb_tr_trplayed,
		pxb_tr_search,
		pxb_tr_uodate;
	Gtk::Image
		col_header_img,
		men_artist_img,
		men_album_img,
		men_playlist_img,
		men_folder_img,
		men_search_img,
		men_delete_img;
	Gtk::Label col_header_label;
	Gtk::HBox headerbox;
	Gtk::TreeViewColumn	viewcol_item;
    int dataModeID, mouse_offset;
	bool b_called_by_activated, b_called_by_collapsexpand;
	gm_commandList dtCommandList;
			
// 	functions
	void set_artist_mode();
	void set_album_mode();
	void set_playlist_mode();
	void set_folder_mode();
	void set_search_mode();
	void deselect_children(Tr_Ch::iterator iter);
	void deselect_parents(Tr_Ch::iterator iter);
	bool on_key_pressed(GdkEventKey*);
	void on_mouse_pressed(GdkEventButton* event);
	void on_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
	void on_row_expanded(const Gtk::TreeModel::iterator&, const Gtk::TreeModel::Path&);
	void on_row_collapsed(const Gtk::TreeModel::iterator&, const Gtk::TreeModel::Path&);
	std::string into_string(int);
	void on_dtdrag_data_get( const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData&, guint, guint);
	void get_dir_items(ustring);
	void on_plists_delete();
};

#endif //  GM_DATATREE_H
