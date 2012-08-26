/*
 *  gm_trayicon.cpp
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


#include "gm_trayicon.h"

gm_trayIcon::gm_trayIcon()
{

}

Glib::RefPtr<gm_trayIcon> gm_trayIcon::create()
{
  return Glib::RefPtr<gm_trayIcon>(new gm_trayIcon());
}

gm_trayIcon::~gm_trayIcon()
{
}


