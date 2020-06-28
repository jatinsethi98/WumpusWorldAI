// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Abdullah Younis
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MYAI_LOCK
#define MYAI_LOCK

#include "Agent.hpp"
#include <map>
#include <vector> 
#include <string>
#include <stack>
#include <algorithm>

class MyAI : public Agent
{
public:
	MyAI ( void );
	
	Action getAction
	(
		bool stench,
		bool breeze,
		bool glitter,
		bool bump,
		bool scream
	);
	
	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================
	
	void initializeGrid(); 
	void updateParameters (bool stench, bool breeze, bool bump);
	
	void updateSurroundings();

	std::pair<int, int> findMove();
	int moveTo(std::pair<int, int> moveLoc);
	void makeDir(int dir);
	void gobackStep();

	bool isValid(int x, int y);
	std::vector<std::pair<int, int> > findAdjacentSquares();
	std::vector<std::pair<int, int> > findUnsafeSquares();

	
	void adjustLocation();
	
	
private: 
	
	struct info {
		bool isVisited = false;
		bool isSafe = false;
		
		bool isBreeze = false;
		bool isStench = false;
		
		int wumpusDanger = 0;
		int pitDanger = 0;
		int checkedSides = 0;
	};
	
	std::map<std::pair<int, int>,info> grid; // MAIN Board
	std::vector<std::pair<int,int> > safeSquares; //All the squares which are safe
	std::vector<int> turning;// Temp for storing the turns i need to take in order to point the right direction
	std::vector<std::pair<int,int> > coordPath;		// My path through the board
	std::vector<int> dirPath;						//Directions through the board
	
	std::pair<int, int> moveLoc;
	int dir;
	
	std::pair<int, int> currentLoc; // Current location
	int currentDir;					// Current Direction
	int height;						// Height of board // Height is x-coordinate 
	int width;						// Width of board  // Width is y-coordinate
	
	int UP = 0;
	int RIGHT = 1;
	int DOWN = 2;
	int LEFT = 3;
	
	bool letsRoll = true;
	bool canShoot = true;
	bool justShot = false; 
	bool gotGold = false; 
	
	int stop = 0;
	Action moveBack;

	bool trace_back = false; //When tracing back on the map;
	bool exit = false;  //True when the player is ready to exit
	std::stack<Action> exitMaze; 
	 
	
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
};

#endif