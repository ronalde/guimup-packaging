/*
 *  gm_Scroller.h
 *  GUIMUP pogress bar
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

#ifndef GM_PROGRESS_H
#define GM_PROGRESS_H

#include <iostream>
    using std::cout;
    using std::endl;
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <gtkmm/main.h>
#include <gtkmm/image.h>
#include <gtkmm/fixed.h>
#include <gtkmm/eventbox.h>


class gm_Progressbar : public Gtk::EventBox
{
public:
	gm_Progressbar();
	virtual ~gm_Progressbar();
	
//	functions
	void set_size_request( int, int );
	void set_fraction(double);
    void set_colors( Gdk::RGBA, Gdk::RGBA );
	
protected:

private:

//  widgets
    Gtk::Fixed pbar_fixed;
    Gtk::Image pbar_img;
//  vars
	Glib::RefPtr< Gdk::Pixbuf > pxb;
	Glib::RefPtr< Gdk::Pixbuf > pxb_scaled;
	int prev_pos, width, height;
	Gdk::RGBA bg_color;
	Gdk::RGBA fg_color;
//	functions
	void render_indicator();
};

#endif //  GM_PROGRESS_H
