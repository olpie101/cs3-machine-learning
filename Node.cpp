#include "Node.h"

Node::Node() :dataChanged(true)
{
}

Node::Node(uint no_input_weights) : dataChanged(true)
{
	std::uniform_real_distribution<double> distribution(0.0, 2.1);
	myclock::time_point beginning = myclock::now();

	weights.resize(no_input_weights);
	inputs.resize(no_input_weights);

	for (uint i = 0; i < weights.size(); ++i){
		std::minstd_rand engine(myclock::now().time_since_epoch().count());
		weights[i] = distribution(engine)-1;
	}
}

Node::~Node()
{
}

void Node::printWeights(){
	//std::cout << "# of weights = " << weights.size();
	for (uint i = 0; i < weights.size(); ++i){
		//printf("%f", weights[i]);
		std::cout << weights[i];

		if (i < weights.size() - 1)
			std::cout << ", ";
	}

	printf("\n");
}

void Node::resize(const uint newSize){
	weights.resize(newSize);
	inputs.resize(newSize);
	dataChanged = true;
}

void Node::resetWeights(){
	for (uint i = 0; i < weights.size(); ++i){
		weights[i] = RandomClamped();
	}
	dataChanged = true;
}

void Node::setInput(uint index, double value){
	inputs[index] = value;
	dataChanged = true;
}

double Node::getInput(uint index){
	return inputs[index];
}

double Node::calculateOutput(){
	double net = 0;
	for (uint i = 0; i < inputs.size(); ++i){
		net += inputs[i] * weights[i];
	}
	net += bias;
	output = Sigmoid::SigmoidFunc(net);
	dataChanged = false;
	return  output;
}

double Node::getOutput(){
	//std::cout << "getting output = " << dataChanged << "\n";
	if (dataChanged){
		return calculateOutput();
	}
	else{
		//std::cout << "no calc" << std::endl;
		return output;
	}
}

double Node::getWeight(uint index){
	return weights[index];
}

void Node::adjustWeight(uint index, double value){
	//std::cout << "weight " << index << " before = " << weights[index] << ", delta = " << deltaWeight << std::endl;
	weights[index] += value;
	//std::cout << "weight " << index << " after = " << weights[index] << std::endl;
}

uint Node::getNumberOfWeights(){
	return weights.size();
}

void Node::printStats(){
	std::cout << "inputs:\n";
	for (uint i = 0; i < inputs.size(); ++i){
		std::cout << inputs[i] << "\n";
	}

	std::cout << "output:\n" << getOutput() << "\n";
}
