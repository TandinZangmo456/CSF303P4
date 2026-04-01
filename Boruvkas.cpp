// ============================================================
// BORUVKA'S ALGORITHM
// Minimum Spanning Tree (MST) for undirected weighted graphs.
//
// Core idea:
//   Each phase: every connected component picks its cheapest
//   outgoing edge and adds it to the MST. Components then merge.
//   Repeat until only one component remains.
//
// Why it works:
//   The cheapest edge leaving any component is always safe to
//   add (Cut Property of MSTs) — it cannot create a cycle
//   across two different components.
//
// Time Complexity:  O(E log V)
//   — O(log V) phases (components halve each phase)
//   — O(E) edge scan per phase
// Space Complexity: O(V + E)
//   — O(V) for Union-Find
//   — O(E) for edge list
// ============================================================
 
#include <iostream>
#include <vector>
#include <tuple>
#include <numeric>
#include <climits>
using namespace std;
 
// ---- Union-Find (Disjoint Set Union) with path compression + union by rank ----
struct UnionFind {
    vector<int> parent, rank_;
 
    UnionFind(int n) : parent(n), rank_(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }
 
    // Find root with path compression
    int find(int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
    }
 
    // Union by rank — returns false if already in same component
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (rank_[a] < rank_[b]) swap(a, b);
        parent[b] = a;
        if (rank_[a] == rank_[b]) rank_[a]++;
        return true;
    }
 
    bool connected(int a, int b) { return find(a) == find(b); }
};
 
// ---- Borůvka's Core ----
// edges: {weight, u, v}  (undirected)
// Returns: {MST total weight, list of MST edges as (u,v,weight)}
pair<long long, vector<tuple<int,int,int>>>
boruvka(int V, vector<tuple<int,int,int>>& edges)
{
    UnionFind uf(V);
    long long mstWeight = 0;
    vector<tuple<int,int,int>> mstEdges;
    int components = V;
    int phase = 0;
 
    // Each phase halves the number of components → O(log V) phases total
    while (components > 1)
    {
        phase++;
        cout << "\n  [Phase " << phase << "] Components remaining: " << components << "\n";
 
        // cheapest[c] = {weight, u, v} of cheapest outgoing edge for component c
        // Initialized to {INT_MAX, -1, -1} meaning "not found yet"
        vector<tuple<int,int,int>> cheapest(V, {INT_MAX, -1, -1});
 
        // Scan every edge — O(E) per phase
        for (auto& [w, u, v] : edges)
        {
            int cu = uf.find(u);
            int cv = uf.find(v);
            if (cu == cv) continue;  // same component, skip (would create cycle)
 
            // Update cheapest outgoing edge for each component
            if (w < get<0>(cheapest[cu])) cheapest[cu] = {w, u, v};
            if (w < get<0>(cheapest[cv])) cheapest[cv] = {w, u, v};
        }
 
        // Add each component's cheapest edge to the MST (if not already merged)
        for (int c = 0; c < V; c++)
        {
            auto& [w, u, v] = cheapest[c];
            if (u == -1) continue;  // component had no outgoing edge
 
            if (uf.unite(u, v))
            {
                mstWeight += w;
                mstEdges.push_back({u, v, w});
                components--;
                cout << "    Added edge " << u << " -- " << v
                     << "  (weight " << w << ")\n";
            }
        }
    }
 
    return {mstWeight, mstEdges};
}
 
// ============================================================
// MAIN — Demo on a classic 9-vertex graph
//
// Vertices: 0..8
// Edges (undirected):
//   0-1 (4),  0-7 (8)
//   1-2 (8),  1-7 (11)
//   2-3 (7),  2-5 (4),  2-8 (2)
//   3-4 (9),  3-5 (14)
//   4-5 (10)
//   5-6 (2)
//   6-7 (1),  6-8 (6)
//   7-8 (7)
//
// Known MST edges: 6-7(1), 2-8(2), 5-6(2), 0-1(4), 2-5(4), 2-3(7), 0-7(8), 3-4(9)
// Known MST total weight: 37
// ============================================================
int main()
{
    int V = 9;
 
    // {weight, u, v}
    vector<tuple<int,int,int>> edges = {
        { 4, 0, 1}, { 8, 0, 7},
        { 8, 1, 2}, {11, 1, 7},
        { 7, 2, 3}, { 4, 2, 5}, { 2, 2, 8},
        { 9, 3, 4}, {14, 3, 5},
        {10, 4, 5},
        { 2, 5, 6},
        { 1, 6, 7}, { 6, 6, 8},
        { 7, 7, 8}
    };
 
    cout << "========================================\n";
    cout << "        BORUVKA'S ALGORITHM (MST)       \n";
    cout << "========================================\n";
 
    cout << "\nGraph: " << V << " vertices, " << edges.size() << " edges\n";
    cout << "\nAll edges:\n";
    for (auto& [w, u, v] : edges)
        cout << "  " << u << " -- " << v << "  (weight " << w << ")\n";
 
    cout << "\nRunning Boruvka's phases:";
    auto [mstW, mstE] = boruvka(V, edges);
 
    cout << "\n========================================\n";
    cout << "MST Result:\n";
    cout << "  Total weight: " << mstW << "\n";
    cout << "  Edges in MST (" << mstE.size() << " edges for " << V << " vertices):\n";
    for (auto& [u, v, w] : mstE)
        cout << "    " << u << " -- " << v << "  (weight " << w << ")\n";
 
    // Verify: MST of a connected graph has exactly V-1 edges
    cout << "\n  Verification: MST has " << mstE.size()
         << " edges (expected " << V - 1 << " for " << V << " vertices)\n";
 
    return 0;
}
 