/*
 *  gm_config.h
 *  GUIMUP configuration class
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

#ifndef GM_CONFIG_H
#define GM_CONFIG_H

#include <glibmm/ustring.h>
    using Glib::ustring;
#include <iostream>
    using std::cout;
    using std::endl;
#include <string>
#include <fstream>
#include <sstream>
#include <list>


struct confitem
{
    ustring key;
    ustring value;
};

class gm_Config {
  public:
//  functions
    gm_Config();
    virtual ~gm_Config ();

    ustring get_string(ustring key);
    int     get_int(ustring key);
    bool    get_bool(ustring key);
    void    set_string(ustring key, ustring value);
    void    set_int(ustring key, int value);
    void    set_bool(ustring key, bool value);
    bool    save_config();
    bool    load_config();

  private:
//  functions
    bool  check_config_file();
//  variables
    ustring path_file;
    std::list <confitem> items;
};


#endif // GM_CONFIG_H
