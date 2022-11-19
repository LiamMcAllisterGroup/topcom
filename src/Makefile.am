bin_PROGRAMS = \
B_A \
B_A_center \
B_D \
B_D_center \
B_S \
B_S_center \
binomial \
check \
chiro2allfinetriangs \
chiro2alltriangs \
chiro2circuits \
chiro2cocircuits \
chiro2dual \
chiro2finetriang \
chiro2finetriangs \
chiro2mintriang \
chiro2nallfinetriangs \
chiro2nalltriangs \
chiro2ncircuits \
chiro2ncocircuits \
chiro2nfinetriangs \
chiro2ntriangs \
chiro2placingtriang \
chiro2triangs \
cocircuits2facets \
cross \
cube \
cyclic \
Dnxk \
hypersimplex \
lattice \
kDn \
permutahedron \
points2allfinetriangs \
points2alltriangs \
points2chiro \
points2circuits \
points2cocircuits \
points2facets \
points2finetriang \
points2finetriangs \
points2flips \
points2gale \
points2mintriang \
points2nallfinetriangs \
points2nalltriangs \
points2ncircuits \
points2ncocircuits \
points2nfinetriangs \
points2nflips \
points2ntriangs \
points2placingtriang \
points2prettyprint \
points2symmetries \
points2triangs \
points2vertices \
points2volume \
santos_triang \
santos_dim4_triang \
santos_22_triang

B_A_SOURCES                        = B_A.cc
B_A_center_SOURCES                 = B_A_center.cc
B_D_SOURCES                        = B_D.cc
B_D_center_SOURCES                 = B_D_center.cc
B_S_SOURCES                        = B_S.cc
B_S_center_SOURCES                 = B_S_center.cc
binomial_SOURCES                   = binomial.cc
check_SOURCES                      = check.cc
chiro2allfinetriangs_SOURCES       = chiro2allfinetriangs.cc
chiro2alltriangs_SOURCES           = chiro2alltriangs.cc
chiro2circuits_SOURCES             = chiro2circuits.cc
chiro2cocircuits_SOURCES           = chiro2cocircuits.cc
chiro2dual_SOURCES                 = chiro2dual.cc
chiro2finetriang_SOURCES           = chiro2finetriang.cc
chiro2finetriangs_SOURCES          = chiro2finetriangs.cc
chiro2mintriang_SOURCES            = chiro2mintriang.cc
chiro2nallfinetriangs_SOURCES      = chiro2nallfinetriangs.cc
chiro2nalltriangs_SOURCES          = chiro2nalltriangs.cc
chiro2ncircuits_SOURCES            = chiro2ncircuits.cc
chiro2ncocircuits_SOURCES          = chiro2ncocircuits.cc
chiro2nfinetriangs_SOURCES         = chiro2nfinetriangs.cc
chiro2ntriangs_SOURCES             = chiro2ntriangs.cc
chiro2placingtriang_SOURCES        = chiro2placingtriang.cc
chiro2triangs_SOURCES              = chiro2triangs.cc
cocircuits2facets_SOURCES          = cocircuits2facets.cc
cross_SOURCES                      = cross.cc
cube_SOURCES                       = cube.cc
cyclic_SOURCES                     = cyclic.cc
Dnxk_SOURCES                       = Dnxk.cc
hypersimplex_SOURCES               = hypersimplex.cc
lattice_SOURCES	                   = lattice.cc
kDn_SOURCES                        = kDn.cc
permutahedron_SOURCES              = permutahedron.cc
points2allfinetriangs_SOURCES      = points2allfinetriangs.cc
points2alltriangs_SOURCES          = points2alltriangs.cc
points2chiro_SOURCES               = points2chiro.cc
points2circuits_SOURCES            = points2circuits.cc
points2cocircuits_SOURCES          = points2cocircuits.cc
points2facets_SOURCES              = points2facets.cc
points2finetriang_SOURCES          = points2finetriang.cc
points2finetriangs_SOURCES         = points2finetriangs.cc
points2flips_SOURCES               = points2flips.cc
points2gale_SOURCES                = points2gale.cc
points2mintriang_SOURCES           = points2mintriang.cc
points2nallfinetriangs_SOURCES     = points2nallfinetriangs.cc
points2nalltriangs_SOURCES         = points2nalltriangs.cc
points2ncircuits_SOURCES           = points2ncircuits.cc
points2ncocircuits_SOURCES         = points2ncocircuits.cc
points2nfinetriangs_SOURCES        = points2nfinetriangs.cc
points2nflips_SOURCES              = points2nflips.cc
points2ntriangs_SOURCES            = points2ntriangs.cc
points2placingtriang_SOURCES       = points2placingtriang.cc
points2prettyprint_SOURCES         = points2prettyprint.cc
points2symmetries_SOURCES          = points2symmetries.cc
points2triangs_SOURCES             = points2triangs.cc
points2vertices_SOURCES            = points2vertices.cc
points2volume_SOURCES              = points2volume.cc
santos_triang_SOURCES              = santos_triang.cc
santos_dim4_triang_SOURCES         = santos_dim4_triang.cc
santos_22_triang_SOURCES           = santos_22_triang.cc

LDADD           = ../lib-src/libTOPCOM.a \
                  ../lib-src-reg/libCHECKREG.a \
                  ../external/lib/libcddgmp.a
if USE_QSOPTEX
LDADD          += ../external/lib/libqsopt_ex.a
endif
if USE_SOPLEX
LDADD          += ../external/lib/libsoplex.a
endif
if USE_LOCAL_GMP
LDADD          += ../external/lib/libgmpxx.a \
                  ../external/lib/libgmp.a
else
LIBS           += -lgmpxx -lgmp
endif


AM_CPPFLAGS    += -I../lib-src
AM_CPPFLAGS    += -I../lib-src-reg
AM_CPPFLAGS    += -I../wrap-gmp-gmpxx
AM_CPPFLAGS    += -I../external/include
AM_CPPFLAGS    += -I$(includedir)

AM_CPPFLAGS    += -ftemplate-depth=100

