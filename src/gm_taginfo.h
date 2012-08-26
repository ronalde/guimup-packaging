/*
 *  gm_taginfo.h
 *  GUIMUP tag information window
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

#ifndef GM_TAGINFO_H
#define GM_TAGINFO_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/alignment.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
	using Glib::ustring;
#include <stdlib.h>

class gm_tagInfo : public Gtk::Window
{
public:
    gm_tagInfo();
    virtual ~gm_tagInfo();

	void show_at(int, int);
	// set stream info
    void set( ustring artist_title, ustring url );
	// set tags info
    void set(
            ustring artist,
            ustring title,
            ustring album,
            ustring track,
            ustring disc,
            ustring year,
            ustring genre,
            ustring composer,
            ustring performer,
            ustring comment);
	// No song
	void reset();

protected:

private:
// 	widgets
    Gtk::VBox vb_tagsbox;
    Gtk::Label
            lb_artist,
            lb_title,
            lb_album,   // with track and disc
            lb_year,
            lb_genre,
            lb_composer,
            lb_performer,
            lb_comment;
	Gtk::Alignment al_spacer;
			
	bool on_delete_event(GdkEventAny* event);
};

#endif //  GM_TAGINFO_H
