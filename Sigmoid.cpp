#include "Sigmoid.h"

double Sigmoid::SigmoidFunc(double net)
{
	return 1 / (1 + exp(-net));
}


double Sigmoid::SigmoidD(double value)
{
	return value*(1 - value);
}
