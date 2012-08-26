/*
 *  gm_fixedlabel.h
 *  GUIMUP fixed-size label
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

#ifndef GM_FXLABEL_H
#define GM_FXLABEL_H

/* #include <iostream>
    using std::cout;
    using std::endl; */
#include <gtkmm/main.h>
#include <gtkmm/image.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;

class gm_FixedLabel : public Gtk::Image
{
public:
	gm_FixedLabel();
	virtual ~gm_FixedLabel();
	
	void set_info(ustring, ustring, ustring);
	void clear();
	void set_minimode(bool);
	void set_size_request(int, int);
	void override_font (const Pango::FontDescription&);
	void set_fg(Gdk::RGBA);
	void set_bg(Gdk::RGBA);
	void set_alpha(int);
	void reload();

protected:

private:

	ustring 
		thealbum,
		theyear,
		thecomment;
	int 
		W_layout,
		H_layout,
		width,
		height;
	Glib::RefPtr<Pango::Layout> layout;
	Cairo::RefPtr<Cairo::Context> cr;
	Cairo::RefPtr<Cairo::ImageSurface> surface;
	Glib::RefPtr<Gdk::Pixbuf> pxb_label;
	Gdk::RGBA
		bg_color,
		fg_color;
	bool b_minimode;
	ustring escapeString(ustring);
};

#endif //  GM_FXLABEL_H
