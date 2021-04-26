# TOPCOM: Triangulations of Point Configurations and Oriented Matroids
#### Modified Version 0.17.8 for CYTools

A modified version of the [TOPCOM](http://www.rambau.wm.uni-bayreuth.de/TOPCOM/) package that is used for the [CYTools](https://github.com/LiamMcAllisterGroup/cytools) package. TOPCOM was originally written by [Jörg Rambau](http://www.rambau.wm.uni-bayreuth.de/) and it was packaged for Debian by [Doug Torrance](https://webwork.piedmont.edu/~dtorrance/). This repository is a fork of the [TOPCOM Debian repository](https://salsa.debian.org/science-team/topcom).

This version of TOPCOM contains additional functionality compared to the original one. In particular, it has the ability to perform random walks in the graph of triangulations by performing random flips, and it can compute $$\mathcal{K}_\cup$$ (i.e. the union of the Kähler cones of toric varieties containing a particular Calabi-Yau hypersurface that is unchanged under the transition to any of those varieties). These tools can be used with the binaries ```topcom-randomwalk``` and ```topcom-computekcup```.

The original TOPCOM code, the modifications by Doug Torrance, and the modifications for CYTools are distributed under the [GPL-2+ license](http://www.gnu.org/licenses/gpl-2.0.html).

The original README file is included as README_ORIGINAL.
