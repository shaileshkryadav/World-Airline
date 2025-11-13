/***********************************************************
  Synopsis: routeSearch - Flight route search program
            Implements 4 query types for finding routes between cities
***********************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <queue>
#include <algorithm>
#include <climits>
#include <sstream>
#include <cstring>

using namespace std;

struct strCmp {
    bool operator()(const string s1, const string s2) const {
        return s1 < s2;
    }
};

class Graph {
public:
    int numCities;
    vector<vector<int>> adjList;  // Adjacency list for efficient traversal
    vector<string> cityNames;     // Map index to city name
    map<string, int, strCmp> cityToIndex;  // Map city name to index
    
    Graph(int n) : numCities(n) {
        adjList.resize(n);
        cityNames.resize(n);
    }
    
    void addEdge(int from, int to) {
        adjList[from].push_back(to);
    }
    
    // BFS to find shortest path from start to end
    // Returns path and number of connections (hops)
    pair<vector<int>, int> shortestPath(int start, int end) {
        if (start == end) {
            return make_pair(vector<int>{start}, 0);
        }
        
        queue<int> q;
        vector<bool> visited(numCities, false);
        vector<int> parent(numCities, -1);
        
        q.push(start);
        visited[start] = true;
        parent[start] = -1;
        
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            
            for (int neighbor : adjList[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    parent[neighbor] = current;
                    q.push(neighbor);
                    
                    if (neighbor == end) {
                        // Reconstruct path
                        vector<int> path;
                        int node = end;
                        while (node != -1) {
                            path.push_back(node);
                            node = parent[node];
                        }
                        reverse(path.begin(), path.end());
                        return make_pair(path, path.size() - 1);
                    }
                }
            }
        }
        
        return make_pair(vector<int>(), -1);  // No path found
    }
    
    // BFS to find all reachable cities from a starting city
    vector<int> getReachableCities(int start) {
        vector<bool> visited(numCities, false);
        queue<int> q;
        vector<int> reachable;
        
        q.push(start);
        visited[start] = true;
        
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            reachable.push_back(current);
            
            for (int neighbor : adjList[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        
        return reachable;
    }
    
    // Find shortest distances from a source to all nodes
    vector<int> shortestDistances(int source) {
        vector<int> dist(numCities, INT_MAX);
        queue<int> q;
        
        dist[source] = 0;
        q.push(source);
        
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            
            for (int neighbor : adjList[current]) {
                if (dist[neighbor] == INT_MAX) {
                    dist[neighbor] = dist[current] + 1;
                    q.push(neighbor);
                }
            }
        }
        
        return dist;
    }
};

// Parse command line arguments for question 1
void question1(Graph& graph, int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Usage: routeSearch 1 <city_A> <city_B> <num_connection>" << endl;
        return;
    }
    
    string cityA = argv[2];
    string cityB = argv[3];
    int maxConnections = atoi(argv[4]);
    
    if (graph.cityToIndex.find(cityA) == graph.cityToIndex.end() ||
        graph.cityToIndex.find(cityB) == graph.cityToIndex.end()) {
        cerr << "Error: One or more cities not found" << endl;
        return;
    }
    
    int start = graph.cityToIndex[cityA];
    int end = graph.cityToIndex[cityB];
    
    pair<vector<int>, int> result = graph.shortestPath(start, end);
    
    if (result.second == -1) {
        cout << "No route found from " << cityA << " to " << cityB << endl;
        return;
    }
    
    if (result.second > maxConnections) {
        cout << "No route found with less than " << maxConnections << " connections" << endl;
        return;
    }
    
    // Output route
    for (size_t i = 0; i < result.first.size(); i++) {
        if (i > 0) cout << " to ";
        cout << graph.cityNames[result.first[i]];
    }
    cout << endl;
    cout << "total connection: " << result.second << endl;
}

// Parse command line arguments for question 2
void question2(Graph& graph, int argc, char* argv[]) {
    if (argc < 8) {
        cerr << "Usage: routeSearch 2 <city_A> through <city_B> and <city_C> to <city_D>" << endl;
        return;
    }
    
    string cityA = argv[2];
    string cityB = argv[4];
    string cityC = argv[6];
    string cityD = argv[8];
    
    if (graph.cityToIndex.find(cityA) == graph.cityToIndex.end() ||
        graph.cityToIndex.find(cityB) == graph.cityToIndex.end() ||
        graph.cityToIndex.find(cityC) == graph.cityToIndex.end() ||
        graph.cityToIndex.find(cityD) == graph.cityToIndex.end()) {
        cerr << "Error: One or more cities not found" << endl;
        return;
    }
    
    int a = graph.cityToIndex[cityA];
    int b = graph.cityToIndex[cityB];
    int c = graph.cityToIndex[cityC];
    int d = graph.cityToIndex[cityD];
    
    // Try both orders: A -> B -> C -> D and A -> C -> B -> D
    // Find shortest path for each combination
    
    pair<vector<int>, int> path1 = graph.shortestPath(a, b);
    pair<vector<int>, int> path2, path3;
    vector<int> bestPath;
    int bestConnections = INT_MAX;
    
    if (path1.second != -1) {
        path2 = graph.shortestPath(b, c);
        if (path2.second != -1) {
            path3 = graph.shortestPath(c, d);
            if (path3.second != -1) {
                // Combine paths (remove duplicate nodes at junctions)
                vector<int> combined;
                for (int node : path1.first) combined.push_back(node);
                for (size_t i = 1; i < path2.first.size(); i++) combined.push_back(path2.first[i]);
                for (size_t i = 1; i < path3.first.size(); i++) combined.push_back(path3.first[i]);
                
                int totalConnections = path1.second + path2.second + path3.second;
                if (totalConnections < bestConnections) {
                    bestConnections = totalConnections;
                    bestPath = combined;
                }
            }
        }
    }
    
    // Try A -> C -> B -> D
    path1 = graph.shortestPath(a, c);
    if (path1.second != -1) {
        path2 = graph.shortestPath(c, b);
        if (path2.second != -1) {
            path3 = graph.shortestPath(b, d);
            if (path3.second != -1) {
                vector<int> combined;
                for (int node : path1.first) combined.push_back(node);
                for (size_t i = 1; i < path2.first.size(); i++) combined.push_back(path2.first[i]);
                for (size_t i = 1; i < path3.first.size(); i++) combined.push_back(path3.first[i]);
                
                int totalConnections = path1.second + path2.second + path3.second;
                if (totalConnections < bestConnections) {
                    bestConnections = totalConnections;
                    bestPath = combined;
                }
            }
        }
    }
    
    if (bestConnections == INT_MAX) {
        cout << "No route found from " << cityA << " through " << cityB 
             << " and " << cityC << " to " << cityD << endl;
        return;
    }
    
    // Output route
    for (size_t i = 0; i < bestPath.size(); i++) {
        if (i > 0) cout << " to ";
        cout << graph.cityNames[bestPath[i]];
    }
    cout << endl;
    cout << "smallest number of connection: " << bestConnections << endl;
}

// Parse command line arguments for question 3
void question3(Graph& graph, int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: routeSearch 3 <city_A>" << endl;
        return;
    }
    
    string cityA = argv[2];
    
    if (graph.cityToIndex.find(cityA) == graph.cityToIndex.end()) {
        cerr << "Error: City not found" << endl;
        return;
    }
    
    int start = graph.cityToIndex[cityA];
    
    // Get all reachable cities
    vector<int> reachable = graph.getReachableCities(start);
    
    if (reachable.size() == 1) {
        // Only the starting city is reachable
        cout << cityA << " to " << cityA << endl;
        cout << "smallest number of connection: 0" << endl;
        return;
    }
    
    // Remove the starting city from the list (we'll return to it at the end)
    vector<int> toVisit;
    for (int city : reachable) {
        if (city != start) {
            toVisit.push_back(city);
        }
    }
    
    // Use a greedy approach: visit cities in a way that minimizes connections
    // We can revisit cities if needed, but we need to visit each city at least once
    vector<int> path;
    path.push_back(start);
    int current = start;
    int totalConnections = 0;
    vector<bool> mustVisit(graph.numCities, false);  // Cities we must visit at least once
    for (int city : toVisit) {
        mustVisit[city] = true;
    }
    
    // Visit all cities using a greedy approach
    while (!toVisit.empty()) {
        int nearestCity = -1;
        int minDist = INT_MAX;
        vector<int> bestPathToNearest;
        
        // Try to find the nearest unvisited city from current position
        for (int city : toVisit) {
            pair<vector<int>, int> result = graph.shortestPath(current, city);
            if (result.second != -1 && result.second < minDist) {
                minDist = result.second;
                nearestCity = city;
                bestPathToNearest = result.first;
            }
        }
        
        // If we can't reach any city from current position, try backtracking to start
        if (nearestCity == -1) {
            // Check if we can get back to start
            pair<vector<int>, int> backToStart = graph.shortestPath(current, start);
            if (backToStart.second == -1) {
                // Can't get back to start either
                cout << "No route found to visit all reachable cities" << endl;
                return;
            }
            
            // Check if any remaining cities are reachable from start
            bool foundReachable = false;
            for (int city : toVisit) {
                pair<vector<int>, int> result = graph.shortestPath(start, city);
                if (result.second != -1) {
                    foundReachable = true;
                    if (result.second < minDist) {
                        minDist = result.second;
                        nearestCity = city;
                        bestPathToNearest = result.first;
                    }
                }
            }
            
            if (!foundReachable) {
                // No remaining cities are reachable even from start
                cout << "No route found to visit all reachable cities" << endl;
                return;
            }
            
            // Add backtrack path to start (skip start itself if it's already in path)
            if (backToStart.first.size() > 1) {
                for (size_t i = 1; i < backToStart.first.size(); i++) {
                    path.push_back(backToStart.first[i]);
                }
                totalConnections += backToStart.second;
            }
            current = start;
        }
        
        // Add path to nearest city (skip the first node as it's already in path)
        for (size_t i = 1; i < bestPathToNearest.size(); i++) {
            int cityOnPath = bestPathToNearest[i];
            path.push_back(cityOnPath);
            // Mark this city as visited if it was in our must-visit list
            if (mustVisit[cityOnPath]) {
                mustVisit[cityOnPath] = false;
                toVisit.erase(remove(toVisit.begin(), toVisit.end(), cityOnPath), toVisit.end());
            }
        }
        
        current = nearestCity;
        totalConnections += minDist;
    }
    
    // Find path back to start
    pair<vector<int>, int> returnPath = graph.shortestPath(current, start);
    if (returnPath.second == -1) {
        cout << "No route found to return to " << cityA << endl;
        return;
    }
    
    // Add return path (skip the first node)
    for (size_t i = 1; i < returnPath.first.size(); i++) {
        path.push_back(returnPath.first[i]);
    }
    totalConnections += returnPath.second;
    
    // Output route
    for (size_t i = 0; i < path.size(); i++) {
        if (i > 0) cout << " to ";
        cout << graph.cityNames[path[i]];
    }
    cout << endl;
    cout << "smallest number of connection: " << totalConnections << endl;
}

// Parse command line arguments for question 4
void question4(Graph& graph, int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Usage: routeSearch 4 <city_A> <city_B> <city_C>" << endl;
        return;
    }
    
    string cityA = argv[2];
    string cityB = argv[3];
    string cityC = argv[4];
    
    if (graph.cityToIndex.find(cityA) == graph.cityToIndex.end() ||
        graph.cityToIndex.find(cityB) == graph.cityToIndex.end() ||
        graph.cityToIndex.find(cityC) == graph.cityToIndex.end()) {
        cerr << "Error: One or more cities not found" << endl;
        return;
    }
    
    int a = graph.cityToIndex[cityA];
    int b = graph.cityToIndex[cityB];
    int c = graph.cityToIndex[cityC];
    
    // Get shortest distances from all three cities
    vector<int> distFromA = graph.shortestDistances(a);
    vector<int> distFromB = graph.shortestDistances(b);
    vector<int> distFromC = graph.shortestDistances(c);
    
    // Find the meeting city that minimizes total connections
    int bestMeetingCity = -1;
    int minTotalConnections = INT_MAX;
    
    for (int i = 0; i < graph.numCities; i++) {
        // Skip the three starting cities
        if (i == a || i == b || i == c) continue;
        
        // Check if all three can reach this city
        if (distFromA[i] != INT_MAX && distFromB[i] != INT_MAX && distFromC[i] != INT_MAX) {
            int totalConnections = distFromA[i] + distFromB[i] + distFromC[i];
            if (totalConnections < minTotalConnections) {
                minTotalConnections = totalConnections;
                bestMeetingCity = i;
            }
        }
    }
    
    if (bestMeetingCity == -1) {
        cout << "No meeting city found that all three can reach" << endl;
        return;
    }
    
    // Find paths for each person
    pair<vector<int>, int> pathA = graph.shortestPath(a, bestMeetingCity);
    pair<vector<int>, int> pathB = graph.shortestPath(b, bestMeetingCity);
    pair<vector<int>, int> pathC = graph.shortestPath(c, bestMeetingCity);
    
    // Output results
    cout << "You three should meet at " << graph.cityNames[bestMeetingCity] << endl;
    
    cout << "Route for first person: ";
    for (size_t i = 0; i < pathA.first.size(); i++) {
        if (i > 0) cout << " to ";
        cout << graph.cityNames[pathA.first[i]];
    }
    cout << " (" << pathA.second << " connections)" << endl;
    
    cout << "Route for second person: ";
    for (size_t i = 0; i < pathB.first.size(); i++) {
        if (i > 0) cout << " to ";
        cout << graph.cityNames[pathB.first[i]];
    }
    cout << " (" << pathB.second << " connections)" << endl;
    
    cout << "Route for third person: ";
    for (size_t i = 0; i < pathC.first.size(); i++) {
        if (i > 0) cout << " to ";
        cout << graph.cityNames[pathC.first[i]];
    }
    cout << " (" << pathC.second << " connections)" << endl;
    
    cout << "Total number of connection: " << minTotalConnections << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: routeSearch <question_number> [arguments...]" << endl;
        cerr << "  Question 1: routeSearch 1 <city_A> <city_B> <num_connection>" << endl;
        cerr << "  Question 2: routeSearch 2 <city_A> through <city_B> and <city_C> to <city_D>" << endl;
        cerr << "  Question 3: routeSearch 3 <city_A>" << endl;
        cerr << "  Question 4: routeSearch 4 <city_A> <city_B> <city_C>" << endl;
        return 1;
    }
    
    int questionNum = atoi(argv[1]);
    
    // Read city names
    ifstream cityFile("city.name");
    if (!cityFile.is_open()) {
        cerr << "Error: Cannot open city.name file" << endl;
        return 1;
    }
    
    vector<string> cities;
    string line;
    while (getline(cityFile, line)) {
        if (!line.empty()) {
            cities.push_back(line);
        }
    }
    cityFile.close();
    
    int numCities = cities.size();
    Graph graph(numCities);
    
    // Initialize city names and mapping
    for (int i = 0; i < numCities; i++) {
        graph.cityNames[i] = cities[i];
        graph.cityToIndex[cities[i]] = i;
    }
    
    // Read flight connections
    ifstream flightFile("flight.txt");
    if (!flightFile.is_open()) {
        cerr << "Error: Cannot open flight.txt file" << endl;
        return 1;
    }
    
    int currentFrom = -1;
    while (getline(flightFile, line)) {
        if (line.find("From:") == 0) {
            string cityName = line.substr(7);  // Skip "From:  "
            if (graph.cityToIndex.find(cityName) != graph.cityToIndex.end()) {
                currentFrom = graph.cityToIndex[cityName];
            }
        } else if (line.find("To  :") == 0) {
            string cityName = line.substr(7);  // Skip "To  :  "
            if (graph.cityToIndex.find(cityName) != graph.cityToIndex.end() && currentFrom != -1) {
                int to = graph.cityToIndex[cityName];
                graph.addEdge(currentFrom, to);
            }
        } else if (line.find("       ") == 0) {
            // Additional destination on next line
            string cityName = line.substr(7);  // Skip "       "
            if (graph.cityToIndex.find(cityName) != graph.cityToIndex.end() && currentFrom != -1) {
                int to = graph.cityToIndex[cityName];
                graph.addEdge(currentFrom, to);
            }
        }
    }
    flightFile.close();
    
    // Handle different questions
    switch (questionNum) {
        case 1:
            question1(graph, argc, argv);
            break;
        case 2:
            question2(graph, argc, argv);
            break;
        case 3:
            question3(graph, argc, argv);
            break;
        case 4:
            question4(graph, argc, argv);
            break;
        default:
            cerr << "Error: Invalid question number. Must be 1-4." << endl;
            return 1;
    }
    
    return 0;
}
