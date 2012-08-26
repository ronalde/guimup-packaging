/*
 *  gm_library_treeview.cc
 *  GUIMUP treeview class
 *  (c) 2008-2009 Johan Spee
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
//  items to be deselected when a drag is started. This is a hack to fix that


#include <gm_treeview.h>

gm_treeview::gm_treeview()
{
	y_offset = 24;
/*		
	 24 is a resonable guess. set_mouse_offset(x), where x is the actual 
	header heigth or 0 when the treeview has no header
*/	 
	b_dragged = false;
	b_interfered = false;
	tvSelection = Gtk::TreeView::get_selection();
}

gm_treeview::~gm_treeview()
{
}


// Catch accelerator keys you want to handle yourself
bool gm_treeview::on_key_press_event(GdkEventKey* event)
{
	if (event->keyval == GDK_space)
	{
		signal_key_pressed.emit(event);
		return false;
	}
	else
		return Gtk::TreeView::on_key_press_event(event);
}

bool gm_treeview::on_button_release_event(GdkEventButton* event)
{
	// No drag was started: restore normal button_press behaviour
	if (!b_dragged && b_interfered)
	{
		Gtk::TreeViewDropPosition pos;
		Gtk::TreeModel::Path path;
		if (Gtk::TreeView::get_dest_row_at_pos(event->x, event->y + y_offset, path, pos))
		{
			tvSelection->unselect_all();
			tvSelection->select(path);
			grab_focus();
		}
	}

	// Always reset
	b_dragged = false;
	b_interfered = false;
	
	return 	Gtk::TreeView::on_button_release_event(event);
}


// Drag: just take notice and pass it on
bool gm_treeview::on_drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
	b_dragged = true;
	return Gtk::TreeView::on_drag_motion(context, x, y, time);	
}


// Prevent unselecting all items on left-button-press
bool gm_treeview::on_button_press_event(GdkEventButton* event)
{
		// signal the event
		signal_mouse_pressed.emit(event);
	
		bool modifier_pressed = event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK);
	
		if (event->button != 1 || modifier_pressed)
		{
			// do not interfere
			return Gtk::TreeView::on_button_press_event(event);	
		}

		Gtk::TreeViewDropPosition pos;
		Gtk::TreeModel::Path path;
		bool path_is_selected = false;
		int num_selected = 0;
		if (Gtk::TreeView::get_dest_row_at_pos(event->x, event->y + y_offset, path, pos))
		{
			path_is_selected = tvSelection->is_selected(path);
			num_selected = tvSelection->count_selected_rows();
		}

		if( path_is_selected && num_selected > 1)
		{
			// interfere, 
			b_interfered = true;
			return true;
		}
		else
		{
			// do not interfere
			return Gtk::TreeView::on_button_press_event(event);
		}
}

/*  
	Mouse y-value and Treeview y-value are not the same! The difference
	is the treeview header. 
	Use the default 24 pix, or, if you can get the exact heigth, set it here.
	No header -> set it to 0.
*/
void gm_treeview::set_mouse_offset(int offset)
{
	y_offset = offset;
}
