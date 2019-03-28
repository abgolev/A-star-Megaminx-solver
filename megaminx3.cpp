/*
Author: Atanas Golev
Course: CS463
Instructor: Professor Judy Goldsmith
Assignment: A* on the Megaminx
Purpose: Creates, randomizes, and solves an instance of the Megaminx dodecahedron puzzle.
*/

#include <iostream>
#include <sstream>
#include <stdlib.h>	//rand
#include <queue>	//priority queue 
#include <vector>
#include <ctime>	//crand()
//#include <cmath>	//ceil()

#define MAX_ITERATIONS 5000000
#define ADJUSTED_PQ_SIZE 345000
#define MAX_PQ_SIZE 350000
#define PRINT_EVERY_N_TIMES 50

using namespace std;

//Megaminx stored here
struct cubeStruct{char arr[12][11];};

//Distance matrix between nodes
short adjacencyMatrix[120][120];

//Used for A*; overload operator needed for ability to use these in a priority queue
struct config{
	cubeStruct c;			//Cube configuration
	unsigned short g;		//Distance from original cube
	unsigned short h;		//Heuristic value
	vector<short> p;		//Turn history to find parents

	//Needed to be able to use priority queue
	bool operator<(const config& rhs) const
		{
			return (g+h) > (rhs.g + rhs.h);
		}
};

//New solved cube
cubeStruct getCube(){
	cubeStruct c;
	for(int i=0; i<11; i++){
		c.arr[0][i]='w';
		c.arr[1][i]='p';
		c.arr[2][i]='r';
		c.arr[3][i]='b';
		c.arr[4][i]='y';
		c.arr[5][i]='g';
		c.arr[6][i]='l';
		c.arr[7][i]='B';
		c.arr[8][i]='P';
		c.arr[9][i]='L';
		c.arr[10][i]='o';
		c.arr[11][i]='G';
	}
	return c;
}

/*
cycleFace and cycleNode are a pair parallel arrays symbolizing the possible rotations for each face.
Movements happen in triplets and the minimum node of each triplet is included in the cycleNode list.

There are 12 faces. When a face is rotated, 5 different stickers change locations. 
cycleFace contains the faces of the stickers, and cycleNode contains the node number.

The exact position of each node on the cube is described in design.pdf
*/

short cycleFace[12][5] = {{5,4,3,2,1},{0,2,6,10,5},{0,3,7,6,1},{0,4,8,7,2},{0,5,9,8,3},{0,1,10,9,4},{11,10,1,2,7},{11,6,2,3,8},{11,7,3,4,9},{11,8,4,5,10},{11,9,5,1,6},{6,7,8,9,10}};	
short cycleNode[12][5] = {{4,4,4,4,4},{0,6,0,8,2},{2,6,0,8,2},{4,6,0,8,2},{6,6,0,8,2},{8,6,0,8,2},{8,6,0,8,2},{6,6,0,8,2},{4,6,0,8,2},{2,6,0,8,2},{0,6,0,8,2},{4,4,4,4,4}};

//Hardcoded solved cube
char solvedCube[12][11] = {
	{'w','w','w','w','w','w','w','w','w','w','w'},
	{'p','p','p','p','p','p','p','p','p','p','p'},
	{'r','r','r','r','r','r','r','r','r','r','r'},
	{'b','b','b','b','b','b','b','b','b','b','b'},
	{'y','y','y','y','y','y','y','y','y','y','y'},
	{'g','g','g','g','g','g','g','g','g','g','g'},
	{'l','l','l','l','l','l','l','l','l','l','l'},
	{'B','B','B','B','B','B','B','B','B','B','B'},
	{'P','P','P','P','P','P','P','P','P','P','P'},
	{'L','L','L','L','L','L','L','L','L','L','L'},
	{'o','o','o','o','o','o','o','o','o','o','o'},
	{'G','G','G','G','G','G','G','G','G','G','G'}
};

//Used in printCube
string printBlanks(int numBlanks){
	string s="";
	for(int i=0; i<numBlanks; i++)
		s+=" ";
	return s;
}

//used to compute the Adjacency Matrix based on the given connections between nodes
void makeAdjacencyMatrix(){
	for(int i=0; i<120; i++){
		for(int j=0; j<120; j++){
			if(i==j)
				adjacencyMatrix[i][j]=0;
			else
				adjacencyMatrix[i][j]=8;
		}
	}	

	//adjacencies on same face
	for(int i=0; i<=11; i++){
		for(int j=0; j<=7; j++)
			adjacencyMatrix[i*10+j][i*10+j+2]=1;
		for(int j=2; j<=9; j++)
			adjacencyMatrix[i*10+j][i*10+j-2]=1;
		adjacencyMatrix[i*10+8][i*10]=1;
		adjacencyMatrix[i*10+9][i*10+1]=1;
		adjacencyMatrix[i*10][i*10+8]=1;
		adjacencyMatrix[i*10+1][i*10+9]=1;
	}
	
	//Finding adjacent nodes using the hardcoded cycles
	int thisFace, thisNode, nextFace, nextNode;

	//for each face
	for(int i=0; i<=11; i++){
		int next_j, prev_j, this_j;

		//for each moving node in the cycle
		for(int j=0; j<5; j++){
			next_j=j+1;
			prev_j=j-1;
			if(next_j==5)
				next_j=0;
			if(prev_j==-1)
				prev_j=4;


			thisFace=cycleFace[i][j];
			thisNode=cycleNode[i][j];

			//for the node going forward in the cycle and the one going back in the cycle
			for(int k=0; k<2; k++){
				if(k)
					this_j=next_j;
				else
					this_j=prev_j;

				nextFace=cycleFace[i][this_j];
				nextNode=cycleNode[i][this_j];
		
				//for each of the two other nodes on the moving triplet
				for(int n=1; n<=2; n++){
					adjacencyMatrix[thisFace*10+thisNode][nextFace*10+nextNode]=1;
					thisNode = thisNode+1;
					nextNode = nextNode+1;
					if(n==2 && thisNode==10)
						thisNode=0;
					if(n==2 && nextNode==10)
						nextNode=0;					
				}
			}
		}
	}	
	
	//and now Floyd Walsall
	for(int k=0; k<120; k++){
		for(int i=0; i<120; i++){
			for(int j=0; j<120; j++){
				if(adjacencyMatrix[i][j] > adjacencyMatrix[i][k]+adjacencyMatrix[k][j])
					adjacencyMatrix[i][j] = adjacencyMatrix[i][k]+adjacencyMatrix[k][j];				
			}
		}
	}

	//Printing the distance matrix
	/*
	cout<<"	";
	for(int i=0; i<120; i++)
		cout<<i<<"	";
	cout<<endl;

	for(int i=0; i<120; i++){
		cout<<i<<"	";
		for(int j=0; j<120; j++){
			cout<<adjacencyMatrix[i][j]<<"	";
		}
		cout<<endl;
	}

	for(int i=0; i<120; i++){
		int count=0;
		for(int j=0; j<120; j++){
			if(adjacencyMatrix[i][j]==1)
				count=count+1;
		}
		cout<<"Adjacencies for ["<<i/10<<"]["<<i%10<<"]: "<<count<<endl;
	}
	*/
}

//Prints a Megaminx
void printCube(cubeStruct cube){
	//Top Face
	cout<<printBlanks(18)<<cube.arr[0][0]<<"\n";
	cout<<printBlanks(15)<<cube.arr[0][2]<<" "<<cube.arr[0][1]<<" "<<cube.arr[0][9]<<" "<<cube.arr[0][8]<<"   \n";
	cout<<printBlanks(16)<<cube.arr[0][3]<<" "<<cube.arr[0][10]<<" "<<cube.arr[0][7]<<" "<<"\n";
	cout<<printBlanks(16)<<cube.arr[0][4]<<" "<<cube.arr[0][5]<<" "<<cube.arr[0][6]<<" "<<"\n";
	cout<<"\n";	

	//Upper 5 Faces
	for(int i=1; i<=5; i++){cout<<" "<<cube.arr[i][6]<<" "<<cube.arr[i][5]<<" "<<cube.arr[i][4]<<"    ";}
	cout<<"\n";
	for(int i=1; i<=5; i++){cout<<" "<<cube.arr[i][7]<<" "<<cube.arr[i][10]<<" "<<cube.arr[i][3]<<"    ";}
	cout<<"\n";
	for(int i=1; i<=5; i++){cout<<cube.arr[i][8]<<" "<<cube.arr[i][9]<<" "<<cube.arr[i][1]<<" "<<cube.arr[i][2]<<"   ";}
	cout<<"\n";
	for(int i=1; i<=5; i++){cout<<"   "<<cube.arr[i][0]<<printBlanks(6);}
	cout<<"\n\n";

	//Lower 5 Faces
	cout<<printBlanks(5);
	for(int i=6; i<=10; i++){cout<<printBlanks(3)<<cube.arr[i][0]<<printBlanks(6);}
	cout<<"\n";
	cout<<printBlanks(5);
	for(int i=6; i<=10; i++){cout<<cube.arr[i][2]<<" "<<cube.arr[i][1]<<" "<<cube.arr[i][9]<<" "<<cube.arr[i][8]<<"   ";}
	cout<<"\n";
	cout<<printBlanks(5);
	for(int i=6; i<=10; i++){cout<<" "<<cube.arr[i][3]<<" "<<cube.arr[i][10]<<" "<<cube.arr[i][7]<<"    ";}
	cout<<"\n";
	cout<<printBlanks(5);
	for(int i=6; i<=10; i++){cout<<" "<<cube.arr[i][4]<<" "<<cube.arr[i][5]<<" "<<cube.arr[i][6]<<"    ";}
	cout<<"\n\n";

	//Bottom Face
	cout<<printBlanks(21)<<cube.arr[11][6]<<" "<<cube.arr[11][5]<<" "<<cube.arr[11][4]<<" "<<"\n";
	cout<<printBlanks(21)<<cube.arr[11][7]<<" "<<cube.arr[11][10]<<" "<<cube.arr[11][3]<<" "<<"\n";
	cout<<printBlanks(20)<<cube.arr[11][8]<<" "<<cube.arr[11][9]<<" "<<cube.arr[11][1]<<" "<<cube.arr[11][2]<<"   \n";
	cout<<printBlanks(23)<<cube.arr[11][0]<<"\n";
	cout<<"\n";	
}

//Input is cube and #0-11, representing which face to rotate
cubeStruct rotateCounterClock(cubeStruct cube, int cycle){

	//Shift the stickers of adjacent faces
	for(int j=0; j<3; j++){
		int firstFace = cycleFace[cycle][0];
		int firstNode = cycleNode[cycle][0]+j;
		if(firstNode==10)
			firstNode=0;
		char tempNode = cube.arr[firstFace][firstNode];

		//Shifts each of the five faces in the appropriate cycle		
		for(int i=0; i<=3; i++){
			int faceNum = cycleFace[cycle][i+1];
			int nodeNum = cycleNode[cycle][i+1]+j;
			if(nodeNum==10)
				nodeNum=0;
			char prevNode = cube.arr[faceNum][nodeNum];
			int nextFace = cycleFace[cycle][i];
			int nextNode = cycleNode[cycle][i]+j;
			if(nextNode==10)
				nextNode=0;
			cube.arr[nextFace][nextNode] = prevNode;
		}	
	
		int lastFace = cycleFace[cycle][4];
		int lastNode = cycleNode[cycle][4]+j;
		if(lastNode==10)
			lastNode=0;	
		cube.arr[lastFace][lastNode]=tempNode;	
	}

	//Shift the stickers of this face itself
	char prevEight = cube.arr[cycle][8];
	char prevNine = cube.arr[cycle][9];
	for(int i=9; i>=2; i=i-1){cube.arr[cycle][i] = cube.arr[cycle][i-2];}
	cube.arr[cycle][0]=prevEight;
	cube.arr[cycle][1]=prevNine;

	return cube;
}

//Same logic as rotateCounterClock
cubeStruct rotateClock(cubeStruct cube, int cycle){

	//Shift the stickers of adjacent faces
	for(int j=0; j<3; j++){
		int lastFace = cycleFace[cycle][4];
		int lastNode = cycleNode[cycle][4]+j;
		if(lastNode==10)
			lastNode=0;
		char tempNode = cube.arr[lastFace][lastNode];

		for(int i=3; i>=0; i=i-1){
			int faceNum = cycleFace[cycle][i];
			int nodeNum = cycleNode[cycle][i]+j;
			if(nodeNum==10)
				nodeNum=0;
			char prevNode = cube.arr[faceNum][nodeNum];
			int nextFace = cycleFace[cycle][i+1];
			int nextNode = cycleNode[cycle][i+1]+j;
			if(nextNode==10)
				nextNode=0;
			cube.arr[nextFace][nextNode] = prevNode;
		}	
	
		int firstFace = cycleFace[cycle][0];
		int firstNode = cycleNode[cycle][0]+j;
		if(firstNode==10)
			firstNode=0;	
		cube.arr[firstFace][firstNode]=tempNode;	
	}

	//Shift the stickers of this face
	char prevZero = cube.arr[cycle][0];
	char prevOne = cube.arr[cycle][1];

	for(int i=0; i<=7; i++){
		cube.arr[cycle][i] = cube.arr[cycle][i+2];
	}

	cube.arr[cycle][8]=prevZero;
	cube.arr[cycle][9]=prevOne;

	return cube;
}

//Helper function matching color character to face #, as shown in design.pdf
short charToFaceInt(char c){
	int j;
	if(c=='w')
		j=0;
	else if(c=='p')
		j=1;
	else if(c=='r')
		j=2;
	else if(c=='b')
		j=3;
 	else if(c=='y')
		j=4;
	else if(c=='g')
		j=5;
	else if(c=='l')
		j=6;
	else if(c=='B')
		j=7;
	else if(c=='P')
		j=8;
	else if(c=='L')
		j=9;
	else if(c=='o')
		j=10;
	else if(c=='G')
		j=11;
	return j;		
}

//Inputs are the numbers of the faces, as described in the design.pdf
//Output is distance between the two faces, according to the heuristic. Returns either 1, 2, or 3 (3 is max distance)
unsigned short dist(int x, int y){
	int smaller, larger;

	//If the two faces are adjacent
	for(int i=0; i<5; i++)
		if(cycleFace[x][i]==y)
			return 1;
	
	//Which face is smaller
	if(x<y){
		smaller=x;
		larger=y;	
	}
	else{
		smaller=y;
		larger=x;
	}

	//Minimum distance from clockwise turns from direct opposite sides of the cube is 3; minimum of 2 otherwise
	if((smaller==0 && larger==11) || (smaller==1 && larger==8) || (smaller==2 && larger==9) || (smaller==3 && larger==10) || (smaller==4 && larger==6) || (smaller==5 && larger==7))
		return 3;
	else	
		return 2;	
}

//more accurate heuristic based on adjacency matrix
//exact face is unknown but picks the minimum distance to an edge or middle face of the same color
short d, min_d;
short distFromMatrix(int thisNode, int faceNum){
	min_d=8;
	int isOdd = thisNode%2; 
	for(int i=0; i<10; i=i+2){
		d = adjacencyMatrix[thisNode][faceNum*10+i+isOdd];
		if(d==0)
			return 0;
		else{
		 	if(min_d>d)
				min_d=d;
		}
	}
	if(min_d==8)
		cout<<"Something bad happened!"<<endl;
	return min_d;
}

//Outputs Manhattan distance between two faces
short distance(cubeStruct cube){
	short d=0;
	for(int i=0; i<12; i++)
		for(int j=0; j<10; j++)
			if(solvedCube[i][0]!=cube.arr[i][j])
				//d+=dist(i, charToFaceInt(cube.arr[i][j]));
				d+=distFromMatrix(i*10+j, charToFaceInt(cube.arr[i][j]));
	return d;	
}

//Returns 1 if solved, 0 if not
int isSolved(cubeStruct cube){
	for(int i=0; i<12; i++)
		for(int j=0; j<11; j++)
			if(cube.arr[i][j]!=solvedCube[i][j])
				return 0;
	return 1;
}

int isSameAs(cubeStruct cube, cubeStruct cube2){
	for(int i=0; i<12; i++)
		for(int j=0; j<11; j++)
			if(cube.arr[i][j]!=cube2.arr[i][j])
				return 0;
	return 1;
}

//Finds all children of a parent cube by looking for all possible clockwise rotations
vector<cubeStruct> findChildren(cubeStruct cube){
	vector<cubeStruct> cubeList;
	for(int i=0; i<12; i++)
		cubeList.push_back(rotateClock(cube, i));
	for(int i=0; i<12; i++)
		cubeList.push_back(rotateCounterClock(cube, i));
	return cubeList;
}

//Runs A* algorithm and prints out the # of nodes expanded and the solution found if any
//Returns 1 if solution found, 0 otherwise
int AStar(cubeStruct cube){
	priority_queue<config> pq;
	queue<config> temp_q;
	short heuristic;
		
	int pqSizeCounter=0;
	int iterationCounter=0;
	int clearCounter=PRINT_EVERY_N_TIMES-1;

	short d = distance(cube);
	vector<short> parents;
	config testConfig = {.c = cube, .g=0, .h=d, .p=parents}; 
	pq.push(testConfig);

	//While the cube has not been solved
	while(pq.top().h!=0){	
		testConfig = pq.top();
		pq.pop();

		if(testConfig.h==0)
			break;

		vector<cubeStruct> children = findChildren(testConfig.c);	//Find children of the cube
		
		//Pushes all children onto the priority queue
		for(short i=0; i<24; i++){
			//heuristic = ceil(distance(children[i])/15);
			heuristic = distance(children[i]);
			config newConfig = {.c = children[i], .h = heuristic, .g=(testConfig.g+15), .p=testConfig.p};
			newConfig.p.push_back(i);
			pq.push(newConfig);
		}

		//Prevent overflow
		if(iterationCounter>MAX_ITERATIONS){
			cout<<"Maximum number of iterations reached sorry. No solution found."<<endl;
			return 0;
		}

		//Prevent overflow via clearing up of priority queue from MAX_PQ_SIZE to QUEUE_ITEMS		
		if(pqSizeCounter>MAX_PQ_SIZE){
			clearCounter=clearCounter+1;
			if(clearCounter==PRINT_EVERY_N_TIMES)
				cout<<"Clearing up space in priority queue. Number of iterations so far: "<<iterationCounter-1<<endl;
			

			if(clearCounter==PRINT_EVERY_N_TIMES){
				cout<<"Top H value: "<<pq.top().h<<endl;
				cout<<"Top G value: "<<pq.top().g<<endl<<endl;
			}

			for(int i=0; i<ADJUSTED_PQ_SIZE; i++){
				temp_q.push(pq.top());
				//temp[i]=pq.top();
				//temp_pq.push(pq.top());
				pq.pop();
			}
			
			if(clearCounter==PRINT_EVERY_N_TIMES){
				cout<<"Removed H value: "<<pq.top().h<<endl;
				cout<<"Removed G value: "<<pq.top().g<<endl<<endl;
				clearCounter=0;
			}

			while(!pq.empty())
				pq.pop();

			for(int i=0; i<ADJUSTED_PQ_SIZE; i++){
			//while(!temp_pq.empty()){
				pq.push(temp_q.front());
				temp_q.pop();
			}

			pqSizeCounter=ADJUSTED_PQ_SIZE-1;
			iterationCounter=iterationCounter-2;
		}

		pqSizeCounter = pqSizeCounter+1;
		iterationCounter = iterationCounter+1;

		//Print solution
		if(pq.top().h==0){
			cout<<"Solution found! Here is the solution sequence:"<<endl;
			config thisCube = pq.top();
			int rotationNum;
			for(int i=0; i<thisCube.p.size(); i++){
				rotationNum = thisCube.p[i];
				if(rotationNum<12)
					cout<<"Clockwise: Face #";
				else
					cout<<"Counterclockwise: Face #";
				cout<<rotationNum%12<<endl;		
			}
		}
	}

	cout<<"The number of nodes expanded to solve the puzzle was: "<<iterationCounter<<endl;
	return 1;
}

int main(){
	srand(time(NULL));
	makeAdjacencyMatrix();

	int n;
	cout<<"Please enter the number of random moves you want to make or enter a negative number to quit: ";
	cin>>n;

	//Randomization Here
	while(n>=0 && !cin.fail()){
		cubeStruct c = getCube();
		int r;
		cout<<endl<<"Here is the randomization sequence:"<<endl;
		for(int i=0; i<n; i++){
			r = rand()%12;
			if(rand()%2){
				c = rotateCounterClock(c,r);
				cout<<"Counterclockwise: Face #"<<r<<endl;
			}
			else{
				c = rotateClock(c,r);	
				cout<<"Clockwise: Face #"<<r<<endl;
			}		
			//cout<<"Manhattan distance after turn "<<i+1<<": "<<distance(c)<<endl;
		}
		cout<<"Original Manhattan distance: "<<distance(c)<<endl<<endl;
		cout<<"Here is the randomized cube: "<<endl;
		printCube(c);
		cout<<endl<<"Searching for solution..."<<endl;
		AStar(c);
		cout<<endl;		 
		//for(int i=0; i<n; i++)
		//	printCube(path[i]);
		cout<<"Please enter the number of turns you'd like to randomize the Megaminx: ";
		cin>>n;
	}
	return 0;
}
