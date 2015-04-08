# Makefile for libskindesignerapi

NAME = skindesignerapi
LIBNAME = lib$(NAME)
MAJOR = 0
MINOR = 0.1
VERSION = $(MAJOR).$(MINOR)
 
SONAME = $(LIBNAME).so.$(MAJOR)
TARGET_LIB = $(SONAME).$(MINOR)

PREFIX ?= /usr/local
INCDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib
PCDIR  ?= $(PREFIX)/lib/pkgconfig
TMPDIR ?= /tmp

### The name of the distribution archive:
ARCHIVE = $(LIBNAME)-$(VERSION)

PKGCFG = $(if $(VDRDIR),$(shell pkg-config --silence-errors --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --silence-errors --variable=$(1) vdr || pkg-config --variable=$(1) ../../../../vdr.pc))
CXXFLAGS = $(call PKGCFG,cxxflags)
LDFLAGS = -shared -Wl,-soname,$(SONAME)

DEFINES += -DAPIVERSION=$(MAJOR) -DLIBVERSION=\"$(VERSION)\"
INCLUDES +=
 
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
 
.PHONY: all
all: ${TARGET_LIB} ${LIBNAME}.pc
 
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

# The main lib

$(TARGET_LIB): $(OBJS)
	$(CXX) ${LDFLAGS} -o $@ $^

# pkg-config

.PHONY: $(LIBNAME).pc
$(LIBNAME).pc:
	@echo "includedir=$(INCDIR)" > $@
	@echo "libdir=$(LIBDIR)" >> $@
	@echo "" >> $@
	@echo "Name: $(LIBNAME)" >> $@
	@echo "Description: skindesigner API Library" >> $@
	@echo "Version: $(VERSION)" >> $@
	@echo "Cflags: -I$(INCDIR)" >> $@
	@echo "Libs: -L$(LIBDIR) -l$(NAME)" >> $@

# install targets

install-lib: $(TARGET_LIB)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^
	if [ -z "$(DESTDIR)" ] ; then ldconfig; fi
	cd $(DESTDIR)$(LIBDIR) ; if [ ! -e $(LIBNAME).so ] ; then ln -s $(TARGET_LIB) $(LIBNAME).so; fi

install-includes:
	@mkdir -p $(DESTDIR)$(INCDIR)/$(LIBNAME)
	@cp -pLR *.h $(DESTDIR)$(INCDIR)/$(LIBNAME)

install-pc: $(LIBNAME).pc
	if [ -n "$(PCDIR)" ] ; then\
	  mkdir -p $(DESTDIR)$(PCDIR) ;\
	  cp $(LIBNAME).pc $(DESTDIR)$(PCDIR) ;\
	fi

install: install-lib install-pc install-includes

# clean & dist

.PHONY: clean
clean:
	-rm -f ${TARGET_LIB} ${OBJS} $(DEPFILE) $(LIBNAME).pc $(LIBNAME).so $(ARCHIVE).tgz

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(ARCHIVE).tgz --exclude .git* --exclude *.o --exclude *.rej --exclude *.orig -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(ARCHIVE).tgz
