# A-star-Megaminx-solver
Create and randomize a Megaminx puzzle, and then use the A* search algorithm to solve it.


**To run:**

g++ -O3 megaminx2.cpp

./a.out

**About the design:**

![Megaminx](https://i.ibb.co/hX72ZNC/megaminx2.jpg)

Figure 1: The Megaminx flattened out. Each pentagon represents a face. Connecting lines represent adjacent faces.

<br/><br/>

![Megaminx2](https://i.ibb.co/NFyBSfz/megaminx3.jpg)

Figure 2: Each blue digit represents a node. Each face has 10 nodes, not counting the stationary central node.

<br/><br/>

![Megaminx3](https://i.ibb.co/rydjL5L/megaminx4.jpg)

Figure 3: A representation of the nodes of adjacent faces that move when Face 3 is turned.
