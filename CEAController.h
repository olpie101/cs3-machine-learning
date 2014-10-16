#pragma once
#include "ccontcontroller.h"
#include <vector>
#include <algorithm>
#include <set>
#include "CNeuralNet.h"
#include "CParams.h"
#include "CBasicEA.h"
#define RECOMBINATION_COUNT 4 //Number of parents to recombine

class CEAController :
	public CContController
{
	/*typedef pair<int, double> SelectionPair;

	struct less_than_fitness{
		inline bool operator() (const SelectionPair sp1, const SelectionPair sp2){
			return (sp1.second < sp2.second);
		}
	};*/

	struct SelectionPair{
		int id;
		double fitness;

		SelectionPair(int i, double fit) : id(i), fitness(fit){}

		bool operator < (const SelectionPair& other) const{
			return (fitness < other.fitness);
		}
	};
protected:
	bool goneThroughSelection = false;
	int deathsThisRound;

public:
	CEAController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	virtual bool Update(void);
	virtual ~CEAController(void);
	void selection();
	void mutate(vector<double> &s);
	vector<vector<double>> crossover(const CContMinesweeper &s1, const CContMinesweeper &s2);
	double fitness(const CContMinesweeper &s);
};

