# ----------------------------
# Makefile Options
# ----------------------------

NAME         = PCAS
COMPRESSED   = YES
# Upstream referenced iconc.png, but that file is not present in the repository.
# Use toolchain's default program icon.
DESCRIPTION  = "PineappleCAS"

CFLAGS       = -Wall -Oz
CXXFLAGS     = -Wall -Oz

include $(shell cedev-config --makefile)
