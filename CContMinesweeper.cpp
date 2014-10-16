#include "CContMinesweeper.h"

//-----------------------------------constructor-------------------------
//
//-----------------------------------------------------------------------
CContMinesweeper::CContMinesweeper():
							 CMinesweeper(),
                             m_dRotation(RandFloat()*CParams::dTwoPi),
							 m_dSpeed(MAX_SPEED_IN_PIXELS), ticksAlive(0),
							 neuralNetwork(INPUT_SIZE, HIDDEN_SIZE, OUTPUT_SIZE, 0.05f, 0.001f),
							 genome(INPUT_SIZE*HIDDEN_SIZE*OUTPUT_SIZE)
{
	//create a random start position
	m_vPosition = SVector2D<double>((RandFloat() * CParams::WindowWidth), 
					                (RandFloat() * CParams::WindowHeight));

	uint count = 0;
	for (uint i = 0; i < HIDDEN_SIZE; ++i){
		for (uint j = 0; j < INPUT_SIZE; ++j){
			genome[count] = neuralNetwork.getHiddenNodes()[i].getWeight(j);
			++count;
		}
	}

	for (uint i = 0; i < OUTPUT_SIZE; ++i){
		for (uint j = 0; j < HIDDEN_SIZE; ++j){
			genome[count] = neuralNetwork.getOutputNodes()[i].getWeight(j);
		}
	}

	/*for (uint i = 0; i < genome.size(); ++i){
		std::cout << genome[i] << ", ";
	}
	std::cout << std::endl;*/
}

//CContMinesweeper::CContMinesweeper(vector<double> offspringGenome):
//					CMinesweeper(),
//					m_dRotation(RandFloat()*CParams::dTwoPi),
//					m_dSpeed(MAX_SPEED_IN_PIXELS), ticksAlive(0),
//					neuralNetwork(INPUT_SIZE, HIDDEN_SIZE, OUTPUT_SIZE, 0.05f, 0.001f),
//					genome(INPUT_SIZE*HIDDEN_SIZE*OUTPUT_SIZE){
//	//create a random start position
//	m_vPosition = SVector2D<double>((RandFloat() * CParams::WindowWidth),
//		(RandFloat() * CParams::WindowHeight));
//
//	uint count = 0;
//	for (uint i = 0; i < HIDDEN_SIZE; ++i){
//		for (uint j = 0; j < INPUT_SIZE; ++j){
//			neuralNetwork.getHiddenNodes()[i].setWeight(j, offspringGenome[count]);
//			++count;
//		}
//	}
//
//	for (uint i = 0; i < OUTPUT_SIZE; ++i){
//		for (uint j = 0; j < HIDDEN_SIZE; ++j){
//			neuralNetwork.getOutputNodes()[i].setWeight(j, offspringGenome[count]);
//		}
//	}
//
//	genome = offspringGenome;
//}

//-------------------------------------------Reset()--------------------
//
//	Resets the sweepers position, MinesGathered and rotation
//
//----------------------------------------------------------------------
void CContMinesweeper::Reset()
{
	std::cout << "Num Ticks = " << ticksAlive << std::endl;
	//reset the sweepers positions
	m_vPosition = SVector2D<double>((RandFloat() * CParams::WindowWidth), 
					                (RandFloat() * CParams::WindowHeight));

	
	CMinesweeper::Reset();

	//and the rotation
	m_dRotation = RandFloat()*CParams::dTwoPi;


	ticksAlive = 0;
	return;
}
//---------------------WorldTransform--------------------------------
//
//	sets up a translation matrix for the sweeper according to its
//  scale, rotation and position. Returns the transformed vertices.
//-------------------------------------------------------------------
void CContMinesweeper::WorldTransform(vector<SPoint> &sweeper)
{
	//create the world transformation matrix
	C2DMatrix matTransform;
	
	//scale
	matTransform.Scale(m_dScale, m_dScale);
	
	//rotate
	matTransform.Rotate(m_dRotation - CParams::dHalfPi);
	
	//and translate
	matTransform.Translate(m_vPosition.x, m_vPosition.y);
	
	//now transform the ships vertices
	matTransform.TransformSPoints(sweeper);
}

//-------------------------------Update()--------------------------------
//
//	First we take sensor readings. These are then fed into the learning algorithm
//
//	The inputs are:
//	
//	A vector to the closest mine (x, y)
//	The sweepers 'look at' vector (x, y)
//	So given a force we calculate the resultant rotation 
//	and acceleration. This is then applied to current velocity vector.
//
//-----------------------------------------------------------------------
bool CContMinesweeper::Update(vector<CContCollisionObject*> &objects)
{
	if (!m_bDead)
		++ticksAlive;
	//update Look At 
	m_vLookAt.x = cos(m_dRotation);
	m_vLookAt.y = sin(m_dRotation);

	//update position
	m_vPosition += (m_vLookAt * m_dSpeed);

	//wrap around window limits
	if (m_vPosition.x > CParams::WindowWidth) m_vPosition.x = 0;
	if (m_vPosition.x < 0) m_vPosition.x = CParams::WindowWidth;
	if (m_vPosition.y > CParams::WindowHeight) m_vPosition.y = 0;
	if (m_vPosition.y < 0) m_vPosition.y = CParams::WindowHeight;
	GetClosestObjects(objects);	
	return true;
}


//----------------------GetClosestObject()---------------------------------
//
//	returns the vector from the sweeper to the closest mine
//
//-----------------------------------------------------------------------
void CContMinesweeper::GetClosestObjects(vector<CContCollisionObject*> &objects)
{
	double			closest_mine_so_far = 99999, closest_rock_so_far = 99999, closest_super_mine_so_far = 99999;

	SVector2D<double>		vClosestObject(0, 0);
		int numberOfTargetedMines = 0;
		//std::cout << "GCO" << CParams::iNumMines << std::endl;
		//cycle through mines to find closest
		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i]->isDead()) continue; //skip if object was destroyed earlier
			if (objects[i]->isTargeted() && m_iClosestMine == -1) {
				++numberOfTargetedMines;
				if (numberOfTargetedMines < CParams::iNumMines){
					//std::cout << "keep looking for new target" << std::endl;
					//std::cout << "mine at ( " << this->Position().x << "," << this->Position().y << ")" << "skipping mine " << "i" << std::endl;
					continue; //skip if current object has been targeted
				}
				else{
					//std::cout << "mine at ( " << this->Position().x << "," << this->Position().y << ")" << "cycled through all mines " << "NOTM =" << numberOfTargetedMines << std::endl;
					//std::cout << "setting random mine" << std::endl;
					do{
						m_iClosestMine = RandInt(0, objects.size()-1);
						//std::cout << "generated random number" << std::endl;
					} while (objects[m_iClosestMine]->getType() != CCollisionObject::ObjectType::Mine);
					//std::cout << "set random mine" << std::endl;
				}
			}
			double len_to_object = Vec2DLength<double>(objects[i]->getPosition() - m_vPosition);

			switch (objects[i]->getType()){
			case CCollisionObject::ObjectType::Mine:
				if (len_to_object < closest_mine_so_far && m_iClosestMine == -1)
				{
					//std::cout << "getting new mine" << m_iClosestMine << ", obj size = " << objects.size() << std::endl;
					closest_mine_so_far = len_to_object;
					vClosestObject = objects[i]->getPosition() - m_vPosition;
					m_iClosestMine = i;
				}
				break;
			case CCollisionObject::ObjectType::Rock:
				if (len_to_object < closest_rock_so_far)
				{
					closest_rock_so_far = len_to_object;
					vClosestObject = objects[i]->getPosition() - m_vPosition;
					m_iClosestRock = i;
				}
				break;
			case CCollisionObject::ObjectType::SuperMine:
				if (len_to_object < closest_super_mine_so_far)
				{
					closest_super_mine_so_far = len_to_object;
					vClosestObject = objects[i]->getPosition() - m_vPosition;
					m_iClosestSupermine = i;
				}
				break;
			}
		}
		//std::cout << "got new mine = " << m_iClosestMine << std::endl;
		//std::cout << "GCO2" << std::endl;
		//This is a problem
		objects[m_iClosestMine]->setTargeted(true); // set closest untargeted mine as now being targeted
		//std:cout << "set new mine as target" << std::endl;
}
//----------------------------- CheckForMine -----------------------------
//
//  this function checks for collision with its closest mine (calculated
//  earlier and stored in m_iClosestMine)
//-----------------------------------------------------------------------
int CContMinesweeper::CheckForObject(vector<CContCollisionObject*> &objects, double size)
{

	SVector2D<double> DistToObject;
	double minMagDistToObject = 0.0f;
		
	if (m_iClosestMine > -1 && !objects[m_iClosestMine]->isDead()){
		DistToObject = m_vPosition - objects[m_iClosestMine]->getPosition();
		
		minMagDistToObject = Vec2DLength<double>(DistToObject);
		if (minMagDistToObject < (size + 5))
		{
			return m_iClosestMine;
		}
	}

	//remove mine if sweeper is passing over it even if not target
	for (int i = 0; i < objects.size(); ++i){
		if (objects[i]->getType() == CCollisionObject::ObjectType::Mine && !objects[i]->isDead()){
			DistToObject = m_vPosition - objects[i]->getPosition();
			if (Vec2DLength<double>(DistToObject) < (size + 5)){
				return i;
			}
		}
	}

	/*DistToObject = m_vPosition - objects[m_iClosestRock]->getPosition();
		
	if (Vec2DLength<double>(DistToObject) < (size + 5))
	{
			return m_iClosestRock;
	}*/

	DistToObject = m_vPosition - objects[m_iClosestSupermine]->getPosition();
		
	if (Vec2DLength<double>(DistToObject) < (size + 5))
	{
			return m_iClosestSupermine;
	}
  return -1;
}
//-----------------------------------------------------------------------
// Getters and setters for speed
// speed is set based on distance provided
//-----------------------------------------------------------------------
void CContMinesweeper::setSpeed(double distance, int distanceMultiplier)
{	
	//std::cout << "speed =" << speed << std::endl;
	//m_dSpeed = (double(MAX_SPEED_IN_PIXELS)*distanceMultiplier)/ (max(CParams::WindowHeight, CParams::WindowWidth))*distance+0.4f;
	m_dSpeed = MAX_SPEED_IN_PIXELS;
}

void CContMinesweeper::setSpeed(double distance)
{
	//std::cout << "speed =" << speed << std::endl;
	//m_dSpeed = (MAX_SPEED_IN_PIXELS / (max(CParams::WindowHeight, CParams::WindowWidth)))*distance + 0.4f;
	m_dSpeed = MAX_SPEED_IN_PIXELS;
}
double CContMinesweeper::getSpeed() const
{
	return m_dSpeed;
}
//-----------------------------------------------------------------------
//Accessor to the current look vector of the sweeper (this is normalized
//already)
//-----------------------------------------------------------------------
SVector2D<double> CContMinesweeper::getLookAt(void) const 
{
	return m_vLookAt;
}
//----------------------------- turn -----------------------------
//
//  Lets the minesweeper turn towards / away from the current direction
//  to pt, at rate_factor * MAX_TURNING_RATE_IN_DEGREES
// 
//-----------------------------------------------------------------------
void CContMinesweeper::turn(SPoint pt, double rate_factor, bool towards)
{
	double aclockRotRads = m_dRotation + (rate_factor*MAX_TURNING_RATE_IN_DEGREES)*CParams::dPi/180;
	double clockRotRads = m_dRotation - (rate_factor*MAX_TURNING_RATE_IN_DEGREES)*CParams::dPi/180;
	SVector2D<double> vLookAC(cos(aclockRotRads),sin(aclockRotRads));
	SVector2D<double> vLookC(cos(clockRotRads),sin(clockRotRads));
	//get the vector to the point from the sweepers current position:
	SVector2D<double> vObj(SVector2D<double>(pt.x,pt.y) - m_vPosition);
	Vec2DNormalize<double>(vObj);
	//remember (MAM1000 / CSC3020) the dot product between two normalized vectors returns
	//1 if the two vectors point in the same direction
	//0 if the two vectors are perpendicular
	//-1 if the two vectors are pointing in opposite directions
	//therefore let's work out which if ACW rotation or CW rotation brings us closer to 1:
	double dot_aclockW = Vec2DDot<double>(vLookAC,vObj);
	double dot_clockW = Vec2DDot<double>(vLookC,vObj);
	if (towards)
		m_dRotation = (abs(1 - dot_aclockW) < abs(1 - dot_clockW)) ? aclockRotRads : clockRotRads;
	else
		m_dRotation = (abs(1 - dot_aclockW) < abs(1 - dot_clockW)) ? clockRotRads : aclockRotRads;
}

CNeuralNet CContMinesweeper::getNeuralNet(){
	return neuralNetwork;
}

int CContMinesweeper::getNumTicksAlive()const{
	return ticksAlive;
}

void CContMinesweeper::setGenome(vector<double> newGenome){
	std::cout << "setting new genome" << std::endl;
	genome = newGenome;
	uint count = 0;
	for (uint i = 0; i < HIDDEN_SIZE; ++i){
		for (uint j = 0; j < INPUT_SIZE; ++j){
			neuralNetwork.getHiddenNodes()[i].setWeight(j, genome[count]);
			++count;
		}
	}

	for (uint i = 0; i < OUTPUT_SIZE; ++i){
		for (uint j = 0; j < HIDDEN_SIZE; ++j){
			neuralNetwork.getOutputNodes()[i].setWeight(j, genome[count]);
		}
	}
}

