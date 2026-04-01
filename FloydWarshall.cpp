// FLOYD-WARSHALL ALGORITHM
// All-pairs shortest path using dynamic programming.
// Handles negative edge weights but NOT negative cycles.
//
// Time Complexity:  O(V^3)
// Space Complexity: O(V^2)
// ============================================================
 
#include <iostream>
#include <vector>
#include <climits>
using namespace std;
 
const int INF = 1e9;
 
// ---- Core Algorithm ----
// dist[i][j]  = shortest distance from i to j
// next[i][j]  = next node on the shortest path from i to j
void floydWarshall(int V,
                   vector<vector<int>>& dist,
                   vector<vector<int>>& next)
{
    // Build 'next' table for path reconstruction
    next.assign(V, vector<int>(V, -1));
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            if (i != j && dist[i][j] != INF)
                next[i][j] = j;
 
    // Relax every pair (i, j) through each intermediate vertex k
    // After k iterations: dist[i][j] = shortest path using {0..k} as intermediates
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
 
    // Detect negative cycles: dist[i][i] < 0 means a negative cycle exists
    for (int i = 0; i < V; i++)
        if (dist[i][i] < 0)
            cout << "WARNING: Negative cycle detected at vertex " << i << "\n";
}
 
// ---- Path Reconstruction ----
vector<int> getPath(int u, int v, const vector<vector<int>>& next)
{
    if (next[u][v] == -1) return {};
    vector<int> path = {u};
    while (u != v) { u = next[u][v]; path.push_back(u); }
    return path;
}
 
// ---- Helpers ----
void printMatrix(const string& title, int V, const vector<vector<int>>& dist)
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
            if (dist[i][j] == INF) cout << " INF ";
            else                   cout << "  " << dist[i][j] << "  ";
        }
        cout << "\n";
    }
}
 
void printPath(int u, int v, const vector<vector<int>>& next,
               const vector<vector<int>>& dist)
{
    cout << "  Path " << u << " -> " << v << ": ";
    if (dist[u][v] == INF) { cout << "No path\n"; return; }
    auto path = getPath(u, v, next);
    for (int i = 0; i < (int)path.size(); i++)
    {
        cout << path[i];
        if (i + 1 < (int)path.size()) cout << " -> ";
    }
    cout << "  (cost " << dist[u][v] << ")\n";
}
 
// ============================================================
// MAIN — Demo on a 4-vertex directed weighted graph
//
// Graph edges:
//   0 -> 1  (weight 3)
//   0 -> 3  (weight 5)
//   1 -> 0  (weight 2)
//   1 -> 3  (weight 4)
//   2 -> 1  (weight 1)
//   3 -> 2  (weight 2)
// ============================================================
int main()
{
    int V = 4;
 
    // Adjacency matrix; INF = no direct edge, 0 on diagonal
    vector<vector<int>> dist = {
        {  0,   3, INF,   5 },
        {  2,   0, INF,   4 },
        { INF,  1,   0, INF },
        { INF, INF,  2,   0 }
    };
 
    vector<vector<int>> next;
 
    cout << "========================================\n";
    cout << "       FLOYD-WARSHALL ALGORITHM         \n";
    cout << "========================================\n";
 
    printMatrix("Initial distance matrix (INF = no direct edge):", V, dist);
 
    floydWarshall(V, dist, next);
 
    printMatrix("Final shortest-path distance matrix:", V, dist);
 
    cout << "\nShortest paths between all pairs:\n";
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            if (i != j) printPath(i, j, next, dist);
 
    return 0;
}