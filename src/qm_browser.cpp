/*
*  qm_browser.cpp
*  QUIMUP library & playlist window
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

#include "qm_browser.h"

qm_browser::qm_browser()
{
    b_mpd_connected = false;
    b_startup = true;
    setupUI();
}


void qm_browser::setupUI()
{
    if (objectName().isEmpty())
        setObjectName("browser_window");
        
    QPixmap qpx;
    qpx = QPixmap(":/mn_icon.png");
    setWindowIcon(qpx);
    
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(620, 400));
    setMaximumSize(QSize(32767, 32767));
    
    main_widget = new QWidget();
    vbox_all = new QVBoxLayout(main_widget);
    setCentralWidget(main_widget);
    vbox_all->setMargin(4);
    vbox_all->setSpacing(0);
    
    theSplitter = new QSplitter(this);
    theSplitter->setOrientation(Qt::Horizontal);
    theSplitter->setHandleWidth(8);
	theSplitter->setChildrenCollapsible(false);
    
    left_group = new QLabel(theSplitter);
	left_group->setMinimumSize(QSize(300, 0));
    left_group->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    vbox_lefthalf = new QVBoxLayout(left_group);
    vbox_lefthalf->setSpacing(4);
    vbox_lefthalf->setMargin(2);
    lb_left_title = new QLabel;
    lb_left_title->setTextFormat(Qt::RichText);
    lb_left_title->setAlignment(Qt::AlignCenter);
    lb_left_title->setAutoFillBackground(true);
    lb_left_title->setMargin(1);
    vbox_lefthalf->addWidget(lb_left_title);
    hbox_leftop = new QHBoxLayout();
    hbox_leftop->setSpacing(4);
    hbox_leftop->setMargin(0);
    cb_dbasemode = new QComboBox();
    cb_dbasemode->setMinimumSize(QSize(100, 26));
	cb_dbasemode->setMaximumHeight(26);
    hbox_leftop->addWidget(cb_dbasemode);
    spacer1 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_leftop->addItem(spacer1);
    bt_library = new QPushButton();
    bt_library->setMinimumSize(QSize(24, 22));
	bt_library->setMaximumHeight(22);
    bt_library->setIcon(QIcon(":/tr_expand1.png"));
    bt_library->setFlat(true);
    hbox_leftop->addWidget(bt_library);
    spacer2 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_leftop->addItem(spacer2);
    bt_update = new QPushButton();
    bt_update->setMinimumSize(QSize(100, 26));
	bt_update->setMaximumHeight(26);
    bt_update->setIcon(QIcon(":/tr_update.png"));
    hbox_leftop->addWidget(bt_update);
    vbox_lefthalf->addLayout(hbox_leftop);
    lib_view = new qm_libview(left_group);
    lib_view->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
    lib_view->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn); // avoid problems with viewport()->width();
    lib_view->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    lib_view->setUniformRowHeights(true);
    lib_view->setAlternatingRowColors(true);
    vbox_lefthalf->addWidget(lib_view);
    lb_dbasestats = new QLabel();
    lb_dbasestats->setAlignment(Qt::AlignHCenter);
    lb_dbasestats->setAutoFillBackground(true);
    vbox_lefthalf->addWidget(lb_dbasestats);
    hbox_leftbottom = new QHBoxLayout();
    hbox_leftbottom->setSpacing(4);
    hbox_leftbottom->setMargin(0);
    cb_searchmode = new QComboBox();
    cb_searchmode->setMinimumSize(QSize(100, 26));
	cb_searchmode->setMaximumHeight(26);
    hbox_leftbottom->addWidget(cb_searchmode);
    le_search = new QLineEdit();
	le_search->setMinimumSize(QSize(0, 24));
	le_search->setMaximumHeight(24);
    hbox_leftbottom->addWidget(le_search);
    bt_search = new QPushButton();
    bt_search->setMinimumSize(QSize(100, 26));
	bt_search->setMaximumHeight(26);
    bt_search->setIcon(QIcon(":/tr_pointleft.png"));
    hbox_leftbottom->addWidget(bt_search);
    vbox_lefthalf->addLayout(hbox_leftbottom);
    theSplitter->addWidget(left_group);
    
    right_group = new QLabel(theSplitter);
	right_group->setMinimumSize(QSize(300, 0));
    right_group->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    vbox_righthalf = new QVBoxLayout(right_group);
    vbox_righthalf->setSpacing(4);
    vbox_righthalf->setMargin(2);
    lb_right_title = new QLabel;
    lb_right_title->setTextFormat(Qt::RichText);
    lb_right_title->setAlignment(Qt::AlignCenter);
    lb_right_title->setAutoFillBackground(true);
	lb_right_title->setMargin(1);
    vbox_righthalf->addWidget(lb_right_title);
    hbox_rightop = new QHBoxLayout();
    hbox_rightop->setSpacing(4);
    hbox_rightop->setMargin(0);
    bt_stream = new QPushButton();
    bt_stream->setMinimumSize(QSize(100, 26));
	bt_stream->setMaximumHeight(26);
    bt_stream->setIcon(QIcon(":/tr_stream.png"));
    hbox_rightop->addWidget(bt_stream);
    spacer3 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_rightop->addItem(spacer3);
    bt_playlist = new QPushButton();
    bt_playlist->setMinimumSize(QSize(24, 22));
	bt_playlist->setMaximumHeight(22);
    bt_playlist->setIcon(QIcon(":/tr_expand1.png"));
    bt_playlist->setFlat(true);
    hbox_rightop->addWidget(bt_playlist);
    spacer4 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_rightop->addItem(spacer4);
    bt_options = new QPushButton();
    bt_options->setMinimumSize(QSize(100, 26));
	bt_options->setMaximumHeight(26);
    bt_options->setIcon(QIcon(":/tr_options.png"));
    hbox_rightop->addWidget(bt_options);
    vbox_righthalf->addLayout(hbox_rightop);
    plist_view = new qm_plistview(right_group);
    plist_view->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
    plist_view->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn); // avoid problems with viewport()->width();
    plist_view->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    plist_view->setUniformRowHeights(true);
    plist_view->setAlternatingRowColors(true);
    vbox_righthalf->addWidget(plist_view);
    lb_pliststats = new QLabel();
    lb_pliststats->setAlignment(Qt::AlignHCenter);
    lb_pliststats->setAutoFillBackground(true);
    vbox_righthalf->addWidget(lb_pliststats);
    hbox_rightbottom = new QHBoxLayout();
    hbox_rightbottom->setSpacing(4);
    hbox_rightbottom->setMargin(0);
    cb_selectmode = new QComboBox();
    cb_selectmode->setMinimumSize(QSize(100, 26));
	cb_selectmode->setMaximumHeight(26);
    hbox_rightbottom->addWidget(cb_selectmode);
    le_select = new QLineEdit();
	le_select->setMinimumSize(QSize(0, 24));
	le_select->setMaximumHeight(24);
    hbox_rightbottom->addWidget(le_select);
    bt_select = new QPushButton();
    bt_select->setMinimumSize(QSize(100, 26));
	bt_select->setMaximumHeight(26);
    bt_select->setIcon(QIcon(":/tr_pointleft.png"));
    hbox_rightbottom->addWidget(bt_select);
    vbox_righthalf->addLayout(hbox_rightbottom);
    theSplitter->addWidget(right_group);
    
    vbox_all->addWidget(theSplitter);
    
    cb_dbasemode->addItem ( QIcon(":/tr_artist.png"), tr("Artists"));
    cb_dbasemode->addItem ( QIcon(":/tr_album.png"), tr("Albums"));
    cb_dbasemode->addItem ( QIcon(":/tr_albumdate.png"), tr("Timestamp"));
	cb_dbasemode->addItem ( QIcon(":/tr_albumyear.png"), tr("Year"));
    cb_dbasemode->addItem ( QIcon(":/tr_genre.png"), tr("Genres"));
    cb_dbasemode->addItem ( QIcon(":/tr_playlist.png"), tr("Playlists"));
    cb_dbasemode->addItem ( QIcon(":/tr_dir.png"), tr("Folders"));
    cb_dbasemode->addItem ( QIcon(":/tr_search.png"), tr("Searched"));
    cb_dbasemode->setIconSize(QSize(16, 16));
    
	cb_searchmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Artists"));
    cb_searchmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Albums"));
    cb_searchmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Titles"));
    cb_searchmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Genres"));
	cb_searchmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Folders"));
    cb_searchmode->setIconSize(QSize(10, 10));
	
	cb_selectmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Artists"));
    cb_selectmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Albums"));
    cb_selectmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("Titles"));
    cb_selectmode->addItem ( QIcon(":/tr_dotsmall.png"), tr("All"));
    cb_selectmode->setIconSize(QSize(10, 10));
	
    sloader = new qm_stream_loader();
    
    options_menu = new QMenu();
    
    a_random = new QAction(options_menu);
    a_random->setText(tr("Random playback"));
    a_random->setCheckable(true);
    a_random->setIcon(QIcon(":/tr_random.png"));
    QObject::connect( a_random, SIGNAL( triggered() ), this, SLOT( set_random() ) );
    options_menu->addAction(a_random);
    
    a_repeat = new QAction(options_menu);
    a_repeat->setText(tr("Repeat list/single"));
    a_repeat->setCheckable(true);
    a_repeat->setIcon(QIcon(":/tr_repeat.png"));
    QObject::connect( a_repeat, SIGNAL( triggered() ), this, SLOT( set_repeat() ) );
    options_menu->addAction(a_repeat);
    
    a_single = new QAction(options_menu);
    a_single->setText(tr("Single mode"));
    a_single->setCheckable(true);
    a_single->setIcon(QIcon(":/tr_single.png"));
    QObject::connect( a_single, SIGNAL( triggered() ), this, SLOT( set_single() ) );
    options_menu->addAction(a_single);
    
    a_consume = new QAction(options_menu);
    a_consume->setText(tr("Consume mode"));
    a_consume->setCheckable(true);
    a_consume->setIcon(QIcon(":/tr_consume.png"));
    QObject::connect( a_consume, SIGNAL( triggered() ), this, SLOT( set_consume() ) );
    options_menu->addAction(a_consume);
    
    a_xfade = new QAction(options_menu);
    a_xfade->setText(tr("Cross fade"));
    a_xfade->setCheckable(true);
    a_xfade->setIcon(QIcon(":/tr_xfade.png"));
    QObject::connect( a_xfade, SIGNAL( triggered() ), this, SLOT( set_xfade() ) );
    options_menu->addAction(a_xfade);
    
    QObject::connect(sloader, SIGNAL(streamdownload_done(QStringList)), this, SLOT(on_streamdownload_done(QStringList)));
    QObject::connect(cb_dbasemode, SIGNAL(activated(int)), this, SLOT(on_dbase_newmode(int)));
    QObject::connect(cb_searchmode, SIGNAL(activated(int)), this, SLOT(set_searchmode(int)));
    QObject::connect(cb_selectmode, SIGNAL(activated(int)), this, SLOT(set_selectmode(int)));
    QObject::connect(bt_search, SIGNAL(clicked()), this, SLOT(search_in_lib()));
    QObject::connect(le_search, SIGNAL(returnPressed()), this, SLOT(search_in_lib()));
    QObject::connect(bt_select, SIGNAL(clicked()), this, SLOT(select_in_plist()));
    QObject::connect(le_select, SIGNAL(returnPressed()), this, SLOT(select_in_plist()));
    QObject::connect(bt_update, SIGNAL(clicked()), this, SLOT(on_update_dbase()));
    QObject::connect(bt_stream, SIGNAL(clicked()), this, SLOT(get_the_stream()));
    QObject::connect(bt_options, SIGNAL(clicked()), this, SLOT(on_options_clicked()));
    QObject::connect(plist_view, SIGNAL(sgnl_plist_statistics(QString)), this, SLOT(set_plist_stats(QString)));
    QObject::connect(plist_view, SIGNAL(sgnl_plist_dropreceived(int)), lib_view, SLOT(on_plist_dropreceived(int)));
    QObject::connect(bt_library, SIGNAL(clicked()), this, SLOT(on_library_toggle()));
    QObject::connect(bt_playlist, SIGNAL(clicked()), this, SLOT(on_playlist_toggle()));
    QObject::connect(theSplitter, SIGNAL( splitterMoved( int, int )), this, SLOT( splittermoved() ));
} // setupUi


void qm_browser::get_the_stream()
{
    QString suggested_name = "http://";
    bool ok;
    QString input = QInputDialog::getText(this, tr("Enter URL"),
                                          tr("Location of the stream's playlist file:"), QLineEdit::Normal,
                                          suggested_name, &ok);
                                          
    if ( !ok || input.isEmpty() || input == "http://")
        return;
        
    if (!input.startsWith("http://"))
    {
        QMessageBox::warning( this, tr("Error"), "URL must begin with \"http://\"");
        return;
    }
    
    sloader->download_this(input);
    // and wait for sgnl_on_streamdownload_done
}


void qm_browser::on_streamdownload_done(QStringList result)
{
    qm_commandList cmdList;
    if (result.value(0) == "error")
    {
        QMessageBox::warning( this, tr("Error"), result.value(1));
        return;
    }
    
    if (result.value(0) == "nothing")
    {
        QString message = tr("Expected playlist file was not found.\nDownload aborted.");
        QMessageBox::warning( this, tr("Error"), message);
        return;
    }
    
    
    if (result.value(0) == "timeout")
    {
        QString message = tr("Download takes too long:") + " " + result.value(1) + " " + tr("% in 10 sec.\nDownload aborted.");
        QMessageBox::warning( this, tr("Error"), message);
        return;
    }
    
    for (int i = 2; i < result.size(); ++i)
    {
        qm_mpd_command newCommand;
        newCommand.cmd = CMD_ADD;
        newCommand.file = result.value(i);
        newCommand.moveTo = 0;
        newCommand.artist = "Stream ";
        newCommand.title = result.value(i);
        newCommand.time = 0;
        newCommand.album = "Info in player ";
        newCommand.track = "";
        newCommand.type = TP_STREAM;
        cmdList.push_back(newCommand);
    }
    
    int count = mpdCom->execute_cmds(cmdList, true );
    QString message =  tr("Downloaded file:") + " " + result.value(1) +  "\n"  + QString::number(count) +  " " + tr("items added to the playlist.");
    QMessageBox::information( this, tr("Confirmation"), message);
}


void qm_browser::on_dbase_newmode(int i)
{

    config->browser_libmode = i;
    lib_view->reload_root();
}


void qm_browser::set_searchmode(int i)
{
    config->browser_searchmode = i;
}


void qm_browser::set_selectmode(int i)
{
    config->browser_selectmode = i;
}


void qm_browser::search_in_lib()
{
    config->search_string = le_search->text();
    cb_dbasemode->blockSignals(true);
    cb_dbasemode->setCurrentIndex(LB_ID_SEARCH);
    cb_dbasemode->blockSignals(false);
    on_dbase_newmode(LB_ID_SEARCH);
    lib_view->setFocus();
}


void qm_browser::select_in_plist()
{
    config->select_string = le_select->text();
    plist_view->select_it(cb_selectmode->currentIndex() );
    plist_view->setFocus();
}


void qm_browser::on_options_clicked()
{
    options_menu->exec(QCursor::pos());
}


void qm_browser::retranslateUI()
{
    setWindowTitle(tr("Quimup Media Browser"));
	
    if (config->show_tooltips)
    {
        bt_stream->setToolTip(tr("Enter a URL to an internet audio stream"));
        bt_options->setToolTip(tr("Set playback options"));
        bt_update->setToolTip(tr("Scan the entire music collection for changes"));
        cb_searchmode->setToolTip(tr("Search mode"));
        cb_selectmode->setToolTip(tr("Select mode"));
        cb_dbasemode->setToolTip(tr("Library mode"));
    }
    
    bt_select->setText(tr("Select"));
    bt_search->setText(tr("Search"));
    bt_update->setText(tr("Update"));
    bt_options->setText(tr("Options"));
    bt_stream->setText(tr("Stream"));
    lb_left_title->setText( "<b>" + tr("Library") +  "</b>");
    lb_right_title->setText("<b>" + tr("Playlist") + "</b>");
}


void qm_browser::set_config(qm_config *conf)
{
	config = conf;
    if (config == NULL)
        return;
		
    retranslateUI();
    cb_dbasemode->blockSignals(true);
    cb_dbasemode->setCurrentIndex(config->browser_libmode);
    cb_dbasemode->blockSignals(false);
    cb_searchmode->setCurrentIndex(config->browser_searchmode);
    if (config->browser_libmode != LB_ID_SEARCH)
        config->search_string = "";
    le_search->setText(config->search_string);
    cb_selectmode->setCurrentIndex(config->browser_selectmode);
    config->select_string = "";
    le_select->setText(config->select_string);
    
    // set the splitter pos
    QList<int> List;
    List.append(config->browser_L_splitter);
    List.append(config->browser_R_splitter);
    theSplitter->setSizes(List);
    
    resize(config->browser_W, config->browser_H );
}


void qm_browser::splittermoved()
{
    if (theSplitter->sizes().value(0) != 0 && theSplitter->sizes().value(1) != 0)
    {
        config->browser_L_splitter = theSplitter->sizes().value(0);
        config->browser_R_splitter = theSplitter->sizes().value(1);
    }
}


void qm_browser::hideEvent(QHideEvent *he)
{
    config->browser_X = x();
    config->browser_Y = y();
    config->browser_W = width();
    config->browser_H = height();
    he->accept();
}


void qm_browser::set_plist_stats(QString stats)
{
    lb_pliststats->setText(stats);
}


void qm_browser::set_dbase_stats(QString stats)
{
    lb_dbasestats->setText(stats);
}


void qm_browser::showEvent(QShowEvent *se)
{
    move(config->browser_X, config->browser_Y);
    se->accept();
}


void qm_browser::closeEvent( QCloseEvent *ce )
{
    this->hide();
    ce->ignore(); // only hide, don't close
}


void qm_browser::set_connected(qm_mpdCom* conn, bool isconnected)
{
    mpdCom = conn;
	if (mpdCom == NULL)
		b_mpd_connected = false;
	else
		b_mpd_connected = isconnected;

	if (b_mpd_connected)
	{
		QObject::connect(mpdCom, SIGNAL( sgnl_dbase_statistics(QString)), this, SLOT(set_dbase_stats(QString)));
		QObject::connect(mpdCom, SIGNAL( sgnl_update_ready()), this, SLOT(on_update_ready()));
		
		if (config->mpd_stats_allowed)
			mpdCom->get_statistics();
		else
			set_dbase_stats("not connected");
		
		if (config->mpd_update_allowed)
			bt_update->setEnabled(true);
		else
			bt_update->setEnabled(false);
			
		if (config->mpd_single_allowed)
			a_single->setEnabled(true);
		else
			a_single->setEnabled(false);
			
		if (config->mpd_consume_allowed)
			a_consume->setEnabled(true);
		else
			a_consume->setEnabled(false);
			
		if (config->mpd_xfade_allowed)
			a_xfade->setEnabled(true);
		else
			a_xfade->setEnabled(false);
			
		if (config->mpd_repeat_allowed)
			a_repeat->setEnabled(true);
		else
			a_repeat->setEnabled(false);
			
		if (config->mpd_random_allowed)
			a_random->setEnabled(true);
		else
			a_random->setEnabled(false);
	}
	else
	{
		set_dbase_stats("...");
		set_plist_stats("...");
		bt_update->setEnabled(false);
		a_single->setEnabled(false);
		a_consume->setEnabled(false);
		a_xfade->setEnabled(false);
		a_repeat->setEnabled(false);
		a_random->setEnabled(false);
	}	
}


void qm_browser::on_update_dbase()
{
    if (!config->mpd_update_allowed)
        return;
	
    bt_update->setEnabled(false);
    bt_update->setIcon(QIcon(":/tr_dot.png"));
    mpdCom->update_all();
}


void qm_browser::on_update_ready()
{
    bt_update->setDisabled(false);
    bt_update->setIcon(QIcon(":/tr_update.png"));
    lib_view->reload_root();
}


void qm_browser::set_menu_random(bool isitso)
{
    a_random->blockSignals(true);
    a_random->setChecked(isitso);
    a_random->blockSignals(false);
}


void qm_browser::set_menu_repeat(bool isitso)
{
    a_repeat->blockSignals(true);
    a_repeat->setChecked(isitso);
    a_repeat->blockSignals(false);
}


void qm_browser::set_menu_single(bool isitso)
{
    a_single->blockSignals(true);
    a_single->setChecked(isitso);
    a_single->blockSignals(false);
}


void qm_browser::set_menu_consume(bool isitso)
{
    a_consume->blockSignals(true);
    a_consume->setChecked(isitso);
    a_consume->blockSignals(false);
}

// called by player
void qm_browser::set_menu_xfade(bool isitso)
{
    a_xfade->blockSignals(true);
    a_xfade->setChecked(isitso);
	config->xfade_is_on = isitso;
    a_xfade->blockSignals(false);
}


void qm_browser::set_random()
{
    if (b_mpd_connected)
        mpdCom->set_random(a_random->isChecked());
    else
        set_menu_random(!a_random->isChecked());
}


void qm_browser::set_repeat()
{
    if (b_mpd_connected)
        mpdCom->set_repeat(a_repeat->isChecked());
    else
        set_menu_repeat(!a_repeat->isChecked());
}


void qm_browser::set_single()
{
    if (b_mpd_connected)
        mpdCom->set_single(a_single->isChecked());
    else
        set_menu_single(!a_single->isChecked());
}


void qm_browser::set_consume()
{
    if (b_mpd_connected)
        mpdCom->set_consume(a_consume->isChecked());
    else
        set_menu_consume(!a_consume->isChecked());
}


void qm_browser::set_xfade()
{
    if (b_mpd_connected) //else a_xfade is disabled
    {
        if (a_xfade->isChecked())
		{
            mpdCom->set_xfade(config->xfade_time);
			config->xfade_is_on = true; // settings window needs this
		}
        else
		{
            mpdCom->set_xfade(0);
			config->xfade_is_on = false;
		}
    }
}


void qm_browser::on_playlist_toggle()
{
    if (theSplitter->sizes().value(0) != 0)
    {
        splitter_left = theSplitter->sizes().value(0);
        splitter_right = theSplitter->sizes().value(1);
        
        QList<int> List;
        List.append(0);
        List.append(splitter_left + splitter_right);
        theSplitter->setSizes(List);
        
        left_group->hide();
		plist_view->set_panel_maxmode(true);
        bt_playlist->setIcon(QIcon(":/tr_expand2.png"));
    }
    else
    {
        left_group->show();
        QList<int> List;
        List.append(splitter_left);
        List.append(splitter_right);
        theSplitter->setSizes(List);
        bt_playlist->setIcon(QIcon(":/tr_expand1.png"));
        plist_view->set_panel_maxmode(false);
    }
}


void qm_browser::on_library_toggle()
{
    if (theSplitter->sizes().value(1) != 0)
    {
		db_mode_initial = config->browser_libmode;
        
        splitter_left = theSplitter->sizes().value(0);
        splitter_right = theSplitter->sizes().value(1);
        
        QList<int> List;
        List.append(splitter_left + splitter_right);
        List.append(0);
        theSplitter->setSizes(List);
        
        right_group->hide();
        lib_view->set_panel_maxmode(true);
        bt_library->setIcon(QIcon(":/tr_expand2.png"));
    }
    else
    {
        right_group->show();
        lib_view->set_panel_maxmode(false);
        QList<int> List;
        List.append(splitter_left);
        List.append(splitter_right);
        theSplitter->setSizes(List);
        bt_library->setIcon(QIcon(":/tr_expand1.png"));
    }
}

// hack (splitter does't update its widget sizes when window resizes)
void qm_browser::resizeEvent ( QResizeEvent * rse)
{
    if (b_startup)
    {
        b_startup = false;
        return;
    }
    
    QList<int> List;
    int new_left = (rse->size().width() * theSplitter->sizes().value(0)) / rse->oldSize().width();
    List.append(new_left);
    int new_right =(rse->size().width() * theSplitter->sizes().value(1)) / rse->oldSize().width();
    List.append(new_right);
    theSplitter->setSizes(List);
    
    splittermoved();
    
    rse->accept();
}


void qm_browser::set_colors(QPalette pal_top)
{
    lb_left_title->setPalette(pal_top);
    lb_right_title->setPalette(pal_top);
	//lb_dbasestats->setPalette(pal_bottom);
	//lb_pliststats->setPalette(pal_bottom);
}


qm_browser::~qm_browser()
{}
