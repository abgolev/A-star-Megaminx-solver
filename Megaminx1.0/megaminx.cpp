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
#include <ctime>	//used for crand()
#include <cmath>	//ceil()

using namespace std;

//Megaminx stored here
struct cubeStruct{char arr[12][11];};

//Used for A*
struct config{
	cubeStruct c;	//Cube configuration
	int g;		//Distance from original cube
	int h;		//Heuristic val

	//To enable priority queue
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
Rotations each possible face. 

There are 12 faces. When a face is rotated, 5 different stickers change locations. 
For each face, hardcoded the five stickers that change when it turns. These are stored in two parallel 2D arrays. 
CycleFace contains the faces of the stickers, and CycleNode contains the location on that face of the stickers.

The exact position of each node on the cube is described in design.pdf
*/
int cycleFace[12][5] = {{5,4,3,2,1},{0,2,6,10,5},{0,3,7,6,1},{0,4,8,7,2},{0,5,9,8,3},{0,1,10,9,4},{11,10,1,2,7},{11,6,2,3,8},{11,7,3,4,9},{11,8,4,5,10},{11,9,5,1,6},{6,7,8,9,10}};	
int cycleNode[12][5] = {{4,4,4,4,4},{0,6,0,8,2},{2,6,0,8,2},{4,6,0,8,1},{6,6,0,8,2},{8,6,0,8,2},{8,6,0,8,2},{6,6,0,8,2},{4,6,0,8,2},{2,6,0,8,2},{0,6,0,8,2},{4,4,4,4,4}};

//Hardcoded solved cube for quicker runtime
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
	{'G','G','G','G','G','G','G','G','G','G','G'}};

//Used in printCube
string printBlanks(int numBlanks){
	string s="";
	for(int i=0; i<numBlanks; i++)
		s+=" ";
	return s;
}

//Prints a Megaminx
void printCube(cubeStruct cube){
	//Top Face
	cout<<printBlanks(23)<<cube.arr[0][0]<<"\n";
	cout<<printBlanks(20)<<cube.arr[0][2]<<" "<<cube.arr[0][1]<<" "<<cube.arr[0][9]<<" "<<cube.arr[0][8]<<"   \n";
	cout<<printBlanks(21)<<cube.arr[0][3]<<" "<<cube.arr[0][10]<<" "<<cube.arr[0][7]<<" "<<"\n";
	cout<<printBlanks(21)<<cube.arr[0][4]<<" "<<cube.arr[0][5]<<" "<<cube.arr[0][6]<<" "<<"\n";
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
	cout<<printBlanks(26)<<cube.arr[11][6]<<" "<<cube.arr[11][5]<<" "<<cube.arr[11][4]<<" "<<"\n";
	cout<<printBlanks(26)<<cube.arr[11][7]<<" "<<cube.arr[11][10]<<" "<<cube.arr[11][3]<<" "<<"\n";
	cout<<printBlanks(25)<<cube.arr[11][8]<<" "<<cube.arr[11][9]<<" "<<cube.arr[11][1]<<" "<<cube.arr[11][2]<<"   \n";
	cout<<printBlanks(28)<<cube.arr[11][0]<<"\n";
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
int charToFaceInt(char c){
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
//Output is distance between the two faces, according to the heuristic. Returns either 1, 2, or 7. Not the most accurate it could be, but gets the job done
int dist(int x, int y){
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

	//Minimum distance from clockwise turns from direct opposite sides of the cube is 7; minimum of 2 otherwise. Lazy... could be made more efficient with hardcoding dist between any 2 faces
	if((smaller==0 && larger==11) || (smaller==1 && larger==8) || (smaller==2 && larger==9) || (smaller==3 && larger==10) || (smaller==4 && larger==6) || (smaller==5 && larger==7))
		return 7;
	else	
		return 2;	

}

//Outputs Manhattan distance between two faces
int distance(cubeStruct cube){
	int d=0;
	for(int i=0; i<12; i++)
		for(int j=0; j<11; j++)
			if(solvedCube[i][0]!=cube.arr[i][j])
				d+=dist(i, charToFaceInt(cube.arr[i][j]));
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

/*
int isSameAs(cubeStruct cube, cubeStruct cube2){
	for(int i=0; i<12; i++)
		for(int j=0; j<11; j++)
			if(cube.arr[i][j]!=cube2.arr[i][j])
				return 0;
	return 1;
}
*/

//Finds all children of a parent cube by looking for all possible clockwise rotations
vector<cubeStruct> findChildren(cubeStruct cube){
	vector<cubeStruct> cubeList;
	for(int i=0; i<12; i++)
		cubeList.push_back(rotateClock(cube, i));
	return cubeList;
}

//Runs A* algorithm and prints out the # of nodes expanded
void AStar(cubeStruct cube){
	int d = distance(cube);
	priority_queue<config> pq;
	int heuristic;		
	int solveCounter=0;

	config testConfig = {.c = cube, .g=0, .h=d}; 
	pq.push(testConfig);

	//While the cube has not been solved
	while(pq.top().h!=0){
		solveCounter = solveCounter+1;
		testConfig = pq.top();
		pq.pop();

		if(testConfig.h==0)
			break;

		vector<cubeStruct> children = findChildren(testConfig.c);	//Finds children of the cube
		
		//Pushes all children onto the priority queue
		while(!children.empty()){
			heuristic = ceil(distance(children.back())/15);
			config newConfig = {.c = children.back(), .h = heuristic, .g=(testConfig.g+1)};
			pq.push(newConfig); 
			children.pop_back();
		}

		//In order to prevent my laptop from exploding
		if(solveCounter>2000000){
			cout<<"Maximum number of expanded nodes (2 million) reached sorry."<<endl;
			break;
		}
	}

	cout<<"The number of nodes expanded to solve the puzzle was: "<<solveCounter<<endl;
	return;
}

int main(){
	srand(time(NULL));

	//Randomization Here
	int n;
	cout<<"Please enter the number of random moves you want to make or enter a negative number to quit: ";
	cin>>n;
	while(n>=0){
		cubeStruct c = getCube();
		cout<<endl;
		for(int i=0; i<n; i++){
			c = rotateCounterClock(c,rand()%12);
			cout<<"Manhattan distance after turn "<<i+1<<": "<<distance(c)<<'\n';
		}
		cout<<"Here is the randomized cube: "<<endl;
		printCube(c);
		cout<<endl;
		AStar(c);
		cout<<endl;		 
		cout<<"Please enter the number of random moves you want to make or enter a negative number to quit: ";
		cin>>n;
	}

	return 0;
}
