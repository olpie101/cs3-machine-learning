#pragma once
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <random>
#include <chrono>
#include <math.h>
#include "Sigmoid.h"

typedef unsigned int uint;
typedef std::chrono::high_resolution_clock myclock;
class Node
{
private:
	std::vector<double> inputs;
	std::vector<double> weights;
	double output = 0.0f;
	bool dataChanged;
	double calculateOutput();
public:
	Node();
	Node(uint no_input_weights);
	~Node();
	void printWeights();
	void resize(const uint newSize);
	void resetWeights();
	void setInput(uint index, double value);
	double Node::getInput(uint index);
	double getOutput();
	double getWeight(uint index);
	void adjustWeight(uint index, double value);
	uint getNumberOfWeights();
	void printStats();
	double signalErrror;
	double bias = 1.0f;
	double deltaWeight = 0.0f;
};

