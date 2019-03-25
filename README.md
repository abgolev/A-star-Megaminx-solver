# A-star-Megaminx-solver
Create and randomize a Megaminx puzzle, and then use the A* search algorithm to solve it.


**To run:**

g++ -O3 megaminx2.cpp

./a.out <br/>

**About the design:**

![Megaminx](https://i.ibb.co/hX72ZNC/megaminx2.jpg)

Figure 1: The Megaminx flattened out. Each pentagon represents a face. Connecting lines represent adjacent faces. <br/>

![Megaminx2](https://i.ibb.co/NFyBSfz/megaminx3.jpg)

Figure 2: Each blue digit represents a node. Each face has 10 nodes, not counting the stationary central node. <br/>

![Megaminx3](https://i.ibb.co/rydjL5L/megaminx4.jpg)

Figure 3: A representation of the nodes of adjacent faces that move when Face 3 is turned. <br/>

**About the Megaminx:**

Runs into issues whenever randomized more than 14 turns because priority queue blows up. 24 new configurations are added to the priority queue any time a previous configuration is expanded. Tried limiting priority queue to 100000 items and deleting the 20000 back ones any time it fills up, but that causes solutions to disappear because heuristic is so naive.

Currently working on a better heuristic that works using an adjacency matrix representing faces which are originally adjacent and then using Floyd-Walsall Algorithm to create a distance matrix for the nearest distance between nodes.
