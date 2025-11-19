/***********************************************************
  Synopsis: read from city.name and flight.txt
            store the graph in a adjacency matrix
            implement route search features
***********************************************************/

#include <iostream>
#include <vector>
#include <ctime> 
#include <cstdlib>
#include <string>
#include <fstream>
#include <map>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

class Graph{
     public:
          Graph(int d);
          ~Graph(){};

          void print();
          void addEdge(int node1, int node2){ adj[node1][node2] = 1; };
          vector<int> findShortestPath(int src, int dest);
          vector<int> findPathThroughUnordered(int src, int via1, int via2, int dest);
          vector<int> findHamiltonianCycle(int start);
          int findMeetingPoint(int a, int b, int c, vector<int>& pathA, vector<int>& pathB, vector<int>& pathC);
          void dfsVisit(int node, vector<bool>& visited);
          int getNumNodes() { return n; }
     private: 
          int n;
          int adj[140][140];
};

Graph::Graph(int d){
    for (int i=0; i < d; i++){
        for (int j=0; j < d; j++){
            adj[i][j] = 0;
        }
    }
    n = d;
};

void Graph::print(){
    for (int i=0; i < n; i++){
        cout << i << " -> ";
        for (int j=0; j < n; j++){
            if (adj[i][j]){
                cout << j << " ";
            }
        }
        cout << endl;
    }
};

// BFS to find shortest path
vector<int> Graph::findShortestPath(int src, int dest){
    vector<int> path;
    if (src == dest) {
        path.push_back(src);
        return path;
    }
    
    vector<int> parent(n, -1);
    vector<bool> visited(n, false);
    queue<int> q;
    
    q.push(src);
    visited[src] = true;
    
    while (!q.empty()){
        int curr = q.front();
        q.pop();
        
        if (curr == dest) break;
        
        for (int i = 0; i < n; i++){
            if (adj[curr][i] && !visited[i]){
                visited[i] = true;
                parent[i] = curr;
                q.push(i);
            }
        }
    }
    
    if (parent[dest] == -1 && dest != src) return path; // No path found
    
    // Reconstruct path
    int curr = dest;
    while (curr != -1){
        path.push_back(curr);
        curr = parent[curr];
    }
    reverse(path.begin(), path.end());
    
    return path;
}

// Find path from src through via1 and via2 to dest
vector<int> Graph::findPathThroughUnordered(int src, int via1, int via2, int dest){
    // Try both orders: src -> via1 -> via2 -> dest and src -> via2 -> via1 -> dest
    vector<int> path1_part1 = findShortestPath(src, via1);
    vector<int> path1_part2 = findShortestPath(via1, via2);
    vector<int> path1_part3 = findShortestPath(via2, dest);
    
    vector<int> path2_part1 = findShortestPath(src, via2);
    vector<int> path2_part2 = findShortestPath(via2, via1);
    vector<int> path2_part3 = findShortestPath(via1, dest);
    
    vector<int> fullPath1, fullPath2;
    
    // Construct path 1: src -> via1 -> via2 -> dest
    if (!path1_part1.empty() && !path1_part2.empty() && !path1_part3.empty()){
        fullPath1.insert(fullPath1.end(), path1_part1.begin(), path1_part1.end());
        fullPath1.insert(fullPath1.end(), path1_part2.begin() + 1, path1_part2.end());
        fullPath1.insert(fullPath1.end(), path1_part3.begin() + 1, path1_part3.end());
    }
    
    // Construct path 2: src -> via2 -> via1 -> dest
    if (!path2_part1.empty() && !path2_part2.empty() && !path2_part3.empty()){
        fullPath2.insert(fullPath2.end(), path2_part1.begin(), path2_part1.end());
        fullPath2.insert(fullPath2.end(), path2_part2.begin() + 1, path2_part2.end());
        fullPath2.insert(fullPath2.end(), path2_part3.begin() + 1, path2_part3.end());
    }
    
    // Return the shorter path
    if (fullPath1.empty()) return fullPath2;
    if (fullPath2.empty()) return fullPath1;
    return (fullPath1.size() <= fullPath2.size()) ? fullPath1 : fullPath2;
}

// DFS helper to find all reachable nodes
void Graph::dfsVisit(int node, vector<bool>& visited){
    visited[node] = true;
    for (int i = 0; i < n; i++){
        if (adj[node][i] && !visited[i]){
            dfsVisit(i, visited);
        }
    }
}

// Find shortest cycle visiting all reachable cities from start
vector<int> Graph::findHamiltonianCycle(int start){
    // First, find all cities reachable from start
    vector<bool> reachable(n, false);
    dfsVisit(start, reachable);
    
    vector<int> reachableCities;
    for (int i = 0; i < n; i++){
        if (reachable[i]){
            reachableCities.push_back(i);
        }
    }
    
    // Check if we can return to start from each reachable city
    vector<int> validCities;
    validCities.push_back(start);
    for (int city : reachableCities){
        if (city != start){
            vector<int> pathBack = findShortestPath(city, start);
            if (!pathBack.empty()){
                validCities.push_back(city);
            }
        }
    }
    
    // If only start city is valid, no cycle possible
    if (validCities.size() == 1){
        return vector<int>();
    }
    
    // Use dynamic programming with bitmask to find shortest Hamiltonian cycle
    int numCities = validCities.size();
    map<int, int> cityIndex;
    for (int i = 0; i < numCities; i++){
        cityIndex[validCities[i]] = i;
    }
    
    // dp[mask][i] = minimum connections to visit cities in mask ending at city i
    vector<vector<int>> dp(1 << numCities, vector<int>(numCities, INT_MAX));
    vector<vector<int>> parent(1 << numCities, vector<int>(numCities, -1));
    
    // Start at city 0 (start city)
    dp[1][0] = 0;
    
    // Fill DP table
    for (int mask = 1; mask < (1 << numCities); mask++){
        for (int last = 0; last < numCities; last++){
            if (!(mask & (1 << last))) continue;
            if (dp[mask][last] == INT_MAX) continue;
            
            for (int next = 0; next < numCities; next++){
                if (mask & (1 << next)) continue;
                
                vector<int> path = findShortestPath(validCities[last], validCities[next]);
                if (!path.empty()){
                    int newMask = mask | (1 << next);
                    int cost = dp[mask][last] + path.size() - 1;
                    if (cost < dp[newMask][next]){
                        dp[newMask][next] = cost;
                        parent[newMask][next] = last;
                    }
                }
            }
        }
    }
    
    // Find best ending city to return to start
    int fullMask = (1 << numCities) - 1;
    int bestLast = -1;
    int bestCost = INT_MAX;
    
    for (int last = 1; last < numCities; last++){
        if (dp[fullMask][last] == INT_MAX) continue;
        vector<int> returnPath = findShortestPath(validCities[last], start);
        if (!returnPath.empty()){
            int totalCost = dp[fullMask][last] + returnPath.size() - 1;
            if (totalCost < bestCost){
                bestCost = totalCost;
                bestLast = last;
            }
        }
    }
    
    if (bestLast == -1) return vector<int>();
    
    // Reconstruct the full cycle
    vector<int> cityOrder;
    int mask = fullMask;
    int curr = bestLast;
    
    while (curr != -1){
        cityOrder.push_back(validCities[curr]);
        int prevMask = mask ^ (1 << curr);
        int prev = parent[mask][curr];
        mask = prevMask;
        curr = prev;
    }
    
    reverse(cityOrder.begin(), cityOrder.end());
    
    // Build the actual path with all intermediate cities
    vector<int> fullCycle;
    for (int i = 0; i < cityOrder.size() - 1; i++){
        vector<int> segment = findShortestPath(cityOrder[i], cityOrder[i + 1]);
        if (i == 0){
            fullCycle.insert(fullCycle.end(), segment.begin(), segment.end());
        } else {
            fullCycle.insert(fullCycle.end(), segment.begin() + 1, segment.end());
        }
    }
    
    // Add return path to start
    vector<int> returnPath = findShortestPath(cityOrder.back(), start);
    fullCycle.insert(fullCycle.end(), returnPath.begin() + 1, returnPath.end());
    
    return fullCycle;
}

// Find optimal meeting point for 3 people
int Graph::findMeetingPoint(int a, int b, int c, vector<int>& pathA, vector<int>& pathB, vector<int>& pathC){
    int bestMeeting = -1;
    int minTotal = INT_MAX;
    
    for (int meeting = 0; meeting < n; meeting++){
        // Meeting point must be different from all three starting cities
        if (meeting == a || meeting == b || meeting == c) continue;
        
        vector<int> pA = findShortestPath(a, meeting);
        vector<int> pB = findShortestPath(b, meeting);
        vector<int> pC = findShortestPath(c, meeting);
        
        if (!pA.empty() && !pB.empty() && !pC.empty()){
            int total = (pA.size() - 1) + (pB.size() - 1) + (pC.size() - 1);
            if (total < minTotal){
                minTotal = total;
                bestMeeting = meeting;
                pathA = pA;
                pathB = pB;
                pathC = pC;
            }
        }
    }
    
    return bestMeeting;
}

struct strCmp {
    bool operator()( const string s1, const string s2 ) const {
      return s1 < s2;
    }
};

map<const string, int, strCmp> city;
map<int, string> cityName;

void printPath(const vector<int>& path){
    for (size_t i = 0; i < path.size(); i++){
        cout << cityName[path[i]];
        if (i < path.size() - 1) cout << " to ";
    }
}

int main(int argc, char *argv[]){
   int i, j, node1, node2, n;
   string line;

   cout << "Please enter the number of cities in your graph: " << endl;
   cout << "---------------------------------------------------" << endl;
   cin >> n;

   Graph graph(n);
 
   char lineChar[256];

   ifstream cityFile;
   cityFile.open("city.name", ios::in);

   for (i = 0; i < n; i++){
      cityFile.getline(lineChar, 256);
      line = lineChar;
      city[line] = i;
      cityName[i] = line;
      cout << line << " is numbered as city " << city[line] << endl;
   }

   cityFile.close();

   ifstream flightFile;
   flightFile.open("flight.txt", ios::in);

   while (flightFile.getline(lineChar, 256)){
      line = lineChar;

      if (line.find("From:", 0) == 0){
          line.erase(0, 7);
          node1 = city[line];
      } else {
          line.erase(0, 7);
          node2 = city[line];
          graph.addEdge(node1, node2);
      }
   }
   flightFile.close();

   cout << endl << "The graph generated can be represented by the following adjacent matrix : " << endl;
   cout << "-----------------------------------------------------------------------------------" << endl;
   graph.print();

   cout << "\n\n======================================" << endl;
   cout << "      Route Search Features" << endl;
   cout << "======================================" << endl;
   cout << "1. Find route with less than x connections" << endl;
   cout << "2. Find route through two cities (unordered)" << endl;
   cout << "3. Visit all reachable cities and return" << endl;
   cout << "4. Find optimal meeting point (different city)" << endl;
   cout << "0. Exit" << endl;
   cout << "======================================" << endl;
   
   int option;
   while (true){
       cout << "\nPlease select an option (0-4): ";
       cin >> option;
       cin.ignore(); 
       
       if (option == 0){
           cout << "Exited" << endl;
           break;
       }
       
       if (option == 1){
           
           string cityA, cityB;
           int maxConnections;
           cout << "Enter starting city: ";
           getline(cin, cityA);
           cout << "Enter destination city: ";
           getline(cin, cityB);
           cout << "Enter maximum number of connections: ";
           cin >> maxConnections;
           cin.ignore();
           
           if (city.find(cityA) == city.end() || city.find(cityB) == city.end()){
               cout << "Error: One or both cities not found!" << endl;
               continue;
           }
           
           vector<int> path = graph.findShortestPath(city[cityA], city[cityB]);
           
           if (path.empty()){
               cout << "There is no such route!" << endl;
           } else {
               int numConnections = path.size() - 1;
               if (numConnections <= maxConnections){
                   cout << "\nRoute: ";
                   printPath(path);
                   cout << "\nTotal connections: " << numConnections << endl;
               } else {
                   cout << "There is no such route with less than " << maxConnections << " connections." << endl;
                   cout << "(Shortest route has " << numConnections << " connections)" << endl;
               }
           }
       }
       else if (option == 2){
           
           string cityA, cityB, cityC, cityD;
           cout << "Enter starting city: ";
           getline(cin, cityA);
           cout << "Enter first city to pass through: ";
           getline(cin, cityB);
           cout << "Enter second city to pass through: ";
           getline(cin, cityC);
           cout << "Enter destination city: ";
           getline(cin, cityD);
           
           if (city.find(cityA) == city.end() || city.find(cityB) == city.end() ||
               city.find(cityC) == city.end() || city.find(cityD) == city.end()){
               cout << "Error: One or more cities not found!" << endl;
               continue;
           }
           
           vector<int> path = graph.findPathThroughUnordered(city[cityA], city[cityB], city[cityC], city[cityD]);
           
           if (path.empty()){
               cout << "There is no such route!" << endl;
           } else {
               cout << "\nRoute: ";
               printPath(path);
               cout << "\nSmallest number of connections: " << (path.size() - 1) << endl;
           }
       }
       else if (option == 3){
           
           string cityA;
           cout << "Enter starting city: ";
           getline(cin, cityA);
           
           if (city.find(cityA) == city.end()){
               cout << "Error: City not found!" << endl;
               continue;
           }
           
           cout << "Computing Hamiltonian cycle (this may take a moment for large graphs)..." << endl;
           vector<int> cycle = graph.findHamiltonianCycle(city[cityA]);
           
           if (cycle.empty()){
               cout << "There is no such route!" << endl;
           } else {
               cout << "\nRoute: ";
               printPath(cycle);
               cout << "\nSmallest number of connections: " << (cycle.size() - 1) << endl;
           }
       }
       else if (option == 4){
           string cityA, cityB, cityC;
           cout << "Enter city for person 1 (you): ";
           getline(cin, cityA);
           cout << "Enter city for person 2 (friend1): ";
           getline(cin, cityB);
           cout << "Enter city for person 3 (friend2): ";
           getline(cin, cityC);
           
           if (city.find(cityA) == city.end() || city.find(cityB) == city.end() ||
               city.find(cityC) == city.end()){
               cout << "Error: One or more cities not found!" << endl;
               continue;
           }
           
           if (cityA == cityB || cityB == cityC || cityA == cityC){
               cout << "Error: All three people must be in different cities!" << endl;
               continue;
           }
           
           vector<int> pathA, pathB, pathC;
           int meeting = graph.findMeetingPoint(city[cityA], city[cityB], city[cityC], pathA, pathB, pathC);
           
           if (meeting == -1){
               cout << "There is no such city!" << endl;
           } else {
               cout << "\nYou three should meet at " << cityName[meeting] << endl;
               cout << "Route for you: ";
               printPath(pathA);
               cout << " (" << (pathA.size() - 1) << " connections)" << endl;
               cout << "Route for John: ";
               printPath(pathB);
               cout << " (" << (pathB.size() - 1) << " connections)" << endl;
               cout << "Route for Ann: ";
               printPath(pathC);
               cout << " (" << (pathC.size() - 1) << " connections)" << endl;
               cout << "Total number of connections: " << (pathA.size() + pathB.size() + pathC.size() - 3) << endl;
           }
       }
       else {
           cout << "Invalid option! Please select 0-4." << endl;
       }
   }

   return 0;
}