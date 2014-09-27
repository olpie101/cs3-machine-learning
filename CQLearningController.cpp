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
	//std::cout << "start R" << m_vecSweepers[0]->Position().x << ", " << m_vecSweepers[0]->Position().y << std::endl;
	int hit = -1;
	for (int i = 0; i < m_vecObjects.size(); ++i){
		SVector2D<int> DistToObject = m_vecSweepers[sweeper_no]->Position() - m_vecObjects[i]->getPosition();

		if (Vec2DLength<int>(DistToObject) < CParams::dMineScale + 5)
		{
			//std::cout << "object " << i << " = " << Vec2DLength<int>(DistToObject) << std::endl;
			hit = i; 
			break;
		}
	}

	double r = -20;
	//std::cout << "hit = " << hit << std::endl;
	if (hit > -1){
		
		//CDiscCollisionObject item = *m_vecObjects[hit];
		switch (m_vecObjects[hit]->getType()){
		case CDiscCollisionObject::Mine:
			if (m_vecObjects[hit]->isDead() && m_vecObjects[hit]->prevState == CDiscCollisionObject::ALIVE){
				//std::cout << "#########################@#############" << std::endl;
				collectedMines.insert(hit);
				m_vecObjects[hit]->prevState = CDiscCollisionObject::DEAD;
				//std::cout << "found mine =>" << m_vecSweepers[0]->prevState << " > " << m_vecSweepers[0]->state << std::endl;
				//std::this_thread::sleep_for(std::chrono::seconds(5));
				r = 100;
			}
			break;
		case CDiscCollisionObject::Rock:
		case CCollisionObject::ObjectType::SuperMine:
			//std::cout << "found danger" << m_vecSweepers[0]->prevState << " > " << m_vecSweepers[0]->state << std::endl;
			//std::this_thread::sleep_for(std::chrono::seconds(5));
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

	//std::cout << "#$#$" << cMines << endl;
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;
	}

	uint cMines = std::count_if(m_vecObjects.begin(),
		m_vecObjects.end(),
		[](CDiscCollisionObject * o)->bool{
		return (o->getType() == CDiscCollisionObject::Mine && o->isDead());
	});


	if (cMines == CParams::iNumMines){
		printf("#All Mines ... skipping to next iteration\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
		cMines = 0;
		m_iTicks = CParams::iNumTicks;
	}

	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead()) continue;
		/**
		Q-learning algorithm according to:
		Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
		*/
		//1:::Observe the current state:
		//TODO
		/*if (sw == 0){
		std::cout << m_vecSweepers[sw]->Position().x << ", " << m_vecSweepers[sw]->Position().y <<
		" vs " << m_vecSweepers[sw]->Position().x / CParams::iGridCellDim << ", " << m_vecSweepers[sw]->Position().y / CParams::iGridCellDim << std::endl;
		}*/

		//2:::Select action with highest historic return:
		//TODO
		//if (sw == 0)
		//std::cout << "step2" << std::endl;
		ROTATION_DIRECTION nextRotation;
		//if (checkIfAllActionsZero(m_vecSweepers[sw])){			//All adjacent blocks have a score of zero
		//	//std::cout << sw << " - all zero" << std::endl;
		//	//std::this_thread::sleep_for(std::chrono::seconds(1));
		//	//std::cout << "setting next rotation to any but opposite" << std::endl;
		//	//std::this_thread::sleep_for(std::chrono::seconds(1));
		//	ROTATION_DIRECTION currentRotation = m_vecSweepers[sw]->getRotation();
		//	ROTATION_DIRECTION oppositeRotation = (ROTATION_DIRECTION)((currentRotation + 2) % 4);
		//	do{
		//		nextRotation = (ROTATION_DIRECTION)(RandInt(0, 100)%4);
		//	} while (nextRotation == oppositeRotation);
		//	nextRotation = (ROTATION_DIRECTION)RandInt(0, 3);
		//	//std::cout << "rotation set" << std::endl;
		//}
		//else{
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			//std::cout << "not all zero" << std::endl;
			ROTATION_DIRECTION action = (ROTATION_DIRECTION)getMaxActionDirection(m_vecSweepers[sw]);
			nextRotation = action;
		//}
		m_vecSweepers[sw]->setRotation(nextRotation);
		//start uncommenting from here!!!!!
		//now call the parents update, so all the sweepers fulfill their chosen action
	}

	//std::cout << "parent update start" << m_vecSweepers[0]->Position().x << ", " << m_vecSweepers[0]->Position().y << std::endl;
	
	CDiscController::Update(); //call the parent's class update. Do not delete this.
	//std::cout << "parent update complete" << m_vecSweepers[0]->Position().x << ", " << m_vecSweepers[0]->Position().y << std::endl;

	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		//if (m_vecSweepers[sw]->isDead() && m_vecSweepers[sw]->prevState == DEAD) continue;
		//TODO:compute your indexes.. it may also be necessary to keep track of the previous state
		//3:::Observe new state:
		//TODO
		//4:::Update _Q_s_a accordingly:
		/*if (sw == 0)*/
		//std::cout << "step4" << std::endl;
		int prevX = m_vecSweepers[sw]->PrevPosition().x / CParams::iGridCellDim;
		int prevY = m_vecSweepers[sw]->PrevPosition().y / CParams::iGridCellDim;
		double Q = R(m_vecSweepers[sw]->PrevPosition().x, m_vecSweepers[sw]->PrevPosition().y, sw) + gamma*getMaxAction(m_vecSweepers[sw]);
		//std::cout << "R = " << R(prevX, prevY, sw) << "Q = " << Q << std::endl;
		m_vecSweepers[sw]->setStateActionEntry(prevX, prevY, m_vecSweepers[sw]->getRotation(), Q);
		//TODO
		//std::cout << "step4 finished" << std::endl;
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

bool CQLearningController::checkIfAllVisited(CDiscMinesweeper *sweeper){
	int x = sweeper->Position().x / CParams::iGridCellDim;
	int y = sweeper->Position().y / CParams::iGridCellDim;
	for (int i = 0; i < 4; ++i){
		if (sweeper->getStateActionEntryBool(x, y, i) == false)
			return false;
	}
	//std::cout << "all visited" << std::endl;
	return true; 
}

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

double CQLearningController::getMaxAction(CDiscMinesweeper *sweeper){
	int x = sweeper->Position().x / CParams::iGridCellDim;
	int y = sweeper->Position().y / CParams::iGridCellDim;
	//std::cout << "maxValue = " << sweeper->getStateActionEntry(x, y, getMaxActionDirection(sweeper)) << std::endl;
	return sweeper->getStateActionEntryFloat(x, y, getMaxActionDirection(sweeper));
}
