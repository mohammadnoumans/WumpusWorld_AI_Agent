// Agent.h

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"
#include "WorldState.h"

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
	void processAction(Action,Percept&);

	WorldState currentState;
	WumpusWorld wumpusWorld(int);
};

#endif // AGENT_H
