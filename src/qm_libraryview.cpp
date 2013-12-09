/*
*  qm_libraryview.cpp
*  QUIMUP library treeview
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

#include "qm_libraryview.h"

qm_libview::qm_libview(QWidget *parent)
{
    setParent(parent);

    if (objectName().isEmpty())
        setObjectName("lib_view");

    config = NULL;
    b_mpd_connected = false;
    b_panel_max = false;
    plist_droppos = 0;
    searched_mode_used = -1;
    setRootIsDecorated(true);
    setItemsExpandable (true);
    setUniformRowHeights(true);
    setAllColumnsShowFocus(false);
    setSelectionMode(ExtendedSelection);
    sortByColumn (-1); // no sorting
    setSortingEnabled(false);
    set_column_count(1);
    setDragDropMode(QAbstractItemView::DragOnly);
    setDropIndicatorShown(false);
    setDragEnabled(true);

    QObject::connect( this, SIGNAL( itemExpanded(QTreeWidgetItem *) ), this, SLOT( on_item_expanded(QTreeWidgetItem *)));
    QObject::connect( this, SIGNAL( itemSelectionChanged() ), this, SLOT( on_selection_changed()));

    // context menu
    context_menu = new QMenu();
    this->setContextMenuPolicy(Qt::DefaultContextMenu);

    QAction *createnew = new QAction(context_menu);
    createnew->setText(tr("Open in new playlist"));
    createnew->setIcon(QIcon(":/tr_plistnew.png"));
    QObject::connect( createnew, SIGNAL( triggered() ), this, SLOT( on_createnew() ) );
    context_menu->addAction(createnew);

    QAction *append = new QAction(context_menu);
    append->setText(tr("Append to playlist"));
    append->setIcon(QIcon(":/tr_plistappend.png"));
    QObject::connect( append, SIGNAL( triggered() ), this, SLOT( on_append() ) );
    context_menu->addAction(append);

    a_update = new QAction(context_menu);
    a_update->setText(tr("Update selection"));
    a_update->setIcon(QIcon(":/menu_load.png"));
    QObject::connect( a_update, SIGNAL( triggered() ), this, SLOT( on_update() ) );
    context_menu->addAction(a_update);

    a_opendir = new QAction(context_menu);
    a_opendir->setText(tr("Open folder"));
    a_opendir->setIcon(QIcon(":/tr_diropen.png"));
    QObject::connect( a_opendir, SIGNAL( triggered() ), this, SLOT( on_opendir() ) );
    context_menu->addAction(a_opendir);

    a_tagedit = new QAction(context_menu);
    a_tagedit->setText(tr("Edit tags"));
    a_tagedit->setIcon(QIcon(":/menu_edit.png"));
    QObject::connect( a_tagedit, SIGNAL( triggered() ), this, SLOT( on_tagedit() ) );
    context_menu->addAction(a_tagedit);

    a_delete = new QAction(context_menu);
    a_delete->setText(tr("Delete selection"));
    a_delete->setIcon(QIcon(":/tr_redcross.png"));
    QObject::connect( a_delete, SIGNAL( triggered() ), this, SLOT( on_delete() ) );
    context_menu->addAction(a_delete);

    QAction * a_collapse = new QAction(context_menu);
    a_collapse->setText(tr("Collapse all"));
    a_collapse->setIcon(QIcon(":/tr_collapse.png"));
    QObject::connect( a_collapse, SIGNAL( triggered() ), this, SLOT( collapseAll() ) );
    context_menu->addAction(a_collapse);

    a_delete->setEnabled(false);
    a_opendir->setEnabled(false);
    a_tagedit->setEnabled(false);
    a_update->setEnabled(false);
}

// http://qt-project.org/faq/answer/is_it_possible_to_reimplement_non-virtual_slots
void qm_libview::fix_header()
 {
    libhview = new QHeaderView(Qt::Horizontal, this);
    libhview->setHighlightSections(false);
    libhview->setClickable(false);
    libhview->setDefaultAlignment(Qt::AlignLeft);
    libhview->setStretchLastSection(true);
    libhview->setMinimumSectionSize(80);
    setHeader(libhview);
    libhview->show();
 }


void qm_libview::on_selection_changed()
{
    int counter = 0;
    QTreeWidgetItemIterator itr(this);
    while (*itr)
    {
        QTreeWidgetItem *current = *itr;

        if ( current->isSelected() )
        {
             // no selection of non-draggable items
            if (current->gettype >= TP_PLISTITEM)
                current->setSelected(false);
            else
            { // no selection if if a parent is already selected
                bool a_parent_selected = false;
                QTreeWidgetItem * up = current->parent();
                while (up != NULL)
                {
                    if ( up->isSelected() )
                        a_parent_selected = true;
                    up = up->parent();
                }

                if (a_parent_selected)
                    current->setSelected(false);
                else
                    counter++;
            }
        }
        ++itr;
    }

    if (counter == 1 && currentItem() != NULL)
    {
        QTreeWidgetItem *current = currentItem();

        if ( 	current != NULL
                && (current->gettype == TP_SONG || current->gettype == TP_DIRECTORY)
                && config->mpd_musicpath_status == 1 )
        {
            a_opendir->setEnabled(true);
            a_tagedit->setEnabled(true);
        }
        else
        {
            a_opendir->setEnabled(false);
            a_tagedit->setEnabled(false);
        }
    }
    else
    {
        a_opendir->setEnabled(false);
        a_tagedit->setEnabled(false);
    }
}


void qm_libview::on_item_expanded(QTreeWidgetItem * active_item)
{
    if (!b_mpd_connected)
        return;

    switch (active_item->gettype)
    {
        case TP_ARTIST:
        {
            if(active_item->getdummy)
            {
                take_dummy_from(active_item);
                expand_artist(active_item);
            }
            break;
        }

        case TP_ALBUM:
        {
            if(active_item->getdummy)
            {
                take_dummy_from(active_item);
                expand_album(active_item);
            }
            break;
        }

        case TP_GENRE:
        {
            if(active_item->getdummy)
            {
                take_dummy_from(active_item);
                expand_genre(active_item);
            }
            break;
        }

        case TP_PLAYLIST:
        {
            if(active_item->getdummy)
            {
                take_dummy_from(active_item);
                expand_playlist(active_item);
            }
            break;
        }

        case TP_DIRECTORY:
        {
            if(active_item->getdummy)
            {
                take_dummy_from(active_item);
                expand_directory(active_item);
            }
            break;
        }

        default: // TP_SONG, TP_PLISTITEM, TP_COMMENT
            break;
    }
}

// tree root items in album mode
void qm_libview::get_albums(QString searchfor, bool sort_by_date)
{
    if (!b_mpd_connected)
    {
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        headerItem()->setText(0, " " + tr("not connected") + " ");
        set_column_count(1);
        return;
    }

    qm_itemList itemlist;

    if (searchfor == "*^*")
        itemlist = mpdCom->get_albumlist(sort_by_date);
    else
        itemlist = mpdCom->get_album_searchlist(searchfor);

    if (itemlist.empty())
    {
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        headerItem()->setText(0, " " + tr("no albums found") + " ");
        set_column_count(1);
        return;
    }

    bool bydate = (sort_by_date &&  searchfor == "*^*");

    if (bydate)
        set_column_count(3);
    else
        set_column_count(2);

    int count = 0;

    itemlist.sort();
    // Skip duplicates!
    QString album = "*^*";
    QString artist = "*^*";

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if (album == iter->album && artist == iter->artist)
            continue;

        album = iter->album;
        artist = iter->artist;

        new_item =  new QTreeWidgetItem();
        if (iter->album.isEmpty())
            new_item->setText(0, "?");
        else
            new_item->setText(0, iter->album);
        if (iter->artist.isEmpty())
            new_item->setText(1, "?");
        else
            new_item->setText(1, iter->artist);
        new_item->setIcon(0, QIcon(":/tr_album.png"));

        new_item->settype(TP_ALBUM);
        new_item->setartist(iter->artist);
        new_item->setalbum(iter->album);

        if (bydate)
            new_item->setText(2, iter->moddate);

        addTopLevelItem(new_item);
        add_dummy_to(new_item);
        count ++;
    }
    itemlist.clear();

    QString str = " " + QString::number(count) + " " + tr("albums") + " ";
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
    headerItem()->setText(0, str);
    headerItem()->setText(1, tr("artist") );
    if (bydate)
        headerItem()->setText(2, tr("Last modified") );
}

// tree root items in year mode
void qm_libview::get_albums_year()
{
    if (!b_mpd_connected)
    {
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        headerItem()->setText(0, " " + tr("not connected") + " ");
        set_column_count(1);
        return;
    }

    qm_itemList itemlist;

    itemlist = mpdCom->get_yearlist();

    if (itemlist.empty())
    {
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        headerItem()->setText(0, " " + tr("no albums found") + " ");
        set_column_count(1);
        return;
    }

    set_column_count(3);
    int count = 0;
    itemlist.sort();
    // Skip duplicates!
    QString album = "*^*";
    QString artist = "*^*";

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if (album == iter->album && artist == iter->artist)
            continue;

        album = iter->album;
        artist = iter->artist;

        new_item =  new QTreeWidgetItem();
        if (iter->album.isEmpty())
            new_item->setText(0, "?");
        else
            new_item->setText(0, iter->album);
        if (iter->artist.isEmpty())
            new_item->setText(1, "?");
        else
            new_item->setText(1, iter->artist);
        new_item->setIcon(0, QIcon(":/tr_album.png"));

        new_item->settype(TP_ALBUM);
        new_item->setartist(iter->artist);
        new_item->setalbum(iter->album);

        new_item->setText(2, iter->year);

        addTopLevelItem(new_item);
        add_dummy_to(new_item);
        count ++;
    }
    itemlist.clear();

    QString str = " " + QString::number(count) + " " + tr("albums") + " ";
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
    headerItem()->setText(0, str);
    headerItem()->setText(1, tr("artist") );
    headerItem()->setText(2, tr("year") );
}

// tree root items in dir mode
void qm_libview::get_directories()
{
    set_column_count(1);

    if (!b_mpd_connected)
    {
        headerItem()->setText(0, " " + tr("not connected") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        return;
    }

    qm_itemList itemlist;

    itemlist = mpdCom->get_directorylist("");

    if (itemlist.empty())
    {
        headerItem()->setText(0, " " + tr("no folders") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        return;
    }

    itemlist.sort();

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if ( iter->type == TP_DIRECTORY )
        {
            new_item = new QTreeWidgetItem();
            new_item->setText(0, iter->name);
            new_item->setIcon(0, QIcon(":/tr_dir.png"));

            new_item->settype(TP_DIRECTORY);
            new_item->setfile(iter->file);
            new_item->setname(iter->name);

            addTopLevelItem(new_item);
            add_dummy_to(new_item);
        }

        if ( iter->type == TP_SONG )
        {
            new_item = new QTreeWidgetItem();
            new_item->setText(0, iter->name);
            new_item->setIcon(0, QIcon(":/tr_track.png"));
            add_songdata_toItem(new_item, *iter);
            addTopLevelItem(new_item);
        }
    }

    itemlist.clear();
    headerItem()->setText(0, " " + tr("folders") + " " );
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
}

// tree root items in dir search mode
void qm_libview::get_directories(QString searchfor)
{
    set_column_count(1);

    if (!b_mpd_connected)
    {
        headerItem()->setText(0, " " + tr("not connected") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        return;
    }

    qm_itemList itemlist = mpdCom->get_dirsearchlist(searchfor);

    if (itemlist.empty())
    {
        headerItem()->setText(0, " " + tr("no folders found") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        return;
    }

    itemlist.sort();
    int count = 0;
    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator itr;
    for (itr = itemlist.begin(); itr != itemlist.end(); ++itr)
    {
        if ( itr->type == TP_DIRECTORY )
        {
            new_item = new QTreeWidgetItem();
            new_item->setText(0, itr->name);
            new_item->setIcon(0, QIcon(":/tr_dir.png"));

            new_item->settype(TP_DIRECTORY);
            new_item->setfile(itr->file);
            new_item->setname(itr->name);

            addTopLevelItem(new_item);
            add_dummy_to(new_item);
            count++;
        }
    }

    itemlist.clear();
    QString str = " " + QString::number(count) + " " + tr("folders found") + " ";
    headerItem()->setText(0, str);
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
}

// tree root items in palylist mode
void qm_libview::get_playlists()
{
    set_column_count(1);

    if (!b_mpd_connected)
    {
        headerItem()->setText(0, " " + tr("not connected") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        return;
    }

    qm_itemList itemlist = mpdCom->get_playlistlist();

    if (itemlist.empty())
    {
        headerItem()->setText(0, " " + tr("no playlists found") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        return;
    }

    itemlist.sort();

    int count = 0;
    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if (iter->name.isEmpty())
            iter->name = "?";

        new_item = new QTreeWidgetItem();
        new_item->setText(0, iter->name);
        new_item->setIcon(0, QIcon(":/tr_playlist.png"));

        new_item->settype(TP_PLAYLIST);
        new_item->setfile(iter->file);
        new_item->setname(iter->name);

        addTopLevelItem(new_item);
        add_dummy_to(new_item);

        count ++;
    }
    itemlist.clear();

    QString str = " " + QString::number(count) + " " + tr("playlists") + " ";
    headerItem()->setText(0, str);
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
}


// tree root items in artist mode
void qm_libview::get_artists(QString searchfor)
{
    set_column_count(1);

    if (!b_mpd_connected)
    {
        headerItem()->setText(0, " " + tr("not connected") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        return;
    }

    qm_itemList itemlist;

    if (searchfor == "*^*")
        itemlist = mpdCom->get_artistlist();
    else
        itemlist = mpdCom->get_artist_searchlist(searchfor);

    if (itemlist.empty())
    {
        headerItem()->setText(0, " " + tr("no artists found") + " ");
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        return;
    }

    int count = 0;

    itemlist.sort();
    // skip duplicates!
    QString artist = "*^*";

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if (artist == iter->artist)
            continue;

        artist = iter->artist;

        new_item = new QTreeWidgetItem();
        if (iter->artist.isEmpty())
            new_item->setText(0, "?");
        else
            new_item->setText(0, iter->artist);
        new_item->setIcon(0, QIcon(":/tr_artist.png"));

        new_item->settype(TP_ARTIST);
        new_item->setartist(iter->artist);
        new_item->setalbum(iter->album);
        new_item->setname(iter->name);

        addTopLevelItem(new_item);
        add_dummy_to(new_item);

        count ++;
    }
    itemlist.clear();

    QString str = " " + QString::number(count) + " " + tr("artists") + " ";
    headerItem()->setText(0, str);
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
}

// tree root items in song (searched) mode
void qm_libview::get_songs(QString searchfor)
{
    if (!b_mpd_connected)
    {
        headerItem()->setText(0, " " + tr("not connected") + " " );
        set_column_count(1);
        return;
    }

    qm_itemList itemlist = mpdCom->get_song_searchlist(searchfor);

    if (itemlist.empty())
    {
        headerItem()->setText(0, " " + tr("no titles found") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        set_column_count(1);
        return;
    }

    set_column_count(2);

    int count = 0;
    itemlist.sort();

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        new_item = new QTreeWidgetItem();
        if (iter->title.isEmpty())
        {
            // use file name as title
            int i = (iter->file).lastIndexOf( '/' ) + 1;
            iter->title = (iter->file).right((iter->file).length() - i);
        }
        new_item->setText(0, iter->title);
        if (iter->artist.isEmpty())
            new_item->setText(1, "?");
        else
            new_item->setText(1, iter->artist);

        new_item->setIcon(0, QIcon(":/tr_track.png"));
        add_songdata_toItem(new_item, *iter);
        addTopLevelItem(new_item);
        count ++;
    }
    itemlist.clear();

    QString str = " " + QString::number(count) + " " + tr("songs") + " ";
    headerItem()->setText(0, str);
    headerItem()->setText(1, tr("artist") );
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
}

// tree root items in genre mode
void qm_libview::get_genres(QString searchfor)
{
    set_column_count(1);

    if (!b_mpd_connected)
    {
        headerItem()->setText(0, " " + tr("not connected") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        return;
    }

    qm_itemList itemlist = mpdCom->get_genrelist(searchfor);

    if (itemlist.empty())
    {
        headerItem()->setText(0, " " + tr("no genres found") + " " );
        headerItem()->setIcon(0, QIcon(":/tr_dot.png"));
        return;
    }

    int count = 0;
    itemlist.sort();
    QTreeWidgetItem *new_item;
    QString genre = "*^*";
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if(iter->genre != genre)
        {
            genre = iter->genre;
            new_item = new QTreeWidgetItem();
            if (iter->genre.isEmpty())
                new_item->setText(0, "?");
            else
                new_item->setText(0, iter->genre);
            new_item->settype(TP_GENRE);
            new_item->setgenre(iter->genre);
            new_item->setIcon(0, QIcon(":/tr_genre.png"));
            addTopLevelItem(new_item);
            add_dummy_to(new_item);
            count++;
        }
    }
    itemlist.clear();

    QString str = " " + QString::number(count) + " " + tr("genres") + " ";
    headerItem()->setText(0, str);
    headerItem()->setIcon(0, QIcon(":/tr_check.png"));
}


void qm_libview::expand_album(QTreeWidgetItem * active_item)
{
    qm_itemList itemlist;

    if ( config->browser_libmode == LB_ID_GENRE)
        itemlist = mpdCom->get_songlist(active_item->getalbum, active_item->getartist, active_item->getgenre);
    else
        itemlist = mpdCom->get_songlist(active_item->getalbum, active_item->getartist, "*^*");

    if (itemlist.empty())
        return;

    itemlist.sort();

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if ( iter->type == TP_SONG )
        {
            if (iter->title.isEmpty()) // DEBUG what about iter->name (see Guimup)
            {
                // use file name as title
                int i = (iter->file).lastIndexOf( '/' ) + 1;
                iter->title = (iter->file).right((iter->file).length() - i);
            }
            if (iter->artist.isEmpty())
                iter->artist = "?";
            new_item = new QTreeWidgetItem();
            QString track = "?";
            if (!iter->track.isEmpty())
                track = iter->track;
            if (!iter->disc.isEmpty())
                track = track + " / " + iter->disc;
            new_item->setText(0, track + " " + iter->title);
            new_item->setIcon(0, QIcon(":/tr_track.png"));
            add_songdata_toItem(new_item, *iter);
            active_item->addChild(new_item);
            addTopLevelItem(new_item);
            // show title across all columns:
            setFirstItemColumnSpanned (new_item, true);
        }
    }
    itemlist.clear();
}


void qm_libview::expand_artist(QTreeWidgetItem * active_item)
{
    qm_itemList itemlist = mpdCom->get_albums_for_artist(active_item->getartist);

    if (itemlist.empty())
        return;

    if (config->sort_albums_year)// add the date to the sorter
    {
        std::list<qm_listitemInfo>::iterator it;
        for (it = itemlist.begin(); it != itemlist.end(); ++it)
        {
            it->year  = mpdCom->get_album_year( it->album, it->artist);
            it->sorter = it->year + it->sorter;
        }
    }

    itemlist.sort();
    // skip duplicates!
    QString album = "*^*";

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if (album == iter->album)
            continue;

        album = iter->album;

        new_item =  new QTreeWidgetItem();

        if (config->sort_albums_year)
        {
            if (iter->album.isEmpty())
                new_item->setText(0, iter->year + "?");
            else
                new_item->setText(0, iter->year + iter->album);
        }
        else
        {
            if (iter->album.isEmpty())
                new_item->setText(0, "?");
            else
                new_item->setText(0, iter->album);
        }

        new_item->setIcon(0, QIcon(":/tr_album.png"));

        new_item->settype(TP_ALBUM);
        new_item->setartist(iter->artist);
        new_item->setalbum(iter->album);
        new_item->setname(iter->name);

        active_item->addChild(new_item);
        add_dummy_to(new_item);
    }
    itemlist.clear();
}


void qm_libview::expand_playlist(QTreeWidgetItem * active_item)
{
    qm_itemList itemlist = mpdCom->get_playlist_itemlist(active_item->getfile);

    if (itemlist.empty())
        return;

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        new_item = new QTreeWidgetItem();
        new_item->setfile(iter->file);
        new_item->settype(TP_PLISTITEM);
        new_item->setForeground( 0, QBrush(QColor("#70707A")));
        if (iter->type == TP_STREAM)
        {
            new_item->setIcon(0, QIcon(":/tr_streamplayed.png"));
            new_item->setText(0, iter->file);
        }
        else
        {
            new_item->setIcon(0, QIcon(":/tr_trackplayed.png"));
            new_item->setText(0, iter->artist + " : " + iter->title);
            new_item->settime(iter->time);
            new_item->setartist(iter->artist);
            new_item->setalbum(iter->album);
            new_item->settitle(iter->title);
            new_item->settrack(iter->track);
            new_item->setname(iter->name);
            new_item->setgenre(iter->genre);
        }
        active_item->addChild(new_item);
    }
    itemlist.clear();
}

void qm_libview::expand_genre(QTreeWidgetItem * active_item)
{
    qm_itemList itemlist = mpdCom->get_genre_artistlist(active_item->getgenre);

    if (itemlist.empty())
        return;

    itemlist.sort();

    QTreeWidgetItem *artist_item = new QTreeWidgetItem();
    QString current_artist = "*^*";
    QTreeWidgetItem *album_item = new QTreeWidgetItem();
    QString current_album = "*^*";
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if (iter->artist != current_artist)
        {
            // add artist to active_item
            current_artist = iter->artist;
            artist_item = new QTreeWidgetItem();
            if (iter->artist.isEmpty())
                artist_item->setText(0, "?");
            else
                artist_item->setText(0, iter->artist);
            artist_item->setIcon(0, QIcon(":/tr_artist.png"));
            artist_item->settype(TP_ARTIST);
            artist_item->setartist(iter->artist);
            artist_item->setgenre(iter->genre);
            active_item->addChild(artist_item);

            // add albums to artist_item
            current_album = iter->album;
            album_item = new QTreeWidgetItem();
            if (iter->album.isEmpty())
                album_item->setText(0, "?");
            else
                album_item->setText(0, iter->album);
            album_item->setIcon(0, QIcon(":/tr_album.png"));
            album_item->settype(TP_ALBUM);
            album_item->setartist(iter->artist);
            album_item->setalbum(iter->album);
            album_item->setgenre(iter->genre);
            add_dummy_to(album_item);
            artist_item->addChild(album_item);
        }
        else // add more albums to artist_item
        {
            if (iter->album != current_album)
            {
                current_album = iter->album;
                album_item = new QTreeWidgetItem();
                if (iter->album.isEmpty())
                    album_item->setText(0, "?");
                else
                    album_item->setText(0, iter->album);
                album_item->setIcon(0, QIcon(":/tr_album.png"));
                album_item->settype(TP_ALBUM);
                album_item->setartist(iter->artist);
                album_item->setalbum(iter->album);
                album_item->setgenre(iter->genre);
                add_dummy_to(album_item);
                artist_item->addChild(album_item);
            }
        }
    }
    itemlist.clear();
}


void qm_libview::expand_directory(QTreeWidgetItem * active_item)
{
    qm_itemList itemlist = mpdCom->get_directorylist(active_item->getfile);

    if (itemlist.empty())
        return;
    else
        itemlist.sort();

    QTreeWidgetItem *new_item;
    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if ( iter->type == TP_DIRECTORY )
        {
            new_item = new QTreeWidgetItem();
            new_item->setText(0, iter->name);
            new_item->setIcon(0, QIcon(":/tr_dir.png"));

            new_item->settype(TP_DIRECTORY);
            new_item->setfile(iter->file);
            new_item->setname(iter->name);

            active_item->addChild(new_item);
            add_dummy_to(new_item);
        }

        if ( iter->type == TP_SONG )
        {
            new_item = new QTreeWidgetItem();
            new_item->setText(0, iter->name);
            new_item->setIcon(0, QIcon(":/tr_track.png"));
            add_songdata_toItem(new_item, *iter);
            active_item->addChild(new_item);
        }
    }

    itemlist.clear();
}


void qm_libview::on_mpd_plistsaved()
{
    if (config->browser_libmode == LB_ID_PLIST)
        reload_root();
}


void qm_libview::set_connected(qm_mpdCom* conn, bool isconnected)
{
    mpdCom = conn;
    if (mpdCom == NULL)
        b_mpd_connected = false;
    else
        b_mpd_connected = isconnected;

    if (b_mpd_connected)
    {
        QObject::connect( mpdCom, SIGNAL( sgnl_plistsaved()), this, SLOT( on_mpd_plistsaved()));
        reload_root();

        context_menu->setEnabled(true);
        if (config->mpd_rm_allowed && config->browser_libmode == LB_ID_PLIST)
            a_delete->setEnabled(true);
        else
            a_delete->setEnabled(false);

        if (config->mpd_rescan_allowed)
            a_update->setText(tr("Rescan selection"));
        else
            a_update->setText(tr("Update selection"));
    }
    else
    {
        clear();
        set_column_count(1);
        headerItem()->setIcon(0, QIcon(":/tr_redcross.png"));
        headerItem()->setText(0, " " + tr("not connected") + " ");
        context_menu->setEnabled(false);
    }
}


void qm_libview::contextMenuEvent ( QContextMenuEvent * event )
{
    if (event->reason() == QContextMenuEvent::Mouse)
        context_menu->exec(QCursor::pos());
}


void qm_libview::add_songdata_toItem(QTreeWidgetItem *theItem, qm_listitemInfo data)
{
    theItem->settype(data.type);
    theItem->settime(data.time);
    theItem->setartist(data.artist);
    theItem->setalbum(data.album);
    theItem->settitle(data.title);
    theItem->setfile(data.file);
    theItem->settrack(data.track);
    theItem->setname(data.name);
    theItem->setgenre(data.genre);
}


void qm_libview::add_dummy_to(QTreeWidgetItem *theItem)
{
    QTreeWidgetItem *dummy = new QTreeWidgetItem();
    dummy->setText(0, "*^*");
    theItem->addChild(dummy);
    theItem->setdummy(true);
}


void qm_libview::take_dummy_from(QTreeWidgetItem *theItem)
{
    theItem->takeChild(0);
    theItem->setdummy(false);
}


void qm_libview::reload_root()
{
    clear();

    if (!b_mpd_connected)
        return;

    if (config->browser_libmode == LB_ID_PLIST)
    {
        a_delete->setEnabled(true);
        a_update->setEnabled(false);
    }
    else
    {
        a_delete->setEnabled(false);
        if (config->mpd_rescan_allowed || config->mpd_update_allowed)
            a_update->setEnabled(true);
        else
            a_update->setEnabled(false);
    }

    switch (config->browser_libmode)
    {
        case LB_ID_ARTIST:
        {
            get_artists("*^*");
            break;
        }
        case LB_ID_ALBUM:
        {
            get_albums("*^*", false);
            break;
        }
        case LB_ID_PLIST:
        {
            get_playlists();
            break;
        }
        case LB_ID_DIR:
        {
            get_directories();
            break;
        }
        case LB_ID_DATE:
        {
            get_albums("*^*", true);
            break;
        }
        case LB_ID_YEAR:
        {
            get_albums_year();
            break;
        }
        case LB_ID_GENRE:
        {
            get_genres("*^*");
            break;
        }
        case LB_ID_SEARCH:
        {
            searched_mode_used = config->browser_searchmode;
            switch (searched_mode_used)
            {
                case SR_ID_ARTIST:
                {
                    get_artists(config->search_string);
                    break;
                }
                case SR_ID_ALBUM:
                {
                    get_albums(config->search_string, false);
                    break;
                }
                case SR_ID_TITLE:
                {
                    get_songs(config->search_string);
                    break;
                }
                case SR_ID_GENRE:
                {
                    get_genres(config->search_string);
                    break;
                }
                case SR_ID_DIR:
                {
                    get_directories(config->search_string);
                    break;
                }
                default:
                    break;
            } // end switch searched_mode
            break;
        }
        default:
            break;
    }

    if (!config->lib_auto_colwidth)
    {
        set_panel_maxmode(b_panel_max);
    }
}


void qm_libview::on_append()
{
    modify_playlist(false, false);
}


void qm_libview::on_createnew()
{
    modify_playlist(true, false);
    if (config->auto_playfirst)
        mpdCom->play(true);
}


void qm_libview::on_delete()
{
    if (config->browser_libmode != LB_ID_PLIST || !b_mpd_connected)
        return;

    QList<QTreeWidgetItem*> tobeRemoved = selectedItems();
    if (tobeRemoved.empty())
        return;

    // Ask for confirmation
    if ( QMessageBox::warning( this, tr("Warning"),
                               tr("This will permanently remove the selected playlists!"), tr("Delete"), tr("Cancel"), QString::null, 1, 0 ) )
        return;

    qm_commandList newCommandList;

    QListIterator<QTreeWidgetItem *> itr(tobeRemoved);
    itr.toBack();
    while (itr.hasPrevious())
    {
        QTreeWidgetItem *current = itr.previous();
        qm_mpd_command newCommand;
        newCommand.cmd = CMD_DPL;
        newCommand.file = current->getfile;
        newCommandList.push_back(newCommand);
    }

    mpdCom->execute_cmds( newCommandList, false );
    newCommandList.clear();
    tobeRemoved.clear();
    reload_root();
}


void qm_libview::on_update()
{
    if (topLevelItemCount() == 0 || !b_mpd_connected || (!config->mpd_rescan_allowed && !config->mpd_update_allowed) )
        return;

    QList<QTreeWidgetItem*> tobeUpdated = selectedItems();
    if (tobeUpdated.empty())
        return;

    std::list <QString> updateFilesList;

    QListIterator<QTreeWidgetItem *> itr(tobeUpdated);
    while (itr.hasNext())
    {
        qm_itemList itemlist;
        QTreeWidgetItem *currentItem = itr.next();
        switch (currentItem->gettype)
        {
            case TP_SONG:
            {
                updateFilesList.push_back(currentItem->getfile);
                break;
            }
            case TP_ARTIST:
            {
                if ( config->browser_libmode == LB_ID_GENRE)
                    itemlist = mpdCom->get_songlist("*^*", currentItem->getartist, currentItem->getgenre);
                else
                    itemlist = mpdCom->get_songlist("*^*", currentItem->getartist, "*^*");

                if (!itemlist.empty())
                {
                    std::list<qm_listitemInfo>::iterator iter;
                    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
                    {
                        updateFilesList.push_back(iter->file);
                    }
                }
                itemlist.clear();
                break;
            }
            case TP_ALBUM:
            {
                if ( config->browser_libmode == LB_ID_GENRE)
                    itemlist = mpdCom->get_songlist(currentItem->getalbum, currentItem->getartist, currentItem->getgenre);
                else
                    itemlist = mpdCom->get_songlist(currentItem->getalbum, currentItem->getartist, "*^*");

                if (!itemlist.empty())
                {
                    std::list<qm_listitemInfo>::iterator iter;
                    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
                    {
                        updateFilesList.push_back(iter->file);
                    }
                }
                itemlist.clear();
                break;
            }
            case TP_GENRE:
            {
                itemlist = mpdCom->get_songlist("*^*", "*^*", currentItem->getgenre);
                if (!itemlist.empty())
                {
                    std::list<qm_listitemInfo>::iterator iter;
                    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
                    {
                        updateFilesList.push_back(iter->file);
                    }
                }
                itemlist.clear();
                break;
            }
            case TP_DIRECTORY:
            {
                updateFilesList.push_back(currentItem->getfile);
                break;
            }
            default:
                break;
        }
    }

    tobeUpdated.clear();

    qm_commandList theUpdateList;

    int count = 0;
    updateFilesList.sort();
    QString previous_dir = "";
    std::list<QString>::iterator iter;
    for (iter = updateFilesList.begin(); iter != updateFilesList.end(); ++iter)
    {
        qm_mpd_command newCommand;
        /*
           We only handle entire directories because of an MPD 0.17 bug:
           Rescanning files often results in a partially updated database.
           So we first sort the list (above), next strip the filename off
           (below) and add the dir when it is different from the previous.
        */
        newCommand.file = (*iter).left((*iter).lastIndexOf("/"));
        if (newCommand.file != previous_dir)
        {
            if (config->mpd_rescan_allowed)
            {
                newCommand.cmd = CMD_SCN; // only updates files, even if timestamp has not changed
                theUpdateList.push_back(newCommand);
            }
            if (config->mpd_update_allowed)
            {
                newCommand.cmd = CMD_UPD; // finds additions/removals and files with changed timestamp
                theUpdateList.push_back(newCommand);
            }
            previous_dir = newCommand.file;
            count++;
        }
    }
    updateFilesList.clear();

    if (config->mpd_rescan_allowed)
        printf("Rescanning directories : %i\n", count);
    if (config->mpd_update_allowed)
        printf("Updating directories : %i\n", count);

    int results = 0;
    if (!theUpdateList.empty())
    {
        results = mpdCom->execute_cmds(theUpdateList, false);
        theUpdateList.clear();
        reload_root();
    }

    if (results < count)
        printf("Some update or rescan commands failed!\n");
}


void qm_libview::on_tagedit()
{
    if (currentItem() == NULL)
        return;

    QString dirpath = currentItem()->getfile;
    if (currentItem()->gettype != TP_DIRECTORY)
        dirpath = dirpath.left(dirpath.lastIndexOf("/"));

    QProcess *proc;
    QString full_path = config->mpd_musicpath + dirpath;
    proc->startDetached(config->tag_editor, QStringList() << full_path);
}


void qm_libview::on_opendir()
{
    if (currentItem() == NULL)
        return;

    QString dirpath = currentItem()->getfile;
    if (currentItem()->gettype != TP_DIRECTORY)
        dirpath = dirpath.left(dirpath.lastIndexOf("/"));

    QProcess *proc;
    QString full_path = config->mpd_musicpath + dirpath;
    proc->startDetached(config->file_manager, QStringList() << full_path);
}


void qm_libview::set_config(qm_config *conf)
{
    config = conf;
    fix_header();
    set_auto_columns();
}


void qm_libview::modify_playlist(bool b_newlist,  bool b_insert)
{
    playlist_modification_list.clear();

    if (topLevelItemCount() == 0 || !b_mpd_connected)
        return;

    QList<QTreeWidgetItem*> tobeAdded;


    QTreeWidgetItemIterator sitr(this);
    while (*sitr)
    {
        if ( (*sitr)->isSelected() )
        {
            tobeAdded.push_back( *sitr );
        }
        ++sitr;
    }

    if (tobeAdded.empty())
        return;

    QListIterator<QTreeWidgetItem *> itr(tobeAdded);
    while (itr.hasNext())
    {
        QTreeWidgetItem *currentItem = itr.next();
        switch (currentItem->gettype)
        {
            case TP_STREAM:
            case TP_SONG:
            {
                qm_mpd_command newCommand;
                if (b_insert)
                    newCommand.cmd = CMD_INS;
                else
                    newCommand.cmd = CMD_ADD;
                newCommand.moveTo = plist_droppos;
                newCommand.type = currentItem->gettype;
                newCommand.file = currentItem->getfile;
                newCommand.time = currentItem->gettime;
                newCommand.artist = currentItem->getartist;
                newCommand.album = currentItem->getalbum;
                newCommand.title = currentItem->gettitle;
                newCommand.track = currentItem->gettrack;
                if (b_insert)
                    playlist_modification_list.push_front(newCommand);
                else
                    playlist_modification_list.push_back(newCommand);
                break;
            }
            case TP_ARTIST:
            {
                if ( config->browser_libmode == LB_ID_GENRE)
                    append_to_modlist("*^*", currentItem->getartist, currentItem->getgenre, b_insert);
                else
                    append_to_modlist("*^*", currentItem->getartist, "*^*", b_insert);
                break;
            }
            case TP_ALBUM:
            {
                if ( config->browser_libmode == LB_ID_GENRE)
                    append_to_modlist(currentItem->getalbum, currentItem->getartist, currentItem->getgenre, b_insert);
                else
                    append_to_modlist(currentItem->getalbum, currentItem->getartist, "*^*", b_insert);
                break;
            }
            case TP_GENRE:
            {
                append_to_modlist("*^*", "*^*", currentItem->getgenre, b_insert);
                break;
            }
            case TP_PLAYLIST:
            {
                append_to_modlist_plist(currentItem->getfile, b_insert);
                break;
            }
            case TP_DIRECTORY:
            {
                append_to_modlist_dir(currentItem->getfile, b_insert);
                break;
            }
            default:
                break;
        }
    }

    tobeAdded.clear();

    if (!playlist_modification_list.isEmpty())
    {
        if (b_newlist)
            mpdCom->clear_list();

        mpdCom->execute_cmds(playlist_modification_list, true );

        playlist_modification_list.clear();
        plist_droppos = 0;
    }
}


void qm_libview::append_to_modlist(QString album,QString artist, QString genre, bool b_insert)
{
    qm_itemList itemlist = mpdCom->get_songlist(album, artist, genre);

    if (itemlist.empty())
        return;

    itemlist.sort();

    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        qm_mpd_command newCommand;
        if (b_insert)
            newCommand.cmd = CMD_INS;
        else
            newCommand.cmd = CMD_ADD;
        newCommand.moveTo = plist_droppos;
        newCommand.type =  iter->type;
        newCommand.file =   iter->file;
        newCommand.time =   iter->time;
        newCommand.artist = iter->artist;
        newCommand.album =  iter->album;
        newCommand.title =  iter->title;
        newCommand.track =  iter->track;

        if (b_insert)
            playlist_modification_list.push_front(newCommand);
        else
            playlist_modification_list.push_back(newCommand);
    }

    itemlist.clear();
}

// calls itself when a dir is found within a dir
void qm_libview::append_to_modlist_dir(QString path, bool b_insert)
{
    qm_itemList itemlist = mpdCom->get_directorylist(path);
    itemlist.sort();

    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        if ( iter->type == TP_DIRECTORY )
            append_to_modlist_dir(iter->file, b_insert);
        else
            if ( iter->type == TP_SONG  || iter->type == TP_STREAM )
            {
                qm_mpd_command newCommand;
                if (b_insert)
                    newCommand.cmd = CMD_INS;
                else
                    newCommand.cmd = CMD_ADD;
                newCommand.moveTo = plist_droppos;
                newCommand.type =   iter->type;
                newCommand.file =   iter->file;
                newCommand.time =   iter->time;
                newCommand.artist = iter->artist;
                newCommand.album =  iter->album;
                newCommand.title =  iter->title;
                newCommand.track =  iter->track;
                if (b_insert)
                    playlist_modification_list.push_front(newCommand);
                else
                    playlist_modification_list.push_back(newCommand);
            }
    }
    itemlist.clear();
}


void qm_libview::append_to_modlist_plist(QString playlist, bool b_insert)
{
    qm_itemList itemlist = mpdCom->get_playlist_itemlist(playlist);

    if (itemlist.empty())
        return;

    std::list<qm_listitemInfo>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
        qm_mpd_command newCommand;
        if (b_insert)
            newCommand.cmd = CMD_INS;
        else
            newCommand.cmd = CMD_ADD;
        newCommand.file = iter->file;
        newCommand.moveTo = plist_droppos;

        if (iter->type == TP_STREAM)
        {
            newCommand.artist = "Stream ";
            newCommand.time = 0;
            newCommand.album = "Info in player ";
            newCommand.track = "";
            newCommand.type = TP_STREAM;
            if (b_insert)
                playlist_modification_list.push_front(newCommand);
            else
                playlist_modification_list.push_back(newCommand);
        }
        else
        {
            newCommand.time = iter->time;
            newCommand.type = TP_SONG;
            newCommand.artist = iter->artist;
            newCommand.album = iter->album;
            newCommand.title = iter->title;
            newCommand.track = iter->track;
            if (b_insert)
                playlist_modification_list.push_front(newCommand);
            else
                playlist_modification_list.push_back(newCommand);
        }
    }
}

// drop from lib on plist: find out what was dropped
void qm_libview::on_plist_dropreceived(int pos)
{
    if (pos == -1) // drop on empty list
    {
        plist_droppos = 0;
        modify_playlist(true, false);
    }
    else // drop on existing list
    {
        plist_droppos = pos;
        modify_playlist(false, true);
    }
}


void qm_libview::startDrag(Qt::DropActions da)
{
    QMimeData *data;
    QModelIndexList indexes = selectedIndexes();
    if (indexes.count() > 0)
    {
        data = model()->mimeData(indexes);
        if (data == 0)
            return;
    }
    else
        return;
    QDrag* drag = new QDrag(this);
    drag->setPixmap(QPixmap(":/tr_dragdrop.png"));
    drag->setMimeData(data);
    drag->exec(da, Qt::IgnoreAction);
}

// called when column's size is changed
void qm_libview::columnResized(int colnr, int oldsize, int newsize)
{
    /* note: we use setStretchLastSection(true), so always
       ignore the last column: it's size is not set by the user */
    if (!config->lib_auto_colwidth && column_count > 1 &&  colnr != 2)
    {
        if (b_panel_max)
        {
            if (column_count == 3) // save 1st or 2nd
            {
                if (colnr == 0)
                    config->lib_max_col0 = newsize;
                else
                if (colnr == 1)
                    config->lib_max_col1 = newsize;
            }
            else // save 1st
            if (colnr == 0)
                config->lib_max_col0 = newsize;
        }
        else
        {
            if (column_count == 3) // save 1st or 2nd
            {
                if (colnr == 0)
                    config->lib_min_col0 = newsize;
                else
                if (colnr == 1)
                    config->lib_min_col1 = newsize;
            }
            else // save 1st
            if (colnr == 0)
                config->lib_min_col0 = newsize;
        }
    }
    // and pass it on
    QTreeWidget::columnResized(colnr, oldsize, newsize);
}


void qm_libview::set_panel_maxmode(bool ismax)
{
    b_panel_max = ismax;

    if (config->lib_auto_colwidth || column_count == 1)
        return;
    /* note: we use setStretchLastSection(true), so always
       ignore the last column: it's size is not set by the user */
    if (b_panel_max)
    {
        if (column_count == 3) // set 1st and 2nd
        {
            setColumnWidth(0, config->lib_max_col0);
            setColumnWidth(1, config->lib_max_col1);
        }
        else // set 1st
            setColumnWidth(0, config->lib_max_col0);
    }
    else
    {
        if (column_count == 3) // set 1st and 2nd
        {
            setColumnWidth(0, config->lib_min_col0);
            setColumnWidth(1, config->lib_min_col1);
        }
        else // set 1st
            setColumnWidth(0, config->lib_min_col0);
    }
}


void qm_libview::set_auto_columns()
{
    if (config == NULL)
    {
        libhview->setResizeMode(QHeaderView::Stretch);
        return;
    }

    if (config->lib_auto_colwidth)
    {
        libhview->setResizeMode(QHeaderView::Stretch);
    }
    else
    {
        libhview->setResizeMode(QHeaderView::Interactive);
        set_panel_maxmode(b_panel_max);
    }
}


void qm_libview::set_column_count(int count)
{
    column_count = count;
    setColumnCount(count);
}


qm_libview::~qm_libview()
{}
