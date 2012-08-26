/*
 *  gm_browser.treeview.h
 *  GUIMUP treeview class
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

//	There is a bug in Gtk (see for instance # 508307) that causes all treeview
//  items to be deselected when a drag is started. This is a hack to fix that.


#ifndef GM_TVIEW_H
#define GM_TVIEW_H

#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>
#include <iostream>

class gm_treeview : public Gtk::TreeView
{

public:
    gm_treeview();
    virtual ~gm_treeview();
	void set_mouse_offset(int);
//  always emitted with an on_button_press_event:
    sigc::signal<void, GdkEventButton* > signal_mouse_pressed;
//  used to override some accelerator keys:
    sigc::signal<void, GdkEventKey* > signal_key_pressed;
protected:

private:

int y_offset;
bool b_dragged, b_interfered;
Glib::RefPtr<Gtk::TreeSelection> tvSelection;

bool on_key_press_event(GdkEventKey* event);
bool on_button_press_event (GdkEventButton* event);
bool on_button_release_event (GdkEventButton* event);
bool on_drag_motion (const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);

};

#endif /*GM_TVIEW_H */
