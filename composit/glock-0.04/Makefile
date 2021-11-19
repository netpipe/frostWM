# $Id: Makefile 18 2006-02-21 00:10:06Z eric $
# Copyright 2005, 2006 Eric L. Smith <eric@brouhaha.com>

# Glock is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.  Note that I am not
# granting permission to redistribute or modify glock under the
# terms of any later version of the General Public License.

# Glock is distributed in the hope that they will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program (in the file "COPYING"); if not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111, USA.


LDFLAGS = -g


# -----------------------------------------------------------------------------
# You shouldn't have to change anything below this point, but if you do please
# let me know why so I can improve this Makefile.
# -----------------------------------------------------------------------------

PACKAGE = glock
RELEASE = 0.04
DISTNAME = $(PACKAGE)-$(RELEASE)

PACKAGES = gtk+-2.0

CFLAGS = -g -Wall `pkg-config $(PACKAGES) --cflags`
LOADLIBES = `pkg-config $(PACKAGES) --libs`
# -lutil

TARGETS = glock

HDRS = 

CSRCS = glock.c

MISC = COPYING README TODO
# ChangeLog

OBJECTS = glock.o

DIST_FILES = $(MISC) Makefile $(HDRS) $(CSRCS)

CFLAGS += -DGLOCK_RELEASE=$(RELEASE)

all: $(TARGETS)


glock:	glock.o

dist:	$(DIST_FILES)
	-rm -rf $(DISTNAME) $(DISTNAME).tar.gz
	mkdir $(DISTNAME)
	for f in $(DIST_FILES); do ln $$f $(DISTNAME)/$$f; done
	tar --gzip -chf $(DISTNAME).tar.gz $(DISTNAME)
	-rm -rf $(DISTNAME)

clean:
	rm -f $(TARGETS) $(OBJECTS)

DEPENDS = $(CSRCS:.c=.d)

%.d: %.c
	$(CC) -M -MG $(CFLAGS) $(CDEFINES) $< | sed -e 's@ /[^ ]*@@g' -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@

include $(DEPENDS)
