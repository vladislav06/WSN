ifeq ($(TARG),transmitter)
  SOURCES = ./transmitter/main.c
endif

ifeq ($(TARG),receiver)
  SOURCES = ./receiver/main.c
endif



# Module is the name of the main module built by this makefile
APPMOD = PD5

# --------------------------------------------------------------------
# Set the key variables
PROJDIR = $(CURDIR)

ifndef MOSROOT
#   MOSROOT = /srv/mans_os
  MOSROOT = ./../../../MansOS
endif

# Include the main makefile
include ${MOSROOT}/mos/make/Makefile
