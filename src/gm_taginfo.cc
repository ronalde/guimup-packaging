/*
 *  gm_taginfo.cc
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

#include "gm_taginfo.h"
#include "grafix/ti_icon.h"

gm_tagInfo::gm_tagInfo()
{
	lb_artist.set_alignment(0, 0);
    lb_artist.set_single_line_mode(false);
    lb_artist.set_line_wrap(true);
	lb_artist.set_size_request(242, -1);
	lb_artist.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	
	lb_title.set_alignment(0, 0);
    lb_title.set_single_line_mode(false);
    lb_title.set_line_wrap(true);
	lb_title.set_size_request(242, -1);
	lb_title.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	
	lb_album.set_alignment(0, 0);
    lb_album.set_single_line_mode(false);
    lb_album.set_line_wrap(true);
	lb_album.set_size_request(242, -1);
	lb_album.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	
	lb_year.set_alignment(0, 0);
	lb_year.set_size_request(242, -1);
	lb_year.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	
	lb_genre.set_alignment(0, 0);
    lb_genre.set_single_line_mode(false);
    lb_genre.set_line_wrap(true);
	lb_genre.set_size_request(242, -1);
	lb_genre.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	
	lb_composer.set_alignment(0, 0);
    lb_composer.set_single_line_mode(false);
    lb_composer.set_line_wrap(true);
	lb_composer.set_size_request(242, -1);
	lb_composer.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	
	lb_performer.set_alignment(0, 0);
    lb_performer.set_single_line_mode(false);
    lb_performer.set_line_wrap(true);
	lb_performer.set_size_request(242, -1);
	lb_performer.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	
	lb_comment.set_alignment(0, 0);
    lb_comment.set_single_line_mode(false);
    lb_comment.set_line_wrap(true);
	lb_comment.set_size_request(242, -1);
	lb_comment.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));

	al_spacer.set_size_request(242, -1);
	
    vb_tagsbox.set_spacing(4);
	
    vb_tagsbox.pack_start(lb_artist, false, false, 2);
    vb_tagsbox.pack_start(lb_title, false, false, 2);
    vb_tagsbox.pack_start(lb_album, false, false, 2);
    vb_tagsbox.pack_start(lb_year, false, false, 2);
    vb_tagsbox.pack_start(lb_genre, false, false, 2);
    vb_tagsbox.pack_start(lb_composer, false, false, 2);
    vb_tagsbox.pack_start(lb_performer, false, false, 2);
    vb_tagsbox.pack_start(lb_comment, false, false, 2);
	// spacer pushes the others up
    vb_tagsbox.pack_end(al_spacer, true, true, 0);
	
	// window properties
	Glib::RefPtr<Gdk::Pixbuf> pxb = Gdk::Pixbuf::create_from_inline(-1, ti_icon, false);
	set_icon(pxb);
    set_title("Song Info");
    set_border_width(4);
	set_resizable(false);
	
    add(vb_tagsbox);
}

void gm_tagInfo::show_at(int xpos,int ypos)
{
	this->show();
	move(xpos, ypos);
}

void gm_tagInfo::reset()
{
	vb_tagsbox.hide_all();
	lb_artist.set_markup("No song");
    lb_artist.show();
	al_spacer.show();
	vb_tagsbox.show();
	set_size_request(250, -1);
}

void gm_tagInfo::set( ustring artist_title, ustring url ) // stream
{
    vb_tagsbox.hide_all();

    lb_artist.set_markup("<small>Audio stream from:&#10;</small>" + url);
    lb_artist.show();

    lb_title.set_markup("<small>Now playing:&#10;</small>" + artist_title);
    lb_title.show();

    al_spacer.show();
    vb_tagsbox.show();
	
	int width, height;
	get_size(width, height);
	set_size_request(250, -1);
}

void gm_tagInfo::set(
            ustring artist,
            ustring title,
            ustring album,
            ustring track,
            ustring disc,
            ustring year,
            ustring genre,
            ustring composer,
            ustring performer,
            ustring comment)
{
    vb_tagsbox.hide_all();

    if (!artist.empty())
    {
        lb_artist.set_markup("<small>Artist:&#10;</small><b>" + artist + "</b>");
        lb_artist.show();
    }

    if (!title.empty())
    {
        lb_title.set_markup("<small>Title:&#10;</small><b>" + title + "</b>");
        lb_title.show();
    }

    if (!album.empty())
    {
        ustring str = "<small>Album:&#10;</small><b>" + album + "</b>";
        if (!track.empty())
        {
            str += "&#10;Track " + track;
            if (!disc.empty())
                str += ", Disc " + disc;
        }
        else
        if (!disc.empty())
            str += "&#10;Disc " + disc;
        lb_album.set_markup(str);
        lb_album.show();
    }

    if (!year.empty())
    {
        lb_year.set_markup("<small>Year:&#10;</small>" + year);
        lb_year.show();
    }

    if (!genre.empty())
    {
        lb_genre.set_markup("<small>Genre:&#10;</small>" + genre);
        lb_genre.show();
    }

    if (!performer.empty())
    {
        lb_performer.set_markup("<small>Performer:&#10;</small>" + performer);
        lb_performer.show();
    }

    if (!composer.empty())
    {
        lb_composer.set_markup("<small>Composer:&#10;</small>" + composer);
        lb_composer.show();
    }

    if (!comment.empty())
    {
        lb_comment.set_markup("<small>Comment:&#10;</small>" + comment);
        lb_comment.show();
    }

    al_spacer.show();
    vb_tagsbox.show();
	
	int width, height;
	get_size(width, height);
	set_size_request(250, -1);
}

bool gm_tagInfo::on_delete_event(GdkEventAny* event)
{
   this->hide();
   return true;
}

gm_tagInfo::~gm_tagInfo()
{
}
