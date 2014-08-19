/*
 * CNeuralNet.h
 *
 *  Created on: 26 Dec 2013
 *      Author: benjamin
 */

#ifndef CNEURALNET_H_
#define CNEURALNET_H_
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include <stdint.h>
#include "HiddenNode.h"

typedef unsigned int uint;
class CBasicEA; //forward declare EA class, which will have the power to access weight vectors

class CNeuralNet {
	friend class CBasicEA;
private:
	uint inputLayerSize;
	uint hiddenLayerSize;
	uint outputLayerSize;
	double learningRate;
	double mseCutOff;
	std::vector<std::vector<double>> desiredOutput;
	std::vector<std::vector<double>> actualOuput;
	std::vector<HiddenNode> hiddenNodes;
protected:
	void feedForward(const std::vector<double> & const inputs); //you may modify this to do std::vector<double> if you want
	void propagateErrorBackward(const std::vector<double> & const desiredOutput); //you may modify this to do std::vector<double> if you want
	double meanSquaredError(const std::vector<double> & const desiredOutput); //you may modify this to do std::vector<double> if you want
public:
	CNeuralNet(uint inputLayerSize, uint hiddenLayerSize, uint outputLayerSize, double lRate, double mse_cutoff);
	void initWeights();
	void train(const std::vector<std::vector<double>> const inputs, const std::vector<std::vector<double>> const outputs, uint trainingSetSize); //you may modify this to do std::vector<std::vector<double> > or do boost multiarray or something else if you want
	uint classify(const std::vector<double> & const input); //you may modify this to do std::vector<double> if you want
	double getOutput(uint index) const;
	virtual ~CNeuralNet();
};

#endif /* CNEURALNET_H_ */
