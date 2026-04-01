**CSF303 — Practical 3**

# **Overview**

This practical required the implementation of three graph algorithms that are Floyd-Warshall, Johnson's Algorithm, and Boruvka's Algorithm. Each algorithm addresses a different class of graph problem.

# **1. Floyd-Warshall Algorithm**

## **Complexity**

Time: O(V³)     •     Space: O(V²)

## **How It Works**

Floyd-Warshall is an all-pairs shortest path algorithm built on the dynamic programming. The core idea is very straightforward and it is like for every pair of vertices (i, j), check whether routing the path through an intermediate vertex k produces a shorter distance. This check is repeated for every possible k, iterating through all V vertices.

The invariant that makes this correct is: after processing intermediate vertex k, dist\[i\]\[j\] holds the shortest path from i to j using only vertices {0..k} as intermediates. By the time k reaches V-1, every possible intermediate has been considered and all distances are optimal.

## **Key Observations**

•  Handles negative edge weights correctly, as long as there are no negative cycles.

•  Negative cycles can be detected by checking if dist\[i\]\[i\] \< 0 after the algorithm completes.

•  The simplicity of the triple nested loop makes it very easy to implement and debug.

•  Best suited for dense graphs where E ≈ V², as the cubic time cost is unavoidable regardless of edge count.

## **Program Output**

![][image1]

*Figure 1: Floyd-Warshall — initial matrix, final shortest-path matrix, and reconstructed paths*

The output shows the distance matrix transforming from the raw adjacency representation to the fully solved all-pairs matrix. Path reconstruction confirms that the algorithm correctly routes paths through intermediate vertices — for example, path 0→3 routes directly (cost 5), while 0→2 routes through vertex 3 (cost 7), correctly avoiding the non-existent direct edge.

# **2\. Johnson's Algorithm**

## **Complexity**

Time: O(VE log V)     •     Space: O(V² \+ E)

## **How It Works**

Johnson's Algorithm solves the same all-pairs shortest path problem as Floyd-Warshall, but is optimized for sparse graphs. The challenge it addresses is that Dijkstra's algorithm — which is faster than Bellman-Ford for individual source queries — cannot handle negative edge weights. Johnson's bridges this gap through a reweighting technique.

The algorithm works in five steps: first, a virtual source vertex is added and connected to every real vertex with zero-weight edges. Bellman-Ford is then run from this virtual source to compute potential values h\[v\] for every vertex. These potentials are used to reweight every edge: w'(u,v) \= w(u,v) \+ h\[u\] \- h\[v\], which is guaranteed to be non-negative. Dijkstra's algorithm then runs from every real vertex on the reweighted graph, and the results are corrected back to true distances by undoing the reweighting.

## **Key Observations**

•  The reweighting formula w' \= w \+ h\[u\] \- h\[v\] is the algorithm's most elegant insight. It preserves relative path lengths because the h values telescope and cancel along any path.

•  Bellman-Ford is only run once — the expensive O(VE) step happens exactly one time regardless of how many source vertices are queried.

•  For sparse graphs where E ≪ V², Johnson's O(VE log V) comfortably beats Floyd-Warshall's O(V³).

•  Like Floyd-Warshall, it detects and rejects graphs containing negative cycles.

## **Program Output**

![][image2]

*Figure 2: Johnson's Algorithm — reweighting potentials, all-pairs matrix, and sample path costs including negative distances*

The output includes the intermediate reweighting potentials h\[\], which confirms that Bellman-Ford ran correctly. The final matrix shows negative distances (e.g., vertex 2 to vertex 1 costs \-1), which is expected given the negative edge 3→1 with weight \-2. This demonstrates that Johnson's correctly handles and reports negative-weight shortest paths after undoing the reweighting transformation.

# **3\. Boruvka's Algorithm**

## **Complexity**

Time: O(E log V)     •     Space: O(V \+ E)

## **How It Works**

Boruvka's Algorithm finds the Minimum Spanning Tree (MST) of an undirected weighted graph — a fundamentally different problem from the shortest path algorithms above. Instead of finding optimal routes between all pairs, it finds the minimum-weight subset of edges that connects all vertices.

The algorithm operates in phases. In each phase, every connected component independently identifies the cheapest edge leaving it and adds that edge to the MST. Components then merge along the added edges, and the process repeats. The correctness relies on the Cut Property of MSTs: the minimum weight edge crossing any cut (partition of vertices into two sets) is always safe to include in the MST.

The number of components at least halves each phase — since every component merges with at least one other — giving O(log V) phases total. Each phase scans all E edges, yielding the overall O(E log V) time complexity. Union-Find with path compression and union by rank keeps the per-phase component management efficient.

## **Key Observations**

•  Boruvka's is the oldest MST algorithm, invented in 1926, yet its phase structure maps naturally onto modern parallel and distributed computing.

•  Unlike Kruskal's (which sorts all edges upfront) or Prim's (which grows a single tree), Boruvka's grows all components simultaneously — making it inherently parallel.

•  Space efficiency is excellent at O(V \+ E), requiring only the edge list and Union-Find arrays.

•  The algorithm correctly produces exactly V-1 edges for a connected graph with V vertices.

## **Program Output**

![][image3]

*Figure 3: Boruvka's Algorithm — phase-by-phase edge additions and final MST with total weight 37*

The output shows both phases clearly. Phase 1 reduces 9 components to 3 by greedily adding the cheapest outgoing edge from each component. Phase 2 adds the final edge to produce a single connected MST with total weight 37 across 8 edges — matching the known optimal for this graph. The verification line confirms the MST has exactly V-1 \= 8 edges, as expected.

# **Reflection**

Working through these three algorithms gave me a clearer picture of how different problem constraints — edge weight polarity, graph density, and whether you need shortest paths or a spanning tree — drive completely different algorithmic strategies.

Floyd-Warshall was the most conceptually straightforward. The triple nested loop is simple to reason about, and watching the distance matrix transform step by step made the dynamic programming logic click intuitively. Its weakness is scale: O(V³) becomes impractical quickly, and there is no way to avoid it regardless of how few edges the graph has.

Johnson's Algorithm felt like a clever engineering solution to a real constraint. The reweighting idea — using Bellman-Ford potentials to make all edges non-negative without changing which paths are shortest — is not obvious at first, but once the math is worked through it is genuinely elegant. The payoff is significant for sparse graphs, and the ability to correctly return negative distances after undoing the reweighting confirms that the transformation is truly transparent to the final result.

Boruvka's Algorithm was the most surprising because it solves a different problem entirely, and does so with a strategy that feels inherently parallel. The insight that every component can independently pick its cheapest outgoing edge simultaneously — and that this greedy choice is always safe by the Cut Property — is both simple and powerful. It also highlighted an important distinction: minimum spanning tree and shortest path are related but separate problems, and confusing them leads to wrong answers.

The clearest lesson across all three is that algorithm selection is always about matching assumptions to constraints. For small dense graphs needing all-pairs distances, Floyd-Warshall is simple and sufficient. For large sparse graphs with possible negative edges, Johnson's is the right tool. And when the problem is connectivity rather than distance — especially in a distributed setting — Boruvka's is not just viable but arguably the most natural fit. Understanding these trade-offs felt more valuable than implementing any single algorithm in isolation.

# **Algorithm Comparison Summary**

The table below summarises the key properties of each algorithm for quick reference.

Floyd-Warshall:   Time O(V³)  |  Space O(V²)  |  Problem: All-pairs SP  |  Best for: Dense graphs

Johnson's:        Time O(VE log V)  |  Space O(V²+E)  |  Problem: All-pairs SP  |  Best for: Sparse graphs with negative edges

Boruvka's:        Time O(E log V)   |  Space O(V+E)    |  Problem: MST           |  Best for: Parallel/distributed settings

