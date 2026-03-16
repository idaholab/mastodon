###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Optional Environment variables
# MOOSE_DIR       - Root directory of the MOOSE project
#
###############################################################################
# Use the MOOSE submodule if it exists and MOOSE_DIR is not set
MOOSE_SUBMODULE    := $(CURDIR)/moose
ifneq ($(wildcard $(MOOSE_SUBMODULE)/framework/Makefile),)
  MOOSE_DIR        ?= $(MOOSE_SUBMODULE)
else
  MOOSE_DIR        ?= $(shell dirname `pwd`)/moose
endif

# framework
FRAMEWORK_DIR      := $(MOOSE_DIR)/framework
include $(FRAMEWORK_DIR)/build.mk
include $(FRAMEWORK_DIR)/moose.mk

################################## MODULES ####################################
CONTACT          := yes
FLUID_PROPERTIES := yes
FSI              := yes
HEAT_TRANSFER    := yes
MISC             := yes
RAY_TRACING      := yes
STOCHASTIC_TOOLS := yes
SOLID_MECHANICS  := yes
XFEM             := yes
NAVIER_STOKES    := yes
include $(MOOSE_DIR)/modules/modules.mk
###############################################################################

# BlackBear (optional)
BLACKBEAR_SUBMODULE := $(CURDIR)/blackbear
ifneq ($(wildcard $(BLACKBEAR_SUBMODULE)/Makefile),)
  BLACKBEAR_DIR    ?= $(BLACKBEAR_SUBMODULE)
endif
ifeq ($(wildcard $(BLACKBEAR_DIR)/Makefile),)
  $(info Could not find BlackBear, so not compiling with it)
else
  APPLICATION_DIR    := $(BLACKBEAR_DIR)
  APPLICATION_NAME   := blackbear
  include            $(FRAMEWORK_DIR)/app.mk
  ADDITIONAL_CPPFLAGS += -DBLACKBEAR_ENABLED
  APP_HEADERS        := $(APP_HEADERS) $(app_HEADER)
endif

# dep apps
APPLICATION_DIR    := $(CURDIR)
APPLICATION_NAME   := mastodon
BUILD_EXEC         := yes
DEP_APPS           := $(shell $(FRAMEWORK_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))
include            $(FRAMEWORK_DIR)/app.mk

###############################################################################
# Additional special case targets should be added here

