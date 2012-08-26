/*
 *  gm_trayicon.h
 *  GUIMUP system-tray-icon class
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


#ifndef GM_TRAY_ICON_H
#define GM_TRAY_ICON_H

#include <gtkmm/statusicon.h>
#include <gtkmm/eventbox.h>

    
class gm_trayIcon : public Gtk::StatusIcon
{
public:

	virtual ~gm_trayIcon();
	gm_trayIcon();
	static Glib::RefPtr<gm_trayIcon> create();

protected:

    
private:

};


#endif /* GM_TRAY_ICON_H */
