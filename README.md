# A-star-Megaminx-solver
Create and randomize a Megaminx puzzle, and then use the A* search algorithm to solve it.


**To compile:**

g++ -O3 megaminx2.cpp <br/>

**To run:**

./a.out <br/>

**About the Megaminx:**

Dodecahedron puzzle with 12 pentagonal faces, each with a different color. Each face can be turned clockwise or counterclockwise, allowing 24 different possible turns. 

**About the design:**

![Megaminx](https://i.ibb.co/BwtbZfC/megaminx5.png)<br/>
Figure 1: The actual Megaminx. The White Face, arbitrarily labeled Face 3, along with its 10 movable nodes, labeled 0-9.

<br/>

![Megaminx2](https://i.ibb.co/hX72ZNC/megaminx2.jpg) <br/>
Figure 2: The Megaminx flattened out. Each pentagon represents a face. Connecting lines represent adjacent faces. 

<br/>

![Megaminx3](https://i.ibb.co/NFyBSfz/megaminx3.jpg)<br/>
Figure 3: Each blue digit represents a node. Each face has 10 nodes, not counting the stationary central node. 

<br/>

![Megaminx4](https://i.ibb.co/rydjL5L/megaminx4.jpg)<br/>
Figure 4: A representation of the nodes of adjacent faces that move when Face 3 is turned. 

<br/>

![Megaminx5](https://i.ibb.co/sP6tDtk/megaminx7.png)<br/>
Figure 5: Sample stdout output of a solved cube with face numbers. In the actual output, the middle node is the respective color of the solved face. Each character represents a color (r = red) and the characters are stored in a 12x10 char array with the rows representing the face number and the columns representing the node number.

<br/>

**About the Megaminx Solver:**

Works well on randomizations of 14 turns or fewer.

Runs into issues whenever randomized more than 14 turns because priority queue blows up. 24 new configurations are added to the priority queue any time a previous configuration is expanded. Limiting priority queue to 100000 items and deleting the 20000 back ones any time it fills doesn't currently work because heuristic isn't fine tuned enough, and the correct path may be deleted. 

Currently working on a better heuristic that works using an adjacency matrix representing faces which are originally adjacent and then using Floyd-Walsall Algorithm to create a distance matrix for the nearest distance between nodes.

**Version 2.0:**

* Rotates and solves both clockwise and counterclockwise.
* Prints the original randomized rotations as well as the rotations needed to solve from the randomized state.
* Working on better heuristic using a distance matrix.
