/*
 *  gm_Slider.h
 *  GUIMUP volume slider
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

#ifndef GM_SLIDER_H
#define GM_SLIDER_H

/* #include <iostream>
    using std::cout;
    using std::endl; */
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <gtkmm/main.h>
#include <gtkmm/image.h>
#include <gtkmm/fixed.h>
#include <gtkmm/eventbox.h>

class gm_Slider : public Gtk::EventBox
{
public:
	gm_Slider();
	virtual ~gm_Slider();
	
//	functions
	void set_size_request( int, int );
	void set_value(int);
	int get_value();
    void set_colors( ustring, ustring );


protected:

private:
	
//  widgets
    Gtk::Fixed pbar_fixed;
    Gtk::Image pbar_img;
//  vars
	Glib::RefPtr< Gdk::Pixbuf > pxb;
	Glib::RefPtr< Gdk::Pixbuf > pxb_scaled;
	int prev_pos, width, height, current_volume;
	Gdk::RGBA bg_color;
	Gdk::RGBA fg_color;
//	functions
	void render_indicator();
};

#endif //  GM_SLIDER_H
