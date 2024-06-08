#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip> // Required for std::quoted

using namespace std;

// Structure to represent an edge
struct Edge {
    string line;
    string destination;
    int cost;
};

// Structure to represent the state in the priority queue
struct State {
    string station;
    int cost;
    string line;

    bool operator>(const State& other) const {
        return cost > other.cost;
    }
};

// Function to read the graph from a file
unordered_map<string, vector<Edge>> readGraph(const string& filename) {
    unordered_map<string, vector<Edge>> graph;
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error: Cannot open file " << filename << endl;
        return graph;
    }

    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string lineName;
        ss >> lineName;
        if (lineName.empty()) continue;
        lineName.pop_back(); // remove the colon

        string station;
        while (ss >> quoted(station)) {
            int cost;
            if (!(ss >> cost)) break; // Check if the cost was successfully read
            string nextStation;
            if (!(ss >> quoted(nextStation))) break; // Check if the next station was successfully read
            graph[station].push_back({lineName, nextStation, cost});
            station = nextStation;
        }
    }
    return graph;
}

// Function to find the shortest path using Dijkstra's algorithm
vector<string> findShortestPath(const unordered_map<string, vector<Edge>>& graph, const string& start, const string& target) {
    unordered_map<string, int> dist;
    unordered_map<string, pair<string, string>> prev; // {station, {prev_station, line}}
    priority_queue<State, vector<State>, greater<State>> pq;

    for (const auto& node : graph) {
        dist[node.first] = numeric_limits<int>::max();
    }
    dist[start] = 0;
    pq.push({start, 0, ""});

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();

        if (current.station == target) break;

        // Check if the current station exists in the graph
        if (graph.find(current.station) == graph.end()) {
            cerr << "Error: Station " << current.station << " not found in the graph" << endl;
            continue;
        }

        for (const Edge& edge : graph.at(current.station)) {
            int newDist = dist[current.station] + edge.cost;
            if (newDist < dist[edge.destination]) {
                dist[edge.destination] = newDist;
                prev[edge.destination] = {current.station, edge.line};
                pq.push({edge.destination, newDist, edge.line});
                cout << "Updated distance for station " << edge.destination << " to " << newDist << " via line " << edge.line << endl;
            }
        }
    }

    vector<string> path;
    string current = target;
    while (current != start) {
        if (prev.find(current) == prev.end()) {
            cerr << "Error: No previous station found for " << current << endl;
            return {};
        }
        path.push_back(current);
        current = prev[current].first;
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

// Function to print the path with details
void printPath(const vector<string>& path, const unordered_map<string, vector<Edge>>& graph) {
    if (path.size() < 2) {
        cout << "No path found." << endl;
        return;
    }

    int totalCost = 0;
    string prevLine = "";
    for (size_t i = 0; i < path.size() - 1; ++i) {
        if (graph.find(path[i]) == graph.end()) {
            cerr << "Error: Station " << path[i] << " not found in the graph" << endl;
            continue;
        }
        bool edgeFound = false;
        for (const Edge& edge : graph.at(path[i])) {
            if (edge.destination == path[i + 1]) {
                edgeFound = true;
                if (edge.line != prevLine) {
                    if (!prevLine.empty()) {
                        cout << "Change at " << path[i] << " to line " << edge.line << endl;
                    }
                    prevLine = edge.line;
                }
                cout << "Take line " << edge.line << " from " << path[i] << " to " << path[i + 1] << " (cost: " << edge.cost << ")" << endl;
                totalCost += edge.cost;
                break;
            }
        }
        if (!edgeFound) {
            cerr << "Error: Edge from " << path[i] << " to " << path[i + 1] << " not found" << endl;
        }
    }
    cout << "Total cost: " << totalCost << endl;
}

int main() {
    string filename = "input2.txt"; // Input .txt file
    string start = "Harrow & Wealdstone"; // Change this to your desired start station
    string target = "Epping"; // Change this to your desired target station

    unordered_map<string, vector<Edge>> graph = readGraph(filename);

    if (graph.empty()) {
        cout << "Graph is empty or could not be loaded. Exiting." << endl;
        return 1;
    }

    vector<string> path = findShortestPath(graph, start, target);
    printPath(path, graph);

    return 0;
}
