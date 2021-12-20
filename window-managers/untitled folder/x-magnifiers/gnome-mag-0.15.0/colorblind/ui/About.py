# -*- coding: utf-8 -*-
from os.path import join
from gettext import gettext as _
from colorblind.defs import VERSION
from colorblind.Utils import load_icon
import gtk, gtk.gdk, gnomevfs, gobject
import colorblind


def on_email(about, mail):
	gnomevfs.url_show("mailto:%s" % mail)

def on_url(about, link):
	gnomevfs.url_show(link)

gtk.about_dialog_set_email_hook(on_email)
gtk.about_dialog_set_url_hook(on_url)

def show_about(parent):
	about = gtk.AboutDialog()
	infos = {
		"name" : _("Colorblind Applet"),
		"logo-icon-name" : "colorblind-applet",
		"version" : VERSION,
		"comments" : _("Image filters for the colorblind. This is applet is part of gnome-mag, a magnification service for GNOME."),
		"copyright" : "Copyright © 2007 Carlos Eduardo Rodrigues Diógenes.",
		"website" : "http://live.gnome.org/GnomeMag",
		"website-label" : _("gnome-mag website"),
	}

	about.set_authors(["Carlos Eduardo Rodrigues Diógenes <cerdiogenes@gmail.com>"])
	about.set_artists(["Daniel Ruoso <daniel@ruoso.com>"])
#	about.set_documenters([])
	
	#translators: These appear in the About dialog, usual format applies.
	about.set_translator_credits( _("translator-credits") )
	
	for prop, val in infos.items():
		about.set_property(prop, val)
	
	about.connect("response", lambda self, *args: self.destroy())
	about.set_screen(parent.get_screen())
	about.show_all()
