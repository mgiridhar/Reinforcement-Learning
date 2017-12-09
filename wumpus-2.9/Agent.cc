// Agent.cc
//

#include <iostream>
#include <list>
#include <vector>
#include "Agent.h"

using namespace std;

Agent::Agent ()
{
	currentState.worldSize = 5; // 4x4
	currentState.wumpusLocation = Location(0,0); // unknown
	currentState.goldLocation = Location(0,0); // unknown
	//q = map<string, vector<double>() >();
	//n = map<string, vector<int>() >();
	discountFactor = 0.9;
	Ne = 5;
}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
	currentState.agentLocation = Location(1,1);
	currentState.agentOrientation = RIGHT;
	currentState.agentHasArrow = true;
	currentState.agentHasGold = false;
	currentState.wumpusAlive = true;
	currentState.agentInCave = true;
	actionList.clear();
	lastAction = CLIMB; // dummy action
	lastPercept = Percept(false, false, false, false, false); // dummy percept
	numActions = 0;
	prevState = "";
	prevReward = 0.0;
}

Action Agent::Process (Percept& percept)
{
	Action action;

	UpdateState(percept);
	lastPercept = percept;

	state = getState(currentState);
	if(q.find(state) == q.end()) {
		q[state] = vector<double>(6, 0.0);
	}
	if(n.find(state) == n.end()) {
		n[state] = vector<int>(6, 0);
	}
	
	reward = -0.01;
	pair<Action, double> maxQ = getMaxQ(q[state]);
	if (prevState != "") {
		n[prevState][lastAction]++;
		learningRate = 100.0 / (99.0 + n[prevState][lastAction]);
		q[prevState][lastAction] = q[prevState][lastAction] + (learningRate * (prevReward + (discountFactor * maxQ.second) - q[prevState][lastAction]));
	}

	prevState = state;
	action = argMaxAction(state); //Exploration(maxQ, n[state][maxQ.first]);
	prevReward = reward;

	lastAction = action;
	numActions++;
	return action;
}

void Agent::UpdateState (Percept& percept)
{
	// Check if wumpus killed
	if (percept.Scream)
	{
		currentState.wumpusAlive = false;
		// Since only kill wumpus point-blank, we know its location is in front of agent
		currentState.wumpusLocation = currentState.agentLocation;
		switch (currentState.agentOrientation)
		{
			case RIGHT: currentState.wumpusLocation.X++; break;
			case UP: currentState.wumpusLocation.Y++; break;
			case LEFT: currentState.wumpusLocation.X--; break;
			case DOWN: currentState.wumpusLocation.Y--; break;
		}
	}
	// Check if have gold
	if (lastAction == GRAB && lastPercept.Glitter == true)
	{
		currentState.agentHasGold = true;
		currentState.goldLocation = currentState.agentLocation;
	}
	// Check if used arrow
	if (lastAction == SHOOT)
	{
		currentState.agentHasArrow = false;
	}
	// Update orientation
	if (lastAction == TURNLEFT)
	{
		currentState.agentOrientation = (Orientation) ((currentState.agentOrientation + 1) % 4);
	}
	if (lastAction == TURNRIGHT)
	{
		currentState.agentOrientation = (Orientation) ((currentState.agentOrientation + 3) % 4);
	}
	// Update location
	if ((lastAction == GOFORWARD) && (! percept.Bump))
	{
		switch (currentState.agentOrientation)
		{
			case RIGHT: currentState.agentLocation.X++; break;
			case UP: currentState.agentLocation.Y++; break;
			case LEFT: currentState.agentLocation.X--; break;
			case DOWN: currentState.agentLocation.Y--; break;
		}
	}
	// Update world size
	if (currentState.agentLocation.X > currentState.worldSize)
	{
		currentState.worldSize = currentState.agentLocation.X;
	}
	if (currentState.agentLocation.Y > currentState.worldSize)
	{
		currentState.worldSize = currentState.agentLocation.Y;
	}
}

string Agent::getState(WorldState s) {
	string state = to_string(s.agentLocation.X) + "" + to_string(s.agentLocation.Y) + "" + to_string(s.agentOrientation) + "" + to_string(s.agentHasGold) + "" + to_string(s.agentHasArrow) + "" + to_string(s.agentInCave);
	//cout << "STATE: " << state << endl;
	return state;
}

pair<Action, double> Agent::getMaxQ(vector<double> qval) {
	double val = qval[0];
	Action act = GOFORWARD;
	//bool same = true;
	for(int i=1; i<6; i++) {
		//if(qval[i] != val) same = false;
		if(qval[i] > val) {
			val = qval[i];
			act = (Action) i;
		}
	}
	//if(same) {
	//	act = (Action) (rand() % 6);
	//}
	return make_pair(act, val);
}

double Agent::Exploration(double qval, int n) {
	if(n < Ne) {
		//cout << "Explore" << endl;
		//return (Action) (rand() % 6);
		return 1000.0;
	}
	//cout << "Q value" << endl;
	return qval;
}

Action Agent::argMaxAction(string state) {
	Action act = GOFORWARD;
	//vector<Action> acts(1, GOFORWARD);
	double val = Exploration(q[state][0], n[state][0]);
	//cout << "{" << 0 << " " << val << " " << n[state][0] << "}" << endl;
	for(int i=1; i<6; i++) {
		int temp = Exploration(q[state][i], n[state][i]);
		//if(temp == val) acts.push_back((Action) i);
		//else 
		if(temp > val) {
			val = temp;
			//acts = vector<Action>(1, (Action) i);
			act = (Action) i;
		}
		//cout << "{" << i << " " << temp << " " << n[state][i] << "}" << endl;
	}
	//act = acts[(rand() % acts.size())];
	//cout << "choosen: {" << act << " " << val << "}" << endl;
	return act;
}

void Agent::GameOver (int score)
{
	//if(numActions < 1000)
	currentState.agentInCave = false;
	if (lastAction != CLIMB && numActions < 1000){
		int x = currentState.agentLocation.X;
		int y = currentState.agentLocation.Y;
	    Orientation orientation = currentState.agentOrientation;
	    if (orientation == RIGHT) {
	    	x++;
	    }
	    if (orientation == UP) {
	    	y++;
	    }
	    if (orientation == LEFT) {
	    	x--;
	    }
	    if (orientation == DOWN) {
	    	y--;
	    }
	    currentState.agentLocation.X = x;
	    currentState.agentLocation.Y = y;
	}

	state = getState(currentState);
	if(q.find(state) == q.end()) {
		q[state] = vector<double>(6, 0.0);
	}
	if(n.find(state) == n.end()) {
		n[state] = vector<int>(6, 0);
	}
	reward = score;
		
	for(int i=0; i<6; i++) {
		q[state][i] = reward;
	}

	pair<Action, double> maxQ = getMaxQ(q[state]);
	if (prevState != "") {
		n[prevState][lastAction]++;
		learningRate = 100.0 / (99.0 + n[prevState][lastAction]);
		//q[prevState][lastAction] = q[prevState][lastAction] + (learningRate * (prevReward + (discountFactor * maxQ.second) - q[prevState][lastAction]));
		q[prevState][lastAction] = q[prevState][lastAction] + (learningRate * (reward + (discountFactor * maxQ.second) - q[prevState][lastAction]));
	}

}



