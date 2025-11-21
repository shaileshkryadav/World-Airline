# Flight Route Search System - README

**Course:** CSCE 2110  
**Date:** November 2025
**TEAM:** TEAM 16


## Project Overview

This program implements a flight route search system that models cities and flights as a directed graph. It provides four key functionalities:

1. **Connection-Limited Route Search:** Find if a route exists between two cities with at most x connections
2. **Path Through Intermediate Cities:** Find shortest route passing through two specified cities (order flexible)
3. **Hamiltonian Cycle:** Visit all reachable cities and return to start with minimum connections
4. **Optimal Meeting Point:** Find best meeting location for three people in different cities

The system uses graph algorithms including BFS (Breadth-First Search) and dynamic programming to efficiently solve these route optimization problems.

---


## File Structure

```
project/
│
├── WA.cpp       # Main program file with all implementations
├── city.name             # Input file: list of city names (one per line)
├── flight.txt            # Input file: flight connections
├── README.md             # This file
├── PROJECT_REPORT.pdf    # Detailed project report
└── Makefile              # Optional: automated compilation
```

./WA
```

### Step 1: Enter Number of Cities

```
Please enter the number of cities in your graph:
---------------------------------------------------
20
```


### Step 2: View Graph Output

The program will display:
- City name to number mappings
- Adjacency matrix representation of the graph

### Step 3: Select Feature

```
======================================
      Route Search Features
======================================
1. Find route with less than x connections
2. Find route through two cities (unordered)
3. Visit all reachable cities and return
4. Find optimal meeting point (different city)
0. Exit
======================================

Please select an option (0-4):
```

### Step 4: Follow Prompts

Each feature will prompt you for the required city names and parameters. Enter city names exactly as they appear in city.name.

### Step 5: View Results

The program displays the route and connection count, or informs you if no route exists.

### Step 6: Continue or Exit

The menu repeats, allowing multiple queries. Select option 0 to exit.

---

## Features and Usage

### Feature 1: Connection-Limited Route Search

**Purpose:** Check if you can fly from city A to city B with at most x connections

**Prompts:**
1. Enter starting city
2. Enter destination city
3. Enter maximum number of connections

**Example:**
```
Please select an option (0-4): 1
Enter starting city: Paris, France
Enter destination city: Hong Kong, SAR
Enter maximum number of connections: 4

Route: Paris, France to Singapore, Singapore to Hong Kong, SAR
Total connections: 2
```

**Output Cases:**
- If route exists within limit: Shows route and connection count
- If shortest route exceeds limit: "There is no such route with less than X connections"
- If no route exists: "There is no such route!"

### Feature 2: Path Through Two Cities (Unordered)

**Purpose:** Find shortest route from A to D passing through both B and C (order doesn't matter)

**Prompts:**
1. Enter starting city
2. Enter first city to pass through
3. Enter second city to pass through
4. Enter destination city

**Example:**
```
Please select an option (0-4): 2             
Enter starting city: Paris, France
Enter first city to pass through: Milan, Italy
Enter second city to pass through: Geneva, Switzerland
Enter destination city: Dublin, Ireland

Route: Paris, France to Milan, Italy to Geneva, Switzerland to New York City, United States to Dublin, Ireland
Smallest number of connections: 4
```

**Note:** The algorithm tries both orders (A→B→C→D and A→C→B→D) and returns the shorter path.

### Feature 3: Visit All Reachable Cities and Return

**Purpose:** Starting from city A, visit all reachable cities and return to A with minimum connections

**Prompts:**
1. Enter starting city

**Example:**
```
Please select an option (0-4): 3
Enter starting city: Paris, France
Computing Hamiltonian cycle (this may take a moment for large graphs)...

Route: Paris, France to Milan, Italy to Singapore, Singapore to Sydney, Australia to Shanghai, People's Republic of China to Oslo, Norway to Beijing, People's Republic of China to Seoul, South Korea to Geneva, Switzerland to New York City, United States to Dublin, Ireland to Osaka, Japan to London, United Kingdom to Istanbul, Turkey to Moscow, Russia to St. Petersburg, Russia to Tokyo, Japan to Paris, France
Smallest number of connections: 17
```

**Important Notes:**
- This feature uses dynamic programming and may take several seconds for graphs with 15+ cities
- Only visits cities that are reachable from start AND from which you can return to start
- Returns "There is no such route!" if no cycle exists

**Performance:**
- 10 cities: < 1 second
- 15 cities: 1-5 seconds
- 20 cities: 10-60 seconds
- 25+ cities: May take several minutes

### Feature 4: Optimal Meeting Point

**Purpose:** Find best city for three people to meet (minimizes total connections)

**Prompts:**
1. Enter city for person 1 (you)
2. Enter city for person 2 (John)
3. Enter city for person 3 (Ann)

**Example:**
```
Please select an option (0-4): 4
Enter city for person 1 (you): Seoul, South Korea
Enter city for person 2 (friend1): Paris, France
Enter city for person 3 (friend2): Singapore, Singapore

You three should meet at Zurich, Switzerland
Route for you: Seoul, South Korea to Zurich, Switzerland (1 connections)
Route for John: Paris, France to Milan, Italy to Zurich, Switzerland (2 connections)
Route for Ann: Singapore, Singapore to Zurich, Switzerland (1 connections)
Total number of connections: 4
```

**Constraints:**
- All three people must be in different cities
- Meeting point must be different from all three starting cities
- Returns "There is no such city!" if no valid meeting point exists

---

