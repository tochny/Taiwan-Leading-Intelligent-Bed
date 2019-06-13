# Application name
APPL ?= freertos-demo

# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
OLEVEL ?= O2

##
# select the operation cpu core
##
CUR_CORE ?= arcem6

# Selected OS
OS_SEL ?= freertos

##
# select debugging jtag
##
JTAG ?= usb

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

MID_SEL = common

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Upper Options must put before include options.mk ###
# include  option parser
include $(EMBARC_ROOT)/options/options.mk