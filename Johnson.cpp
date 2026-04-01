// ============================================================
// JOHNSON'S ALGORITHM
// All-pairs shortest path optimized for sparse graphs.
// Handles negative edge weights but NOT negative cycles.
//
// Steps:
//   1. Add a virtual source connected to all vertices (weight 0)
//   2. Run Bellman-Ford to compute reweighting potentials h[]
//   3. Reweight all edges so weights are non-negative
//   4. Run Dijkstra from every vertex on the reweighted graph
//   5. Undo the reweighting to get true distances
//
// Time Complexity:  O(VE log V)
// Space Complexity: O(V^2 + E)
// ============================================================

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
using namespace std;

const int INF = 1e9;

struct Edge {
    int u, v, w;
};

// ---- Step 1+2: Bellman-Ford from virtual source ----
// Virtual source (index V) has a zero-weight edge to every real vertex.
// Returns false if a negative cycle is detected.
bool bellmanFord(int V, const vector<Edge>& edges, vector<int>& h)
{
    h.assign(V + 1, INF);
    h[V] = 0;  // virtual source distance = 0

    for (int iter = 0; iter <= V; iter++)
    {
        bool updated = false;

        // Virtual source -> all real vertices (weight 0)
        for (int v = 0; v < V; v++)
            if (h[v] > 0) { h[v] = 0; updated = true; }

        // Real edges
        for (const auto& e : edges)
            if (h[e.u] != INF && h[e.u] + e.w < h[e.v])
            {
                if (iter == V)
                {
                    cout << "ERROR: Negative cycle detected!\n";
                    return false;
                }
                h[e.v] = h[e.u] + e.w;
                updated = true;
            }

        if (!updated) break;
    }
    return true;
}

// ---- Step 4: Dijkstra on reweighted graph ----
// Reweighted edge (u->v, w) becomes w' = w + h[u] - h[v]  (always >= 0)
vector<int> dijkstra(int V, int src,
                     const vector<vector<pair<int,int>>>& adj,
                     const vector<int>& h)
{
    vector<int> dist(V, INF);
    dist[src] = 0;

    // min-heap: {reweighted dist, vertex}
    priority_queue<pair<int,int>,
                   vector<pair<int,int>>,
                   greater<>> pq;
    pq.push({0, src});

    while (!pq.empty())
    {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;

        for (auto [v, w] : adj[u])
        {
            int rw = w + h[u] - h[v];   // reweighted, guaranteed >= 0
            if (dist[u] + rw < dist[v])
            {
                dist[v] = dist[u] + rw;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

// ---- Main Johnson's routine ----
// Returns all-pairs shortest path matrix (INF = unreachable).
vector<vector<int>> johnsons(int V,
                              const vector<Edge>& edges,
                              const vector<vector<pair<int,int>>>& adj)
{
    // Step 2: Bellman-Ford for potentials
    vector<int> h;
    if (!bellmanFord(V, edges, h)) return {};

    cout << "Reweighting potentials h[]: ";
    for (int i = 0; i < V; i++) cout << "h[" << i << "]=" << h[i] << " ";
    cout << "\n";

    // Step 3+4+5: Dijkstra from every vertex, undo reweighting
    vector<vector<int>> result(V, vector<int>(V, INF));

    for (int src = 0; src < V; src++)
    {
        vector<int> d = dijkstra(V, src, adj, h);
        for (int v = 0; v < V; v++)
            if (d[v] != INF)
                // True distance = d'[v] - h[src] + h[v]
                result[src][v] = d[v] - h[src] + h[v];
        result[src][src] = 0;
    }
    return result;
}

// ---- Helpers ----
void printMatrix(const string& title, int V,
                 const vector<vector<int>>& mat)
{
    cout << "\n" << title << "\n";
    cout << "     ";
    for (int j = 0; j < V; j++) cout << "  " << j << "  ";
    cout << "\n";
    for (int i = 0; i < V; i++)
    {
        cout << "  " << i << " ";
        for (int j = 0; j < V; j++)
        {
            if (mat[i][j] == INF) cout << " INF ";
            else                   cout << "  " << mat[i][j] << "  ";
        }
        cout << "\n";
    }
}

// ============================================================
// MAIN — Demo on a 4-vertex directed graph with a negative edge
//
// Graph edges:
//   0 -> 1  (weight  3)
//   1 -> 2  (weight  2)
//   2 -> 3  (weight  1)
//   0 -> 3  (weight  6)
//   3 -> 1  (weight -2)   <-- negative edge
// ============================================================
int main()
{
    int V = 4;

    vector<Edge> edges = {
        {0, 1,  3},
        {1, 2,  2},
        {2, 3,  1},
        {0, 3,  6},
        {3, 1, -2}
    };

    // Build adjacency list
    vector<vector<pair<int,int>>> adj(V);
    for (auto& e : edges) adj[e.u].push_back({e.v, e.w});

    cout << "========================================\n";
    cout << "          JOHNSON'S ALGORITHM           \n";
    cout << "========================================\n";
    cout << "\nGraph edges:\n";
    for (auto& e : edges)
        cout << "  " << e.u << " -> " << e.v << "  (weight " << e.w << ")\n";

    auto result = johnsons(V, edges, adj);

    if (!result.empty())
    {
        printMatrix("\nAll-pairs shortest path matrix:", V, result);

        cout << "\nSample paths (direct cost lookup):\n";
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (i != j)
                    cout << "  " << i << " -> " << j << " : "
                         << (result[i][j] == INF ? "INF" : to_string(result[i][j]))
                         << "\n";
    }

    return 0;
}

