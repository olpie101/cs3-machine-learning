#include "NeuroSweeper.h"


NeuroSweeper::NeuroSweeper(int inputSize, int hiddenSize, int outputSize) :neuralNetwork(inputSize, hiddenSize, outputSize, 0.05f, 0.001f),
	genome(inputSize*hiddenSize*outputSize), CContMinesweeper()
{
	uint count = 0;
	for (uint i = 0; i < hiddenSize; ++i){
		for (uint j = 0; j < inputSize; ++j){
			genome[count] = neuralNetwork.getHiddenNodes()[i].getWeight(j);
			++count;
		}
	}

	for (uint i = 0; i < outputSize; ++i){
		for (uint j = 0; j < hiddenSize; ++j){
			genome[count] = neuralNetwork.getOutputNodes()[i].getWeight(j);
		}
	}

	for (uint i = 0; i < genome.size(); ++i){
		std::cout << genome[i] << ", ";
	}
	std::cout << std::endl;
}


NeuroSweeper::~NeuroSweeper()
{
}
