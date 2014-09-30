/**
         (                                      
   (     )\ )                                   
 ( )\   (()/(   (    ) (        (        (  (   
 )((_)   /(_)) ))\( /( )(   (   )\  (    )\))(  
((_)_   (_))  /((_)(_)|()\  )\ |(_) )\ )((_))\  
 / _ \  | |  (_))((_)_ ((_)_(_/((_)_(_/( (()(_) 
| (_) | | |__/ -_) _` | '_| ' \)) | ' \)) _` |  
 \__\_\ |____\___\__,_|_| |_||_||_|_||_|\__, |  
                                        |___/   

Refer to Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
for a detailed discussion on Q Learning
*/
#include "CQLearningController.h"


CQLearningController::CQLearningController(HWND hwndMain):
	CDiscController(hwndMain),
	_grid_size_x(CParams::WindowWidth / CParams::iGridCellDim + 1),
	_grid_size_y(CParams::WindowHeight / CParams::iGridCellDim + 1)
{
}
/**
 The update method should allocate a Q table for each sweeper (this can
 be allocated in one shot - use an offset to store the tables one after the other)

 You can also use a boost multiarray if you wish
*/
void CQLearningController::InitializeLearningAlgorithm(void)
{
	for (int i = 0; i < m_vecSweepers.size(); ++i){
		std::cout << "initializing QTable for mine " << i << std::endl;
		m_vecSweepers[i]->initializeQTable();
	}
}
/**
 The immediate reward function. This computes a reward upon achieving the goal state of
 collecting all the mines on the field. It may also penalize movement to encourage exploring all directions and 
 of course for hitting supermines/rocks!
*/
double CQLearningController::R(uint x,uint y, uint sweeper_no){
	//TODO: roll your own here!

	//Find out if sweeper hit anything
	int hit = -1;
	for (int i = 0; i < m_vecObjects.size(); ++i){
		SVector2D<int> DistToObject = m_vecSweepers[sweeper_no]->Position() - m_vecObjects[i]->getPosition();
		if (Vec2DLength<int>(DistToObject) < CParams::dMineScale + 5)
		{
			hit = i; 
			break;
		}
	}

	double r = -20;			//Default reward if sweeper has moved into a blank spot
	if (hit > -1){
		switch (m_vecObjects[hit]->getType()){
		case CDiscCollisionObject::Mine:
			if (m_vecObjects[hit]->isDead() && m_vecObjects[hit]->prevState == CDiscCollisionObject::ALIVE){
				//Mine was found and was alive in the previous tick, give reward
				m_vecObjects[hit]->prevState = CDiscCollisionObject::DEAD;
				r = 100;
			}
			break;
		case CDiscCollisionObject::Rock:
		case CCollisionObject::ObjectType::SuperMine:
			//Rock or Supermine Found, punish
			r = -100;
			break;
		}
	}
	return r;
}
/**
The update method. Main loop body of our Q Learning implementation
See: Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
*/
bool CQLearningController::Update(void)
{
	//m_vecSweepers is the array of minesweepers
	//everything you need will be m_[something] ;)
	uint cDead = std::count_if(m_vecSweepers.begin(),
							   m_vecSweepers.end(),
						       [](CDiscMinesweeper * s)->bool{
								return s->isDead();
							   });

	//Check if all sweepers dead
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;
	}

	uint cMines = std::count_if(m_vecObjects.begin(),
		m_vecObjects.end(),
		[](CDiscCollisionObject * o)->bool{
		return (o->getType() == CDiscCollisionObject::Mine && o->isDead());
	});

	//Check if all mines have been collected
	if (cMines == CParams::iNumMines){
		printf("#All Mines ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;
	}

	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead()) continue;
		/**
		Q-learning algorithm according to:
		Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
		*/
		//1:::Observe the current state:
		//2:::Select action with highest historic return:
		//For next direction which sweeper should face
		ROTATION_DIRECTION nextRotation = (ROTATION_DIRECTION)getMaxActionDirection(m_vecSweepers[sw]);
		m_vecSweepers[sw]->setRotation(nextRotation);
		//now call the parents update, so all the sweepers fulfill their chosen action
	}
	
	CDiscController::Update(); //call the parent's class update. Do not delete this.

	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		//TODO:compute your indexes.. it may also be necessary to keep track of the previous state
		//3:::Observe new state:
		//4:::Update _Q_s_a accordingly:
		int prevX = m_vecSweepers[sw]->PrevPosition().x;
		int prevY = m_vecSweepers[sw]->PrevPosition().y;

		//Update prev state action
		double Q = R(prevX, prevY, sw) + gamma*getMaxAction(m_vecSweepers[sw]);
		m_vecSweepers[sw]->setStateActionEntry(prevX, prevY, m_vecSweepers[sw]->getRotation(), Q);
	}
	return true;
}

CQLearningController::~CQLearningController(void)
{
	//TODO: dealloc stuff here if you need to	
}


//Checks if all actions are zero
bool CQLearningController::checkIfAllActionsZero(CDiscMinesweeper *sweeper){
	int x = sweeper->Position().x / CParams::iGridCellDim;
	int y = sweeper->Position().y / CParams::iGridCellDim;

	for (int i = 0; i < 4; ++i){
		if (sweeper->getStateActionEntryFloat(x, y, i) != 0)
			return false;
	}
	return true;
}

//Get Direction of next possible action
int CQLearningController::getMaxActionDirection(CDiscMinesweeper *sweeper){
	float maxValue = -DBL_MAX;
	int maxIndex = -1;
	bool allVisited = checkIfAllVisited(sweeper);
	int x = sweeper->Position().x / CParams::iGridCellDim;
	int y = sweeper->Position().y / CParams::iGridCellDim;
	for (int i = 0; i < 4; ++i){
		if (allVisited &&
			sweeper->getStateActionEntryFloat(x, y, i) > maxValue){
			maxValue = sweeper->getStateActionEntryFloat(x, y, i);
			maxIndex = i;
		}
		else if (
			sweeper->getStateActionEntryFloat(x, y, i) > maxValue){
			maxValue = sweeper->getStateActionEntryFloat(x, y, i);
			maxIndex = i;
		}
	}
	return maxIndex;
}

//get the value of next possible action
double CQLearningController::getMaxAction(CDiscMinesweeper *sweeper){
	int x = sweeper->Position().x / CParams::iGridCellDim;
	int y = sweeper->Position().y / CParams::iGridCellDim;
	return sweeper->getStateActionEntryFloat(x, y, getMaxActionDirection(sweeper));
}
