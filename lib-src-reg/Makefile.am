lib_LIBRARIES = libCHECKREG.a

libCHECKREG_a_SOURCES = \
SPXinterface.cc \
LPinterface.cc \
RegularityCheck.cc

include_HEADERS = \
SPXinterface.hh \
RegularityCheck.hh \
LPinterface.hh

AM_CPPFLAGS     = -I../lib-src 
AM_CPPFLAGS    += -I../wrap-gmp-gmpxx
AM_CPPFLAGS    += -I../external/include
AM_CPPFLAGS    += -I$(includedir)

AM_CPPFLAGS    += -DVERBOSE -DGMPRATIONAL -ftemplate-depth-30

AM_CXXFLAGS     = -O2
