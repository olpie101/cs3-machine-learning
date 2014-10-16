#ifndef CContMINESWEEPER_H
#define CContMINESWEEPER_H

//------------------------------------------------------------------------
//
//	Name: CMineSweeper.h
//
//  Author: Mat Buckland 2002
//
//  Desc: Class to create a minesweeper object 
//
//------------------------------------------------------------------------
#include <vector>
#include <math.h>
#include "utils.h"
#include "C2DMatrix.h"
#include "SVector2D.h"
#include "CParams.h"
#include "CContCollisionObject.h"
#include "CMinesweeper.h"
#include "CNeuralNet.h"
#define MAX_TURNING_RATE_IN_DEGREES 2.0
#define MAX_SPEED_IN_PIXELS 2.0
#define INPUT_SIZE 2
#define HIDDEN_SIZE 5
#define OUTPUT_SIZE 2
using namespace std;


class CContMinesweeper:public CMinesweeper
{

private:
	//its position in the world
	SVector2D<double>		m_vPosition;

	//direction sweeper is facing
	SVector2D<double>		m_vLookAt;

	//its rotation (surprise surprise)
	double			m_dRotation;

	double			m_dSpeed;
	//sets the internal closest object variables for the 3 types of objects
	void GetClosestObjects(vector<CContCollisionObject*> &objects);
	int ticksAlive;
	CNeuralNet neuralNetwork;
	vector<double> genome;
public:
	
	void setSpeed(double speed, int distanceMultipler);
	void setSpeed(double distance);
	double getSpeed() const;
	SVector2D<double> getLookAt(void) const;
	CContMinesweeper();
	CContMinesweeper(vector<double> offspringGenome);
	
	//updates the information from the sweepers enviroment
	bool			Update(vector<CContCollisionObject*> &objects);

	//used to transform the sweepers vertices prior to rendering
	void			WorldTransform(vector<SPoint> &sweeper);

	//checks to see if the minesweeper has 'collected' a mine
	int       CheckForObject(vector<CContCollisionObject*> &objects, double size);

	void			Reset();
  

	//-------------------accessor functions
	SVector2D<double>	Position()const{return m_vPosition;}
	
	//turs towards/away from the specified point at a specified rate
	void turn(SPoint pt, double rate_factor, bool towards = true);
	CNeuralNet getNeuralNet();
	int getNumTicksAlive()const;
	const int genomeSize = INPUT_SIZE*HIDDEN_SIZE*OUTPUT_SIZE;
	vector<double> getGenome()const { return genome; }
	void setGenome(vector<double> newGenome);
};


#endif

	
	