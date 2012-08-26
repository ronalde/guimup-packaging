## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GUIMUP_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = guimup

guimup_SOURCES = \
	main.cc \
	gm_player.h \
	gm_player.cc \
	gm_player_pix.h \
	libmpdclient.c \
	libmpdclient.h \
	gm_scroller.cc \
	gm_scroller.h \
	gm_config.h \
	gm_config.cc \
	gm_taginfo.h \
	gm_taginfo.cc \
	gm_mpdcom.cc \
	gm_mpdcom.h \
	gm_tracks.h \
	gm_tracks.cc \
	gm_songlist.h \
	gm_tracks_mpdcom.cc \
	gm_tracks_mpdcom.h \
	gm_tracks_playlist.h \
	gm_tracks_playlist.cc \
	gm_tracks_datatree.cc \
	gm_tracks_datatree.h \
	gm_commandlist.h \
	gm_itemlist.h \
	gm_settings.h \
	gm_settings.cc \
	gm_settings_pix.h \
	gm_modeid.h \
	gm_tracks_pix.h \
	gm_tracks_playlist_pix.h \
	gm_tracks_datatree_pix.h

guimup_LDFLAGS = 

guimup_LDADD = $(GUIMUP_LIBS)

EXTRA_DIST = 
