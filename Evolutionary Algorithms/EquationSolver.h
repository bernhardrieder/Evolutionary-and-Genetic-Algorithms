#pragma once

#include <vector>
#include <random>

struct Individual
{
	int Genes[4];

	Individual& operator= (const Individual& other)
	{
		for (int i = 0; i < 4; ++i)
			Genes[i] = other.Genes[i];
		return *this;
	}
};

class EquationSolver
{
	enum EquationSolverStrategy
	{
		None,
		OnePlusOne,
		muPlusLambda,
		muCommaLambda,
		muSlashRohSharpLambda
	};
public:
	EquationSolver();
	~EquationSolver();

	int Execute(int argc, char** argv);

private:
	bool parseCommandLine(int argc, char** argv);
	static void showUsage(char* appExecutionPath);

	void initializeRandomGenerators();

	int getDiffenceOfEvolutionStrategyEquation(const int& x, const int& y, const int& a, const int& b) const;
	bool isEvolutionStrategyCondition1Fulfilled(const Individual& individual) const;
	static bool isEvolutionStrategyCondition2Fulfilled(const Individual& individual);
		
	void printSolution(const Individual& solution, const int& iterationCounter, const int& deathCounter) const;
	static bool saveToFile(const std::vector<int>& qualityOverIterations, const std::string& path);

	void onePlusOneEvolutionStrategy();

	EquationSolverStrategy m_strategy;
	int m_individualRandomRange[2]; 
	int m_mutationRandomRange[2];

	std::random_device m_randomDevice; 
	std::mt19937_64 m_mersenneTwisterEngine;
	std::uniform_int_distribution<> m_randomIndividualDistribution;
	std::uniform_int_distribution<> m_randomMutationDistribution;
};

