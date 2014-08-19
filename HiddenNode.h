#pragma once
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>

typedef unsigned int uint;
class HiddenNode
{
private:
	std::vector<double> weights;
	double output;
public:
	HiddenNode(uint no_input_weights);
	~HiddenNode();
	void printWeights();
};

