#include "HiddenNode.h"


HiddenNode::HiddenNode(uint no_input_weights)
{
	/* initialize random seed: */
	srand(time(NULL));

	weights = std::vector<double>(no_input_weights);
	for (uint i = 0; i < weights.size(); ++i){
		weights[i] = (rand() % 2) - 1;
	}
}

HiddenNode::~HiddenNode()
{
}

void HiddenNode::printWeights(){
	for (uint i = 0; i < weights.size(); ++i){
		std::cout << weights[i] << std::endl;
	}
}
