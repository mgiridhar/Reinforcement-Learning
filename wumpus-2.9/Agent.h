// Agent.h
//

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"

#include "Location.h"
#include "Orientation.h"
#include <list>
#include <vector>
#include <map>

#include "WorldState.h"

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);

	void UpdateState (Percept& percept);
	//bool FacingDeath();
	WorldState currentState;
	Action lastAction;
	Percept lastPercept;
	list<Action> actionList;
	int numActions;

	double prevReward, reward;
	string prevState, state;
	map<string, vector<double> > q;
	map<string, vector<int> > n;
	double discountFactor;
	double learningRate;
	int Ne;

	string getState(WorldState s);
	pair<Action, double> getMaxQ(vector<double> q);
	double Exploration(double q, int n);
	Action argMaxAction(string state);
};

#endif // AGENT_H
