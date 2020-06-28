// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"
#include <typeinfo>

MyAI::MyAI() : Agent()
{
	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================
	
	currentLoc = std::make_pair(0, 0);
	currentDir = RIGHT;
	height = 7;
	width = 7;
	
	dirPath.push_back(currentDir);
	
	initializeGrid();
	
	exitMaze.push(CLIMB);
	
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
}
	
Agent::Action MyAI::getAction
(
	bool stench,
	bool breeze,
	bool glitter,
	bool bump,
	bool scream
)
{
	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================
	if (exit)
		goto end;
	if (trace_back)
		goto trace_back;
		
	if (currentLoc == std::make_pair(0, 0) && (breeze || stench)) {return CLIMB;}  //Special case
	
	
	if (letsRoll)
	{
		updateParameters(stench,breeze,bump);
		
		if (glitter) {
			gotGold = true;
			exit = true;
			exitMaze.push(TURN_RIGHT);
			exitMaze.push(TURN_RIGHT);
			return GRAB;
		}
		
		updateSurroundings();
		
		moveLoc = findMove();

		if (moveLoc == std::make_pair(-1,-1))
		{
			exitMaze.push(TURN_RIGHT);
			exitMaze.push(TURN_RIGHT);
			trace_back = true;
			goto trace_back;
		}

		dir = moveTo(moveLoc);

		makeDir(dir); //list of moves required to make change direction to desired location.

		letsRoll = false;
	}
	

	for (int i = 0; i < turning.size(); ++i) //Would it stop as soon as I erase the element because of the for loop conditions. NO.
	{
		int action = turning[i];
		turning.erase(turning.begin()+i);

		if (action == RIGHT) {
			exitMaze.push(TURN_RIGHT);
			return TURN_RIGHT;
		}
		else if (action == LEFT) {
			exitMaze.push(TURN_LEFT);
			return TURN_LEFT;
		}
	}


	currentLoc = moveLoc;
	currentDir = dir;

	dirPath.push_back(dir);
	coordPath.push_back(currentLoc);

	exitMaze.push(FORWARD);	
	letsRoll = true;
	
	return FORWARD;

	//----------- TRACE BACK BEGINGS HERE -----------------
	trace_back:

	if (stop == 3)
	{
		stop = 0;
		trace_back = false;
		dirPath.pop_back();
		coordPath.pop_back();
		currentDir = dirPath.back();
		currentLoc = coordPath.back();
		return Action::TURN_RIGHT;
	}

	moveBack = exitMaze.top();
	exitMaze.pop();

	if (moveBack == Action::TURN_LEFT)
		return Action::TURN_RIGHT;
	
	if (moveBack == Action::TURN_RIGHT)
		return Action::TURN_LEFT;

	stop++;

	if (stop == 2 && moveBack == FORWARD)
	{
		exitMaze.push(FORWARD);
		stop++;
		return Action::TURN_RIGHT;
	} 

	return moveBack;

	//----------- TRACE BACK ENDS HERE -----------------
	
	end:
	
	moveBack = exitMaze.top();
	exitMaze.pop();
	if (moveBack == Action::TURN_LEFT)
		return Action::TURN_RIGHT;
	
	if (moveBack == Action::TURN_RIGHT)
		return Action::TURN_LEFT;

	return moveBack;
	
	
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
}

// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

void MyAI::initializeGrid() //compiles //Works permanently set to 4x4 rn
{
	for (int col = 0; col < 7; ++col) {
		for(int row = 0; row < 7; ++row) {
			std::pair<int, int> coord;
			coord = std::make_pair(row, col);
			this->grid[coord] = info{};
		}
	}
}

void MyAI::updateParameters(bool stench, bool breeze, bool bump) //compiles
{
	grid[currentLoc].isVisited = true;
	grid[currentLoc].isSafe = true;
	grid[currentLoc].isBreeze = breeze;
	grid[currentLoc].isStench = stench;
	
	if (bump)
	{
		if (currentDir == UP)
			height == currentLoc.first+1;
		if (currentDir == RIGHT)
			width == currentLoc.second+1;
		coordPath.pop_back();
		currentLoc = coordPath.back();
		exitMaze.pop();
	}	
	if (std::find(safeSquares.begin(), safeSquares.end(), currentLoc) == safeSquares.end()) //Add the current position to list of safe squares if not already present.
		safeSquares.push_back(currentLoc);
}

void MyAI::updateSurroundings() //compiles
{
	std::vector<std::pair<int, int> > unsafeSquares = findUnsafeSquares();

	if (grid[currentLoc].isBreeze) // If breeze perceived, increase danger in adjacent-unsafe squares
	{
		for (int i = 0; i < unsafeSquares.size(); ++i) {
			grid[unsafeSquares[i]].pitDanger += (100/unsafeSquares.size());
			grid[unsafeSquares[i]].checkedSides ++;
		}
	}
	
	if (grid[currentLoc].isStench) // If stench perceived, increase danger in adjacent-unsafe squares //What-if Wumpus already known?
		for (int i = 0; i < unsafeSquares.size(); ++i)
			grid[unsafeSquares[i]].wumpusDanger += (100/unsafeSquares.size());
	
	if ((!grid[currentLoc].isBreeze) && (!grid[currentLoc].isStench)) // If nothing perceived, mark all the squares around you safe.
	{
		std::vector<std::pair<int, int> > adjacentSquares = findAdjacentSquares();  // Could've used the function adjacent-unsafe squares too, but ok.
		for (int i = 0; i < adjacentSquares.size(); ++i) {
			if (std::find(safeSquares.begin(), safeSquares.end(), adjacentSquares[i]) == safeSquares.end()) {
				grid[adjacentSquares[i]].isSafe = true;
				safeSquares.push_back(adjacentSquares[i]);
			}
		}
	}
}

std::pair<int, int> MyAI::findMove()
{
	std::vector<std::pair<int, int> > adjacentSquares = findAdjacentSquares();
	std::pair<int, int> toMoveDir;
	
	for (int i = 0; i < adjacentSquares.size(); ++i)
		for (int j = 0; j < safeSquares.size(); ++j)
			if (adjacentSquares[i] == safeSquares[j] && !(grid[adjacentSquares[i]].isVisited))
				return adjacentSquares[i];

	return std::make_pair(-1,-1);
}

int MyAI::moveTo(std::pair<int, int> moveLoc)
{
	if (currentLoc.first == moveLoc.first && currentLoc.second-moveLoc.second == -1) // MOVE RIGHT
		return RIGHT;
	
	else if (currentLoc.first == moveLoc.first && currentLoc.second-moveLoc.second == 1) //MOVE LEFT
		return LEFT;
		
	else if (currentLoc.first-moveLoc.first == -1 && currentLoc.second == moveLoc.second) //MOVE UP
		return UP;
		
	else if (currentLoc.first-moveLoc.first == 1 && currentLoc.second == moveLoc.second) //MOVE DOWN
		return DOWN;
	
	else {
		if (currentDir > 2)
			return currentDir-2;
		else
			return currentDir+2;
	}
}

void MyAI::makeDir(int dir)
{
	turning.clear();
	
	if (currentDir == dir)
		return;
	
	else if (dir == UP && currentDir == LEFT)
		turning.push_back(RIGHT);
	
	else if (dir == LEFT && currentDir == UP)
		turning.push_back(LEFT);
	
	else if (dir > currentDir) {
		turning.push_back(RIGHT);
		if (dir - currentDir == 2)
			turning.push_back(RIGHT);
	}
	
	else if (dir < currentDir) {
		turning.push_back(LEFT);
		if (currentDir - dir == 2)
			turning.push_back(LEFT);
	}
	
}

void MyAI::gobackStep() //FLAWED
{
	exitMaze.pop();
	exitMaze.pop();
	exitMaze.pop();
	exitMaze.pop();
	exitMaze.push(TURN_RIGHT);
	exitMaze.push(TURN_RIGHT);

}

//helper
bool MyAI::isValid(int x, int y) //compiles //Works
{
	if (x < 0 || y < 0 || x > (height-1) || y > (width-1))		
		return false;
	return true;
}

//helper
std::vector<std::pair<int, int> > MyAI::findAdjacentSquares() //compiles
{
	std::vector<std::pair<int, int> > adjacentSquares;
	
	if (isValid(currentLoc.first+1, currentLoc.second)) 
		adjacentSquares.push_back(std::make_pair(currentLoc.first+1, currentLoc.second));
		
	if (isValid(currentLoc.first, currentLoc.second+1)) 
		adjacentSquares.push_back(std::make_pair(currentLoc.first, currentLoc.second+1));
	
	if (isValid(currentLoc.first-1, currentLoc.second)) 
		adjacentSquares.push_back(std::make_pair(currentLoc.first-1, currentLoc.second));
	
	if (isValid(currentLoc.first, currentLoc.second-1)) 
		adjacentSquares.push_back(std::make_pair(currentLoc.first, currentLoc.second-1));
		
	return adjacentSquares;
}

//helper
std::vector<std::pair<int, int> > MyAI::findUnsafeSquares() //compiles
{
	std::vector<std::pair<int, int> > unsafeSquares = findAdjacentSquares();
	for (int i = 0; i < unsafeSquares.size(); ++i)
	{
		if (grid[unsafeSquares[i]].isSafe)
			unsafeSquares.erase(unsafeSquares.begin()+i);
	}
	return unsafeSquares;
}

void MyAI::adjustLocation() // Adjust location for one step in the current direction
{							// CAN CLEAN THIS LATER, dont need temp? Yeah, you don't.
	std::pair<int, int> temp = currentLoc;
	if (currentDir == UP)
		currentLoc = std::make_pair(temp.first+1, temp.second);
	else if (currentDir == RIGHT)
		currentLoc = std::make_pair(temp.first, temp.second+1);
	else if (currentDir == DOWN)
		currentLoc = std::make_pair(temp.first-1, temp.second);
	else if (currentDir == LEFT)
		currentLoc = std::make_pair(temp.first, temp.second-1);
}
// ======================================================================
// YOUR CODE ENDS
// ======================================================================