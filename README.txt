graphGen.cpp generate a directed graph with each node as a city
and each edge as a flight service from one node to the
other. The output is in the format of:
-------------------------------------------------------------
From:   Moscow, Russia
To  :   Beijing, People's Republic of China
        Montevideo, Uruguay

From:   Hong Kong, SAR
To  :   Miami, United States
        Dhaka, Bangladesh
...
-------------------------------------------------------------  

WA.cpp read from city.name and flight.txt and intialize a
adjacency matrix based graph representaion of the flight
information.  It associates each city with an integer as
its ID and prints out the flight information from each city 
to other cities using the IDs assigned.

To compile and run:
  download graphGen.cpp, city.name, and makefile
  type "make"

To run graphGen:
  type "./graphGen <number_cities>"
 
Note: number_cities is between 1 - 140

To run WA:
  type "./WA"
