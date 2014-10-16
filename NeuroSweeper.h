#pragma once
#include "CContMinesweeper.h"
#include "CNeuralNet.h"
class NeuroSweeper :
	public CContMinesweeper
{
public:
	NeuroSweeper();
	NeuroSweeper(int inputSize, int hiddenSize, int outputSize);
	~NeuroSweeper();
	//vector<vector<double>>;

private:
	CNeuralNet neuralNetwork;
	vector<double> genome;
};

