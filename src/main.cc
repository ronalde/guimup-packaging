/*
 *  main.cc
 *  GUIMUP main source file
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

#include <gtkmm/main.h>
#include "gm_player.h"

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    gm_Player the_player;
    Gtk::Main::run(the_player);

    return 0;
}
