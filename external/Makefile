##############################################################################
## Makefile for building external packages
## 2003/01/27 jr
##############################################################################
SH           = sh

GMP_DONE     = gmp_done
CDD_DONE     = cdd_done

GMP_NAME     = gmp
CDD_NAME     = cddlib

GMP_INCLUDES = include/gmp.h
CDD_INCLUDES = include/cdd.h  include/cddmp.h  include/cddtypes.h  include/setoper.h

GMP_LIBS     = lib/libgmp.a
CDD_LIBS     = lib/libcddgmp.a

GMP_VERSION = 6.1.1
GMP_COMPRESSFORMAT = bz2
GMP_EXTRACTOPTIONS = --bzip2 -xvf

CDD_VERSION = 0.94f-TOPCOM
CDD_COMPRESSFORMAT = gz
CDD_EXTRACTOPTIONS = -xzvf

GMP_DIR = $(GMP_NAME)-$(GMP_VERSION)
CDD_DIR = $(CDD_NAME)-$(CDD_VERSION)

GMP_TAR      = $(GMP_NAME)-$(GMP_VERSION).tar.$(GMP_COMPRESSFORMAT)
CDD_TAR      = $(CDD_NAME)-$(CDD_VERSION).tar.$(CDD_COMPRESSFORMAT)

GMP_DIR      = $(GMP_NAME)-$(GMP_VERSION)
CDD_DIR      = $(CDD_NAME)-$(CDD_VERSION)

all: gmp cdd

gmp: $(GMP_DONE)

cdd: $(CDD_DONE)

$(GMP_DONE): $(GMP_DIR)
	@echo ==============================================================================
	@echo making gmp ...
	@echo ==============================================================================
	$(SH) -c 'cd $(GMP_DIR); ./configure --prefix=`pwd`/.. --libdir=`pwd`/../lib --enable-cxx'
	make -C $(GMP_DIR)
	make -C $(GMP_DIR) check
	make -C $(GMP_DIR) install
	@echo ==============================================================================
	@echo installing patched wrapper:
	@echo ==============================================================================
	$(SH) -c 'cp gmpxx-patch/gmpxx.h include/'
	@echo ==============================================================================
	@echo ... done making gmp.
	@echo ==============================================================================
	touch $(GMP_DONE)

$(CDD_DONE): $(CDD_DIR)
	@echo ==============================================================================
	@echo making cdd ...
	@echo ==============================================================================
	$(SH) -c 'cd $(CDD_DIR); ./configure --prefix=`pwd`/.. --libdir=`pwd`/../lib '
	make -C $(CDD_DIR)
	make -C $(CDD_DIR) install
	@echo ==============================================================================
	@echo ... done making cdd.
	@echo ==============================================================================
	touch $(CDD_DONE)

$(GMP_DIR): $(GMP_TAR)
	@echo ==============================================================================
	@echo unpacking gmp ...
	@echo ==============================================================================
	tar $(GMP_EXTRACTOPTIONS) $(GMP_TAR)
	@echo ==============================================================================
	@echo ... done unpacking gmp.
	@echo ==============================================================================

$(CDD_DIR): $(CDD_TAR)
	@echo ==============================================================================
	@echo unpacking cdd ...
	@echo ==============================================================================
	tar $(CDD_EXTRACTOPTIONS) $(CDD_TAR)
	@echo ==============================================================================
	@echo ... done unpacking cdd.
	@echo ==============================================================================

.PHONY: clean clean_installdirs clean_gmp clean_cdd

clean: clean_gmp clean_cdd

clean_installdirs:
	rm -rf include lib info bin

clean_gmp:
	rm -f $(GMP_DONE)
	rm -rf $(GMP_DIR)
	rm -rf include lib info bin

clean_cdd:
	rm -f $(CDD_DONE)
	rm -rf $(CDD_DIR)


# eof Makefile jr.
