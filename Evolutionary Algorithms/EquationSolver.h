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

	void Reset()
	{
		for (int i = 0; i < 4; ++i)
			Genes[i] = 0;
		Usable = false;
		Quality = -1;
	}
};
namespace EquationSolverStrategy
{
	enum Enum
	{
		None,
		OnePlusOne,
		muPlusLambda,
		muCommaLambda,
		muSlashRohPlusLambda,
		muSlashRohCommaLambda
	};
}
namespace MuSlashRohSharpLambdaRecombination
{
	enum Enum
	{
		None,
		Melting,
		Combination
	};
}
class EquationSolver
{
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
	void muSlashRohSharpLambdaEvolutionStrategy(EquationSolverStrategy::Enum strategy);
	void muSlashRohSharpLambaRecombination( std::vector<Individual>& parents, Individual& outChild, const std::uniform_int_distribution<>& randomParentDistribution, const std::uniform_int_distribution<>& randomGeneDistribution);
	bool checkConditionsAndReturnTrueIfSolutionFound(std::vector<Individual>& inOutIndividuals, Individual& outSolution) const;
	void plusSelection(std::vector<Individual>& inOutIndividuals, std::vector<Individual>& inOutParents, std::vector<int>& inOutQualityOverIterations);
	void commaSelection(std::vector<Individual>& inOutParents, std::vector<Individual>& inOutChildren, const std::vector<Individual>& individuals, std::vector<int>& inOutQualityOverIterations);

	EquationSolverStrategy::Enum m_strategy = EquationSolverStrategy::None;
	int m_individualRandomRange[2] = { 0,0 }; 
	int m_mutationRandomRange[2] = { 0,0 };
	int m_mu = 0;
	int m_lambda = 0;
	int m_roh = 0;
	MuSlashRohSharpLambdaRecombination::Enum m_muSlashRohSharpLambdaRecombination = MuSlashRohSharpLambdaRecombination::None;

	std::random_device m_randomDevice; 
	std::mt19937_64 m_mersenneTwisterEngine;
	std::uniform_int_distribution<> m_randomIndividualDistribution;
	std::uniform_int_distribution<> m_randomMutationDistribution;

	const struct CmdIdentifier
	{
		const char* Strategy = "--strategy";
		const char* OnePlusOne = "(1+1)";
		const char* MuPlusLambda = "(m+l)";
		const char* MuCommaLambda = "(m,l)";
		const char* MuSlashRohPlusLambda = "(m/r+l)";
		const char* MuSlashRohCommaLambda = "(m/r,l)";
		const char* IndividualRandomRange = "--i-range";
		const char* MutationRandomRange = "--m-range";
		const char* Mu = "--mu";
		const char* Lambda = "--lambda";
		const char* Roh = "--roh";
		const char* Melting = "--melting";
		const char* Combination = "--combination";
	} CMD_IDs;
};

