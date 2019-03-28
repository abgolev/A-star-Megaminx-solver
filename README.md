# A-star-Megaminx-solver
Create and randomize a Megaminx puzzle, and then use the A* search algorithm to solve it.

**To compile:**

g++ -O3 megaminx3.cpp <br/>

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

Input: the number of turns that you want to randomize a solved Megaminx.

Output: the randomized Megaminx and the random rotations that were done mix the Megaminx, followed by the rotations to solve, if found.

Notes: Works well on randomizations of 14 turns or fewer. But runs into issues whenever randomized more than 14 turns because priority queue blows up if solution is not found within the first 350,000 iterations. However, still finds solutions for some instances n>14, but not all. Sometimes it does so quickly, sometimes slowly, sometimes not at all. 

**Version 2.0:**

* Rotates and solves both clockwise and counterclockwise.
* Prints the original randomized rotations as well as the rotations needed to solve from the randomized state.
* Limited to 100,000 items in the priority queue; this could be changed by editing global vars.

**Version 3.0:**
* More accurate heuristic: Floyd-Walsall algorithm used to create a distance matrix between nodes.
* Priority queue limited to 350,000 items, with the 5,000 back items deleted each time the limit is hit.
* After hitting the 350,000 iteration limit, prints the top item removed every 50,000 iterations, for sanity checks.
* Currently set to a max of 5 million iterations; this can be bumped up by editing global vars.

