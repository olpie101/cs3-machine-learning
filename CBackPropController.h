#pragma once
#include "ccontcontroller.h"
#include "CNeuralNet.h"
#include "utils.h"
#include <algorithm>
#include <assert.h>
class CBackPropController :
	public CContController
{
protected:
	CNeuralNet* _neuralnet;
	double speedMultiplier;
public:
	CBackPropController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	virtual bool Update(void);
	~CBackPropController(void);
};

