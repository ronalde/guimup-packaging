/*
*  qm_settings.cpp
*  QUIMUP settings window
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

#include "qm_settings.h"

qm_settings::qm_settings()
{
    setupUI();
    setFixedSize(0,0); // disable resizing
    b_mpd_connected = false;
    b_profile_readonly = false;
    dev1_wasChecked = false;
    dev2_wasChecked = false;
    dev3_wasChecked = false;
    dev4_wasChecked = false;
    dev5_wasChecked = false;
    b_new_custom_colors = false;
}

void qm_settings::setupUI()
{
    if (objectName().isEmpty())
        setObjectName("settings_window");

    QPixmap qpx;
    qpx = QPixmap(":/mn_icon.png");
    setWindowIcon(qpx);
    setWindowTitle(tr("Quimup Settings") );

    main_widget = new QWidget;
    vbox_all = new QVBoxLayout(main_widget);
    setCentralWidget(main_widget);

    tab_widget = new QTabWidget();
    tab_widget->setMinimumSize(QSize(356, 0));
    vbox_all->addWidget(tab_widget);
    hbox_buttons = new QHBoxLayout();
    hbox_buttons->setSpacing(8);
    hbox_buttons->setMargin(4);
    hbox_buttons->setAlignment(Qt::AlignRight);
    bt_apply = new QPushButton();
    bt_apply->setMinimumSize(QSize(110, 28));
    bt_apply->setAutoDefault(false);
    bt_apply->setDefault(false);
    bt_apply->setIcon(QIcon(":/tr_check.png"));
    hbox_buttons->addWidget(bt_apply);
    bt_close = new QPushButton();
    bt_close->setMinimumSize(QSize(110, 28));
    bt_close->setAutoDefault(false);
    bt_close->setDefault(false);
    bt_close->setIcon(QIcon(":/tr_redcross.png"));
    hbox_buttons->addWidget(bt_close);
    vbox_all->addLayout(hbox_buttons);
    // TAB CONNECT
    tab_con = new QWidget();
    vbox_connect = new QVBoxLayout(tab_con);
    vbox_connect->setSpacing(8);
    vbox_connect->setMargin(10);
    cb_autoConn = new QCheckBox();
    hbox_autoconn = new QHBoxLayout();
    hbox_autoconn->setMargin(8);
    hbox_autoconn->addWidget(cb_autoConn);
    vbox_connect->addLayout(hbox_autoconn);
    gbox_connect = new QGroupBox(tab_con);
    grid_connect = new QGridLayout(gbox_connect);
    grid_connect->setSpacing(8);
    grid_connect->setMargin(8);
    lb_profiles = new QLabel();
    grid_connect->addWidget(lb_profiles, 0, 0);
    cm_profiles = new QComboBox();
    cm_profiles->insertItems(0, QStringList() << "0" << "1" << "2" << "3" << "4");
    grid_connect->addWidget(cm_profiles, 0, 1);
    lb_profilename = new QLabel(gbox_connect);
    grid_connect->addWidget(lb_profilename, 1, 0);
    le_profilename = new QLineEdit(gbox_connect);
    grid_connect->addWidget(le_profilename, 1, 1);
    lb_host = new QLabel(gbox_connect);
    grid_connect->addWidget(lb_host, 2, 0);
    le_host_text = new QLineEdit(gbox_connect);
    grid_connect->addWidget(le_host_text, 2, 1);
    lb_port = new QLabel(gbox_connect);
    grid_connect->addWidget(lb_port, 3, 0);
    le_port_text = new QLineEdit(gbox_connect);
    grid_connect->addWidget(le_port_text, 3, 1);
    lb_passwd = new QLabel(gbox_connect);
    grid_connect->addWidget(lb_passwd, 4, 0);
    le_pass_text = new QLineEdit(gbox_connect);
    //le_pass_text->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    grid_connect->addWidget(le_pass_text, 4, 1);
    vbox_connect->addWidget(gbox_connect);
    lb_musicdir = new QLabel(gbox_connect);
    grid_connect->addWidget(lb_musicdir, 5, 0);
    le_musicdir_text = new QLineEdit(gbox_connect);
    grid_connect->addWidget(le_musicdir_text, 5, 1);
    bt_connect = new QPushButton();
    bt_connect->setMinimumSize(QSize(180, 32));
    bt_connect->setIcon(QIcon(":/mn_config.png"));
    grid_connect->addWidget(bt_connect, 6, 1);
    spacer_tab_con =  new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox_connect->addItem(spacer_tab_con);
    gbox_status = new QGroupBox;
    grid_connect_status = new QGridLayout(gbox_status);
    grid_connect_status->setSpacing(8);
    grid_connect_status->setMargin(8);
    lb_conn_icon = new QLabel();
    lb_conn_icon->setPixmap(QPixmap( ":/con_dis.png" ));
    lb_conn_icon->setFixedSize(QSize(32, 32));
    lb_conn_icon->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    grid_connect_status->addWidget(lb_conn_icon, 0, 0);
    lb_conn_status_text = new QLabel();
    lb_conn_status_text->setTextFormat(Qt::RichText);
    lb_conn_status_text->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    grid_connect_status->addWidget(lb_conn_status_text, 0, 1);
    lb_musicdir_icon = new QLabel();
    lb_musicdir_icon->setPixmap(QPixmap( ":/musdir_nocon.png" ));
    lb_musicdir_icon->setFixedSize(QSize(32, 32));
    lb_musicdir_icon->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    grid_connect_status->addWidget(lb_musicdir_icon, 1, 0);
    lb_musicdir_status_text = new QLabel();
    lb_musicdir_status_text->setTextFormat(Qt::RichText);
    lb_musicdir_status_text->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    grid_connect_status->addWidget(lb_musicdir_status_text, 1, 1);
    vbox_connect->addWidget(gbox_status);
    tab_widget->addTab(tab_con, "");
    // TAB SERVER
    tab_server = new QWidget();
    vbox_server = new QVBoxLayout(tab_server);
    vbox_server->setSpacing(6);
    vbox_server->setMargin(10);
    gbox_xfade = new QGroupBox;
    gbox_xfade->setEnabled(false);
    hbox_xfade = new QHBoxLayout(gbox_xfade);
    sb_xfade = new QSpinBox();
    sb_xfade->setMaximumSize(QSize(46, 26));
    sb_xfade->setMaximum(30);
    sb_xfade->setMinimum(0);
    sb_xfade->setSingleStep(1);
    hbox_xfade->addWidget(sb_xfade);
    lb_xfade = new QLabel();
    hbox_xfade->addWidget(lb_xfade);
    vbox_server->addWidget(gbox_xfade);
    gbox_replaygain = new QGroupBox;
    gbox_replaygain->setEnabled(false);
    hbox_replaygain =  new QHBoxLayout(gbox_replaygain);
    hbox_replaygain->setSpacing(8);
    hbox_replaygain->setMargin(8);
    hbox_replaygain->setAlignment(Qt::AlignLeft);
    rb_rpg_off = new QRadioButton("", this);
    hbox_replaygain->addWidget(rb_rpg_off);
    rb_rpg_track = new QRadioButton("", this);
    hbox_replaygain->addWidget(rb_rpg_track);
    rb_rpg_album = new QRadioButton("", this);
    hbox_replaygain->addWidget(rb_rpg_album);
    rb_rpg_auto = new QRadioButton("", this);
    hbox_replaygain->addWidget(rb_rpg_auto);
    vbox_server->addWidget(gbox_replaygain);
    gbox_outputs = new QGroupBox();
    gbox_outputs->setEnabled(false);
    vbox_devices = new QVBoxLayout(gbox_outputs);
    vbox_devices->setSpacing(4);
    vbox_devices->setMargin(8);
    cb_dev1 = new QCheckBox();
    vbox_devices->addWidget(cb_dev1);
    cb_dev2 = new QCheckBox();
    vbox_devices->addWidget(cb_dev2);
    cb_dev3 = new QCheckBox();
    vbox_devices->addWidget(cb_dev3);
    cb_dev4 = new QCheckBox();
    vbox_devices->addWidget(cb_dev4);
    cb_dev5 = new QCheckBox();
    vbox_devices->addWidget(cb_dev5);
    vbox_server->addWidget(gbox_outputs);
    gbox_manage_mpd = new QGroupBox();
    grid_manage = new QGridLayout(gbox_manage_mpd);
    cb_startserver = new QCheckBox();
    grid_manage->addWidget(cb_startserver,0,0);
    grid_manage->setSpacing(4);
    grid_manage->setMargin(4);
    vbox_startmpd = new QVBoxLayout();
    vbox_startmpd->setSpacing(2);
    vbox_startmpd->setMargin(2);
    lb_startcomm = new QLabel();
    vbox_startmpd->addWidget(lb_startcomm);
    le_startcommand = new QLineEdit();
    vbox_startmpd->addWidget(le_startcommand);
    grid_manage->addLayout(vbox_startmpd,0,1);
    cb_killserver = new QCheckBox();
    grid_manage->addWidget(cb_killserver,1,0);
    vbox_killmpd = new QVBoxLayout();
    vbox_killmpd->setSpacing(2);
    vbox_killmpd->setMargin(2);
    lb_killcomm = new QLabel();
    vbox_killmpd->addWidget(lb_killcomm);
    le_killcommand = new QLineEdit();
    vbox_killmpd->addWidget(le_killcommand);
    grid_manage->addLayout(vbox_killmpd,1,1);
    vbox_server->addWidget(gbox_manage_mpd);
    spacer_tab_server =  new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox_server->addItem(spacer_tab_server);
    tab_widget->addTab(tab_server, "");
    // TAB CLIENT
    tab_client = new QWidget();
    vbox_client = new QVBoxLayout(tab_client);
    vbox_client->setSpacing(8);
    vbox_client->setMargin(10);
    // General options
    gbox_general = new QGroupBox();
    vbox_general = new QVBoxLayout(gbox_general);
    vbox_general->setSpacing(4);
    vbox_general->setMargin(4);
    cb_useSysTray = new QCheckBox();
    vbox_general->addWidget(cb_useSysTray);
    cb_showTips = new QCheckBox();
    vbox_general->addWidget(cb_showTips);
    cb_disable_aart = new QCheckBox();
    vbox_general->addWidget(cb_disable_aart);
    vbox_client->addWidget(gbox_general);
    // library
    gbox_library =  new QGroupBox();
    vbox_library = new QVBoxLayout(gbox_library);
    vbox_library->setSpacing(4);
    vbox_library->setMargin(4);
    cb_sortwithyear = new QCheckBox();
    vbox_library->addWidget(cb_sortwithyear);
    cb_ignore_the = new QCheckBox();
    vbox_library->addWidget(cb_ignore_the);
    cb_lib_auto_colwidth = new QCheckBox();
    vbox_library->addWidget(cb_lib_auto_colwidth);
    vbox_client->addWidget(gbox_library);
    // playlist
    gbox_playlist =  new QGroupBox();
    vbox_playlist = new QVBoxLayout(gbox_playlist);
    vbox_playlist->setSpacing(4);
    vbox_playlist->setMargin(4);
    cb_auto_playfirst = new QCheckBox();
    vbox_playlist->addWidget(cb_auto_playfirst);
    cb_mark_played = new QCheckBox();
    vbox_playlist->addWidget(cb_mark_played);
    cb_plist_auto_colwidth = new QCheckBox();
    vbox_playlist->addWidget(cb_plist_auto_colwidth);
    vbox_client->addWidget(gbox_playlist);
    // external programs
    gbox_ext_progs = new QGroupBox();
    grid_extprogs = new QGridLayout(gbox_ext_progs);
    grid_extprogs->setSpacing(6);
    grid_extprogs->setMargin(8);
    lb_artviewer = new QLabel();
    grid_extprogs->addWidget(lb_artviewer, 0, 0);
    le_artviewer = new QLineEdit();
    grid_extprogs->addWidget(le_artviewer, 0, 1);
    lb_tageditor = new QLabel();
    grid_extprogs->addWidget(lb_tageditor, 1, 0);
    le_tageditor = new QLineEdit();
    grid_extprogs->addWidget(le_tageditor, 1, 1);
    lb_filebrowser = new QLabel();
    grid_extprogs->addWidget(lb_filebrowser, 2, 0);
    le_filebrowser = new QLineEdit();
    grid_extprogs->addWidget(le_filebrowser, 2, 1);
    vbox_client->addWidget(gbox_ext_progs);
    spacer_tab_client =  new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox_client->addItem(spacer_tab_client);
    tab_widget->addTab(tab_client, "");
    // TAB STYLE
    tab_style = new QWidget();
    vbox_style = new QVBoxLayout(tab_style);
    vbox_style->setSpacing(8);
    vbox_style->setMargin(10);
    gbox_fonts = new QGroupBox();
    grid_fonts = new QGridLayout(gbox_fonts);
    grid_fonts->setSpacing(8);
    grid_fonts->setMargin(8);
    sb_titlefont = new QSpinBox();
    sb_titlefont->setMaximumSize(QSize(46, 26));
    sb_titlefont->setMaximum(14);
    sb_titlefont->setMinimum(6);
    sb_titlefont->setSingleStep(1);
    grid_fonts->addWidget(sb_titlefont,0,0);
    lb_titlefont = new QLabel();
    lb_titlefont->setTextFormat(Qt::RichText);
    grid_fonts->addWidget(lb_titlefont,0,1);
    sb_infofont = new QSpinBox();
    sb_infofont->setMaximumSize(QSize(46, 26));
    sb_infofont->setMaximum(14);
    sb_infofont->setMinimum(6);
    sb_infofont->setSingleStep(1);
    grid_fonts->addWidget(sb_infofont,1,0);
    lb_infofont = new QLabel();
    grid_fonts->addWidget(lb_infofont,1,1);
    sb_timefont = new QSpinBox();
    sb_timefont->setMaximumSize(QSize(46, 26));
    sb_timefont->setMaximum(14);
    sb_timefont->setMinimum(6);
    sb_timefont->setSingleStep(1);
    grid_fonts->addWidget(sb_timefont,2,0);
    lb_timefont = new QLabel();
    grid_fonts->addWidget(lb_timefont,2,1);
    sb_albumfont = new QSpinBox();
    sb_albumfont->setMaximumSize(QSize(46, 26));
    sb_albumfont->setMaximum(14);
    sb_albumfont->setMinimum(6);
    sb_albumfont->setSingleStep(1);
    grid_fonts->addWidget(sb_albumfont,3,0);
    lb_albumfont = new QLabel();
    grid_fonts->addWidget(lb_albumfont,3,1);
    sb_commentfont = new QSpinBox();
    sb_commentfont->setMaximumSize(QSize(46, 26));
    sb_commentfont->setMaximum(14);
    sb_commentfont->setMinimum(6);
    sb_commentfont->setSingleStep(1);
    grid_fonts->addWidget(sb_commentfont,4,0);
    lb_commentfont = new QLabel();
    grid_fonts->addWidget(lb_commentfont,4,1);
    sb_liblistfont = new QSpinBox();
    sb_liblistfont->setMaximumSize(QSize(46, 26));
    sb_liblistfont->setMaximum(14);
    sb_liblistfont->setMinimum(6);
    sb_liblistfont->setSingleStep(1);
    grid_fonts->addWidget(sb_liblistfont,5,0);
    lb_liblistfont = new QLabel();
    grid_fonts->addWidget(lb_liblistfont,5,1);
    bt_fonts_reset = new QPushButton();
    bt_fonts_reset->setMinimumSize(QSize(130, 24));
    grid_fonts->addWidget(bt_fonts_reset,5,2);
    vbox_style->addWidget(gbox_fonts);
    gbox_colors = new QGroupBox();
    hbox_colors = new QHBoxLayout(gbox_colors);
    hbox_colors->setSpacing(12);
    hbox_colors->setMargin(8);
    vbox_colorchoice = new QVBoxLayout();
    vbox_colorchoice->setAlignment(Qt::AlignBottom);
    vbox_colorchoice->setSpacing(8);
    vbox_colorchoice->setMargin(0);
    rb_sysdef = new QRadioButton();
    vbox_colorchoice->addWidget(rb_sysdef);
    rb_qmcool = new QRadioButton();
    vbox_colorchoice->addWidget(rb_qmcool);
    rb_custom = new QRadioButton();
    vbox_colorchoice->addWidget(rb_custom);
    spacer_l = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox_colorchoice->addItem(spacer_l);
    hbox_colors->addLayout(vbox_colorchoice);
    vbox_colorcustom = new QVBoxLayout();
    vbox_colorcustom->setAlignment(Qt::AlignBottom);
    vbox_colorcustom->setSpacing(8);
    vbox_colorcustom->setMargin(0);
    cb_color_browser = new QCheckBox();
    vbox_colorcustom->addWidget(cb_color_browser);
    cb_color_albinfo = new QCheckBox();
    vbox_colorcustom->addWidget(cb_color_albinfo);
    spacer_r = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox_colorcustom->addItem(spacer_r);
    vbox_colorlabels  = new QVBoxLayout();
    vbox_colorlabels->setMargin(0);
    vbox_colorlabels->setSpacing(0);
    lb_custcolor_main = new QLabel();
    lb_custcolor_main->setTextFormat(Qt::RichText);
    lb_custcolor_main->setFrameShape(QFrame::StyledPanel);
    lb_custcolor_main->setFrameShadow(QFrame::Sunken);
    lb_custcolor_main->setAlignment(Qt::AlignCenter);
    lb_custcolor_main->setTextFormat(Qt::RichText);
    lb_custcolor_main->setMinimumSize(QSize(120, 30));
    lb_custcolor_main->setMaximumSize(QSize(180, 30));
    lb_custcolor_main->setAutoFillBackground(true);
    vbox_colorlabels->addWidget(lb_custcolor_main);
    lb_custcolor_album = new QLabel();
    lb_custcolor_album->setTextFormat(Qt::RichText);
    lb_custcolor_album->setFrameShape(QFrame::StyledPanel);
    lb_custcolor_album->setFrameShadow(QFrame::Sunken);
    lb_custcolor_album->setAlignment(Qt::AlignCenter);
    lb_custcolor_album->setTextFormat(Qt::RichText);
    lb_custcolor_album->setMinimumSize(QSize(120, 30));
    lb_custcolor_album->setMaximumSize(QSize(180, 30));
    lb_custcolor_album->setAutoFillBackground(true);
    vbox_colorlabels->addWidget(lb_custcolor_album);
    vbox_colorcustom->addLayout(vbox_colorlabels);
    bt_cust_color = new QPushButton();
    bt_cust_color->setMinimumSize(QSize(130, 24));
    vbox_colorcustom->addWidget(bt_cust_color);
    hbox_colors->addLayout(vbox_colorcustom);
    vbox_style->addWidget(gbox_colors);
    spacer_tab_style =  new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox_style->addItem(spacer_tab_style);
    tab_widget->addTab(tab_style, "");
    // TAB ABOUT
    tb_about = new QWidget();
    vbox_about = new QVBoxLayout(tb_about);
    vbox_about->setSpacing(8);
    vbox_about->setMargin(10);
    hbox_about_top = new QHBoxLayout();
    hbox_about_top->setSpacing(10);
    hbox_about_top->setMargin(8);
    px_about = new QLabel();
    px_about->setFixedSize(32, 32);
    px_about->setPixmap(QPixmap( ":/mn_icon.png" ));
    hbox_about_top->addWidget(px_about);
    lb_version = new QLabel();
    lb_version->setTextFormat(Qt::RichText);
    lb_version->setText("<b>Quimup</b>");
    hbox_about_top->addWidget(lb_version);
    vbox_about->addLayout(hbox_about_top);
    txt_about = new QTextEdit();
    txt_about->setReadOnly(true);
    vbox_about->addWidget(txt_about);
    tab_widget->addTab(tb_about, "");

    colorpicker = new qm_colorpicker();
    QObject::connect(bt_close, SIGNAL(clicked()), this, SLOT(cancel_changes()));
    QObject::connect(bt_apply, SIGNAL(clicked()), this, SLOT(apply_settings()));
    QObject::connect(cm_profiles, SIGNAL(currentIndexChanged(int)), this, SLOT(set_profile(int)));
    QObject::connect(bt_connect, SIGNAL(clicked()), this, SLOT(server_connect()));
    QObject::connect(bt_fonts_reset, SIGNAL(clicked()), this, SLOT(reset_fonsizes()));
    QObject::connect(bt_cust_color, SIGNAL(clicked()), this, SLOT(show_colorpicker()));
    QObject::connect(colorpicker, SIGNAL(sgnl_newcolors(QColor, QColor)), this, SLOT(on_new_customcolors(QColor, QColor)));
} // setupUi


void qm_settings::set_profile(int profile)
{
    if ( profile != 0 && b_profile_readonly )
    {
        le_profilename->setReadOnly(false);
        le_port_text->setReadOnly(false);
        le_host_text->setReadOnly(false);
        le_pass_text->setReadOnly(false);
        le_musicdir_text->setReadOnly(false);
        b_profile_readonly = false;
    }

    switch (profile)
    {
        case 0:
        {
            if (!b_profile_readonly)
            {
                b_profile_readonly = true;
                le_profilename->setReadOnly(true);
                le_port_text->setReadOnly(true);
                le_host_text->setReadOnly(true);
                le_pass_text->setReadOnly(true);
                le_musicdir_text->setReadOnly(true);
            }

            le_profilename->setText("Autodetect");
            le_port_text->setText(QString::number(config->MPD_port_0));
            le_host_text->setText(config->MPD_host_0);
            le_pass_text->setText(config->MPD_password_0);
            le_musicdir_text->setText(config->mpd_musicpath);

            break;
        }
        case 1:
        {
            le_profilename->setText(config->Profilename_1);
            le_port_text->setText(QString::number(config->MPD_port_1));
            le_host_text->setText(config->MPD_host_1);
            le_pass_text->setText(config->MPD_password_1);
            le_musicdir_text->setText(config->MPD_musicdir_1);
            break;
        }
        case 2:
        {
            le_profilename->setText(config->Profilename_2);
            le_port_text->setText(QString::number(config->MPD_port_2));
            le_host_text->setText(config->MPD_host_2);
            le_pass_text->setText(config->MPD_password_2);
            le_musicdir_text->setText(config->MPD_musicdir_2);
            break;
        }
        case 3:
        {
            le_profilename->setText(config->Profilename_3);
            le_port_text->setText(QString::number(config->MPD_port_3));
            le_host_text->setText(config->MPD_host_3);
            le_pass_text->setText(config->MPD_password_3);
            le_musicdir_text->setText(config->MPD_musicdir_3);
            break;
        }
        case 4:
        {
            le_profilename->setText(config->Profilename_4);
            le_port_text->setText(QString::number(config->MPD_port_4));
            le_host_text->setText(config->MPD_host_4);
            le_pass_text->setText(config->MPD_password_4);
            le_musicdir_text->setText(config->MPD_musicdir_4);
            break;
        }
        default:
            break;
    }
}


void qm_settings::retranslateUI()
{
    if (config->show_tooltips)
    {
        cb_autoConn->setToolTip(tr("Automatically connect to MPD on start up") );
        le_profilename->setToolTip(tr("A description of this profile") );
        le_port_text->setToolTip(tr("Higher than 1024, usually 6600") );
        le_host_text->setToolTip(tr("\"/home/USER/.mpd/socket\", \"localhost\", or a network address")) ;
        le_killcommand->setToolTip("<nobr>\"mpd --kill\"</nobr> " + tr("or \"killall mpd\""));
        cm_profiles->setToolTip(tr("Connection profiles") );
        le_startcommand->setToolTip(tr("\"mpd\" or \"/usr/bin/mpd\""));
        bt_cust_color->setToolTip(tr("Select custom colors"));
        le_artviewer->setToolTip(tr("For example: \"gwenview\" or \"eog\""));
        le_tageditor->setToolTip(tr("For example: \"kid3\" or \"easytag\""));
        le_filebrowser->setToolTip(tr("For example: \"dolphin\" or \"nautilus\""));
        lb_xfade->setToolTip(tr("Turn on-off in the media browser window (options menu)"));
        bt_close->setToolTip(tr("Close this window (does not apply the settings)"));
        bt_apply->setToolTip(tr("Apply current settings (does not close this window)"));
        bt_connect->setToolTip(tr("Apply profile and connect to MPD"));
        lb_musicdir_icon->setToolTip(tr("Accessibility status of MPD's music directory"));
        lb_conn_icon->setToolTip(tr("Status of the connection with MPD"));
        QString s_outputs = tr("Output, as defined in MPD's configuration");
        cb_dev1->setToolTip(s_outputs);
        cb_dev2->setToolTip(s_outputs);
        cb_dev3->setToolTip(s_outputs);
        cb_dev4->setToolTip(s_outputs);
        cb_dev5->setToolTip(s_outputs);

    }
    bt_close->setText(tr("Close"));
    bt_apply->setText(tr("Apply"));
    bt_connect->setText(tr("Connect"));
    lb_profiles->setText(tr("Profile"));
    gbox_connect->setTitle(tr("Connection profile"));
    lb_profilename->setText(tr("Description:"));
    lb_passwd->setText(tr("Password:"));
    lb_port->setText(tr("Port:"));
    lb_host->setText(tr("Host:"));
    lb_musicdir->setText(tr("Music dir:"));
    cb_autoConn->setText(tr("Autoconnect"));
    gbox_status->setTitle(tr("Status"));
    gbox_replaygain->setTitle(tr("Replaygain mode"));
    rb_rpg_off->setText(tr("Off"));
    rb_rpg_track->setText(tr("Track"));
    rb_rpg_album->setText(tr("Album"));
    rb_rpg_auto->setText(tr("Auto"));
    gbox_outputs->setTitle(tr("Ouput devices"));
    gbox_general->setTitle(tr("General"));
    gbox_library->setTitle(tr("Library"));
    gbox_playlist->setTitle(tr("Playlist"));
    gbox_manage_mpd->setTitle(tr("Manage MPD"));
    lb_killcomm->setText(tr("command:"));
    le_killcommand->setText("mpd --kill");
    cb_startserver->setText(tr("Start with quimup"));
    lb_startcomm->setText(tr("command:"));
    le_startcommand->setText("mpd");
    cb_killserver->setText(tr("Quit with quimup"));
    gbox_fonts->setTitle(tr("Font sizes"));
    gbox_xfade->setTitle(tr("Cross fade"));
    lb_xfade->setText(tr("Time (sec).") + "<i> " + tr("Turn on-off in playlist") + "</i>");
    lb_titlefont->setText("&#187; " + tr("Artist : Title") + " &#171;");
    lb_infofont->setText(tr("Codec properties"));
    lb_timefont->setText(tr("Time [total time]"));
    lb_albumfont->setText(tr("Album and year"));
    lb_commentfont->setText(tr("Comment"));
    lb_liblistfont->setText(tr("Media Browser"));
    bt_fonts_reset->setText(tr("Reset"));
    gbox_colors->setTitle(tr("Colors"));
    rb_sysdef->setText(tr("System colors"));
    rb_qmcool->setText(tr("Quimup colors"));
    rb_custom->setText(tr("Custom colors:"));
    bt_cust_color->setText(tr("Set colors"));
    lb_custcolor_main->setText("<b>&#187; " + tr("custom") + " &#171;</b>");
    lb_custcolor_album->setText(tr("colors"));
    cb_useSysTray->setText(tr("Use system tray icon (restart)"));
    cb_showTips->setText(tr("Show tooltips (restart)"));
    cb_sortwithyear->setText(tr("In artist mode: Sort albums by year"));
    cb_disable_aart->setText(tr("Disable album art"));
    cb_ignore_the->setText(tr("Ignore leading 'The' when sorting artists"));
    cb_lib_auto_colwidth->setText(tr("Automatic column width"));
    cb_auto_playfirst->setText(tr("Start first song in a new playlist"));
    cb_mark_played->setText(tr("Mark songs that have played"));
    cb_plist_auto_colwidth->setText(tr("Automatic column width"));
    cb_color_browser->setText(tr("Apply to browser"));
    cb_color_albinfo->setText(tr("Apply to album info"));

    gbox_ext_progs->setTitle(tr("External programs"));
    lb_artviewer->setText(tr("Album art viewer"));
    lb_tageditor->setText(tr("Audio tag editor"));
    lb_filebrowser->setText(tr("File manager"));
    tab_widget->setTabText(tab_widget->indexOf(tab_con), tr("Connect") + " ");
    tab_widget->setTabText(tab_widget->indexOf(tab_server), tr("Server") + " ");
    tab_widget->setTabText(tab_widget->indexOf(tab_client), tr("Client") + " ");
    tab_widget->setTabText(tab_widget->indexOf(tab_style), tr("Style") + " ");
    tab_widget->setTabText(tab_widget->indexOf(tb_about), tr("About") + " ");
    QString s_tips     = "<font face=\"Sans Serif\"><p align=\"center\"><br><b>Q</b>t <b>U</b>ser <b>I</b>nterface for the <b>MU</b>sic <b>P</b>layer daemon<br><br>\251 2008-2013 Johan Spee<br>quimup@coonsden.com<br><br><font color=\"darkblue\">";
            s_tips    += tr("License")
                       + "<div align=\"justify\"> <font size=\"-1\">"
                       + "This program is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but without any warrenty; without even the implied warranty of merchantability or fitness for a particular purpose. See the GNU General Public License for details."
                       + "</font></font><br></div><b>"
                       + tr("Tips")
                       + "</b><br> <div align=\"justify\">"
                       + tr("To show albumart or use external programs Quimup must be able to access MPD's <b>music directory</b> (the current status of which is shown in the 'Connect' tab). The directory is requested directly from MPD 0.17 but with earlier versions must be read from mpd.conf. To avoid problems set MPDCONF to <i>/home/USER/.mpd/mpd.conf</i> in /etc/default/mpd.")
                       + "<br><br>"
                       + tr("To make sure you have all necessary <b>user rights</b> it is recommended to set START_MPD to <i>false</i> in /etc/default/mpd and let Quimup start MPD (see the 'Server' tab).")
                       + "<br><br>"
                       + tr("Right-clicking the player brings up a menu where you can start <b>external programs</b> that can be set here, in the Client tab. Note that the programs must be able to access MPD's music directory (see above).")
                       + "<br><br>"
                       + tr("Clicking the time counter toggles the <b>time mode</b>, i.e. <i>time elapsed</i> and <i>time remaining</i>.")
                       + "<br><br>"
                       + tr("Clicking the middle mouse-button on the system-tray icon toggles <b>play / pause.")
                       + "</b><br><br>"
                       + tr("The <b>volume</b> can be controlled with the mouse wheel when the cursor is on the system-tray icon - and by using the slider in the player.")
                       + "<br><br>"
                       + tr("Pressing the spacebar in the playlist scrolls the <b>current song</b> into view.")
                       + "<br><br>"
                       + tr("A <b>scrolling title</b> speeds up (quite a bit) when the mouse cursor is moved over it.")
                       + "<br><br>"
                       + tr("<b>Album-art</b> must be located in the same directory with the music file. The program looks for a filename that matches")
                       + " <i>*albumart*, *folder*, *front*, *cover* or *</i> "
                       + tr("(in that order). Note that Quimup must be able to access MPD's music directory (see above).")
                       + "<br><br>"
                       + tr("<b>Selecting</b> an item in the library tree will disable the selection of subitems. A selected artist's albums, for instance, can not be selected because the artist already includes those albums.")
                       + "<br><br>"
                       + tr("Quimup will accept files that are <b>drag & dropp</b>ed on the player or the playlist, or that are added using '<b>open with</b>' in your file manager - but only if it is connected to MPD using a socket (both <i>bind_to_address</i> in mpd.conf and <i>host</i> in Quimup's connection profile must point to <i>/home/USER/.mpd/socket</i>).")
                       + "<br><br>"
                       + tr("Run <i>quimup -help</i> in a terminal for a list of the <b>command line parameters</b>.")
                       + "<br><br></div><b><font color=\"darkblue\">"
                       + tr("Enjoy!")
                       + "<br><br></b></p></font></font>";
    txt_about->setText(s_tips);
} // retranslateUI


void qm_settings::reset_fonsizes()
{
    sb_titlefont->setValue(10);
    sb_infofont->setValue(8);
    sb_timefont->setValue(8);
    sb_albumfont->setValue(8);
    sb_commentfont->setValue(8);
    sb_liblistfont->setValue(9);
}


void qm_settings::cancel_changes()
{
    this->close();
}

void qm_settings::apply_settings()
{
    set_outputs();
    apply_fontsizes();
    apply_colors();

    config->xfade_time = sb_xfade->value();
    if (config->xfade_is_on && b_mpd_connected)
        mpdCom->set_xfade(config->xfade_time);

    config->auto_connect = cb_autoConn->isChecked();
    config->profile = cm_profiles->currentIndex();

    switch (config->profile)
    {
        case 1:
        {
            config->Profilename_1 = (le_profilename->text()).simplified();
            config->MPD_port_1 = (le_port_text->text()).toInt();
            config->MPD_host_1 = (le_host_text->text()).simplified();
            config->MPD_password_1 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_1 = (le_musicdir_text->text()).simplified();
            break;
        }
        case 2:
        {
            config->Profilename_2 = (le_profilename->text()).simplified();
            config->MPD_port_2 = (le_port_text->text()).toInt();
            config->MPD_host_2 = (le_host_text->text()).simplified();
            config->MPD_password_2 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_2 = (le_musicdir_text->text()).simplified();
            break;
        }
        case 3:
        {
            config->Profilename_3 = (le_profilename->text()).simplified();
            config->MPD_port_3 = (le_port_text->text()).toInt();
            config->MPD_host_3 = (le_host_text->text()).simplified();
            config->MPD_password_3 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_3 = (le_musicdir_text->text()).simplified();
            break;
        }
        case 4:
        {
            config->Profilename_4 = (le_profilename->text()).simplified();
            config->MPD_port_4 = (le_port_text->text()).toInt();
            config->MPD_host_4 = (le_host_text->text()).simplified();
            config->MPD_password_4 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_4 = (le_musicdir_text->text()).simplified();
            break;
        }
        default:
            break;
    }

    config->use_trayicon = cb_useSysTray->isChecked();
    config->show_tooltips = cb_showTips->isChecked();
    config->sort_albums_year = cb_sortwithyear->isChecked();
    config->disable_albumart = cb_disable_aart->isChecked();
    config->ignore_leading_the = cb_ignore_the->isChecked();
    if (config->plist_auto_colwidth != cb_plist_auto_colwidth->isChecked())
    {
        config->plist_auto_colwidth = cb_plist_auto_colwidth->isChecked();
        emit sgnl_plist_auto_cols(); //to player (->browser->plistview)
    }
    if (config->lib_auto_colwidth != cb_lib_auto_colwidth->isChecked())
    {
        config->lib_auto_colwidth = cb_lib_auto_colwidth->isChecked();
        emit sgnl_lib_auto_cols(); //to player (->browser->libview)
    }

    bool b_set_rpgain = false;

    if (rb_rpg_off->isChecked() &&  config->rpgain_mode != "off")
    {
        config->rpgain_mode = "off";
        b_set_rpgain = true;
    }
    else
    if (rb_rpg_track->isChecked() && config->rpgain_mode != "track")
    {
        config->rpgain_mode = "track";
        b_set_rpgain = true;
    }
    else
    if (rb_rpg_album->isChecked() && config->rpgain_mode != "album")
    {
        config->rpgain_mode = "album";
        b_set_rpgain = true;
    }
    else
    if (rb_rpg_auto->isChecked() && config->rpgain_mode != "auto")
    {
        config->rpgain_mode = "auto";
        b_set_rpgain = true;
    }

    if (b_set_rpgain)
        set_replaygain_mode();

    config->auto_playfirst = cb_auto_playfirst->isChecked();
    if (config->mark_played != cb_mark_played->isChecked())
    {
        config->mark_played = cb_mark_played->isChecked();
        emit sgnl_markplayed(); //to player (->browser->plistview)
    }
    config->art_viewer = (le_artviewer->text()).simplified();
    config->tag_editor = (le_tageditor->text()).simplified();
    config->file_manager = (le_filebrowser->text()).simplified();

    config->quitMPD_onquit = cb_killserver->isChecked();
    config->onquit_mpd_command = (le_killcommand->text()).simplified();

    config->startMPD_onstart = cb_startserver->isChecked();
    config->onstart_mpd_command = (le_startcommand->text()).simplified();

    config->save_config();
}


void qm_settings::server_connect()
{
    if (mpdCom == NULL)
        return;

    config->auto_connect = cb_autoConn->isChecked();
    config->profile = cm_profiles->currentIndex();
    switch (config->profile)
    {
        case 1:
        {
            config->Profilename_1 = (le_profilename->text()).simplified();
            config->MPD_port_1 = (le_port_text->text()).toInt();
            config->MPD_host_1 = (le_host_text->text()).simplified();
            config->MPD_password_1 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_1 = (le_musicdir_text->text()).simplified();

            break;
        }
        case 2:
        {
            config->Profilename_2 = (le_profilename->text()).simplified();
            config->MPD_port_2 = (le_port_text->text()).toInt();
            config->MPD_host_2 = (le_host_text->text()).simplified();
            config->MPD_password_2 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_2 = (le_musicdir_text->text()).simplified();
            break;
        }
        case 3:
        {
            config->Profilename_3 = (le_profilename->text()).simplified();
            config->MPD_port_3 = (le_port_text->text()).toInt();
            config->MPD_host_3 = (le_host_text->text()).simplified();
            config->MPD_password_3 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_3 = (le_musicdir_text->text()).simplified();
            break;
        }
        case 4:
        {
            config->Profilename_4 = (le_profilename->text()).simplified();
            config->MPD_port_4 = (le_port_text->text()).toInt();
            config->MPD_host_4 = (le_host_text->text()).simplified();
            config->MPD_password_4 = (le_pass_text->text()).simplified();
            config->MPD_musicdir_4 = (le_musicdir_text->text()).simplified();
            break;
        }
        default:
            break;
    }

    mpdCom->configure();
    mpdCom->mpd_connect();

    if (config->profile == 0)
    {
        le_port_text->setText(QString::number(config->MPD_port_0));
        le_host_text->setText(config->MPD_host_0);
        le_pass_text->setText(config->MPD_password_0);
        le_musicdir_text->setText(config->mpd_musicpath);
    }
}

void qm_settings::apply_fontsizes()
{
    bool fonts_changed = false;

    if (config->maintitle_fontsize != sb_titlefont->value())
    {
        config->maintitle_fontsize = sb_titlefont->value();
        fonts_changed = true;
    }
    if (config->codecinfo_fontsize != sb_infofont->value())
    {
        config->codecinfo_fontsize = sb_infofont->value();
        fonts_changed = true;
    }
    if (config->time_fontsize != sb_timefont->value())
    {
        config->time_fontsize = sb_timefont->value();
        fonts_changed = true;
    }
    if (config->album_fontsize != sb_albumfont->value())
    {
        config->album_fontsize = sb_albumfont->value();
        fonts_changed = true;
    }
    if (config->comment_fontsize != sb_commentfont->value())
    {
        config->comment_fontsize = sb_commentfont->value();
        fonts_changed = true;
    }
    if (config->liblist_fontsize != sb_liblistfont->value())
    {
        config->liblist_fontsize = sb_liblistfont->value();
        fonts_changed = true;
    }

    if (fonts_changed)
        emit sgnl_fonts(); // to player
}


void qm_settings::apply_colors()
{
    bool colors_changed = false;

    if (config->rb_sysdef_checked != rb_sysdef->isChecked())
    {
        config->rb_sysdef_checked = rb_sysdef->isChecked();
        colors_changed = true;
    }

    if (config->rb_qmcool_checked != rb_qmcool->isChecked())
    {
        config->rb_qmcool_checked = rb_qmcool->isChecked();
        colors_changed = true;
    }

    if (config->rb_custom_checked != rb_custom->isChecked())
    {
        config->rb_custom_checked = rb_custom->isChecked();
        colors_changed = true;
    }
    if (config->rb_custom_checked && b_new_custom_colors)
        colors_changed = true;

    if (config->color_browser != cb_color_browser->isChecked())
    {
        config->color_browser = cb_color_browser->isChecked();
        colors_changed = true;
    }

    if (config->color_albuminfo != cb_color_albinfo->isChecked())
    {
        config->color_albuminfo = cb_color_albinfo->isChecked();
        colors_changed = true;
    }

    if (colors_changed)
        emit sgnl_colors(); // to player
}

// set a pointer to the parent's config
void qm_settings::set_config(qm_config *conf)
{
    config = conf;
    if (config == NULL)
        return;

    lb_version->setText("<b>Quimup " + config->version + "</b>");

    retranslateUI();

    QPalette pal1 (this->palette());
    pal1.setColor(QPalette::Window, QColor(config->color_bg)); // background
    pal1.setColor(QPalette::Foreground, QColor(config->color_fg)); // text
    lb_custcolor_main->setPalette(pal1);
    QColor fgcolor_album = QColor(config->color_bg);
    QColor bgcolor_album = QColor(config->color_bg);
    fgcolor_album.setHsv(fgcolor_album.hue(), fgcolor_album.saturation(), 64);
    bgcolor_album.setHsv(bgcolor_album.hue(), bgcolor_album.saturation()/3, 200);
    QPalette pal2 (this->palette());
    pal2.setColor(QPalette::Window, bgcolor_album); // background
    pal2.setColor(QPalette::Foreground, fgcolor_album); // text
    lb_custcolor_album->setPalette(pal2);

    sb_xfade->setValue(config->xfade_time);
    cb_autoConn->setChecked(config->auto_connect);
    cm_profiles->setItemText( 0, " Autodetect" );
    cm_profiles->setItemText( 1, " " + config->Profilename_1 );
    cm_profiles->setItemText( 2, " " + config->Profilename_2 );
    cm_profiles->setItemText( 3, " " + config->Profilename_3 );
    cm_profiles->setItemText( 4, " " + config->Profilename_4 );

    cm_profiles->setCurrentIndex(config->profile);
    set_profile(config->profile);

    cb_useSysTray->setChecked(config->use_trayicon);
    cb_showTips->setChecked(config->show_tooltips);
    cb_sortwithyear->setChecked(config->sort_albums_year);
    cb_disable_aart->setChecked(config->disable_albumart);
    cb_ignore_the->setChecked(config->ignore_leading_the);
    cb_plist_auto_colwidth->setChecked(config->plist_auto_colwidth);
    cb_lib_auto_colwidth->setChecked(config->lib_auto_colwidth);
    cb_auto_playfirst->setChecked(config->auto_playfirst);
    cb_mark_played->setChecked(config->mark_played);

    le_artviewer->setText(config->art_viewer);
    le_tageditor->setText(config->tag_editor);
    le_filebrowser->setText(config->file_manager);

    cb_killserver->setChecked(config->quitMPD_onquit);
    le_killcommand->setText(config->onquit_mpd_command);

    cb_startserver->setChecked(config->startMPD_onstart);
    le_startcommand->setText(config->onstart_mpd_command);

    sb_titlefont->setValue(config->maintitle_fontsize);
    sb_infofont->setValue(config->codecinfo_fontsize);
    sb_timefont->setValue(config->time_fontsize);
    sb_albumfont->setValue(config->album_fontsize);
    sb_commentfont->setValue(config->comment_fontsize);
    sb_liblistfont->setValue(config->liblist_fontsize);

    cb_color_browser->setChecked(config->color_browser);
    cb_color_albinfo->setChecked(config->color_albuminfo);

    if (config->rb_sysdef_checked)
        rb_sysdef->setChecked(true);
    else
        if (config->rb_qmcool_checked)
            rb_qmcool->setChecked(true);
        else
            if (config->rb_custom_checked)
                rb_custom->setChecked(true);

    if (config->rpgain_mode == "track")
        rb_rpg_track->setChecked(true);
    else
        if (config->rpgain_mode == "album")
            rb_rpg_album->setChecked(true);
        else
            if (config->rpgain_mode == "auto")
                rb_rpg_auto->setChecked(true);
            else
                rb_rpg_off->setChecked(true); // default
}

// set a pointer to the parent's mpdCom
void qm_settings::set_connected(qm_mpdCom *conn, bool isconnected)
{
    mpdCom = conn;
    if (mpdCom == NULL)
        b_mpd_connected = false;
    else
        b_mpd_connected = isconnected;

    if (!b_mpd_connected)
    {
        lb_conn_status_text->setText(tr("Not connected"));
        lb_conn_icon->setPixmap(QPixmap( ":/con_dis.png" ));
        lb_musicdir_status_text->setText(tr(""));
        lb_musicdir_icon->setPixmap(QPixmap( ":/musdir_nocon.png" ));
        gbox_outputs->setEnabled(false);
        gbox_replaygain->setEnabled(false);
        gbox_xfade->setEnabled(false);
    }
    else
    {
        switch (config->mpd_musicpath_status)
        {
            case 0://" not determined
            {
                lb_musicdir_status_text->setText(tr("Music dir: not determined"));
                lb_musicdir_icon->setPixmap(QPixmap( ":/musdir_error.png" ));
                break;
            }
            case 1: // accessible
            {
                lb_musicdir_status_text->setText(tr("Music dir: accessible"));
                lb_musicdir_icon->setPixmap(QPixmap( ":/musdir_ok.png" ));
                break;
            }
            case 2: // read-only
            {
                lb_musicdir_status_text->setText(tr("Music dir: read only"));
                lb_musicdir_icon->setPixmap(QPixmap( ":/musdir_warn.png" ));
                break;
            }
            case 3: // does not exist
            {
                lb_musicdir_status_text->setText(tr("Music dir: does not exist"));
                lb_musicdir_icon->setPixmap(QPixmap( ":/musdir_error.png" ));
                break;
            }
            default: // unknown status
            {
                lb_musicdir_status_text->setText(tr("Music dir: unknown status"));
                lb_musicdir_icon->setPixmap(QPixmap( ":/musdir_error.png" ));
                break;
            }
        }

        lb_conn_status_text->setText( tr("Connected to MPD") + " " + mpdCom->get_version());
        lb_conn_icon->setPixmap(QPixmap( ":/con_ok.png" ));

        if (config->mpd_setrpgain_allowed)
        {
            gbox_replaygain->setEnabled(true);
            set_replaygain_mode();
        }
        else
            gbox_replaygain->setEnabled(false);

        if (config->mpd_xfade_allowed)
            gbox_xfade->setEnabled(true);
        else
            gbox_xfade->setEnabled(false);

        if (config->mpd_op_listall_allowed)
        {
            get_outputs();

            if (config->mpd_op_disble_allowed || config->mpd_op_enable_allowed)
            {
                gbox_outputs->setEnabled(true);
            }
            else
                gbox_outputs->setEnabled(false);
        }
        else
            gbox_outputs->setEnabled(false);
    }
}


void qm_settings::set_replaygain_mode()
{
    if (!b_mpd_connected)
        return;

    if(!config->mpd_setrpgain_allowed)
    {
        printf ("Replaygain not set : MPD does not allow it.\n");
        return;
    }

    if (config->rpgain_mode == "off" )
    {
        mpdCom->set_replaygain_mode(0);
        printf ("Replaygain mode set to \"off\"\n");
    }
    else
    if (config->rpgain_mode == "track" )
    {
        mpdCom->set_replaygain_mode(1);
        printf ("Replaygain mode set to \"track\"\n");
    }
    else
    if (config->rpgain_mode == "album" )
    {
        mpdCom->set_replaygain_mode(2);
        printf ("Replaygain mode set to \"album\"\n");
    }
    else
    if (config->rpgain_mode == "auto" )
    {
        mpdCom->set_replaygain_mode(3);
        printf ("Replaygain mode set to \"auto\"\n");
    }
    else
        printf ("Replaygain not set by Quimup : MPD rules.\n");
}

void qm_settings::set_outputs()
{
    if (!b_mpd_connected)
        return;

    bool devs_changed = false;
    qm_outputList outputlist;

    // dev1
    if (cb_dev1->isCheckable())
    {
        qm_output output;
        output.id = 0;
        output.enabled = cb_dev1->isChecked();
        outputlist.push_back(output);
        if (cb_dev1->isChecked()!= dev1_wasChecked)
        {
            devs_changed = true;
            dev1_wasChecked = cb_dev1->isChecked();
        }
    }
    // dev2
    if (cb_dev2->isCheckable() )
    {
        qm_output output;
        output.id = 1;
        output.enabled = cb_dev2->isChecked();
        outputlist.push_back(output);
        if (cb_dev2->isChecked()!= dev2_wasChecked)
        {
            devs_changed = true;
            dev2_wasChecked = cb_dev2->isChecked();
        }
    }
    // dev3
    if (cb_dev3->isCheckable())
    {
        qm_output output;
        output.id = 2;
        output.enabled = cb_dev3->isChecked();
        outputlist.push_back(output);
        if (cb_dev3->isChecked()!= dev3_wasChecked)
        {
            devs_changed = true;
            dev3_wasChecked = cb_dev3->isChecked();
        }
    }
    // dev4
    if (cb_dev4->isCheckable())
    {
        qm_output output;
        output.id = 3;
        output.enabled = cb_dev4->isChecked();
        outputlist.push_back(output);
        if (cb_dev4->isChecked()!= dev4_wasChecked)
        {
            devs_changed = true;
            dev4_wasChecked = cb_dev4->isChecked();
        }
    }
    // dev5
    if (cb_dev5->isCheckable())
    {
        qm_output output;
        output.id = 4;
        output.enabled = cb_dev5->isChecked();
        outputlist.push_back(output);
        if (cb_dev5->isChecked()!= dev5_wasChecked)
        {

            devs_changed = true;
            dev5_wasChecked = cb_dev5->isChecked();
        }
    }

    if (devs_changed)
    {
        if (mpdCom->set_outputs(outputlist))
            printf ("Setting MPD outputs : OK\n");
        else
            printf ("Setting MPD outputs : error!\n");
    }
}

// set the output devices
void qm_settings::get_outputs()
{
    if (!b_mpd_connected)
        return;

    qm_outputList outputlist;
    qm_output output;
    dev1_wasChecked = false;
    dev2_wasChecked = false;
    dev3_wasChecked = false;
    dev4_wasChecked = false;
    dev5_wasChecked = false;
    outputlist = mpdCom->get_outputs();
    // Set up the output devices
    QListIterator<qm_output> i(outputlist);

    if (i.hasNext())
    {
        cb_dev1->setCheckable(true);
        output = i.next();
        cb_dev1->setText(output.name);
        cb_dev1->setChecked(output.enabled);
        dev1_wasChecked = output.enabled;
    }
    else
    {
        cb_dev1->setCheckable(false);
        cb_dev1->setChecked(false);
        cb_dev1->setText( tr("Not connected!") );
        cb_dev2->setCheckable(false);
        cb_dev2->setChecked(false);
        cb_dev2->setText("..");
        cb_dev3->setCheckable(false);
        cb_dev3->setChecked(false);
        cb_dev3->setText("..");
        cb_dev4->setCheckable(false);
        cb_dev4->setChecked(false);
        cb_dev4->setText("..");
        cb_dev5->setCheckable(false);
        cb_dev5->setChecked(false);
        cb_dev5->setText("..");
        return;
    }

    if (i.hasNext())
    {
        cb_dev2->setCheckable(true);
        output = i.next();
        cb_dev2->setText(output.name);
        cb_dev2->setChecked(output.enabled);
        dev2_wasChecked = output.enabled;
    }
    else
    {
        cb_dev2->setCheckable(false);
        cb_dev2->setChecked(false);
        cb_dev2->setText("..");
        cb_dev3->setCheckable(false);
        cb_dev3->setChecked(false);
        cb_dev3->setText("..");
        cb_dev4->setCheckable(false);
        cb_dev4->setChecked(false);
        cb_dev4->setText("..");
        cb_dev5->setCheckable(false);
        cb_dev5->setChecked(false);
        cb_dev5->setText("..");
        return;
    }

    if (i.hasNext())
    {
        cb_dev3->setCheckable(true);
        output = i.next();
        cb_dev3->setText(output.name);
        cb_dev3->setChecked(output.enabled);
        dev3_wasChecked = output.enabled;
    }
    else
    {
        cb_dev3->setCheckable(false);
        cb_dev3->setChecked(false);
        cb_dev3->setText("..");
        cb_dev4->setCheckable(false);
        cb_dev4->setChecked(false);
        cb_dev4->setText("..");
        cb_dev5->setCheckable(false);
        cb_dev5->setChecked(false);
        cb_dev5->setText("..");
        return;
    }

    if (i.hasNext())
    {
        cb_dev4->setCheckable(true);
        output = i.next();
        cb_dev4->setText(output.name);
        cb_dev4->setChecked(output.enabled);
        dev4_wasChecked = output.enabled;
    }
    else
    {
        cb_dev4->setCheckable(false);
        cb_dev4->setChecked(false);
        cb_dev4->setText("..");
        cb_dev5->setCheckable(false);
        cb_dev5->setChecked(false);
        cb_dev5->setText("..");
        return;
    }

    if (i.hasNext())
    {
        cb_dev5->setCheckable(true);
        output = i.next();
        cb_dev5->setText(output.name);
        cb_dev5->setChecked(output.enabled);
        dev5_wasChecked = output.enabled;
    }
    else
    {
        cb_dev5->setCheckable(false);
        cb_dev5->setChecked(false);
        cb_dev5->setText("..");
        return;
    }
}


void qm_settings::closeEvent( QCloseEvent * ce )
{
    this->hide();
    ce->ignore(); // only hide, don't close
}

void qm_settings::hideEvent(QHideEvent *he)
{
    if (colorpicker->isVisible())
        colorpicker->hide();

    config->settings_X = x();
    config->settings_Y = y();
    config->settings_W = width();
    config->settings_H = height();
    he->accept();
}


void qm_settings::showEvent(QShowEvent *se)
{
    move(config->settings_X, config->settings_Y);
    se->accept();
}


void qm_settings::show_colorpicker()
{
    if (colorpicker->isVisible())
    {
        colorpicker->hide();
        return;
    }

    colorpicker->move(x()+160, y()+190);
    colorpicker->set_colors(config->color_fg, config->color_bg);
    colorpicker->show();
}


void qm_settings::on_new_customcolors(QColor fgcol, QColor bgcol)
{
    config->color_bg = bgcol.name(); // "#RRGGBB"
    config->color_fg = fgcol.name();
    QPalette pal1 (this->palette());
    pal1.setColor(QPalette::Window, QColor(config->color_bg)); // background
    pal1.setColor(QPalette::Foreground, QColor(config->color_fg)); // text
    lb_custcolor_main->setPalette(pal1);

    QColor fgcolor_album;
    QColor bgcolor_album;
    fgcolor_album.setHsv(fgcol.hue(), fgcol.saturation(), 64);
    bgcolor_album.setHsv(bgcol.hue(), bgcol.saturation()/3, 200);
    QPalette pal2 (this->palette());
    pal2.setColor(QPalette::Window, bgcolor_album); // background
    pal2.setColor(QPalette::Foreground, fgcolor_album); // text
    lb_custcolor_album->setPalette(pal2);

    b_new_custom_colors = true;
}


qm_settings::~qm_settings()
{
    delete(colorpicker);
}
