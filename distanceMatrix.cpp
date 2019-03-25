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

#define MAX_ITERATIONS 2000000
#define QUEUE_ITEMS 75000
#define MAX_PQ_SIZE 100000

using namespace std;

//Megaminx stored here
struct cubeStruct{char arr[12][11];};
int adjacencyMatrix[119][119]={8};


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

int main(){
	int n;
//	cin>>n;
	for(int i=0; i<=119; i++)
		adjacencyMatrix[i][i]=0;

	
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

	int thisFace, thisNode, nextFace, nextNode;
	for(int i=0; i<=11; i++){
		for(int j=0; j<=3; j++){
			thisFace=cycleFace[i][j];
			thisNode=cycleNode[i][j];
			nextFace=cycleFace[i][j+1];
			nextNode=cycleNode[i][j+1];
			adjacencyMatrix[thisFace*10+thisNode][nextFace*10+nextNode]=1;
		}
		for(int j=1; j<=4; j++){
			thisFace=cycleFace[i][j];
			thisNode=cycleNode[i][j];
			nextFace=cycleFace[i][j-1];
			nextNode=cycleNode[i][j-1];
			adjacencyMatrix[thisFace*10+thisNode][nextFace*10+nextNode]=1;
		}

		thisFace=cycleFace[i][4];
		thisNode=cycleNode[i][4];
		nextFace=cycleFace[i][0];
		nextNode=cycleNode[i][0];
		adjacencyMatrix[thisFace*10+thisNode][nextFace*10+nextNode]=1;

		thisFace=cycleFace[i][0];
		thisNode=cycleNode[i][0];
		nextFace=cycleFace[i][4];
		nextNode=cycleNode[i][4];
		adjacencyMatrix[thisFace*10+thisNode][nextFace*10+nextNode]=1;
	}	
	//and now Floyd Walsall

	for(int i=0; i<=119; i++){
		for(int j=0; j<=119; j++){
			cout<<" "<<adjacencyMatrix[i][j];
		}
		cout<<endl;
	}

	return 0;
}
