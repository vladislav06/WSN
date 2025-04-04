ifeq ($(TARG),sensor)
  SOURCES = ./sensor/main.c
endif

ifeq ($(TARG),relay)
  SOURCES = ./relay/main.c
endif

ifeq ($(TARG),gateway)
  SOURCES = ./gateway/main.c
endif

SOURCES += ./utilities/idChip.c
SOURCES += ./protocol/protocol.c


# Module is the name of the main module built by this makefile
APPMOD = PD6

# --------------------------------------------------------------------
# Set the key variables
PROJDIR = $(CURDIR)

ifndef MOSROOT
#   MOSROOT = /srv/mans_os
  MOSROOT = ./../../../MansOS
endif

# Include the main makefile
include ${MOSROOT}/mos/make/Makefile
