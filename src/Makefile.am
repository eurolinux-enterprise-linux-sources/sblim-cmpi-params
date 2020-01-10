# $Id: Makefile.am,v 1.2 2009/05/21 23:13:07 tyreld Exp $
# ==================================================================
# (C) Copyright IBM Corp. 2005, 2009
#
# THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
# ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Eclipse Public License from
# http://www.opensource.org/licenses/eclipse-1.0.php
#
# Author:       Lynn Moss <lynnmoss@us.ibm.com>
# Description:  Automake input file for Params Providers
# ==================================================================


# SBLIM Include Directory
sblimincdir = $(includedir)/sblim

# Start from an empty extra distribution file list
EXTRA_DIST=

# docdir defines where the documentation goes
docdir=$(datadir)/doc/$(PACKAGE)-$(VERSION)

#
# Automake instructions for documentation
#
doc_DATA=README AUTHORS COPYING 

# ADD EXTRA DOC FILES IF PRESENT
doc_DATA+=DEBUG README.tog-pegasus
EXTRA_DIST+=DEBUG README.tog-pegasus

# providerdir defines where provider libraries will be installed (@PROVIDERDIR@ is set by the configure script). 
providerdir = @PROVIDERDIR@

#
# Automake instructions for main dir
#

#  Params CMPI provider libraries
provider_LTLIBRARIES = 	libLinux_ABIParameter.la \
			libLinux_FileSystemParameter.la \
			libLinux_KernelParameter.la \
			libLinux_NetworkCoreParameter.la \
			libLinux_NetworkIPv4Parameter.la \
			libLinux_NetworkUnixParameter.la \
			libLinux_VirtualMemoryParameter.la 

# ABIParameter
libLinux_ABIParameter_la_SOURCES = \
		Linux_ABIParameter.c
libLinux_ABIParameter_la_LIBADD = \
		-lpthread -lcmpiOSBase_Common 
libLinux_ABIParameter_la_LDFLAGS = -avoid-version -shared

# FileSystemParameter
libLinux_FileSystemParameter_la_SOURCES = \
		Linux_FileSystemParameter.c
libLinux_FileSystemParameter_la_LIBADD = \
		-lpthread -lcmpiOSBase_Common 
libLinux_FileSystemParameter_la_LDFLAGS = -avoid-version -shared

# KernelParameter
libLinux_KernelParameter_la_SOURCES = \
		Linux_KernelParameter.c
libLinux_KernelParameter_la_LIBADD = \
		-lpthread -lcmpiOSBase_Common 
libLinux_KernelParameter_la_LDFLAGS = -avoid-version -shared


# NetworkCoreParameter
libLinux_NetworkCoreParameter_la_SOURCES = \
		Linux_NetworkCoreParameter.c
libLinux_NetworkCoreParameter_la_LIBADD = \
		-lpthread -lcmpiOSBase_Common 
libLinux_NetworkCoreParameter_la_LDFLAGS = -avoid-version -shared


# NetworkIPv4Parameter
libLinux_NetworkIPv4Parameter_la_SOURCES = \
		Linux_NetworkIPv4Parameter.c
libLinux_NetworkIPv4Parameter_la_LIBADD = \
		-lpthread -lcmpiOSBase_Common 
libLinux_NetworkIPv4Parameter_la_LDFLAGS = -avoid-version -shared

# NetworkUnixParameter
libLinux_NetworkUnixParameter_la_SOURCES = \
		Linux_NetworkUnixParameter.c
libLinux_NetworkUnixParameter_la_LIBADD = \
		-lpthread -lcmpiOSBase_Common 
libLinux_NetworkUnixParameter_la_LDFLAGS = -avoid-version -shared

# VirtualMemoryParameter
libLinux_VirtualMemoryParameter_la_SOURCES = \
		Linux_VirtualMemoryParameter.c
libLinux_VirtualMemoryParameter_la_LIBADD = \
		-lpthread -lcmpiOSBase_Common 
libLinux_VirtualMemoryParameter_la_LDFLAGS = -avoid-version -shared

# Installable Header Files

# Non-Installable Header Files

# We must explicity add the RPM spec files to the distribution package
EXTRA_DIST+=$(PACKAGE).spec $(PACKAGE).rh.spec

#
# Automake instructions for ./mof subdir
#
SCHEMAS=mof/Linux_ABIParameter.mof \
	mof/Linux_FileSystemParameter.mof \
	mof/Linux_KernelParameter.mof \
	mof/Linux_NetworkCoreParameter.mof \
	mof/Linux_NetworkIPv4Parameter.mof \
	mof/Linux_NetworkUnixParameter.mof \
	mof/Linux_VirtualMemoryParameter.mof 
REGISTRATIONS=mof/Linux_ABIParameter.registration \
	mof/Linux_FileSystemParameter.registration \
	mof/Linux_KernelParameter.registration \
	mof/Linux_NetworkCoreParameter.registration \
	mof/Linux_NetworkIPv4Parameter.registration \
	mof/Linux_NetworkUnixParameter.registration \
	mof/Linux_VirtualMemoryParameter.registration 

# We must explicity add all the schema files to the distribution package
pkgdata_DATA=$(SCHEMAS) $(REGISTRATIONS)
pkgdata_SCRIPTS=provider-register.sh
EXTRA_DIST+=mof $(pkgdata_SCRIPTS)

# Register the provider(s) and class definition(s) to the current CIM server/CIMOM
postinstall:
	sh provider-register.sh -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

preuninstall:
	sh provider-register.sh -d -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

dist-hook:
	test -d "$(distdir)" &&	rm -rf `find $(distdir) -type d -name CVS`
#
# Automake instructions for ./test subdir
#
EXTRA_DIST+=test README.TEST
if TESTSUITE
testsuitedir=@TESTSUITEDIR@
testsuitesystemdir=@TESTSUITEDIR@/system/linux
testsuitecimdir=@TESTSUITEDIR@/cim

testsuite_SCRIPTS=test/sblim-cmpi-params-test.sh

testsuitesystem_DATA=test/system/linux/Linux_ABIParameter.system \
	test/system/linux/Linux_FileSystemParameter.system \
	test/system/linux/Linux_KernelParameter.system \
	test/system/linux/Linux_NetworkCoreParameter.system \
	test/system/linux/Linux_NetworkIPv4Parameter.system \
	test/system/linux/Linux_NetworkUnixParameter.system \
	test/system/linux/Linux_VirtualMemoryParameter.system

testsuitesystem_SCRIPTS=test/system/linux/Linux_ABIParameter.sh \
	test/system/linux/Linux_FileSystemParameter.sh \
	test/system/linux/Linux_KernelParameter.sh \
	test/system/linux/Linux_NetworkCoreParameter.sh \
	test/system/linux/Linux_NetworkIPv4Parameter.sh \
	test/system/linux/Linux_NetworkUnixParameter.sh \
	test/system/linux/Linux_VirtualMemoryParameter.sh

testsuitecim_DATA=test/cim/Linux_ABIParameter.cim \
	test/cim/Linux_FileSystemParameter.cim \
	test/cim/Linux_KernelParameter.cim \
	test/cim/Linux_NetworkCoreParameter.cim \
	test/cim/Linux_NetworkIPv4Parameter.cim \
	test/cim/Linux_NetworkUnixParameter.cim \
	test/cim/Linux_VirtualMemoryParameter.cim 

runtest: install
	cd $(DESTDIR)$(TESTSUITEDIR) &&	./sblim-cmpi-params-test.sh

doc_DATA+=README.TEST
endif

# Add files which can be used to create new param classes. 
EXTRA_DIST+= TEMPLATE.c create_files kernel-params 
