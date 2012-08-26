/*
 *  gm_Scroller.cc
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

#include "gm_scroller.h"

gm_Scroller::gm_Scroller()
{
    // Pango context and layout
	context = Glib::wrap(gdk_pango_context_get());
    g_assert(context);
	layout = Pango::Layout::create(context);
	g_assert(layout);
	// defaults etc.
	current_artist = " ";
	current_title  = "";
	fg_color.set("#000000");
	bg_color.set("#ffffff");
	steptime = 80;  // msec
	scrollstep = 1;
	scrollpos = -scrollstep;
	b_busy = false;
	b_pause = false;
	b_scrolling = false;
	Gtk::Image::set_alignment(0.5, 0.5); // center, center
	W_layout = H_layout = 0;
	W_cropped = 100;
	pFont.set_family("sans");
	pFont.set_weight(Pango::WEIGHT_BOLD);
	pFont.set_size(11);
	cR = cG = cB = 255; // fade in/out color
}


bool gm_Scroller::render()
{
	// reset timer
	if (b_scrolling)
	{
		scrollpos = -scrollstep;
		timer.disconnect();
		while (timer)
		{/* wait for time out */}
		b_scrolling = false;
	}
	
	layout->set_font_description(pFont);
	layout->set_markup(newString);
    layout->get_pixel_size(W_layout, H_layout);

	surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, W_layout, H_layout);
	cr = Cairo::Context::create(surface);

		//draw background
	cr->set_source_rgb(bg_color.get_red(),bg_color.get_green(),bg_color.get_blue());
	cr->rectangle(0, 0, W_layout, H_layout);
	cr->fill();
	cr->move_to(0,0);
	
		// draw text
	layout->update_from_cairo_context(cr);
	cr->set_source_rgb(fg_color.get_red(),fg_color.get_green(),fg_color.get_blue());
	layout->add_to_cairo_context(cr);
	cr->fill();

	if (W_layout <= W_cropped)
	{
		// display can handle the full text
		pxb_display = Gdk::Pixbuf::create(surface, 0, 0, W_layout, H_layout);
		g_assert(pxb_display);
		Gtk::Image::set(pxb_display);
	}
	else
	{
		// store the full text
		pxb_fulltext = Gdk::Pixbuf::create(surface, 0, 0, W_layout, H_layout);
		
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

	// get string
	if (title.empty())
		newString = "☆ " + artist + " ☆";
	else
		newString = "☆ " + artist + " : " + title + " ☆";
	newString = escapeString(newString);
	render();
}


ustring gm_Scroller::escapeString(ustring str_in)
{
	ustring str = str_in;
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
	b_scrolling = true;
	timer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Scroller::scrollerstep), steptime);
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
		scrollpos -= W_layout;
	
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
			
			if (w < 24)
			{
	  			*ptr_pos_put = (w * *ptr_pos_get + (24 - w) * cR)/24; // R
				ptr_pos_put++; 
				ptr_pos_get++;
	  			*ptr_pos_put = (w * *ptr_pos_get + (24 - w) * cG)/24; // G
				ptr_pos_put++; 
				ptr_pos_get++;				
	  			*ptr_pos_put = (w * *ptr_pos_get + (24 - w) * cB)/24; // B
			}
			else
			if (w > W_cropped - 24)
			{
				int wr = W_cropped - w;
	  			*ptr_pos_put = (wr * *ptr_pos_get + (24 - wr) * cR)/24; // R
				ptr_pos_put++; 
				ptr_pos_get++;
	  			*ptr_pos_put = (wr * *ptr_pos_get + (24 - wr) * cG)/24; // G
				ptr_pos_put++; 
				ptr_pos_get++;				
	  			*ptr_pos_put = (wr * *ptr_pos_get + (24 - wr) * cB)/24; // B
			}
			else
			{
	  			ptr_pos_put[0] = ptr_pos_get[0]; // R
	  			ptr_pos_put[1] = ptr_pos_get[1]; // G
	  			ptr_pos_put[2] = ptr_pos_get[2]; // B
			}
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

// background color
void gm_Scroller::set_colors( Gdk::RGBA bg, Gdk::RGBA fg )
{
	fg_color = fg;
	bg_color = bg;

	cR = (int)(256.0 * bg_color.get_red());
	cG = (int)(256.0 * bg_color.get_green());
	cB = (int)(256.0 * bg_color.get_blue());

	render();
}

// overloaded function
void gm_Scroller::set_size_request( int w, int h )
{
	if (w > 0)
		W_cropped = w;
	Gtk::Image::set_size_request(w, h);
}


// delay time between scroll steps (msec)
void gm_Scroller::set_delay(int delay)
{
	if (delay < 20)
		delay = 20;
	steptime = delay;
}


void gm_Scroller::set_font(Pango::FontDescription pfd)
{
	pFont = pfd;
	set_title(current_artist, current_title);
}


void gm_Scroller::on_mouse_enter()
{
	if (!b_scrolling)
		return;
	
	timer.disconnect();
	// wait for time out!
	while (timer) {	}
	
	timer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Scroller::scrollerstep), 10);
}


void gm_Scroller::on_mouse_leave()
{
	if (!b_scrolling)
		return;
	
	timer.disconnect();
	// wait for time out!
	while (timer) {}
	
	timer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Scroller::scrollerstep), steptime);
}


gm_Scroller::~gm_Scroller()
{
}
