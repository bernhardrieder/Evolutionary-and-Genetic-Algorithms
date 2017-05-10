﻿#include "EquationSolver.h"
#include <iostream>
#include <fstream>
#include <random>
#include <string>

EquationSolver::EquationSolver()
{
	m_mersenneTwisterEngine = std::mt19937_64(m_randomDevice());
}

EquationSolver::~EquationSolver()
{
}

int EquationSolver::Execute(int argc, char** argv)
{
	if (!parseCommandLine(argc, argv))
		return 0;

	initializeRandomGenerators();

	switch (m_strategy)
	{
	default:
	case OnePlusOne: onePlusOneEvolutionStrategy();
		break;
	case muPlusLambda: break;
	case muCommaLambda: break;
	case muSlashRohSharpLambda: break;
	}

	getchar();
	return 1;
}

bool EquationSolver::parseCommandLine(int argc, char** argv)
{
	const unsigned requiredArgv = 8; //at least for (1+1)
	if (argc != requiredArgv + 1)
	{
		showUsage(argv[0]);
		return false;
	}

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg == "-h" || arg == "--help")
		{
			showUsage(argv[0]);
			return false;
		}

		if (arg == CMD_IDs.strategy && i + 1 < argc)
		{
			std::string strategy = argv[++i];
			if (strategy == CMD_IDs.onePlusOne)
				m_strategy = OnePlusOne;
			else if (strategy == CMD_IDs.muPlusLambda)
				m_strategy = muPlusLambda;
			else if (strategy == CMD_IDs.muCommaLambda)
				m_strategy = muCommaLambda;
			else if (strategy == CMD_IDs.muSlashRohSharpLambda)
				m_strategy = muSlashRohSharpLambda;
		}

		if (arg == CMD_IDs.individualRandomRange && i + 2 < argc)
		{
			std::string min = argv[++i], max = argv[++i];
			m_individualRandomRange[0] = std::stoi(min);
			m_individualRandomRange[1] = std::stoi(max);
		}

		if (arg == CMD_IDs.mutationRandomRange && i + 2 < argc)
		{
			std::string min = argv[++i], max = argv[++i];
			m_mutationRandomRange[0] = std::stoi(min);
			m_mutationRandomRange[1] = std::stoi(max);
		}

		if (arg == CMD_IDs.mu && i + 1 < argc)
			m_mu = std::stoi(argv[++i]);

		if (arg == CMD_IDs.lambda && i + 1 < argc)
			m_lambda = std::stoi(argv[++i]);

		if (arg == CMD_IDs.roh && i + 1 < argc)
			m_roh = std::stoi(argv[++i]);
	}

	if (hasCommandLineInputError())
	{
		std::cerr << "ERROR: Input Error!\n";
		return false;
	}

	return true;
}

bool EquationSolver::hasCommandLineInputError()
{
	if (m_strategy == None)
		return true;
	if (m_individualRandomRange[0] == 0 && m_individualRandomRange[1] == 0)
		return true;
	if (m_mutationRandomRange[0] == 0 && m_mutationRandomRange[1] == 0)
		return true;
	if((m_strategy == muCommaLambda || m_strategy == muPlusLambda || m_strategy == muSlashRohSharpLambda) 
		&& (m_mu == 0 || m_lambda == 0))
			return true;
	if (m_strategy == muSlashRohSharpLambda 
		&& m_roh == 0)
		return true;
	return false;
}

void EquationSolver::showUsage(char* appExecutionPath) const
{
	std::cout
		<< "Usage: " << appExecutionPath << " <option(s)>\n\n"
		<< "Options:\n"
		<< "\t-h,--help\tShow this help message.\n"
		<< "\t" << CMD_IDs.strategy << " <ES>\tchoose a Evolution Strategy listed below!\n"

		<< "\nEvolution Strategies:\n"
		<< "\t" << CMD_IDs.onePlusOne << "\tExecutes solver with (1+1) Evolution Strategy with given random parameter.\n"
		<< "\t" << CMD_IDs.muPlusLambda << "\tExecutes solver with (mu+lambda) Evolution Strategy with given random parameter.\n"
		<< "\t" << CMD_IDs.muCommaLambda << "\tExecutes solver with (mu,lambda) Evolution Strategy with given random parameter.\n"
		<< "\t" << CMD_IDs.muSlashRohSharpLambda << "\tExecutes solver with (mu/roh#lambda) Evolution Strategy with given random parameter.\n"

		<< "\nGeneral Strategy Parameters:\n"
		<< "\t" << CMD_IDs.individualRandomRange << " <MIN> <MAX>\tIndividual self-replication random range.\n"
		<< "\t" << CMD_IDs.mutationRandomRange << " <MIN> <MAX>\tMutation random range.\n"

		<< "\n" << CMD_IDs.muPlusLambda << " & " << CMD_IDs.muCommaLambda << " specific Parameter:\n"
		<< "\t" << CMD_IDs.mu << " <NUM>\t\tDefines amount of parents. Needs to be a positive number!\n"
		<< "\t" << CMD_IDs.lambda << " <NUM>\t\tDefines amount of children. Needs to be a positive number!\n"

		<< "\n" << CMD_IDs.muSlashRohSharpLambda << " specific Parameter:\n"
		<< "\t" << CMD_IDs.mu << ", " << CMD_IDs.lambda << "\t==> same as above!\n"
		<< "\t" << CMD_IDs.roh << " <NUM>\tNO IDEA!" //TODO
		<< std::endl;
}

void EquationSolver::initializeRandomGenerators()
{
	m_randomIndividualDistribution = std::uniform_int_distribution<>(m_individualRandomRange[0], m_individualRandomRange[1]);
	m_randomMutationDistribution = std::uniform_int_distribution<>(m_mutationRandomRange[0], m_mutationRandomRange[1]);
}

int EquationSolver::getDiffenceOfEvolutionStrategyEquation(const int& x, const int& y, const int& a, const int& b) const
{
	int left = 3 * std::pow(x, 2) + 5 * std::pow(y, 3);
	int right = 7 * a + 3 * std::pow(b, 2);
	return std::abs(left - right); // difference
}

bool EquationSolver::isEvolutionStrategyCondition1Fulfilled(const Individual& individual) const
{
	return getDiffenceOfEvolutionStrategyEquation(individual.Genes[0], individual.Genes[1], individual.Genes[2], individual.Genes[3]) == 0;
}

bool EquationSolver::isEvolutionStrategyCondition2Fulfilled(const Individual& individual)
{
	return individual.Genes[2] > individual.Genes[3];
}

void EquationSolver::printSolution(const Individual& solution, const int& iterationCounter, const int& deathCounter) const
{
	int x = solution.Genes[0], y = solution.Genes[1], a = solution.Genes[2], b = solution.Genes[3];

	std::cout
		<< "\n############ SOLUTION ############\n"
		<< "(<x,y,a,b>) ==> (<" << x << ", " << y << ", " << a << ", " << b << ">)" << std::endl
		<< "\n############ EQUATION & CONDITION 1 ############\n"
		<< "3x^2 + 5y^3 = 7a + 3b^2" << std::endl
		<< "3 * " << x << "^2 + 5 * " << y << "^3 = 7 * " << a << " + 3 " << b << "^2" << std::endl
		<< 3 * std::pow(x, 2) + 5 * std::pow(y, 3) << " = " << 7 * a + 3 * std::pow(b, 2) << std::endl
		<< "\n############ CONDITION 2 ############\n"
		<< "a > b ==> " << a << " > " << b << std::endl
		<< "\n############ SUMMARY ############\n"
		<< "It took " << iterationCounter << " iterations and " << deathCounter << " death" << ((deathCounter > 0) ? "s" : "") << "!" << std::endl
		<< "Individual self replication random range: (" << m_individualRandomRange[0] << ", " << m_individualRandomRange[1] << ")" << std::endl
		<< "Mutation random range: (" << m_mutationRandomRange[0] << ", " << m_mutationRandomRange[1] << ")" << std::endl;
}

bool EquationSolver::saveToFile(const std::vector<int>& qualityOverIterations, const std::string& path)
{
	std::ofstream outputFile;
	outputFile.open(path);
	if (outputFile.is_open())
	{
		outputFile << "Iteration" << ";" << "Quality" << "\n";
		for (int i = 0; i < qualityOverIterations.size(); ++i)
		{
			outputFile << i << ";" << qualityOverIterations[i] << "\n";
		}
		outputFile.close();
		return true;
	}

	std::cout << "ERROR: Could not save file '" << path.c_str() << "'!\n";
	return false;
}

void EquationSolver::onePlusOneEvolutionStrategy()
{
	Individual individual, solution;
	size_t sizeOfGenes = _countof(individual.Genes);

	int deathCounter = 0;
	int iterationCounter = 0;
	int maxIterations = 500000;

	std::vector<int> qualityOverIterations;
	qualityOverIterations.reserve(maxIterations);

	bool initNewIndividual = true;

	while (iterationCounter <= maxIterations)
	{
		++iterationCounter;
		/* -------------------------------------- SELF-REPLICATION -------------------------------------- */
		if (initNewIndividual)
		{
			for (int i = 0; i < sizeOfGenes; ++i)
				individual.Genes[i] = m_randomIndividualDistribution(m_mersenneTwisterEngine);
			initNewIndividual = false;
		}

		Individual mutation = individual;

		/* -------------------------------------- RANDOM MUTATION -------------------------------------- */
		for (int i = 0; i < sizeOfGenes; ++i)
			mutation.Genes[i] += m_randomMutationDistribution(m_mersenneTwisterEngine);


		/* -------------------------------------- SELECTION -------------------------------------- */
		bool individualUsable = isEvolutionStrategyCondition2Fulfilled(individual);
		bool mutationUsable = isEvolutionStrategyCondition2Fulfilled(mutation);

		//both are shit!
		if (!individualUsable && !mutationUsable)
		{
			initNewIndividual = true;
			++deathCounter;
			continue;
		}

		//is individual a solution?
		if (individualUsable && isEvolutionStrategyCondition1Fulfilled(individual))
		{
			solution = individual;
			break;
		}

		//is mutation a solution?
		if (mutationUsable && isEvolutionStrategyCondition1Fulfilled(mutation))
		{
			solution = mutation;
			break;
		}

		//is just individual usable? -> go ahead
		if (individualUsable && !mutationUsable)
			continue;

		//is just mutation usable -> use mutation as new individual
		if (mutationUsable && !individualUsable)
		{
			individual = mutation;
			continue;
		}

		if (individualUsable || mutationUsable)
		{
			//determine better
			int individualQuality = getDiffenceOfEvolutionStrategyEquation(individual.Genes[0], individual.Genes[1], individual.Genes[2], individual.Genes[3]);
			int mutationQuality = getDiffenceOfEvolutionStrategyEquation(mutation.Genes[0], mutation.Genes[1], mutation.Genes[2], mutation.Genes[3]);

			//save current quality level
			qualityOverIterations.push_back(individualQuality);

			//if mutation is better
			if (mutationQuality < individualQuality)
				individual = mutation;
		}
	}
	qualityOverIterations.shrink_to_fit();

	if (iterationCounter < maxIterations)
		printSolution(solution, iterationCounter, deathCounter);
	else
		std::cout << "Exceeded maximum iterations of " << maxIterations << "! -> Couldn't find a solution!\n";
	saveToFile(qualityOverIterations, "1+1.csv");
}
