#------------------------------------------------------------------------------
# Copyright (c) 2016, 2025, Oracle and/or its affiliates.
#
# This software is dual-licensed to you under the Universal Permissive License
# (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
# 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
# either license.
#
# If you elect to accept the software under the Apache License, Version 2.0,
# the following applies:
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Sample Makefile showing how ODPI-C can be built as a shared library on
# platforms other than Windows. For Windows, see Makefile.win32.
#
# See https://oracle.github.io/odpi/doc/installation.html
# for the platforms and compilers known to work.
#------------------------------------------------------------------------------

vpath %.c src
vpath %.h include src

PREFIX ?= /usr/local
INSTALL_LIB_DIR = $(PREFIX)/lib
INSTALL_INC_DIR = $(PREFIX)/include
INSTALL_SHARE_DIR = $(PREFIX)/share/odpi

BUILD_DIR = build
LIB_DIR = lib
SAMPLES_DIR = samples
TESTS_DIR = test

CC ?= gcc
AWK ?= awk

MAJOR_VERSION := $(shell $(AWK) '/define.*DPI_MAJOR_VERSION/ {print $$3}' \
	include/dpi.h )
MINOR_VERSION := $(shell $(AWK) '/define.*DPI_MINOR_VERSION/ {print $$3}' \
	include/dpi.h )
PATCH_LEVEL := $(shell $(AWK) '/define.*DPI_PATCH_LEVEL/ {print $$3}' \
	include/dpi.h )

INSTALL ?= install
CFLAGS ?= -O2 -g -Wall -Wextra -fPIC
EXTRA_CFLAGS ?=
INCLUDES ?= -Iinclude
LIBS ?= -ldl -lpthread
LDFLAGS ?= -shared
EXTRA_LDFLAGS ?=
VERSION_LIB_NAME = $(LIB_NAME).$(MAJOR_VERSION)
FULL_LIB_NAME = $(VERSION_LIB_NAME).$(MINOR_VERSION).$(PATCH_LEVEL)
ifeq ($(shell uname -s), Darwin)
	LIB_NAME = libodpic.dylib
	LIB_OUT_OPTS = -dynamiclib \
		-install_name $(shell pwd)/$(LIB_DIR)/$(LIB_NAME) \
		-o $(LIB_DIR)/$(FULL_LIB_NAME)
else
	LIB_NAME = libodpic.so
	LIB_OUT_OPTS = -o $(LIB_DIR)/$(FULL_LIB_NAME)
	LDFLAGS += -Wl,-soname,$(LIB_NAME).$(MAJOR_VERSION)
endif

SRCS = dpiConn.c dpiContext.c dpiData.c dpiEnv.c dpiError.c dpiGen.c \
       dpiGlobal.c dpiLob.c dpiObject.c dpiObjectAttr.c dpiObjectType.c \
       dpiPool.c dpiStmt.c dpiUtils.c dpiVar.c dpiOracleType.c dpiSubscr.c \
       dpiDeqOptions.c dpiEnqOptions.c dpiMsgProps.c dpiRowid.c dpiOci.c \
       dpiDebug.c dpiHandlePool.c dpiHandleList.c dpiSodaColl.c \
       dpiSodaCollCursor.c dpiSodaDb.c dpiSodaDoc.c dpiSodaDocCursor.c \
       dpiQueue.c dpiJson.c dpiStringList.c dpiVector.c
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

SAMPLES_FILES := $(SAMPLES_DIR)/Makefile $(SAMPLES_DIR)/README.md \
		$(wildcard $(SAMPLES_DIR)/*.c) $(wildcard $(SAMPLES_DIR)/*.h) \
		$(wildcard $(SAMPLES_DIR)/sql/*.sql)
SAMPLES_TARGETS := $(SAMPLES_FILES:%=$(INSTALL_SHARE_DIR)/%)
INSTALL_SAMPLES_SQL_DIR := $(INSTALL_SHARE_DIR)/$(SAMPLES_DIR)/sql

TESTS_FILES := $(TESTS_DIR)/Makefile $(TESTS_DIR)/README.md \
		$(wildcard $(TESTS_DIR)/*.c) $(wildcard $(TESTS_DIR)/*.h) \
		$(wildcard $(TESTS_DIR)/sql/*.sql)
TESTS_TARGETS := $(TESTS_FILES:%=$(INSTALL_SHARE_DIR)/%)
INSTALL_TESTS_SQL_DIR := $(INSTALL_SHARE_DIR)/$(TESTS_DIR)/sql

INSTALL_TARGETS = $(INSTALL_INC_DIR)/dpi.h \
		$(INSTALL_LIB_DIR)/$(LIB_NAME) $(INSTALL_LIB_DIR)/$(FULL_LIB_NAME) \
		$(INSTALL_LIB_DIR)/$(VERSION_LIB_NAME) $(INSTALL_SHARE_DIR)

all: $(LIB_DIR)/$(FULL_LIB_NAME) $(LIB_DIR)/$(VERSION_LIB_NAME) \
		$(LIB_DIR)/$(LIB_NAME)

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BUILD_DIR)/%.o: %.c dpi.h dpiImpl.h dpiErrorMessages.h
	$(CC) -c $(INCLUDES) $(CFLAGS) $(EXTRA_CFLAGS) $< -o $@

$(LIB_DIR)/$(FULL_LIB_NAME): $(BUILD_DIR) $(LIB_DIR) $(OBJS)
	$(CC) $(LDFLAGS) $(EXTRA_LDFLAGS) $(LIB_OUT_OPTS) $(OBJS) $(LIBS)

$(LIB_DIR)/$(VERSION_LIB_NAME): $(LIB_DIR)/$(FULL_LIB_NAME)
	ln -sf $(FULL_LIB_NAME) $@

$(LIB_DIR)/$(LIB_NAME): $(LIB_DIR)/$(VERSION_LIB_NAME)
	ln -sf $(VERSION_LIB_NAME) $@

$(INSTALL_LIB_DIR):
	mkdir -p $@

$(INSTALL_INC_DIR):
	mkdir -p $@

$(INSTALL_SAMPLES_SQL_DIR):
	mkdir -p $@

$(INSTALL_TESTS_SQL_DIR):
	mkdir -p $@

$(INSTALL_INC_DIR)/%.h: %.h
	$(INSTALL) $< $@

$(INSTALL_LIB_DIR)/$(FULL_LIB_NAME): $(LIB_DIR)/$(LIB_NAME)
	$(INSTALL) $< $@
	if test "`uname -s`" = Darwin; then \
			install_name_tool -id $(INSTALL_LIB_DIR)/$(VERSION_LIB_NAME) $@; fi

$(INSTALL_LIB_DIR)/$(VERSION_LIB_NAME): $(INSTALL_LIB_DIR)/$(FULL_LIB_NAME)
	ln -sf $(FULL_LIB_NAME) $@

$(INSTALL_LIB_DIR)/$(LIB_NAME): $(INSTALL_LIB_DIR)/$(VERSION_LIB_NAME)
	ln -sf $(VERSION_LIB_NAME) $@

$(INSTALL_SHARE_DIR)/%: %
	$(INSTALL) $< $@

install: $(INSTALL_SAMPLES_SQL_DIR) $(INSTALL_TESTS_SQL_DIR) \
		$(INSTALL_INC_DIR) $(INSTALL_LIB_DIR) $(INSTALL_TARGETS) \
		$(INSTALL_SHARE_DIR)/README.md $(INSTALL_SHARE_DIR)/LICENSE.txt \
		$(SAMPLES_TARGETS) $(TESTS_TARGETS)

uninstall:
	rm -rf $(INSTALL_TARGETS)
