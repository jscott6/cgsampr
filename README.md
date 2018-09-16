# cgsampr

This package provides efficient MCMC algorithms for uniform 
	sampling of graphs conditional on vertex level data. The algorithms use
	state dependent kernel selection to efficiently traverse the state space.
	Two settings are considered: 
	
1. uniform sampling of unweighted graphs conditional on vertex degrees.
2. uniform sampling of weighted graphs conditional on vertex strengths. 
	
In addition, one can fix an arbitrary set of possible edges as present or absent in all the sampled graphs.

The algorithms are flexible, and require little tuning to the problem at hand. In comparison to competing MCMC methods, they do not require (expensive) computation of a Markov basis when considering structurally fixed edges/non-edges. By construction, the samplers are irreducible in the face of arbitrary patterns of fixed edges/non-edges. They are efficient both in extremely sparse and dense graphs.
	
The package has a wide variety of possible applications; including network science, psychometrics, community ecology and categorical data analysis.
	
Please read the [introduction to cgsampr](./vignettes/introduction.Rmd).
	
Note that the package currently implements samplers only for directed graphs. In future versions, we hope to extend the algorithms to the undirected case.

### Installation

The package is easily installed using devtools. For example, you could use the command

~~~~
devtools::install_github()
~~~~

## Example Usage

~~~~
library(cgsampr)
adj <- diag(2)
f <- matrix(0, 2,2)
g <- new(UnweightedGraph, adj, f)
g$sample(5,0,0)
~~~~








