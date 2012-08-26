/*
 *  gm_Scroller.cc
 *  GUIMUP graphical text scroller
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

#include "gm_scroller.h"

gm_Scroller::gm_Scroller()
{
    // Pango context and layout
    context = Glib::wrap(gdk_pango_context_get());
    g_assert(context);
    layout = Pango::Layout::create(context);
    g_assert(layout);
	// defaults etc.
	current_artist = "";
	current_title  = "";
	fg_color = "#000000";
	bg_color = "#ffffff";
	steptime = 80;  // msec
	scrollstep = 1;
	scrollpos = -scrollstep;
	b_busy = false;
	b_pause = false;
	b_scrolling = false;
	b_fast = false;
	Gtk::Image::set_alignment(0.5, 0.5); // center, center
	W_layout = H_layout = 0;
	W_cropped = 100;
	the_font = "sans bold 12";
}


bool gm_Scroller::render()
{
	// Format the string
    ustring markupString = "<span font_desc=\"" + the_font + "\">" + newString + "</span>";
	// Put the string in a layout
	layout->set_markup(markupString);
	// Get layout size in pixels
    layout->get_pixel_size(W_layout, H_layout);
	// Set the layout colors
	layout->set_markup("<span foreground=\"" + fg_color + "\" background=\"" + bg_color + "\">" + markupString + "</span>");	
	// Create a pixmap
    Glib::RefPtr<Gdk::Pixmap> pxmp = Gdk::Pixmap::create(Glib::RefPtr<Gdk::Drawable>(NULL), W_layout, H_layout, 24);
    g_assert(pxmp);
	// Render layout to pixmap
    pxmp->draw_layout(Gdk::GC::create(pxmp), 0, 0, layout);
	// Convert pixmap to pixbuffer
	if (W_layout <= W_cropped)
	{
		// display can take the full text
		pxb_display = Gdk::Pixbuf::create(Glib::RefPtr<Gdk::Drawable>(pxmp), pxmp->get_colormap(), 0, 0, 0, 0, W_layout, H_layout);
		g_assert(pxb_display);
		Gtk::Image::set(pxb_display);
	}
	else
	{
		// store the full text
		pxb_fulltext = Gdk::Pixbuf::create(Glib::RefPtr<Gdk::Drawable>(pxmp), pxmp->get_colormap(), 0, 0, 0, 0, W_layout, H_layout);
		g_assert(pxb_fulltext);	
		// display a scrolling view of the full text
		pxb_display = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, W_cropped, H_layout);
		g_assert(pxb_display);
		startScroll();
	}
	return false;
}


void gm_Scroller::set_title(ustring artist, ustring title)
{
	current_artist = artist;
	current_title  = title;
	// reset timer
	if (b_scrolling)
	{
		scrollpos = -scrollstep;
		timer.disconnect();
		// wait for time out!
		while (timer) {}
		b_scrolling = false;
	}
	// get string
	if (title.empty())
		newString = "☆ " + artist + " ☆";
	else
		newString = "☆ " + artist + " : " + title + " ☆";
	newString = escapeString(newString);
	render();
}


ustring gm_Scroller::escapeString(ustring & str)
{
	Glib::ustring::size_type pos;

	// escape the & sign
	for (pos = str.find("&"); pos != Glib::ustring::npos; pos = str.find("&", pos+1))
	{
	    str.replace(pos, 1, "&amp;");
	}
	// escape the < sign
	for (pos = str.find("<"); pos != Glib::ustring::npos; pos = str.find("<", pos+1))
	{
	    str.replace(pos, 1, "&lt;");
	}
	// escape the > sign
	for (pos = str.find(">"); pos != Glib::ustring::npos; pos = str.find(">", pos+1))
	{
	    str.replace(pos, 1, "&gt;");
	}
	// escape the ' sign
	for (pos = str.find("'"); pos != Glib::ustring::npos; pos = str.find("'", pos+1))
	{
	    str.replace(pos, 1, "&apos;");
	}
	// escape the " sign
	for (pos = str.find("\""); pos != Glib::ustring::npos; pos = str.find("\"", pos+1))
	{
	    str.replace(pos, 1, "&quot;");
	}

	return str;
}


void gm_Scroller::startScroll()
{
	ptr_get = pxb_fulltext->get_pixels();
	ptr_put = pxb_display->get_pixels();
	all_stride = pxb_fulltext->get_rowstride();
	crp_stride = pxb_display->get_rowstride();

	int delay = steptime;
	if (b_fast)
		delay = 10;
	b_scrolling = true;
	timer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Scroller::scrollerstep), delay);
}


bool gm_Scroller::scrollerstep()
{
	if (!b_scrolling)
		return false;
	
	if (b_pause || b_busy)
		return true;
	
	b_busy = true;
	
	scrollpos ++;
	
	if (scrollpos >= W_layout)
		scrollpos = 0;
	
	int channels = 3; // no alpha channel
	
	guint8 *ptr_pos_get = NULL;
	guint8 *ptr_pos_put = NULL;
	uint put_hpos, put_pos, get_hpos, get_pos = 0;
	
	for (int  h = 0; h < H_layout; h++)
    {	
		put_hpos = (h * crp_stride);
		get_hpos = (h * all_stride);
		
		for (int  w = 0; w < W_cropped; w++)
		{
			put_pos = put_hpos + (w * channels);
			get_pos = get_hpos + ((scrollpos + w) * channels);
			// wrap at the end
			if (scrollpos + w >= W_layout)
				get_pos -= (W_layout * channels);

			ptr_pos_put = ptr_put + put_pos;
			ptr_pos_get = ptr_get + get_pos;
			
  			ptr_pos_put[0] = ptr_pos_get[0]; // R
  			ptr_pos_put[1] = ptr_pos_get[1]; // G
  			ptr_pos_put[2] = ptr_pos_get[2]; // B
		}
	}	
	// display the result
	Gtk::Image::set(pxb_display);
	b_busy = false;
	return b_scrolling;
}

// freezes the scroller
void gm_Scroller::set_pause(bool pause)
{
	b_pause = pause;
}

// foreground color in #123def format
void gm_Scroller::set_fg( ustring fg )
{
	fg_color = fg;
}

// background color in #123def format
void gm_Scroller::set_bg( ustring bg )
{
	bg_color = bg;
}

// overloaded function
void gm_Scroller::set_size_request( int w, int h )
{
	if (w > 0)
		W_cropped = w;
	Gtk::Image::set_size_request(w, h);
}

// toggle step between 1 (false) and 4 pixels
void gm_Scroller::set_fast(bool isfast)
{
	b_fast = isfast;
	if (b_scrolling)
	{
		timer.disconnect();
		// wait for time out!
		while (timer) {}
		int delay = steptime;
		if (b_fast)
			delay = 10;
		timer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Scroller::scrollerstep), delay);	
	}
}

// delay time between scroll steps (msec)
void gm_Scroller::set_delay(int delay)
{
	if (delay < 20)
		delay = 20;
	steptime = delay;
}

// Font description: [Family, Family, [ ]] [Style Style [ ]] SIZE
// example: bold italic 14         // example: Sans, Helvetica 12
void gm_Scroller::set_font(ustring font_desc)
{
	if (!font_desc.empty())
		the_font = font_desc;
	set_title(current_artist, current_title);
}

gm_Scroller::~gm_Scroller()
{
}
