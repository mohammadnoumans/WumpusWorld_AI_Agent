// Agent.cc

#include <iostream>
#include "Agent.h"
#include "WorldState.h"
#include "WumpusWorld.h"

#define PIT_PROBABILITY 0.2

using namespace std;

Agent::Agent ()
{

}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
	int size = 4; //Information given in the question

	currentState.worldSize = size;
	// Choose Wumpus location [4,4] as given in the problem
	currentState.wumpusLocation = Location(size, size);

	// Choose gold location (anywhere except [1,1])
	currentState.goldLocation = Location(1,4);
	currentState.agentLocation = Location(1,1);
	currentState.agentOrientation = RIGHT;
	currentState.agentAlive = true;
	currentState.agentHasArrow = true;
	currentState.agentHasGold = false;
	currentState.agentInCave = true;
	currentState.wumpusAlive = true;
}

Action Agent::Process (Percept& percept)
{
	char c;
	Action action;
	bool validAction = false;

	if(percept.Glitter == true)
	{
		action = GRAB;
		processAction(action,percept);
		return action;
	}
	else if((currentState.agentLocation.X == 1)  && (currentState.agentLocation.Y == 1) && (currentState.agentHasGold == true))
	{
		action = CLIMB;
		processAction(action,percept);
		return action;
	}
	else if((currentState.agentHasArrow == true) && (currentState.agentLocation.Y == 4) && (currentState.agentOrientation == RIGHT))
	{
		action = SHOOT;
		processAction(action,percept);
		return action;
	}
	else if((currentState.agentHasArrow == true) && (currentState.agentLocation.X == 4) && (currentState.agentOrientation == UP))
	{
		action = SHOOT;
		processAction(action,percept);
		return action;
	}

	while (! validAction)
	{
		validAction = true;
		cout << "Action? ";
		cin >> c;
		if (c == 'f') {
			action = GOFORWARD;
		} else if (c == 'l') {
			action = TURNLEFT;
		} else if (c == 'r') {
			action = TURNRIGHT;
		} else if (c == 'g') {
			action = GRAB;
		} else if (c == 's') {
			action = SHOOT;
		} else if (c == 'c') {
			action = CLIMB;
		} else {
			cout << "Huh?" << endl;
			validAction = false;
		}
	}
	processAction(action,percept);
	return action;		
}

void Agent::processAction(Action action, Percept& currentPercept)
{
	//WumpusWorld EXECUTE ACTION method

	currentPercept.Bump = false;
	currentPercept.Scream = false;

	if (action == GOFORWARD)
	{
		if (currentState.agentOrientation == RIGHT)
		{
			if (currentState.agentLocation.X < currentState.worldSize)
			{
				currentState.agentLocation.X++;
			} else {
				currentPercept.Bump = true;
			}
		} else if (currentState.agentOrientation == UP)
		{
			if (currentState.agentLocation.Y < currentState.worldSize)
			{
				currentState.agentLocation.Y++;
			} else {
				currentPercept.Bump = true;
			}
		} else if (currentState.agentOrientation == LEFT)
		{
			if (currentState.agentLocation.X > 1)
			{
				currentState.agentLocation.X--;
			} else {
				currentPercept.Bump = true;
			}
		} else if (currentState.agentOrientation == DOWN)
		{
			if (currentState.agentLocation.Y > 1)
			{
				currentState.agentLocation.Y--;
			} else {
				currentPercept.Bump = true;
			}
		}
		// Update glitter percept
		currentPercept.Glitter = false;
		if ((! currentState.agentHasGold) && (currentState.agentLocation == currentState.goldLocation))
		{
			currentPercept.Glitter = true;
		}
		// Update stench percept (if agent co-located with or adjacent to alive or dead wumpus)
		currentPercept.Stench = false;
		if (Adjacent (currentState.agentLocation, currentState.wumpusLocation) ||
			(currentState.agentLocation == currentState.wumpusLocation))
		{
			currentPercept.Stench = true;
		}
		// Update breeze percept and check for death by pit
		currentPercept.Breeze = false;
		vector<Location>::iterator itr;
		for (itr = currentState.pitLocations.begin(); itr != currentState.pitLocations.end(); itr++)
		{
			if (Adjacent (currentState.agentLocation, (*itr)))
			{
				currentPercept.Breeze = true;
			}
			if (currentState.agentLocation == (*itr))
			{
				currentState.agentAlive = false;
			}
		}
		// Check for death by wumpus
		if (currentState.wumpusAlive && (currentState.agentLocation == currentState.wumpusLocation))
		{
			currentState.agentAlive = false;
		}
	}

	if (action == TURNLEFT)
	{
		if (currentState.agentOrientation == RIGHT)
		{
			currentState.agentOrientation = UP;
		} else if (currentState.agentOrientation == UP)
		{
			currentState.agentOrientation = LEFT;
		} else if (currentState.agentOrientation == LEFT)
		{
			currentState.agentOrientation = DOWN;
		} else if (currentState.agentOrientation == DOWN)
		{
			currentState.agentOrientation = RIGHT;
		}
	}

	if (action == TURNRIGHT)
	{
		if (currentState.agentOrientation == RIGHT)
		{
			currentState.agentOrientation = DOWN;
		} else if (currentState.agentOrientation == UP)
		{
			currentState.agentOrientation = RIGHT;
		} else if (currentState.agentOrientation == LEFT)
		{
			currentState.agentOrientation = UP;
		} else if (currentState.agentOrientation == DOWN)
		{
			currentState.agentOrientation = LEFT;
		}
	}

	if (action == GRAB)
	{
		if ((! currentState.agentHasGold) && (currentState.agentLocation == currentState.goldLocation))
		{
			currentState.agentHasGold = true;
			currentPercept.Glitter = false;
		}
	}
	if (action == SHOOT)
	{
		if (currentState.agentHasArrow)
		{
			currentState.agentHasArrow = false;
			if (currentState.wumpusAlive)
			{
				if (((currentState.agentOrientation == RIGHT) &&
					 (currentState.agentLocation.X < currentState.wumpusLocation.X) &&
					 (currentState.agentLocation.Y == currentState.wumpusLocation.Y)) ||
					((currentState.agentOrientation == UP) &&
					 (currentState.agentLocation.X == currentState.wumpusLocation.X) &&
					 (currentState.agentLocation.Y < currentState.wumpusLocation.Y)) ||
					((currentState.agentOrientation == LEFT) &&
					 (currentState.agentLocation.X > currentState.wumpusLocation.X) &&
					 (currentState.agentLocation.Y == currentState.wumpusLocation.Y)) ||
					((currentState.agentOrientation == DOWN) &&
					 (currentState.agentLocation.X == currentState.wumpusLocation.X) &&
					 (currentState.agentLocation.Y > currentState.wumpusLocation.Y)))
				{
					currentState.wumpusAlive = false;
					currentPercept.Scream = true;
				}
			}
		}
	}

	if (action == CLIMB)
	{
		if ((currentState.agentLocation.X == 1) && (currentState.agentLocation.Y == 1))
		{
			currentState.agentInCave = false;
			currentPercept.Stench = false;
			currentPercept.Breeze = false;
			currentPercept.Glitter = false;
		}
	}
}

void Agent::GameOver (int score)
{

}

