bin_PROGRAMS = checkregularity

checkregularity_SOURCES = checkregularity.cc

LDADD           = ../lib-src-reg/libCHECKREG.a \
                  ../lib-src/libTOPCOM.a \
                  ../external/lib/libcddgmp.a \
                  ../external/lib/libgmpxx.a \
                  ../external/lib/libgmp.a

AM_CPPFLAGS     = -I../lib-src
AM_CPPFLAGS    += -I../lib-src-reg
AM_CPPFLAGS    += -I../wrap-gmp-gmpxx
AM_CPPFLAGS    += -I../external/include
AM_CPPFLAGS    += -I$(includedir)


AM_CPPFLAGS    += -DVERBOSE -DGMPRATIONAL -ftemplate-depth-30

AM_CXXFLAGS     = -O2

