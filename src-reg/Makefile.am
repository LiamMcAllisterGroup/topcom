bin_PROGRAMS = checkregularity

checkregularity_SOURCES = checkregularity.cc

LDADD           = ../lib-src/libTOPCOM.a \
                  ../lib-src-reg/libCHECKREG.a \
                  ../external/lib/libcddgmp.a \
                  ../external/lib/libqsopt_ex.a
if USE_SOPLEX
LDADD          += ../external/lib/libsoplex.a
endif
LDADD          += ../external/lib/libgmpxx.a \
                  ../external/lib/libgmp.a

AM_CPPFLAGS    += -I../lib-src
AM_CPPFLAGS    += -I../lib-src-reg
AM_CPPFLAGS    += -I../wrap-gmp-gmpxx
AM_CPPFLAGS    += -I../external/include
AM_CPPFLAGS    += -I$(includedir)

AM_CPPFLAGS    += -ftemplate-depth=100
