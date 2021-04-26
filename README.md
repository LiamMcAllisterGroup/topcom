# TOPCOM: Triangulations of Point Configurations and Oriented Matroids
#### Modified Version 0.17.8 for CYTools

A modified version of the [TOPCOM](http://www.rambau.wm.uni-bayreuth.de/TOPCOM/) package that is used for the [CYTools](https://github.com/LiamMcAllisterGroup/cytools) package. TOPCOM was originally written by [Jörg Rambau](http://www.rambau.wm.uni-bayreuth.de/) and it was packaged for Debian by [Doug Torrance](https://webwork.piedmont.edu/~dtorrance/). This repository is a fork of the [TOPCOM Debian repository](https://salsa.debian.org/science-team/topcom).

This version of TOPCOM contains additional functionality compared to the original one. In particular, it has the ability to perform random walks in the graph of triangulations by performing random flips, and it can compute $$\mathcal{K}_\cup$$ (i.e. the union of the Kähler cones of toric varieties containing a particular Calabi-Yau hypersurface that is unchanged under the transition to any of those varieties). These tools can be used with the binaries ```topcom-randomwalk``` and ```topcom-computekcup```.

The original TOPCOM code, the modifications by Doug Torrance, and the modifications for CYTools are distributed under the [GPL-2+ license](http://www.gnu.org/licenses/gpl-2.0.html).

The original README file is included as README_ORIGINAL.

## Mod Usage
#### Random Walks
A random walk with $n$ random steps can be performed with ```topcom-randomwalk```. The input file takes the standard format from TOPCOM, followed by the number of random flips $n$ enclosed in parenthesis. If $$n$$ is negative, regularity is not checked at each step. The random flips include non-fine and non-star triangulations, but the flags can be hardcoded in the source file to restrict to these. In CYTools we find it more convenient to impose these restrictions in Python. After performing $$n-1$$ flips it prints all of the triangulations that differ by an additional flip. Thus, one can obtain the neighbor triangulations from a starting one by using $$n=1$$.

This is an example random walk with of a polytope with $h^{1,1}=5 that performs 10 random flips without checking regularity.
```bash
echo "[[-1, 0, 0, 0, 1], [1, -1, 1, 0, 1], [-1, 0, -1, 0, 1], [-1, 0, -1, -1, 1], [-1, 0, 0, -1, 1], [1, 0, -1, 2, 1], [-1, 1, -1, -1, 1], [0, 0, 1, -1, 1], [0, 0, -1, 1, 1], [0, 0, 0, 0, 1]] [] {{3,7,6,9,1},{6,9,0,8,5},{1,4,9,2,0},{4,6,9,2,0},{7,1,0,9,5},{9,1,0,8,5},{1,3,9,2,4},{3,6,9,2,4},{6,7,0,9,5},{1,7,6,9,5},{1,3,7,9,4},{3,1,6,9,5},{3,6,7,9,4},{3,1,9,8,5},{9,1,0,2,8},{3,9,6,8,5},{6,9,0,2,8},{3,1,9,2,8},{3,9,6,2,8},{1,4,7,9,0},{4,6,7,9,0}} (-10)" | topcom-randomwalk
```

#### Computation of $$\mathcal{K}_\cup$$
The computation of $$\mathcal{K}_\cup$$ can be performed with ```topcom-computekcup```. Again, the input follows the standard format, except that the 2-faces of the polytope must be specified with an array using parenthesis. The output of the function are the extremal rays of $$\mathcal{M}_\cap$$ (i.e. the dual of $$\mathcal{K}_\cup$$).

This is an example with a polytope with $h^{1,1}=5$.
```bash
echo "[[-1, 0, 0, 0, 1], [1, -1, 1, 0, 1], [-1, 0, -1, 0, 1], [-1, 0, -1, -1, 1], [-1, 0, 0, -1, 1], [1, 0, -1, 2, 1], [-1, 1, -1, -1, 1], [0, 0, 1, -1, 1], [0, 0, -1, 1, 1], [0, 0, 0, 0, 1]] [] {{3,7,6,9,1},{6,9,0,8,5},{1,4,9,2,0},{4,6,9,2,0},{7,1,0,9,5},{9,1,0,8,5},{1,3,9,2,4},{3,6,9,2,4},{6,7,0,9,5},{1,7,6,9,5},{1,3,7,9,4},{3,1,6,9,5},{3,6,7,9,4},{3,1,9,8,5},{9,1,0,2,8},{3,9,6,8,5},{6,9,0,2,8},{3,1,9,2,8},{3,9,6,2,8},{1,4,7,9,0},{4,6,7,9,0}} ((0,1,2),(1,2,3),(0,1,4),(1,3,4),(0,2,3,4),(0,1,5),(0,2,5,8),(1,2,5,8),(1,3,5),(2,3,5,8),(1,3,6),(2,3,6),(3,4,6),(1,5,6),(2,5,6,8),(3,5,6),(0,2,6),(0,5,6),(0,4,6),(0,1,7),(1,3,7),(0,4,7),(1,4,7),(3,4,7),(0,5,7),(1,5,7),(1,6,7),(3,6,7),(4,6,7),(5,6,7),(0,6,7))" | topcom-computekcup
```
Note that trivial 2-faces can be safely ignored, but we included them here for completeness.
