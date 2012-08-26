/*
 *  gm_libraryview.h
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

#ifndef GM_DATATREE_H
#define GM_DATATREE_H

#include <gtkmm/main.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/treestore.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/image.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/imagemenuitem.h>
#include <sigc++/sigc++.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <string>
#include <iostream>
    using std::cout;
    using std::endl;
#include <list>
#include "gm_treeview.h"
#include "gm_songinfo.h"
#include "gm_itemlist.h"
#include "gm_commandlist.h"
#include "gm_modeid.h"
#include "gm_mpdcom.h"

// for convenience
typedef Gtk::TreeModel::Children 	 tm_Children;
typedef Gtk::TreeModel::Path 		 tm_Path;
typedef Gtk::TreeModel::iterator 	 tm_Iterator;
typedef Gtk::TreeModel::RowReference tm_RowRef;
typedef Gtk::TreeModel::Row			 tm_Row;

class gm_libraryView : public Gtk::ScrolledWindow
{
public:
	gm_libraryView();
	virtual ~gm_libraryView();

// 	functions
    void set_mpdcom(gm_mpdCom*);
	void set_connected(bool);
	void set_config(gm_Config *);
	void get_configs();
	void get_artists(ustring);
	void get_albums(ustring, bool);
	void get_albums_year();
	void get_genres(ustring);
	void get_folders(ustring);
	void get_playlists();
	void search_for(ustring, int);
	void set_listfont(Pango::FontDescription);
	void on_plist_dropreceived(int);
	void set_columns_fixed();

protected:

private:
// 	widgets
    class libColumnRecord : public Gtk::TreeModelColumnRecord
    {
        public:
        libColumnRecord() { add(col0_icon); add(col0_Info); add(col0_color);
							add(col1_Info);add(col_listitem); add(col_haschildren);}
        Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > col0_icon;
		Gtk::TreeModelColumn<Glib::ustring> col0_Info;
		Gtk::TreeModelColumn<Glib::ustring> col1_Info;	
		Gtk::TreeModelColumn<gm_listItem> col_listitem;
		Gtk::TreeModelColumn<Glib::ustring> col0_color;
		Gtk::TreeModelColumn<bool> col_haschildren;
    };
    libColumnRecord libCols;
    Glib::RefPtr<Gtk::TreeStore> libTreeStore;
    gm_treeview libTreeView;
    Glib::RefPtr<Gtk::TreeSelection> theSelection;
    Gtk::Menu *rClickMenu;
	Gtk::Menu *rClickSubMenu;
	Gtk::ImageMenuItem 
		*imi_delplist,
		*imi_newplist,
		*imi_append,
		*imi_fileman,
		*imi_tagedit,
		*imi_reload,
		*imi_submenu;
// 	variables
  	std::vector<Gtk::TargetEntry> DnDlist;
    gm_mpdCom *mpdCom;
    Glib::RefPtr<Gdk::Pixbuf>
        pxb_tr_track,
		pxb_tr_track_played,
		pxb_tr_trackx_played,
		pxb_tr_stream_played,
        pxb_tr_album,
        pxb_tr_artist,
        pxb_tr_playlist,
		pxb_drag_playlist,
		pxb_tr_folder,
		pxb_tr_check,
		pxb_tr_redcross,
		pxb_tr_genre;
	Gtk::Image
		img_men_plistappend,
    	img_men_plistnew,
		img_men_plistdel,
		img_men_fileman,
		img_men_edit,
		img_men_reload,
		img_men_submenu,
		col0_header_img;
	Gtk::Label 
		col0_header_label,
		col1_header_label;
	Gtk::Grid
		col0_headerbox;
	Gtk::TreeViewColumn
		column0,
		column1;
    int 
		plist_droppos,
		mouse_offset,
		libModeID;
	bool 
		b_called_by_activated,
		b_called_by_clapsxpand,
		b_connected;
	gm_commandList thePlistModList;
	gm_Config *config;		
// 	functions
	bool on_button_pressed(GdkEventButton*);
	void on_selection_changed();
	void deselect_children(tm_Children::iterator iter);
	void on_row_activated(const tm_Path&, Gtk::TreeViewColumn*);
	void on_row_expanded(const tm_Iterator&, const tm_Path&);
	void on_row_collapsed(const tm_Iterator&, const tm_Path&);
	std::string into_string(int);
	bool expand_artist(tm_Row);
	bool expand_album(tm_Row);
	bool expand_genre(tm_Row);
	bool expand_playlist(tm_Row);
	bool expand_folder(tm_Row);
	void create_theModList(bool, bool);
	void append_toModlist(ustring, ustring, ustring, bool);
	void append_toModlist_plist(ustring, bool);
	void append_toModlist_folder(ustring, bool);
	void on_dtdrag_data_get( const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData&, guint, guint);
	void on_plists_delete();
	void on_menu_plistnew();
	void on_menu_plistappend();
	void on_menu_filemanager();
	void on_menu_tageditor();
	void on_menu_reload();
	void create_rClickmenu();
	void get_songs(ustring);
};

#endif //  GM_DATATREE_H
