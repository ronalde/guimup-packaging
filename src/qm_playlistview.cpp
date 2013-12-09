/*
*  qm_playlistview.cpp
*  QUIMUP playlist listview
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


#include "qm_playlistview.h"

qm_plistview::qm_plistview(QWidget *parent)
{
    setParent(parent);

    if (objectName().isEmpty())
        setObjectName("plist_view");

    // initVars
    dropBeforeIndex = -1;
    line = new QWidget(viewport());
    line->hide();
    line->setAutoFillBackground(true);
    QPalette pal = line->palette();
    pal.setColor(line->backgroundRole(), QColor("#5f8ba9"));
    line->setPalette(pal);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDropIndicatorShown(false);
    setDragEnabled(true);

    current_songPos = -1;
    current_songID = -1;
    current_status = 0;
    b_panel_max = false;
    b_streamplaying = false;
    b_waspurged = true;
    b_mpd_connected = false;
    col_default_fg = this->palette().color(QPalette::Foreground);
    col_played_fg  = QColor("#70707A");
    col_playing_fg = QColor("#003070");

    //setupUI
    setRootIsDecorated(false);
    setItemsExpandable (false);
    setUniformRowHeights(true);
    setAllColumnsShowFocus(true);
    setSelectionMode(ExtendedSelection);
    sortByColumn (-1); // no sorting
    setColumnCount(6);
    QStringList labels;
    labels << "#" << " " << " " + tr("artist") + " " << " " + tr("title") + " " << " " + tr("m:s") + " " << " " + tr("album : nr") + " ";
    setHeaderLabels(labels);
    headerItem()->setIcon(1, QIcon(":/tr_trackplayed.png"));
    QObject::connect( this, SIGNAL( itemDoubleClicked (QTreeWidgetItem *, int)), this, SLOT( on_item_dclicked(QTreeWidgetItem *)));

    context_menu = new QMenu();
    this->setContextMenuPolicy(Qt::DefaultContextMenu);

    a_clearlist = new QAction(context_menu);
    a_clearlist->setText(tr("Clear list"));
    a_clearlist->setIcon(QIcon(":/tr_redcross.png"));
    QObject::connect( a_clearlist, SIGNAL( triggered() ), this, SLOT( clear_it() ) );
    context_menu->addAction(a_clearlist);

    a_delsel = new QAction(context_menu);
    a_delsel->setText(tr("Remove selected"));
    a_delsel->setIcon(QIcon(":/tr_delete.png"));
    QObject::connect( a_delsel, SIGNAL( triggered() ), this, SLOT( delete_it() ) );
    context_menu->addAction(a_delsel);

    QAction *purge = new QAction(context_menu);
    purge->setText(tr("Purge played"));
    purge->setIcon(QIcon(":/tr_purge.png"));
    QObject::connect( purge, SIGNAL( triggered() ), this, SLOT( purge_it() ) );
    context_menu->addAction(purge);

    a_shuffle = new QAction(context_menu);
    a_shuffle->setText(tr("Shuffle list"));
    a_shuffle->setIcon(QIcon(":/tr_shuffle.png"));
    QObject::connect( a_shuffle, SIGNAL( triggered() ), this, SLOT( shuffle_it() ) );
    context_menu->addAction(a_shuffle);

    a_savelist = new QAction(context_menu);
    a_savelist->setText(tr("Save list"));
    a_savelist->setIcon(QIcon(":/tr_save.png"));
    QObject::connect( a_savelist, SIGNAL( triggered() ), this, SLOT( save_it() ) );
    context_menu->addAction(a_savelist);

    a_savesel = new QAction(context_menu);
    a_savesel->setText(tr("Save selected"));
    a_savesel->setIcon(QIcon(":/tr_saveselect.png"));
    QObject::connect( a_savesel, SIGNAL( triggered() ), this, SLOT( save_selected() ) );
    context_menu->addAction(a_savesel);
    context_menu->addSeparator();
}

// http://qt-project.org/faq/answer/is_it_possible_to_reimplement_non-virtual_slots
void qm_plistview::fix_header()
 {
    plisthview = new QHeaderView(Qt::Horizontal, this);
    plisthview->setHighlightSections(false);
    plisthview->setClickable(false);
    plisthview->setDefaultAlignment(Qt::AlignLeft);
    plisthview->setStretchLastSection(true);
    plisthview->setMinimumSectionSize(80);
    setHeader(plisthview);
    plisthview->show();
 }


void qm_plistview::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        on_item_dclicked(currentItem());
        return;
    }

    if (event->key() == Qt::Key_Delete)
    {
        delete_it();
        return;
    }

    if (event->key() == Qt::Key_Space)
    {
        if (current_songPos != -1 && current_songPos < topLevelItemCount() )
            scrollToItem(topLevelItem(current_songPos));
        return;
    }

    if (event->key() == Qt::Key_Home)
    {
        if (topLevelItemCount() > 0)
            scrollToItem(topLevelItem(0));
        return;
    }

    if (event->key() == Qt::Key_End)
    {
        if (topLevelItemCount() > 0)
            scrollToItem(topLevelItem(topLevelItemCount() - 1));
        return;
    }

    if (event->key() == Qt::Key_Down)
    {
        if (currentItem()->get_pos < topLevelItemCount())
            setCurrentItem(itemBelow ( currentItem() ));
        return;
    }

    if (event->key() == Qt::Key_Up)
    {
        if (currentItem()->get_pos > 1)
            setCurrentItem(itemAbove ( currentItem() ));
        return;
    }

}


void qm_plistview::set_playlist_stats(int numTracks, int TotalTime)
{
    QString plist_stats = "<i>" + tr("Tracks") + "</i> " + QString::number(numTracks) + " &middot; <i>" + tr("Total Time") + "</i> " + into_time(TotalTime);
    emit sgnl_plist_statistics(plist_stats);
}


void qm_plistview::on_item_dclicked(QTreeWidgetItem *item)
{
    int itemID = item->get_id;
    if (itemID != -1 && b_mpd_connected)
    {
        // a stream may trigger on_mpd_newlist before set_newsong
        // and reset the list. So set ID and b_streamplaying now.
        current_songID = itemID;
        if (item->get_type == TP_STREAM)
            b_streamplaying = true;
        else
            b_streamplaying = false;
        mpdCom->play_this(itemID);
    }
}


void qm_plistview::set_status(int newStatus)
{
    if (current_songPos == -1 || current_songPos >= topLevelItemCount() )
        return;

    switch (newStatus)
    {
        case 1: // MPD_STATE_STOP
        {
            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_stop.png"));
            if ( topLevelItem(current_songPos)->get_state != STATE_PLAYING )
                for (int i = 0; i < 6; i++)
                    topLevelItem(current_songPos)->setForeground( i, QBrush(col_playing_fg));
            break;
        }

        case 2: // MPD_STATE_PLAY
        {
            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_play.png"));
            for (int i = 0; i < 6; i++)
                topLevelItem(current_songPos)->setForeground( i, QBrush(col_playing_fg));
            topLevelItem(current_songPos)->set_state(STATE_PLAYING);
            break;
        }

        case 3: // MPD_STATE_PAUSE
        {
            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_pause.png"));
            if ( topLevelItem(current_songPos)->get_state != STATE_PLAYING )
                for (int i = 0; i < 6; i++)
                    topLevelItem(current_songPos)->setForeground( i, QBrush(col_playing_fg));
            break;
        }

        default: // 0 (MPD_STATE_UNKNOWN) or -1 (Disconnected)
        {
            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_wait.png"));
            break;
        }
    }
    current_status = newStatus;
}

// called by set_markplayed() from Player upon signal from Settings
void qm_plistview::set_markplayed()
{
    if ( topLevelItemCount() == 0 )
        return;

    if (config->mark_played) // show visual 'played' state
    {
        QTreeWidgetItemIterator itr(this);
        while (*itr)
        {
            if ( (*itr)->get_state == STATE_PLAYED && (*itr)->get_id != current_songID)
            {
                if ((*itr)->get_type == TP_SONG)
                    (*itr)->setIcon(1, QIcon(":/tr_trackplayed.png"));
                else
                {
                    if ((*itr)->get_type == TP_SONGX)
                        (*itr)->setIcon(1, QIcon(":/tr_trackxplayed.png"));
                    else
                        (*itr)->setIcon(1, QIcon(":/tr_streamplayed.png"));
                }
                for (int i = 0; i < 6; i++)
                    (*itr)->setForeground( i, QBrush(col_played_fg));
            }
            ++itr;
        }
    }
    else // remove visual 'played' state
    {
        QTreeWidgetItemIterator itr(this);
        while (*itr)
        {
            if ( (*itr)->get_state == STATE_PLAYED && (*itr)->get_id != current_songID)
            {
                if ((*itr)->get_type == TP_SONG)
                    (*itr)->setIcon(1, QIcon(":/tr_track.png"));
                else
                {
                    if ((*itr)->get_type == TP_SONGX)
                        (*itr)->setIcon(1, QIcon(":/tr_trackx.png"));
                    else
                        (*itr)->setIcon(1, QIcon(":/tr_stream.png"));
                }
                for (int i = 0; i < 6; i++)
                    (*itr)->setForeground( i, QBrush(col_default_fg));
            }
            ++itr;
        }
    }
}


void qm_plistview::set_newsong(int newPos)
{
    if ( topLevelItemCount() == 0 || !(newPos < topLevelItemCount()) )
        return;

    // reset current
    if (current_songPos != -1)
    {
        switch (topLevelItem(current_songPos)->get_state)
        {
            case STATE_PLAYING:
            {
                topLevelItem(current_songPos)->set_state(STATE_PLAYED);
                if (config->mark_played)
                {
                    if (topLevelItem(current_songPos)->get_type == TP_SONG)
                        topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_trackplayed.png"));
                    else
                    {
                        if (topLevelItem(current_songPos)->get_type == TP_SONGX)
                            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_trackx_played.png"));
                        else
                            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_streamplayed.png"));
                    }
                    for(int i = 0; i < 6; i++)
                        topLevelItem(current_songPos)->setForeground( i, QBrush(col_played_fg));

                    break;
                }
                // else no break -> default
            }

            case STATE_PLAYED:
            {
                if (config->mark_played)
                {
                    if (topLevelItem(current_songPos)->get_type == TP_SONG)
                        topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_trackplayed.png"));
                    else
                    {
                        if (topLevelItem(current_songPos)->get_type == TP_SONGX)
                            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_trackx_played.png"));
                        else
                            topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_streamplayed.png"));
                    }
                    for (int i = 0; i < 6; i++)
                        topLevelItem(current_songPos)->setForeground( i, QBrush(col_played_fg));
                    break;
                }
                // else no break -> default
            }

            default: // STATE_NEW or !config->mark_played
            {
                if (topLevelItem(current_songPos)->get_type == TP_SONG)
                    topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_track.png"));
                else
                {
                    if (topLevelItem(current_songPos)->get_type == TP_SONGX)
                        topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_trackx.png"));
                    else
                        topLevelItem(current_songPos)->setIcon(1, QIcon(":/tr_stream.png"));
                }
                for (int i = 0; i < 6; i++)
                    topLevelItem(current_songPos)->setForeground( i, QBrush(col_default_fg));
                break;
            }
        } // end switch
    }

    if (newPos < 0 ) // 'no song'
    {
        current_songID = -1;
        if (topLevelItemCount() > 0)
        {
            current_songPos = 0;
            set_status(-1);
        }
        else
            current_songPos = -1;
        return;
    }

    current_songID = topLevelItem(newPos)->get_id;

    if (topLevelItem(newPos)->get_type == TP_STREAM)
        b_streamplaying = true;
    else
        b_streamplaying = false;

    current_songPos = newPos;

    set_status(current_status);
}


qm_IDlist qm_plistview::get_played_IDs()
{
    qm_IDlist the_list;
    QTreeWidgetItemIterator itr(this);
    while (*itr)
    {
        if ( (*itr)->get_state == STATE_PLAYED)
        {
            the_list.push_back((*itr)->get_id);
        }
        ++itr;
    }
    return the_list;
}


void qm_plistview::on_mpd_newlist(qm_songinfo newPlaylist, int changerID)
{
    // prevent resetting by a stream
    if (b_streamplaying)
    {
        // the current song did indeed reset the list
        if (current_songID == changerID)
        {
            b_waspurged = false;
            return;
        }
    }

    // reset current song
    current_songID = -10;
    // keep current_status: it is needed until the status actualyy changes

    qm_IDlist shadow_list;
    if (!b_waspurged)
        shadow_list= get_played_IDs();// ID-list of the 'played' items

    clear();

    QTreeWidgetItem *new_item;
    int pos = 0;
    int plist_totaltime = 0;

    QList<qm_songInfo>::iterator iter;
    for (iter = newPlaylist.begin(); iter != newPlaylist.end(); ++iter)
    {
        bool b_wasplayed = false;
        if (!b_waspurged)
        {
            QListIterator<int> itr(shadow_list);
            QList<int>::iterator IDitr;
            for (IDitr = shadow_list.begin(); IDitr != shadow_list.end(); ++IDitr)
            {
                if (iter->songID == *IDitr)
                {
                    b_wasplayed = true;
                    break;
                }
            }
        }

        if (!iter->nosong)
        {
            pos++;
            if (iter->type == TP_STREAM)
            {
                QStringList labels;
                labels << " " + QString::number(pos) << "" << " " + tr("Stream") + " " << tr("Info in player") << "  . . .  " << " "+  iter->file + " ";
                new_item =  new QTreeWidgetItem(labels);
                new_item->set_pos(pos);
                if (b_wasplayed)
                {
                    new_item->set_state(STATE_PLAYED);
                    if (config->mark_played)
                    {
                        new_item->setIcon(1, QIcon(":/tr_streamplayed.png"));
                        for (int i = 0; i < 6; i++)
                            new_item->setForeground( i, QBrush(col_played_fg));
                    }
                    else
                        new_item->setIcon(1, QIcon(":/tr_stream.png"));
                }
                else
                {
                    new_item->setIcon(1, QIcon(":/tr_stream.png"));
                    new_item->set_state(STATE_NEW);
                }
                new_item->set_id(iter->songID);
                new_item->set_type(TP_STREAM);
                new_item->set_time(0);
                new_item->set_file(iter->file);
                addTopLevelItem(new_item);
            }
            else
                if (iter->type == TP_SONG || iter->type == TP_SONGX) // should always be true
                {
                    plist_totaltime += iter->time;
                    if (iter->title.isEmpty())
                    {
                        // use file name as title
                        int i = (iter->file).lastIndexOf( '/' ) + 1;
                        iter->title = (iter->file).right((iter->file).length() - i);
                    }
                    if (iter->album.isEmpty())
                        iter->album = "?";
                    if (iter->artist.isEmpty())
                        iter->artist = "?";
                    QStringList labels;
                    QString track = "?";
                    if (!iter->track.isEmpty())
                        track = iter->track;
                    if (!iter->disc.isEmpty())
                        track = track + " / " + iter->disc;
                    labels  << " " + QString::number(pos)
                    << "" // icon
                    << " " + iter->artist + " "
                    << " " + iter->title + " "
                    << " " + into_time(iter->time) + " "
                    << " " + iter->album + " : " + track + " ";
                    new_item =  new QTreeWidgetItem(labels);
                    new_item->set_pos(pos);
                    if (iter->type == TP_SONG)
                    {
                        if (b_wasplayed)
                        {
                            new_item->set_state(STATE_PLAYED);
                            if (config->mark_played)
                            {
                                new_item->setIcon(1, QIcon(":/tr_trackplayed.png"));
                                for (int i = 0; i < 6; i++)
                                    new_item->setForeground( i, QBrush(col_played_fg));
                            }
                            else
                                new_item->setIcon(1, QIcon(":/tr_track.png"));
                        }
                        else
                        {
                            new_item->setIcon(1, QIcon(":/tr_track.png"));
                            new_item->set_state(STATE_NEW);
                        }
                    }
                    else
                    {
                        if (b_wasplayed)
                        {
                            new_item->set_state(STATE_PLAYED);
                            if (config->mark_played)
                            {
                                new_item->setIcon(1, QIcon(":/tr_trackxplayed.png"));
                                for (int i = 0; i < 6; i++)
                                    new_item->setForeground( i, QBrush(col_played_fg));
                            }
                            else
                                new_item->setIcon(1, QIcon(":/tr_trackx.png"));
                        }
                        else
                        {
                            new_item->setIcon(1, QIcon(":/tr_trackx.png"));
                            new_item->set_state(STATE_NEW);
                        }
                    }
                    new_item->set_id(iter->songID);
                    new_item->set_type(iter->type);

                    new_item->set_time(iter->time);
                    new_item->set_file(iter->file);
                    addTopLevelItem(new_item);
                }
        }
    }
    newPlaylist.clear();

    set_playlist_stats(topLevelItemCount(), plist_totaltime);

    set_panel_maxmode(b_panel_max);

    if (topLevelItemCount() > 0)
    {
        current_songPos = 0;
        topLevelItem(0)->setIcon(1, QIcon(":/tr_wait.png"));
    }
    else
        current_songPos = -1;

    b_waspurged = false;
}


QString qm_plistview::into_time(int time)
{
    QString formattedTime = "";
    int hrs, mins, secs;

    hrs = (int) (time / 3600);
    if (hrs > 0)
    {
        formattedTime += QString::number(hrs) + ":";
        mins = (time % 3600)/60;
        if(mins < 10)
            formattedTime += "0";
    }
    else
        mins = (int)(time / 60);

    formattedTime += QString::number(mins) + ":";

    secs = time % 60;
    if(secs < 10)
        formattedTime += "0";
    formattedTime += QString::number(secs);
    return formattedTime;
}


void qm_plistview::select_it(int mode)
{

    QString findThis = config->select_string;

    if (findThis.isEmpty() || topLevelItemCount() == 0)
        return;

    // remove current selections
    QListIterator<QTreeWidgetItem *> itr(selectedItems());
    while (itr.hasNext())
    {
        itr.next()->setSelected(false);
    }

    int column;

    switch (mode)
    {
        case SR_ID_ARTIST:
        {
            column = 2;
            break;
        }
        case SR_ID_TITLE:
        {
            column = 3;
            break;
        }
        case SR_ID_ALBUM:
        {
            column = 5;
            break;
        }
        default: // SR_ID_ALL
            column = 0;
    }

    if (column == 0)
        for(int colm = 2; colm < 6; colm++)
        {
            QList<QTreeWidgetItem *> result = findItems( findThis, Qt::MatchContains|Qt::MatchFixedString, colm );
            QListIterator<QTreeWidgetItem *> it(result);
            while (it.hasNext())
            {
                it.next()->setSelected(true);
            }
            result.clear();
        }
    else
    {
        QList<QTreeWidgetItem *> result = findItems( findThis, Qt::MatchContains|Qt::MatchFixedString, column );
        QListIterator<QTreeWidgetItem *> it(result);
        while (it.hasNext())
        {
            it.next()->setSelected(true);
        }
        result.clear();
    }
}

void qm_plistview::purge_it()
{
    if (topLevelItemCount() == 0 || !b_mpd_connected )
        return;

    qm_commandList newCommandList;

    QTreeWidgetItemIterator itr(this);
    while (*itr)
    {
        if ( (*itr)->get_state == STATE_PLAYED)
        {
            qm_mpd_command newCommand;
            newCommand.cmd = CMD_DEL;
            int ID = (*itr)->get_id;
            newCommand.songid = ID;
            // higher pos first (push_front)
            newCommandList.push_front(newCommand);
        }
        ++itr;
    }

    if (newCommandList.size() > 0)
        mpdCom->execute_cmds( newCommandList, true );

    newCommandList.clear();
}


void qm_plistview::delete_it()
{
    if (topLevelItemCount() == 0 || !b_mpd_connected)
        return;

    qm_commandList newCommandList;

    QTreeWidgetItemIterator itr(this);
    while (*itr)
    {
        if ( (*itr)->isSelected())
        {
            qm_mpd_command newCommand;
            newCommand.cmd = CMD_DEL;
            int ID = (*itr)->get_id;
            newCommand.songid = ID;
            // higher pos first (push_front)
            newCommandList.push_front(newCommand);
        }
        ++itr;
    }

    if (newCommandList.size() > 0)
        mpdCom->execute_cmds( newCommandList, true );

    newCommandList.clear();
}


void qm_plistview::clear_it()
{
    if (topLevelItemCount() == 0 || !b_mpd_connected )
        return;

    b_streamplaying = false;
    clear();
    mpdCom->clear_list();
    current_songID = -1;
    current_songPos = -1;

}


void qm_plistview::shuffle_it()
{
    if (topLevelItemCount() < 2 || !b_mpd_connected )
        return;
    mpdCom->shuffle_list();
}


void qm_plistview::save_it()
{
    if (topLevelItemCount() == 0 )
        return;

    QString suggested_name = (topLevelItem(0)->text(2)).simplified();
    bool ok;
    QString listname = QInputDialog::getText(this, tr("Save"),
                       tr("Enter a name for the playlist"), QLineEdit::Normal,
                       suggested_name, &ok);
    if ( !ok || listname.isEmpty() )
        return;

    QString result = mpdCom->save_playlist(listname);

    if (result.startsWith("Error:"))
    {
        result = result.section("} ",1);
        QMessageBox::warning( this, tr("Error"), result);
    }
    else
        QMessageBox::information( this, tr("Confirmation"), result);
}


void qm_plistview::save_selected()
{
    if (topLevelItemCount() == 0 )
        return;

    QStringList tobeSaved;

    QTreeWidgetItemIterator itr(this);
    while (*itr)
    {
        if ( (*itr)->isSelected() )
        {
            tobeSaved.push_back( (*itr)->get_file );
        }
        ++itr;
    }

    if (tobeSaved.empty())
        return;

    if (tobeSaved.size() == topLevelItemCount())
    {
        save_it();
        return;
    }

    bool ok;
    QString listname = QInputDialog::getText(this, tr("Save"),
                       tr("Enter a name for the playlist"), QLineEdit::Normal,
                       "selection", &ok);
    if ( !ok || listname.isEmpty() )
        return;

    QString result = mpdCom->save_selection(tobeSaved, listname);

    if (result.startsWith("Error:"))
    {
        result = result.section("} ",1);
        QMessageBox::warning( this, tr("Error"), result);
    }
    else
        QMessageBox::information( this, tr("Confirmation"), result);
}


void qm_plistview::set_connected(qm_mpdCom* conn, bool isconnected)
{
    mpdCom = conn;
    if (mpdCom == NULL)
        b_mpd_connected = false;
    else
        b_mpd_connected = isconnected;

    if (b_mpd_connected)
    {
        QObject::connect( mpdCom, SIGNAL( sgnl_plistupdate (qm_songinfo, int)), this, SLOT( on_mpd_newlist(qm_songinfo, int)));

        context_menu->setEnabled(true);

        if (config->mpd_save_allowed)
        {
            a_savelist->setEnabled(true);
            a_savesel->setEnabled(true);
        }
        else
        {
            a_savelist->setEnabled(false);
            a_savesel->setEnabled(false);
        }

        if (config->mpd_shuffle_allowed)
            a_shuffle->setEnabled(true);
        else
            a_shuffle->setEnabled(false);

        if (config->mpd_deleteid_allowed)
            a_delsel->setEnabled(true);
        else
            a_delsel->setEnabled(false);

        if (config->mpd_clear_allowed)
            a_clearlist->setEnabled(true);
        else
            a_clearlist->setEnabled(false);
    }
    else
    {
        clear();
        current_songID = -1;
        current_songPos = -1;
        context_menu->setEnabled(false);
        a_clearlist->setEnabled(false);
        a_delsel->setEnabled(false);
        a_shuffle->setEnabled(false);
        a_savelist->setEnabled(false);
        a_savesel->setEnabled(false);
    }
}


void qm_plistview::contextMenuEvent ( QContextMenuEvent * event )
{
    if (event->reason() == QContextMenuEvent::Mouse)
        context_menu->exec(QCursor::pos());
}


void qm_plistview::set_config(qm_config *conf)
{
    config = conf;
    fix_header();
    set_auto_columns();
}


void qm_plistview::startDrag(Qt::DropActions da)
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


void qm_plistview::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->source() == 0) // drag from outside the app
    {
        if (!e->mimeData()->hasUrls() || !config->mpd_socket_conn)
        {
            e->ignore();
            return;
        }
    }


    QTreeWidgetItem *item = itemAt(e->pos());
    if (item)
    {
        dropBeforeIndex = indexOfTopLevelItem(item);
        showLine_at(visualItemRect(item).top());
    }
    else if (e->pos().y() >= 0)
    {
        dropBeforeIndex = topLevelItemCount();
        showLine_at(visualItemRect(topLevelItem(topLevelItemCount() - 1)).bottom());
    }
    else
    {
        dropBeforeIndex = -1;
        line->hide();
    }
    e->accept();

}

void qm_plistview::dragEnterEvent(QDragEnterEvent *e)
{
    if ( e->source() != 0 ) // source within application
    {
        line->show();
        e->accept();
    }
    else
    {
        if (e->mimeData()->hasUrls() && config->mpd_socket_conn)
        {
            line->show();
            e->accept();
        }
        else
            e->ignore();
    }
}

void qm_plistview::dragLeaveEvent(QDragLeaveEvent *e)
{
    line->hide();
    dropBeforeIndex = -1;

    e->accept();
}

void qm_plistview::on_open_with_droprequest(QDropEvent *e)
{
    if (e->source() != 0) // drag from outside the app
    {
        e->ignore();
        return;
    }

    if (b_mpd_connected && e->mimeData()->hasUrls() && config->mpd_socket_conn)
    {
        int plistsize = topLevelItemCount();
        QList<QUrl> urlList = e->mimeData()->urls();
        int count = 0;
        int last = urlList.size();
        if (last > 0)
        {
            for (int i = 0; i < urlList.size(); ++i)
            {
                QString url = urlList.at(i).path();
                if ( file_check(url) )
                {
                    if ( !url.startsWith("file://", Qt::CaseInsensitive) )
                    {
                        url = "file://" + url;
                    }
                    qm_mpd_command newCommand;
                    newCommand.cmd = CMD_ADD;
                    newCommand.file = url;
                    count++;
                    if (count == last) // last one triggers a playlist update
                        mpdCom->execute_single_command(newCommand, true);
                    else
                        mpdCom->execute_single_command(newCommand, false);
                }
            }
            scrollToItem(topLevelItem(plistsize));
        }
    }

    this->setFocus();
    dropBeforeIndex = -1;
    line->hide();
    e->ignore();
}

 // called by the core
void qm_plistview::on_open_with_request(const QString& inlist)
{
    if (!config->mpd_socket_conn || !b_mpd_connected || mpdCom == NULL)
        return;

    QString uri = inlist;

    bool b_newlist = false;
    if  (uri.startsWith("-play:"))
    {
        b_newlist = true;
        uri = uri.right(uri.length() - 6); // remove "-play:"
        clear_it();
    }

    QStringList split_uri = uri.split( "file://", QString::SkipEmptyParts, Qt::CaseInsensitive );
    int count = 0;
    int last = split_uri.size();

    if (last == 0)
        return;

    if (b_newlist)
        mpdCom->clear_list();

    QStringList::iterator iter;
    for (iter = split_uri.begin(); iter != split_uri.end(); ++iter)
    {
        QString url = *iter;

        if ( file_check(url) )
        {
            if ( !url.startsWith("file://", Qt::CaseInsensitive) )
                url = "file://" + url;
            qm_mpd_command newCommand;
            newCommand.cmd = CMD_ADD;
            newCommand.file = url;
            count ++;
            if (count == last)
                mpdCom->execute_single_command(newCommand, true); // last one triggers a playlist update
            else
                mpdCom->execute_single_command(newCommand, false);
        }
    }

    this->setFocus();

    if (b_newlist)
        mpdCom->play(true);
}


void qm_plistview::dropEvent(QDropEvent *e)
{
    QList<QTreeWidgetItem*> tobeMoved;

    if (e->source() == this) // drag from inside playlist
    {
        QTreeWidgetItemIterator itr(this);
        while (*itr)
        {
            if ( (*itr)->isSelected() )
            {
                tobeMoved.push_back( *itr );
            }
            ++itr;
        }

         if (topLevelItemCount() == 0 || !b_mpd_connected || tobeMoved.empty() || tobeMoved.size() == topLevelItemCount())
        {
            this->setFocus();
            line->hide();
            dropBeforeIndex = -1;
            e->ignore();
            return;
        }

        qm_commandList move_out_List;
        qm_commandList move_in_List;
        int endpos = topLevelItemCount();

        QListIterator<QTreeWidgetItem *> witr(tobeMoved);
        int put_it_here = dropBeforeIndex;
        witr.toBack();
        while (witr.hasPrevious())
        {
            QTreeWidgetItem *current = witr.previous();
            if (current->get_pos <= dropBeforeIndex)
                put_it_here--;
            qm_mpd_command move_out_Command;
            move_out_Command.cmd = CMD_MOV;
            move_out_Command.songid = current->get_id;
            move_out_Command.moveTo = endpos-1;
            move_out_List.push_front(move_out_Command);
        }
        witr.toBack();
        while (witr.hasPrevious())
        {
            QTreeWidgetItem *current = witr.previous();
            qm_mpd_command move_in_Command;
            move_in_Command.cmd = CMD_MOV;
            move_in_Command.songid = current->get_id;
            move_in_Command.moveTo = put_it_here;
            move_in_List.push_back(move_in_Command);
        }
        // first move selection to the end of the list (move-out)
        // next move the items into position (move_in) and update the list
        mpdCom->execute_cmds( move_out_List, false );
        mpdCom->execute_cmds( move_in_List, true );
        move_out_List.clear();
        move_in_List.clear();

        scrollToItem(topLevelItem(put_it_here));
    }
    else
    if (e->source() != this && e->source() != 0) // drag from library
    {
        if (topLevelItemCount() == 0)
        {
            emit sgnl_plist_dropreceived(-1);
        }
        else
        if (dropBeforeIndex != -1)
        {
            emit sgnl_plist_dropreceived(dropBeforeIndex);
            scrollToItem(topLevelItem(dropBeforeIndex));
        }
    }
    else
    if (e->source() == 0) // drag from outside the app
    {
        if (b_mpd_connected && e->mimeData()->hasUrls() && config->mpd_socket_conn)
        {
            QList<QUrl> urlList = e->mimeData()->urls();
            int count = 0;
            int last = urlList.size();

            if (last > 0)
            {
                for (int i = 0; i < urlList.size(); ++i)
                {
                    QString url = urlList.at(i).path();
                    if ( file_check(url) )
                    {
                        if ( !url.startsWith("file://", Qt::CaseInsensitive) )
                        {
                            url = "file://" + url;
                        }
                        qm_mpd_command newCommand;
                        newCommand.cmd = CMD_INS;
                         // + count or order is reversed:
                        newCommand.moveTo = dropBeforeIndex + count;
                        newCommand.file = url;
                        count++;
                        if (count == last) // last one triggers a playlist update
                            mpdCom->execute_single_command(newCommand, true);
                        else
                            mpdCom->execute_single_command(newCommand, false);
                    }
                }
                scrollToItem(topLevelItem(dropBeforeIndex));
            }
        }
    }

    this->setFocus();
    dropBeforeIndex = -1;
    line->hide();
    e->ignore();
}


bool qm_plistview::file_check(const QString &url)
{
    int i = url.lastIndexOf( '/' ) + 1;
    QString fname = url.right(url.length() - i);

    if  (   url.endsWith(".mp3",  Qt::CaseInsensitive) ||
            url.endsWith(".ogg",  Qt::CaseInsensitive) ||
            url.endsWith(".mpc",  Qt::CaseInsensitive) ||
            //url.endsWith(".cue",  Qt::CaseInsensitive) || // playlist: PATH? FIXME
            //url.endsWith(".m3u",  Qt::CaseInsensitive) || // playlist: PATH? FIXME
            url.endsWith(".flac", Qt::CaseInsensitive) ||
            url.endsWith(".ape",  Qt::CaseInsensitive) ||
            url.endsWith(".oga",  Qt::CaseInsensitive) ||
            url.endsWith(".mp2",  Qt::CaseInsensitive) ||
            url.endsWith(".aif",  Qt::CaseInsensitive) ||
            url.endsWith(".aiff", Qt::CaseInsensitive) ||
            url.endsWith(".au",   Qt::CaseInsensitive) ||
            url.endsWith(".wav",  Qt::CaseInsensitive) ||
            url.endsWith(".wv",   Qt::CaseInsensitive) ||
            url.endsWith(".mp4",  Qt::CaseInsensitive) ||
            url.endsWith(".aac",  Qt::CaseInsensitive) ||
            url.endsWith(".mod",  Qt::CaseInsensitive) ||
            url.endsWith(".mov",  Qt::CaseInsensitive) ||
            url.endsWith(".mkv",  Qt::CaseInsensitive) ||
            url.endsWith(".mpg",  Qt::CaseInsensitive) ||
            url.endsWith(".mpeg", Qt::CaseInsensitive) ||
            url.endsWith(".avi",  Qt::CaseInsensitive) )
    {
        printf ("File added : %s\n", (const char*)fname.toUtf8() );
        return true;
    }
    else
    {
        printf ("File skipped : %s\n", (const char*)fname.toUtf8() );
        return false;
    }
}


void qm_plistview::showLine_at(int y)
{
    line->setGeometry(2, y, viewport()->width()-4, 2);
}

void qm_plistview::set_auto_columns()
{
    // we use setStretchLastSection(true) for col 5
    plisthview->setResizeMode(0, QHeaderView::Fixed);
    plisthview->setResizeMode(1, QHeaderView::Fixed);
    plisthview->setResizeMode(4, QHeaderView::Fixed);

    if (config == NULL)
    {
        plisthview->setResizeMode(2, QHeaderView::Stretch);
        plisthview->setResizeMode(3, QHeaderView::Stretch);
        set_panel_maxmode(b_panel_max);
        return;
    }

    if (config->plist_auto_colwidth)
    {
        plisthview->setResizeMode(2, QHeaderView::Stretch);
        plisthview->setResizeMode(3, QHeaderView::Stretch);
        set_panel_maxmode(b_panel_max);
    }
    else
    {
        plisthview->setResizeMode(2, QHeaderView::Interactive);
        plisthview->setResizeMode(3, QHeaderView::Interactive);
        set_panel_maxmode(b_panel_max);
    }
}


void qm_plistview::columnResized(int colnr, int oldsize, int newsize)
{
    /* note: we use setStretchLastSection(true), so ignore the last
       column (5): it's size is not set by the user. Columns 0, 1 and 4
       are always fitted to contents. -> only 2 and 3 need to be handled.*/
    if (!config->plist_auto_colwidth && (colnr == 2 || colnr == 3) )
    {
        if (b_panel_max)
        {
            if (colnr == 2)
                config->plist_max_col2 = newsize;
            else // colnr == 3
                config->plist_max_col3 = newsize;
        }
        else
        {
            if (colnr == 2)
                config->plist_min_col2 = newsize;
            else // colnr == 3
                config->plist_min_col3 = newsize;
        }
    }
    // and pass it on
    QTreeWidget::columnResized(colnr, oldsize, newsize);
}


void qm_plistview::set_panel_maxmode(bool ismax)
{
    b_panel_max = ismax;

    // Resize the 'fixed' columns
    if (topLevelItemCount() == 0)
    {
        // sizeHintForColumn == 0
        plisthview->resizeSection(0, 19);
        plisthview->resizeSection(1, 28);
        plisthview->resizeSection(4, 50);
    }
    else
    {
        setColumnWidth(0, sizeHintForColumn(0));
        setColumnWidth(1, sizeHintForColumn(1));
        setColumnWidth(4, sizeHintForColumn(4));
    }

    if (config->plist_auto_colwidth)
        return;
    // we use setStretchLastSection(true) for col 5
    if (b_panel_max)
    {
        setColumnWidth(2, config->plist_max_col2);
        setColumnWidth(3, config->plist_max_col3);
    }
    else
    {
        setColumnWidth(2, config->plist_min_col2);
        setColumnWidth(3, config->plist_min_col3);
    }
}


qm_plistview::~qm_plistview()
{}
