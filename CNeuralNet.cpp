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
	std::cout << "inside neuro construct, LR= " << learningRate << std::endl;
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
		//std::cout << "inside feed forward2 " << i + 1 << " => " << actualOutput[outputIndex].size() << std::endl;
		//std::cout << "actualOutput[0]Size = " << actualOutput[outputIndex].size() << std::endl;
		//TODO allocate space beforehand in the train method
		actualOutput[outputIndex][i] = outputNodes[i].getOutput();
		//std::cout << "OI= " << outputIndex << ", output => " << actualOutput[outputIndex].back() << " vs " << outputNodes[i].getOutput() << std::endl;
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
 3. Adjust the weights from the hidden to the output layer: learning rate * error at the output layer * hidden layer output value
    for each connection between the hidden and output layers
 4. Adjust the weights from the input to the hidden layer: learning rate * error at the hidden layer * input layer node value
    for each connection between the input and hidden layers
 5. REMEMBER TO FREE ANY ALLOCATED MEMORY WHEN YOU'RE DONE (or use std::vector ;)
*/
void CNeuralNet::propagateErrorBackward(const std::vector<double> & const desiredOutput){
	//TODO
	/*std::cout << "d vs a => " << desiredOutput[0] << " vs " << actualOutput[outputIndex][0]
		<< "\n\t" << desiredOutput[1] << " vs " << actualOutput[outputIndex][1] << std::endl;*/
	for (int i = 0; i < desiredOutput.size(); ++i){
		double deltaOutput = desiredOutput[i] - actualOutput[outputIndex][i];
		outputNodes[i].signalErrror = deltaOutput*Sigmoid::SigmoidD(actualOutput[outputIndex][i]);
		//std::cout << "ONSigErr" << i << " =" << outputNodes[i].signalErrror << std::endl;
	}

	double sum;
	//std::cout << "hidden layer size = " << hiddenNodes.size() << std::endl;
	for (int i = 0; i < hiddenNodes.size(); ++i){
		sum = 0;
		for (int j = 0; j < outputNodes.size(); ++j){
			sum += outputNodes[j].getWeight(i)*(desiredOutput[j] - actualOutput[outputIndex][j]);
		}

		hiddenNodes[i].signalErrror = Sigmoid::SigmoidD(hiddenNodes[i].getOutput())*sum;
		//std::cout << "HNSigErr" << i << " =" << hiddenNodes[i].signalErrror << std::endl;
	}

	//std::cout << "about to handle weights" << std::endl;

	for (int i = 0; i < outputNodes.size(); ++i){
		for (int j = 0; j < outputNodes[i].getNumberOfWeights(); j++){
			if (i > 0){
				outputNodes[i].deltaWeight = (learningRate*outputNodes[i].signalErrror*outputNodes[i].getInput(j))+outputNodes[i-1].deltaWeight*0.3;
			}
			else{
				outputNodes[i].deltaWeight = learningRate*outputNodes[i].signalErrror*outputNodes[i].getInput(j);
			}
			//std::cout << "out weight (" << i << "," << j << ") was " << outputNodes[i].getWeight(j);
			outputNodes[i].adjustWeight(j, outputNodes[i].deltaWeight);
			//std::cout << " => now " << outputNodes[i].getWeight(j) << std::endl;
		}
	}

	for (int i = 0; i < hiddenNodes.size(); ++i){
		for (int j = 0; j < hiddenNodes[i].getNumberOfWeights(); j++){
			if (i > 0){
				hiddenNodes[i].deltaWeight = (learningRate*hiddenNodes[i].signalErrror*hiddenNodes[i].getInput(j))+hiddenNodes[i-1].deltaWeight*0.3;
			}
			else{
				hiddenNodes[i].deltaWeight = learningRate*hiddenNodes[i].signalErrror*hiddenNodes[i].getInput(j);
			}
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
		//std::cout << "mse" << outputIndex << "-" << i << " => " << desiredOutput[i] << "-" << actualOutput[outputIndex][i] << "=" << err << std::endl;
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
	actualOutput.resize(trainingSetSize+1);
	std::copy(outputs.begin(), outputs.end(), desiredOutput.begin());
	for (int i = 0; i < actualOutput.size(); ++i){
		actualOutput[i].resize(desiredOutput[0].size());
	}
	
	printf("resized\n");
	
	int count = 1;
	do{
		//std::cout << "k = " << k << std::endl;
		for (int i = 0; i < inputs.size(); ++i){
			outputIndex = i;
			feedForward(inputs[i]);
			//std::cout << "fed forward" << std::endl;
			propagateErrorBackward(desiredOutput[i]);
		}
		outputIndex = 0;
		if (count % 20 == 0)
			//std::cout << "mse0 =" << meanSquaredError(desiredOutput[0]) << std::endl;
			printAllMSEs();

		++count;
		//std::cout << "DO = " << desiredOutput[0][0] << desiredOutput[0][1] << std::endl;
		//std::cout << "AO = " << actualOutput[0][0] << actualOutput[0][1] << std::endl;
 	} while (!checkMSE());
	//} while (count < 5);

	std::cout << "##########################"
		<< std::endl << "mse0 =" << meanSquaredError(desiredOutput[0]) << std::endl
		<< "##########################"
		<< std::endl;
	printf("leaving train\n");
}
/**
Once our network is trained we can simply feed it some input though the feed forward
method and take the maximum index value as the classification
*/
uint CNeuralNet::classify(const std::vector<double> & const input){
	outputIndex = actualOutput.size() - 1;
	feedForward(input);
	uint maxIndex = 0;
	double maxValue = -DBL_MAX;

	//std::cout << "outputs = " << actualOutput[outputIndex][0] << ", " << actualOutput[outputIndex][1] << std::endl;

	for (int i = 0; i < actualOutput[outputIndex].size(); ++i){
		if (actualOutput[outputIndex][i] > maxValue){
			maxValue = actualOutput[outputIndex][i];
			maxIndex = i;
		}
	}
	//std::cout << "maxIndex = " << maxIndex << std::endl;
	return maxIndex;
}
/**
Gets the output at the specified index
*/
std::vector<double> CNeuralNet::getOutput(uint index) const{
	return actualOutput[index];
}

/**
* Checks all MSEs and if one is above cutt off, blame all.
* false = MSE is too high
* true = MSE is correct
**/
bool CNeuralNet::checkMSE(){
	//std::cout << "inside CMSE" << std::endl;
	double sum = 0;
	for (int i = 0; i < desiredOutput.size(); ++i){
		outputIndex = i;
		sum += meanSquaredError(desiredOutput[i]);
	}
	//for (int i = 0; i < desiredOutput.size(); ++i){
	//	outputIndex = i;
	//	//sum += meanSquaredError(desiredOutput[i]);
	//	if (meanSquaredError(desiredOutput[i]) > mseCutOff){
	//		outputIndex = 0;
	//		return false;
	//	}
	//}
	outputIndex = 0;
	
	if ((sum / desiredOutput.size()) > mseCutOff){
		return false;
	}
	else{
		return true;
	}
	//return true;
}

void CNeuralNet::printAllMSEs(){
	double sum = 0;
	for (int i = 0; i < desiredOutput.size(); ++i){
		outputIndex = i;
		sum += meanSquaredError(desiredOutput[i]);
		 
		if (meanSquaredError(desiredOutput[i]) > mseCutOff)
			std::cout << "mse" << i << " = " << meanSquaredError(desiredOutput[i]) << std::endl;
	}
	std::cout << "mse avg = " << (sum / desiredOutput.size()) << std::endl;
}