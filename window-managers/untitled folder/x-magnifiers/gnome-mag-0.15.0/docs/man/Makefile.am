
man_MANS = magnifier.1

EXTRA_DIST = $(man_MANS)

MAN_PAGE = magnifier.1.man

all: all-am

magnifier.1:
	cp $(MAN_PAGE) $@

magnifier.html:
	man2html $(MAN_PAGE) > $@

CLEANFILES = $(mag_MANS)

