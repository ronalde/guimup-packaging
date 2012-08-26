/*
 *  gm_fixedlabel.cc
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

#include "gm_fixedlabel.h"

gm_FixedLabel::gm_FixedLabel()
{
	width = height = 100;
	W_layout = H_layout = 0;
	layout = Pango::Layout::create(Glib::wrap(gdk_pango_context_get()));
	fg_color.set("#000000");
	bg_color.set("#ffffff");
	b_minimode = false;
	thealbum = "";
	theyear = "";
	thecomment = "";
}


void gm_FixedLabel::override_font (const Pango::FontDescription& font_desc)
{
	layout->set_font_description(font_desc);
}


void gm_FixedLabel::clear()
{
	thealbum = "";
	theyear = "";
	thecomment = "";
	
	surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
	cr = Cairo::Context::create(surface);

	cr->set_source_rgb(bg_color.get_red(),bg_color.get_green(),bg_color.get_blue());
	cr->rectangle(0, 0, width, height);
	cr->fill();
	cr->move_to(0,0);

	pxb_label = Gdk::Pixbuf::create(surface, 0, 0, width, height);
	g_assert(pxb_label);
	// add alpha channel
	pxb_label = pxb_label->add_alpha(false,0,0,0);
	if (b_minimode)
		set_alpha(0);
	else
		set(pxb_label);
}


void gm_FixedLabel::set_info (ustring album, ustring year, ustring comment)
{
		// [1] truncate the comment until it fits the image
		layout->set_markup("çÖqÑ"); // high & low chars
		layout->get_pixel_size(W_layout, H_layout);
		int lineH = H_layout;
	
		// store for reload()
		thealbum = album;
		theyear = year;
		thecomment = comment;
	
		ustring album_year = "<b>" + escapeString(album) + "</b>  " + escapeString(year);
	
	    layout->set_markup(album_year + "<small>&#10;&#10;" + escapeString(comment) + "</small>");
    	layout->get_pixel_size(W_layout, H_layout);

		int count = 0;
		while ( (H_layout > height || W_layout > width) && comment.length() > 2)
		{
			count++;
			if (H_layout - height >= 4*lineH)
			{    // huge diference > huge steps
				comment = comment.substr(0, comment.length() - 60);
			}
			else // big diference > big steps
			if (H_layout - height >= 2*lineH)  
			{
				comment = comment.substr(0, comment.length() - 20);
			}
			else
			{ 	 // small diference > small steps
				comment = comment.substr(0, comment.length() - 2);
			}

			layout->set_markup(album_year + "<small>&#10;&#10;" + escapeString(comment) + "</small>");

			layout->get_pixel_size(W_layout, H_layout);		
		}
		if (count && comment.length() > 4) // show the string was truncated
				comment.replace(comment.length()-4,comment.length()," ...");
	
		layout->set_markup(album_year + "<small>&#10;&#10;" + escapeString(comment) + "</small>");

		// [2] create & set the image
		surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
		cr = Cairo::Context::create(surface);

			//draw background
		cr->set_source_rgb(bg_color.get_red(),bg_color.get_green(),bg_color.get_blue());
		cr->rectangle(0, 0, width, height);
		cr->fill();
		cr->move_to(0,0);
	
			// draw text
		layout->update_from_cairo_context(cr);
		cr->set_source_rgb(fg_color.get_red(),fg_color.get_green(),fg_color.get_blue());
		layout->add_to_cairo_context(cr);
		cr->fill();

		pxb_label = Gdk::Pixbuf::create(surface, 0, 0, width, height);
		// add alpha channel
		pxb_label = pxb_label->add_alpha(false,0,0,0);
		g_assert(pxb_label);

		if (b_minimode)
			set_alpha(0);
		else
			set(pxb_label);
}

// used when the color changes
void gm_FixedLabel::reload()
{
	set_info (thealbum, theyear, thecomment);
}


void gm_FixedLabel::set_minimode(bool bmini)
{
	b_minimode = bmini;
}


void gm_FixedLabel::set_size_request(int w , int h)
{
	height = h;
	width = w;
	layout->set_width (width  * PANGO_SCALE);
	Gtk::Image::set_size_request(w,h);
}


void gm_FixedLabel::set_alpha(int alpha)
{
		guint8 *ptr_pixel;
		guint8 *ptr_firstpixel = pxb_label->get_pixels();
        int rows = 	pxb_label->get_rowstride();
		for (int h = 0; h < height; h++)
		{
			ptr_pixel = ptr_firstpixel + h * rows;
			for (int w = 0; w < width; w++)
			{
				ptr_pixel[3] = alpha;
				ptr_pixel += 4; // R,G,B,a
			}
		}
		set(pxb_label);
}

// escape pango's reserved characters
ustring gm_FixedLabel::escapeString(ustring str_in)
{
	ustring str = str_in;
	Glib::ustring::size_type pos;
	// escape &
	for (pos = str.find("&"); pos != Glib::ustring::npos; pos = str.find("&", pos+1))
	{
	    str.replace(pos, 1, "&amp;");
	}
	// escape <
	for (pos = str.find("<"); pos != Glib::ustring::npos; pos = str.find("<", pos+1))
	{
	    str.replace(pos, 1, "&lt;");
	}
	// escape >
	for (pos = str.find(">"); pos != Glib::ustring::npos; pos = str.find(">", pos+1))
	{
	    str.replace(pos, 1, "&gt;");
	}
	// escape '
	for (pos = str.find("'"); pos != Glib::ustring::npos; pos = str.find("'", pos+1))
	{
	    str.replace(pos, 1, "&apos;");
	}
	// escape "
	for (pos = str.find("\""); pos != Glib::ustring::npos; pos = str.find("\"", pos+1))
	{
	    str.replace(pos, 1, "&quot;");
	}

	return str;
}


void gm_FixedLabel::set_fg( Gdk::RGBA fg )
{
	fg_color = fg;
}


void gm_FixedLabel::set_bg( Gdk::RGBA bg )
{
	bg_color = bg;
}


gm_FixedLabel::~gm_FixedLabel()
{
}
