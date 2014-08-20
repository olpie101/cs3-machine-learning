/*
                                                                           
   (               )                                        )              
 ( )\     )     ( /(       (                  (  (     ) ( /((             
 )((_) ( /(  (  )\())`  )  )(   (  `  )   (   )\))( ( /( )\())\  (   (     
((_)_  )(_)) )\((_)\ /(/( (()\  )\ /(/(   )\ ((_))\ )(_)|_))((_) )\  )\ )  
 | _ )((_)_ ((_) |(_|(_)_\ ((_)((_|(_)_\ ((_) (()(_|(_)_| |_ (_)((_)_(_/(  
 | _ \/ _` / _|| / /| '_ \) '_/ _ \ '_ \/ _` |/ _` |/ _` |  _|| / _ \ ' \)) 
 |___/\__,_\__||_\_\| .__/|_| \___/ .__/\__,_|\__, |\__,_|\__||_\___/_||_|  
                    |_|           |_|         |___/                         

 For more information on back-propagation refer to:
 Chapter 18 of Russel and Norvig (2010).
 Artificial Intelligence - A Modern Approach.
 */

#include "CNeuralNet.h"

/**
 The constructor of the neural network. This constructor will allocate memory
 for the weights of both input->hidden and hidden->output layers, as well as the input, hidden
 and output layers.
*/
CNeuralNet::CNeuralNet(uint inputLayerSize, uint hiddenLayerSize, uint outputLayerSize, double lRate, double mse_cutoff):
inputLayerSize(inputLayerSize), hiddenLayerSize(hiddenLayerSize), outputLayerSize(outputLayerSize), learningRate(lRate), mseCutOff(mse_cutoff)
	//you probably want to use an initializer list here
{	
	printf("inside neuro construct");
	//TODO
	
	hiddenNodes.resize(this->hiddenLayerSize);
	outputNodes.resize(this->outputLayerSize);
	initWeights();
}
/**
 The destructor of the class. All allocated memory will be released here
*/
CNeuralNet::~CNeuralNet() {
	//TODO
}
/**
 Method to initialize the both layers of weights to random numbers
*/
void CNeuralNet::initWeights(){
	//TODO
	for (int i = 0; i < hiddenNodes.size(); ++i){
		hiddenNodes[i].resize(inputLayerSize);
		hiddenNodes[i].resetWeights();
	}

	for (int i = 0; i < outputNodes.size(); ++i){
		outputNodes[i].resize(hiddenLayerSize);
		outputNodes[i].resetWeights();
	}
}
/**
 This is the forward feeding part of back propagation.
 1. This should take the input and copy the memory (use memcpy / std::copy)
 to the allocated _input array.
 2. Compute the output of at the hidden layer nodes 
 (each _hidden layer node = sigmoid (sum( _weights_h_i * _inputs)) //assume the network is completely connected
 3. Repeat step 2, but this time compute the output at the output layer
*/
void CNeuralNet::feedForward(const std::vector<double> & const inputs) {
	  //TODO
	//All hidden layer nodes output the same value to 2 output nodes

	for (uint i = 0; i < hiddenNodes.size(); ++i){
		for (uint j = 0; j < inputs.size(); ++j){
			hiddenNodes[i].setInput(j, inputs[j]);
		}
		//std::cout << "inside feed forward1 " << i+1 << " => " << hiddenNodes[i].getOutput() << std::endl;
	}

	//std::cout << "onto output layer" << std::endl;

	for (uint i = 0; i < outputNodes.size(); ++i){
		for (uint j = 0; j < hiddenNodes.size(); ++j){
			outputNodes[i].setInput(j, hiddenNodes[j].getOutput());
		}

		//outputNodes[i].printStats();
		//std::cout << "inside feed forward2 " << i + 1 << " => " << outputNodes[i].getOutput() << std::endl;
		//std::cout << "actualOutput[0]Size = " << actualOutput[outputIndex].size() << std::endl;
		//TODO allocate space beforehand in the train method
		actualOutput[outputIndex].push_back(outputNodes[i].getOutput());
	}
}
/**
 This is the actual back propagation part of the back propagation algorithm
 It should be executed after feeding forward. Given a vector of desired outputs
 we compute the error at the hidden and output layers (allocate some memory for this) and
 assign 'blame' for any error to all the nodes that fed into the current node, based on the
 weight of the connection.
 Steps:
 1. Compute the error at the output layer: sigmoid_d(output) * (difference between expected and computed outputs)
    for each output
 2. Compute the error at the hidden layer: sigmoid_d(hidden) * 
	sum(weights_o_h * difference between expected output and computed output at output layer)
	for each hidden layer node
 3. Adjust the weights from the hidden to the output layer: learning rate * error at the output layer * error at the hidden layer
    for each connection between the hidden and output layers
 4. Adjust the weights from the input to the hidden layer: learning rate * error at the hidden layer * input layer node value
    for each connection between the input and hidden layers
 5. REMEMBER TO FREE ANY ALLOCATED MEMORY WHEN YOU'RE DONE (or use std::vector ;)
*/
void CNeuralNet::propagateErrorBackward(const std::vector<double> & const desiredOutput){
	//TODO
	//std::cout << "in prop back dSize = " << desiredOutput.size() << std::endl;
	for (int i = 0; i < desiredOutput.size(); ++i){
		double deltaOutput = desiredOutput[i] - actualOutput[outputIndex][i];
		outputNodes[i].signalErrror = deltaOutput*actualOutput[outputIndex][i] * (1 - actualOutput[outputIndex][i]);
	}

	double sum;
	for (int i = 0; i < hiddenNodes.size(); ++i){
		sum = 0;
		for (int j = 0; j < outputNodes.size(); ++j){
			sum += outputNodes[j].getWeight(i)*outputNodes[j].signalErrror;
		}

		hiddenNodes[i].signalErrror = hiddenNodes[i].getOutput()*(1 - hiddenNodes[i].getOutput())*sum;
	}

	//std::cout << "about to handle weights" << std::endl;

	for (int i = 0; i < outputNodes.size(); ++i){
		for (int j = 0; j < outputNodes[i].getNumberOfWeights(); j++){
			outputNodes[i].deltaWeight = learningRate*outputNodes[i].signalErrror*outputNodes[i].getInput(j);
			outputNodes[i].adjustWeight(j, outputNodes[i].deltaWeight);
		}
	}

	for (int i = 0; i < hiddenNodes.size(); ++i){
		for (int j = 0; j < hiddenNodes[i].getNumberOfWeights(); j++){
			hiddenNodes[i].deltaWeight = learningRate*hiddenNodes[i].signalErrror*hiddenNodes[i].getInput(j);
			hiddenNodes[i].adjustWeight(j, hiddenNodes[i].deltaWeight);
		}
	}
}
/**
This computes the mean squared error
A very handy formula to test numeric output with. You may want to commit this one to memory
*/
double CNeuralNet::meanSquaredError(const std::vector<double> & const desiredOutput){
	/*TODO:
	sum <- 0
	for i in 0...outputLayerSize -1 do
		err <- desiredoutput[i] - actualoutput[i]
		sum <- sum + err*err
	return sum / outputLayerSize
	*/
	double sum =0;
	for (int i = 0; i < outputLayerSize; ++i){
		double err = desiredOutput[i] - actualOutput[outputIndex][i];
		sum += err*err;
	}
	return sum/outputLayerSize;
}
/**
This trains the neural network according to the back propagation algorithm.
The primary steps are:
for each training pattern:
  feed forward
  propagate backward
until the MSE becomes suitably small
*/
void CNeuralNet::train(std::vector<std::vector<double>>  const inputs,
	std::vector<std::vector<double>> const outputs, uint trainingSetSize) {
	//TODO
	outputIndex = 0;
	printf("inside train\n");
	desiredOutput.resize(trainingSetSize);
	actualOutput.resize(trainingSetSize);
	printf("resized\n");
	std::copy(outputs.begin(), outputs.end(), desiredOutput.begin());
	//do{
	for (int k = 0; k < 1000; ++k){
		//std::cout << "k = " << k << std::endl;
		for (int i = 0; i < inputs.size(); ++i){
			outputIndex = i;
			feedForward(inputs[i]);
			//std::cout << "fed forward" << std::endl;
			propagateErrorBackward(desiredOutput[i]);
		}
		if (k%100 == 0)
			std::cout << k << " mse for " << 0 << " => " << meanSquaredError(desiredOutput[0]);
		std::string test;
		std::getline(std::cin, test);
	}
	//} while (meanSquaredError(desiredOutput[0]) > mseCutOff);
	printf("leaving train\n");
}
/**
Once our network is trained we can simply feed it some input though the feed forward
method and take the maximum value as the classification
*/
uint CNeuralNet::classify(const std::vector<double> & const input){
	return 1; //TODO: fix me
}
/**
Gets the output at the specified index
*/
double CNeuralNet::getOutput(uint index) const{
	return 0; //TODO: fix me
}