/*
 *  gm_Scroller.h
 *  GUIMUP graphical text scroller
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

#ifndef GM_SCROLLER_H
#define GM_SCROLLER_H


/* #include <iostream>
    using std::cout;
    using std::endl;*/
#include <gtkmm/main.h>
#include <gtkmm/image.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;

class gm_Scroller : public Gtk::Image
{
public:
//  functions
    gm_Scroller();
    virtual ~gm_Scroller();
    void set_title( ustring artist, ustring title = "" );
    void set_pause( bool );
    void set_colors(Gdk::RGBA, Gdk::RGBA);
    void set_size_request( int, int );
    void set_delay( int );
    void set_font( Pango::FontDescription );
	void on_mouse_enter();
	void on_mouse_leave();

protected:

private:
//  functions
    void startScroll();
    bool scrollerstep();
    bool render();
    ustring escapeString( ustring str );

//  variables
    ustring
        newString,
		current_artist,
        current_title;
	Pango::FontDescription pFont;
	
    bool
        b_scrolling,
        b_pause,
        b_busy;
    int
        scrollpos,
        scrollstep,
        steptime,
        W_layout,
        H_layout,
        W_cropped,
        all_stride,
        crp_stride,
		cR, cG, cB;
	Glib::RefPtr<Gdk::Pixbuf> 
		pxb_fulltext,
		pxb_display;
    Glib::RefPtr<Pango::Context> context;
    Glib::RefPtr<Pango::Layout> layout;
	Cairo::RefPtr<Cairo::Context> cr;
	Cairo::RefPtr<Cairo::ImageSurface> surface;
    guint8 *ptr_get, *ptr_put;
    sigc::connection timer;
	Gdk::RGBA bg_color;
	Gdk::RGBA fg_color;
};

#endif //  GM_SCROLLER_H
