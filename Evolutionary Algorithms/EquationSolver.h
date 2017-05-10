#pragma once

#include <vector>
#include <random>

struct Individual
{
	int Genes[4];
	bool Usable;
	int Quality;

	Individual& operator= (const Individual& other)
	{
		for (int i = 0; i < 4; ++i)
			Genes[i] = other.Genes[i];
		Usable = other.Usable;
		Quality = other.Quality;
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
	bool hasCommandLineInputError();
	void showUsage(char* appExecutionPath) const;

	void initializeRandomGenerators();

	int getDiffenceOfEvolutionStrategyEquation(const int& x, const int& y, const int& a, const int& b) const;
	bool isEvolutionStrategyCondition1Fulfilled(const Individual& individual) const;
	static bool isEvolutionStrategyCondition2Fulfilled(const Individual& individual);
		
	void printSolution(const Individual& solution, const int& iterationCounter, const int& deathCounter) const;
	static bool saveToFile(const std::vector<int>& qualityOverIterations, const std::string& path);

	void onePlusOneEvolutionStrategy();
	void muPlusLambdaEvolutionStrategy();
	bool foundSolution(const Individual* individualsArray, const size_t& amount, Individual& outSolution) const;
	void muCommaLambdaEvolutionStrategy();
	void muSlashRohSharpLambdaEvolutionStrategy();

	EquationSolverStrategy m_strategy = None;
	int m_individualRandomRange[2] = { 0,0 }; 
	int m_mutationRandomRange[2] = { 0,0 };
	int m_mu = 0;
	int m_lambda = 0;
	int m_roh = 0;

	std::random_device m_randomDevice; 
	std::mt19937_64 m_mersenneTwisterEngine;
	std::uniform_int_distribution<> m_randomIndividualDistribution;
	std::uniform_int_distribution<> m_randomMutationDistribution;

	const struct CmdIdentifier
	{
		const char* strategy = "--strategy";
		const char* onePlusOne = "(1+1)";
		const char* muPlusLambda = "(m+l)";
		const char* muCommaLambda = "(m,l)";
		const char* muSlashRohSharpLambda = "(m/r#l)";
		const char* individualRandomRange = "--i-range";
		const char* mutationRandomRange = "--m-range";
		const char* mu = "--mu";
		const char* lambda = "--lambda";
		const char* roh = "--roh";
	} CMD_IDs;
};

