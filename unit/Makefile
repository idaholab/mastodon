###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Required Environment variables (one of the following)
# PACKAGES_DIR  - Location of the MOOSE redistributable package
#
# Optional Environment variables
# MOOSE_DIR     - Root directory of the MOOSE project
# FRAMEWORK_DIR - Location of the MOOSE framework
#
###############################################################################
# Use the MOOSE submodule if it exists and MOOSE_DIR is not set
MOOSE_SUBMODULE    := $(CURDIR)/../moose
ifneq ($(wildcard $(MOOSE_SUBMODULE)/framework/Makefile),)
  MOOSE_DIR        ?= $(MOOSE_SUBMODULE)
else
  MOOSE_DIR        ?= $(shell dirname `pwd`)/../moose
endif

# Framework
FRAMEWORK_DIR      ?= $(MOOSE_DIR)/framework
###############################################################################

# framework
include $(FRAMEWORK_DIR)/build.mk
include $(FRAMEWORK_DIR)/moose.mk

################################## MODULES ####################################
# set desired physics modules equal to 'yes' to enable them
CHEMICAL_REACTIONS        := no
CONTACT                   := yes
FLUID_PROPERTIES          := no
HEAT_CONDUCTION           := no
MISC                      := no
NAVIER_STOKES             := no
PHASE_FIELD               := no
RDG                       := no
RICHARDS                  := no
SOLID_MECHANICS           := yes
STOCHASTIC_TOOLS          := yes
TENSOR_MECHANICS          := yes
WATER_STEAM_EOS           := no
XFEM                      := no
POROUS_FLOW               := no
LEVEL_SET                 := no
include           $(MOOSE_DIR)/modules/modules.mk
###############################################################################

# Extra stuff for GTEST
ADDITIONAL_INCLUDES	:= -I$(FRAMEWORK_DIR)/contrib/gtest
ADDITIONAL_LIBS 	:= $(FRAMEWORK_DIR)/contrib/gtest/libgtest.la

# BlackBear (optional)
BLACKBEAR_SUBMODULE := $(CURDIR)/../blackbear
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
CURRENT_DIR        := $(shell pwd)
APPLICATION_DIR    := $(CURRENT_DIR)/..
APPLICATION_NAME   := mastodon
include            $(FRAMEWORK_DIR)/app.mk

APPLICATION_DIR    := $(CURRENT_DIR)
APPLICATION_NAME   := mastodon-unit
BUILD_EXEC         := yes

DEP_APPS    ?= $(shell $(FRAMEWORK_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))
include $(FRAMEWORK_DIR)/app.mk

# Find all the Mastodon unit test source files and include their dependencies.
mastodon_unit_srcfiles := $(shell find $(MOOSE_DIR)/unit/src -name "*.C")
mastodon_unit_deps := $(patsubst %.C, %.$(obj-suffix).d, $(mastodon_unit_srcfiles))
-include $(mastodon_unit_deps)

###############################################################################
# Additional special case targets should be added here
