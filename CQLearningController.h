#pragma once
#include "cdisccontroller.h"
#include "CParams.h"
#include "CDiscCollisionObject.h"
#include <cmath>
#include <set>
#include <chrono>
#include <thread>

typedef unsigned int uint;
class CQLearningController :
	public CDiscController
{
private:
	uint _grid_size_x;
	uint _grid_size_y;
	const double gamma = 0.5f;
	std::set<int> collectedMines;
public:
	CQLearningController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	double R(uint x, uint y, uint sweeper_no);
	virtual bool Update(void);
	virtual ~CQLearningController(void);
	bool checkIfAllActionsZero(CDiscMinesweeper *sweeper);
	int getMaxActionDirection(CDiscMinesweeper *sweeper);
	double getMaxAction(CDiscMinesweeper *sweeper);
	bool checkIfAllVisited(CDiscMinesweeper *sweeper);

};

