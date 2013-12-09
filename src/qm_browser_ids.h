/*
*  qm_browser_ids.h
*  QUIMUP library & playlist combobox id's
*  © 2008-2013 Johan Spee
*
*  This file is part of Quimup
*
*  QUIMUP is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  QUIMUP is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#ifndef qm_browser_IDS_H
#define qm_browser_IDS_H

enum
{   // library modes
    LB_ID_ARTIST,
    LB_ID_ALBUM,
    LB_ID_DATE,
	LB_ID_YEAR,
    LB_ID_GENRE,
    LB_ID_PLIST,
    LB_ID_DIR,
    LB_ID_SEARCH
};

enum
{// search modes // keep this order!
    SR_ID_ARTIST,
    SR_ID_ALBUM,
    SR_ID_TITLE,
    SR_ID_GENRE,
	SR_ID_DIR
};

enum
{// select modes // keep this order!
    SL_ID_ARTIST,
    SL_ID_ALBUM,
    SL_ID_TITLE,
    SL_ID_ALL
};

#endif // qm_browser_IDS_H
