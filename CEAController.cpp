/**
    )                                                         
 ( /(                                (         )              
 )\())  (   (  (         (    )      )\  (  ( /((             
((_)\  ))\ ))\ )(   (    )\  /((  ( ((_)))\ )\())\  (   (     
 _((_)/((_)((_|()\  )\  ((_)(_))\ )\ _ /((_|_))((_) )\  )\ )  
| \| (_))(_))( ((_)((_) | __|)((_|(_) (_))(| |_ (_)((_)_(_/(  
| .` / -_) || | '_/ _ \ | _|\ V / _ \ | || |  _|| / _ \ ' \)) 
|_|\_\___|\_,_|_| \___/ |___|\_/\___/_|\_,_|\__||_\___/_||_|  
                                              
	   (                )        (  (           
	   )\            ( /((       )\ )\  (  (    
	 (((_)  (   (    )\())(   ( ((_|(_)))\ )(   
	 )\___  )\  )\ )(_))(()\  )\ _  _ /((_|()\  
	((/ __|((_)_(_/(| |_ ((_)((_) || (_))  ((_) 
	 | (__/ _ \ ' \))  _| '_/ _ \ || / -_)| '_| 
	  \___\___/_||_| \__|_| \___/_||_\___||_|   
                                            
*/
#include "CEAController.h"


CEAController::CEAController(HWND hwndMain):
	CContController(hwndMain)
{
	
}

void CEAController::InitializeLearningAlgorithm(void)
{
	//TODO: set up any data structures you need to store the genomes here
}

/**
Returns the dot product between the sweeper's look vector and the vector from the sweeper to the object
*/
inline double dot_between_vlook_and_vObject(const CContMinesweeper &s,const CContCollisionObject &o){
	SVector2D<double> vLook = s.getLookAt();
	SVector2D<double> pt = o.getPosition();
		//get the vector to the point from the sweepers current position:
		SVector2D<double> vObj(SVector2D<double>(pt.x,pt.y) - s.Position());
		Vec2DNormalize<double>(vObj);
		//remember (MAM1000 / CSC3020) the dot product between two normalized vectors returns
		//1 if the two vectors point in the same direction
		//0 if the two vectors are perpendicular
		//-1 if the two vectors are pointing in opposite directions
		return Vec2DDot<double>(vLook,vObj);
}

bool CEAController::Update(void)
{
	uint cDead = std::count_if(m_vecSweepers.begin(),
							   m_vecSweepers.end(),
						       [](CContMinesweeper * s)->bool{
								return s->isDead();
							   });

	deathsThisRound = cDead;
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;
	}

	if (m_iTicks+1 >= CParams::iNumTicks && !goneThroughSelection){
		//TODO: when the round is over select, cross over and mutate here
		selection();
		goneThroughSelection = true;
	}else if (goneThroughSelection){
		goneThroughSelection = false;
	}

	int ticks = m_iTicks;
	CContController::Update(); //call the parent's class update. Do not delete this.
	for (uint i = 0; i < CParams::iNumSweepers; ++i){
		CContMinesweeper * s = m_vecSweepers[i]; 
		//TODO: set up your sensory inputs and motor outputs here
		
		//compute the dot between the look vector and vector to the closest mine:
		double dot_mine = dot_between_vlook_and_vObject(*s, *m_vecObjects[s->getClosestMine()]);
		//double dot_rock = dot_between_vlook_and_vObject(*s, *m_vecObjects[s->getClosestRock()]);
		double dot_supermine = dot_between_vlook_and_vObject(*s, *m_vecObjects[s->getClosestSupermine()]);
		double dist_mine = Vec2DLength(m_vecObjects[s->getClosestMine()]->getPosition() - s->Position());
		//double dist_rock = Vec2DLength(m_vecObjects[s->getClosestRock()]->getPosition() - s->Position());
		double dist_supermine = Vec2DLength(m_vecObjects[s->getClosestSupermine()]->getPosition() - s->Position());


		double minDanger;
		if ( dist_supermine < dist_mine){	//dist to mine or supermine < dist to mine
			s->setSpeed(dist_supermine, 1);
		}
		else{ s->setSpeed(dist_mine, 10); }
		//(*s)->setSpeed(min(dist_mine/2, min(dist_rock, dist_supermine)));

		double dotSuperMineOrRock = (dist_supermine < 25) ? dot_supermine : -1;

		//Clamp(dot_supermine, 0, 1);

		//if (m_iIterations > 0){ std::cout << "iteration 1 got values" << std::endl; }
		//cheat a bit here... passing the distance into the neural net as well increases the search space dramatrically... :
		std:vector<double> dots = { dot_mine, dotSuperMineOrRock };

		if (s->getNeuralNet().classify(dots) == 0){ // turn towards the mine
			SPoint pt(m_vecObjects[s->getClosestMine()]->getPosition().x,
				m_vecObjects[s->getClosestMine()]->getPosition().y);
			s->turn(pt, 1);
		}
		else if (s->getNeuralNet().classify(dots) == 1) {//turn away from a rock or supermine
			SPoint pt(m_vecObjects[s->getClosestSupermine()]->getPosition().x,
				m_vecObjects[s->getClosestSupermine()]->getPosition().y);
			s->turn(pt, 1, false);
		}
		else{
			//Do nothining
		}
		
	}

	return true; //method returns true if successful. Do not delete this.
}

CEAController::~CEAController(void)
{
	//TODO:: release any allocations here
}

void CEAController::selection(){
	std::cout << "inside selection" << std::endl;
	vector<SelectionPair> currentFitness;

	for (int i = 0; i < m_vecSweepers.size(); ++i){
		currentFitness.push_back(SelectionPair(i, fitness(*m_vecSweepers[i])));
		if (fitness(*m_vecSweepers[i]) < 100 && m_vecSweepers[i]->MinesGathered() == 0){
			++floatersThisRound;
		}
	}

	std::sort(currentFitness.begin(), currentFitness.end());
	std::reverse(currentFitness.begin(), currentFitness.end());

	for (int i = 0; i < currentFitness.size(); ++i){
		std::cout << currentFitness[i].id << ", " << currentFitness[i].fitness << std::endl;
	}
	std::cout << std::endl;

	vector<vector<double>> offspringGenomes;
	vector<vector<double>> tempOffspringGenomes;

	for (int i = 0; i < RECOMBINATION_COUNT; i += 2){
		std::cout << "getting sweeper " << currentFitness[i].id << " & " << currentFitness[i + 1].id << std::endl;
		CContMinesweeper *s1 = m_vecSweepers[currentFitness[i].id];
		CContMinesweeper *s2 = m_vecSweepers[currentFitness[i+1].id];
		tempOffspringGenomes = crossover(*s1, *s2);
		for (int j = 0; j < tempOffspringGenomes.size(); ++j){
			/*if (i == 0 && j==0){
				std::cout << "weights before " << std::endl;
				for (int k = 0; k < tempOffspringGenomes[j].size(); ++k){
					std::cout << tempOffspringGenomes[j][k] << ", ";
				}
				std::cout << std::endl;
			}*/
			mutate(tempOffspringGenomes[j]);

			/*if (i == 0 && j == 0){
				std::cout << "weights after " << std::endl;
				for (int k = 0; k < tempOffspringGenomes[j].size(); ++k){
				std::cout << tempOffspringGenomes[j][k] << ", ";
				}
				std::cout << std::endl;
			}*/

			offspringGenomes.push_back(tempOffspringGenomes[j]);
		}
		//tempOffspringGenomes.clear();
	}

	uint index = currentFitness.size() - 1;
	for (int i = 0; i < RECOMBINATION_COUNT; ++i){
		int sweeperIndex = currentFitness[index].id;
		std::cout << "replacing sweeper " << sweeperIndex << std::endl;
		m_vecSweepers[sweeperIndex]->setGenome(offspringGenomes[i]);
		--index;
	}
	floatersThisRound = 0;
	return;
}

void CEAController::mutate(vector<double> &s){
	std::cout << "floaters = " << floatersThisRound << std::endl;
	int numberOfMutations = ((m_vecSweepers[0]->genomeSize) / 2 * deathsThisRound / m_vecSweepers.size())+(floatersThisRound/8);
	std::cout << "numberOfMutations = " << numberOfMutations << std::endl;

	std::set<int> mutatedWeights;

	int count = 0;
	while (count < numberOfMutations){
		int index = RandInt(0, (m_vecSweepers[0]->genomeSize) - 1);
		if (mutatedWeights.find(index) == mutatedWeights.end()){
			double deltaWeight = RandomClamped() / 10;
			Clamp(deltaWeight, -1, 1);
			//std::cout << "mutatuing gene " << index << " from " << s[index] << " => " << s[index]+deltaWeight << std::endl;
			s[index] = RandomClamped(); //+= deltaWeight;
			mutatedWeights.insert(index);
			++count;
		}
	}
	return;
}

vector<vector<double>> CEAController::crossover(const CContMinesweeper &s1, const CContMinesweeper &s2){
	vector<vector<double>> offspringGenomes;

	vector<double> genome1 = s1.getGenome();
	vector<double> genome2 = s2.getGenome();

	int crossoverPoint = RandInt(1, s1.genomeSize - 2);
	std::cout << "crossoverPoint = " << crossoverPoint << std::endl;

	vector<double> offspringGenome1;
	vector<double> offspringGenome2;

	for (int i = 0; i < s1.genomeSize; ++i){
		if (i < crossoverPoint){
			offspringGenome1.push_back(genome1[i]);
			offspringGenome2.push_back(genome2[i]);
		}else{
			offspringGenome1.push_back(genome2[i]);
			offspringGenome2.push_back(genome1[i]);
		}
	}

	offspringGenomes.push_back(offspringGenome1);
	offspringGenomes.push_back(offspringGenome2);

	return offspringGenomes;
}

double CEAController::fitness(const CContMinesweeper &s){
	double liveScore = s.getNumTicksAlive()*0.1f;
	return 400 * s.MinesGathered() + liveScore;
}
