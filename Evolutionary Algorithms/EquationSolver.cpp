#include "EquationSolver.h"
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <map>
#include <algorithm>

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
	case EquationSolverStrategy::OnePlusOne: onePlusOneEvolutionStrategy();	break;
	case EquationSolverStrategy::muPlusLambda: muPlusLambdaEvolutionStrategy(); break;
	case EquationSolverStrategy::muCommaLambda: muCommaLambdaEvolutionStrategy(); break;
	case EquationSolverStrategy::muSlashRohPlusLambda: muSlashRohSharpLambdaEvolutionStrategy(EquationSolverStrategy::muSlashRohPlusLambda); break;
	case EquationSolverStrategy::muSlashRohCommaLambda: muSlashRohSharpLambdaEvolutionStrategy(EquationSolverStrategy::muSlashRohCommaLambda); break;
	}

	return 1;
}

bool EquationSolver::parseCommandLine(int argc, char** argv)
{
	const unsigned requiredArgv = 8; //at least for (1+1)
	if (argc < requiredArgv + 1)
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

		if (arg == CMD_IDs.Strategy && i + 1 < argc)
		{
			std::string strategy = argv[++i];
			if (strategy == CMD_IDs.OnePlusOne)
				m_strategy = EquationSolverStrategy::OnePlusOne;
			else if (strategy == CMD_IDs.MuPlusLambda)
				m_strategy = EquationSolverStrategy::muPlusLambda;
			else if (strategy == CMD_IDs.MuCommaLambda)
				m_strategy = EquationSolverStrategy::muCommaLambda;
			else if (strategy == CMD_IDs.MuSlashRohPlusLambda)
				m_strategy = EquationSolverStrategy::muSlashRohPlusLambda;
			else if (strategy == CMD_IDs.MuSlashRohCommaLambda)
				m_strategy = EquationSolverStrategy::muSlashRohCommaLambda;
		}

		if (arg == CMD_IDs.IndividualRandomRange && i + 2 < argc)
		{
			std::string min = argv[++i], max = argv[++i];
			m_individualRandomRange[0] = std::stoi(min);
			m_individualRandomRange[1] = std::stoi(max);
		}

		if (arg == CMD_IDs.MutationRandomRange && i + 2 < argc)
		{
			std::string min = argv[++i], max = argv[++i];
			m_mutationRandomRange[0] = std::stoi(min);
			m_mutationRandomRange[1] = std::stoi(max);
		}

		if (arg == CMD_IDs.Mu && i + 1 < argc)
			m_mu = std::stoi(argv[++i]);

		if (arg == CMD_IDs.Lambda && i + 1 < argc)
			m_lambda = std::stoi(argv[++i]);

		if (arg == CMD_IDs.Roh && i + 1 < argc)
			m_roh = std::stoi(argv[++i]);

		if (arg == CMD_IDs.Combination)
			m_muSlashRohSharpLambdaRecombination = MuSlashRohSharpLambdaRecombination::Combination;

		if (arg == CMD_IDs.Melting)
			m_muSlashRohSharpLambdaRecombination = MuSlashRohSharpLambdaRecombination::Melting;
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
	if (m_strategy == EquationSolverStrategy::None)
		return true;
	if ((m_individualRandomRange[0] == 0 && m_individualRandomRange[1] == 0) || m_individualRandomRange[0] > m_individualRandomRange[1])
		return true;
	if ((m_mutationRandomRange[0] == 0 && m_mutationRandomRange[1] == 0) || m_mutationRandomRange[0] > m_mutationRandomRange[1])
		return true;
	if((m_strategy == EquationSolverStrategy::muCommaLambda || m_strategy == EquationSolverStrategy::muPlusLambda || m_strategy == EquationSolverStrategy::muSlashRohPlusLambda || m_strategy == EquationSolverStrategy::muSlashRohCommaLambda)
		&& (m_mu <= 0 || m_lambda <= 0))
			return true;
	if ((m_strategy == EquationSolverStrategy::muSlashRohPlusLambda || m_strategy == EquationSolverStrategy::muSlashRohCommaLambda)
		&& (m_roh <= 0 || m_muSlashRohSharpLambdaRecombination == MuSlashRohSharpLambdaRecombination::None))
		return true;
	return false;
}

void EquationSolver::showUsage(char* appExecutionPath) const
{
	std::cout
		<< "Usage: " << appExecutionPath << " <option(s)>\n\n"
		<< "Options:\n"
		<< "\t-h,--help\tShow this help message.\n"
		<< "\t" << CMD_IDs.Strategy << " <ES>\tchoose a Evolution Strategy listed below!\n"

		<< "\nEvolution Strategies:\n"
		<< "\t" << CMD_IDs.OnePlusOne << "\tExecutes solver with (1+1) Evolution Strategy with given random parameter.\n"
		<< "\t" << CMD_IDs.MuPlusLambda << "\tExecutes solver with (mu+lambda) Evolution Strategy with given random parameter.\n"
		<< "\t" << CMD_IDs.MuCommaLambda << "\tExecutes solver with (mu,lambda) Evolution Strategy with given random parameter.\n"
		<< "\t" << CMD_IDs.MuSlashRohPlusLambda << "\tExecutes solver with (mu/roh+lambda) Evolution Strategy with given random parameter.\n"
		<< "\t" << CMD_IDs.MuSlashRohCommaLambda << "\tExecutes solver with (mu/roh,lambda) Evolution Strategy with given random parameter.\n"

		<< "\nGeneral Strategy Parameters:\n"
		<< "\t" << CMD_IDs.IndividualRandomRange << " <MIN> <MAX>\tIndividual self-replication random range.\n"
		<< "\t" << CMD_IDs.MutationRandomRange << " <MIN> <MAX>\tMutation random range.\n"

		<< "\n" << CMD_IDs.MuPlusLambda << " & " << CMD_IDs.MuCommaLambda << " specific Parameter:\n"
		<< "\t" << CMD_IDs.Mu << " <NUM>\t\tDefines amount of parents. Needs to be a positive number!\n"
		<< "\t" << CMD_IDs.Lambda << " <NUM>\t\tDefines amount of children. Needs to be a positive number!\n"

		<< "\n" << CMD_IDs.MuSlashRohPlusLambda << " & " << CMD_IDs.MuSlashRohCommaLambda << " specific Parameter:\n"
		<< "\t" << CMD_IDs.Mu << ", " << CMD_IDs.Lambda << "\t==> same as above!\n"
		<< "\t" << CMD_IDs.Roh << " <NUM>\tNumber of parent individuals.\n"
		<< "\t" << CMD_IDs.Melting << "\tRecombination function 'Melting' -> average of parent genes.\n"
		<< "\t" << CMD_IDs.Combination << "\tRecombination function 'Combination' -> random gene selection."
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
		<< "############ SOLUTION ############\n"
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

	int iterationCounter = 0;
	int maxIterations = 1000000;

	std::vector<int> qualityOverIterations;
	qualityOverIterations.reserve(maxIterations);

	/* -------------------------------------- RANDOM START INDIVIDUAL -------------------------------------- */
	for (int i = 0; i < sizeOfGenes; ++i)
		individual.Genes[i] = m_randomIndividualDistribution(m_mersenneTwisterEngine);

	/* -------------------------------------- START EVOLUTION -------------------------------------- */
	for (; iterationCounter <= maxIterations; ++iterationCounter)
	{
		/* -------------------------------------- SELF-REPLICATION -------------------------------------- */
		Individual mutation = individual;

		/* -------------------------------------- RANDOM MUTATION -------------------------------------- */
		for (int i = 0; i < sizeOfGenes; ++i)
			mutation.Genes[i] += m_randomMutationDistribution(m_mersenneTwisterEngine);

		/* -------------------------------------- SELECTION -------------------------------------- */
		individual.Usable = isEvolutionStrategyCondition2Fulfilled(individual);
		mutation.Usable = isEvolutionStrategyCondition2Fulfilled(mutation);

		//is one of the individuals a solution?
		if (foundSolution(&individual, 1, solution) || foundSolution(&mutation, 1, solution))
			break;

		//is just individual usable?
		if (individual.Usable && !mutation.Usable)
			continue;

		//is just mutation usable -> use mutation as new individual
		if (mutation.Usable && !individual.Usable)
		{
			individual = mutation;
			continue;
		}

		//determine better
		individual.Quality = individual.Usable ? getDiffenceOfEvolutionStrategyEquation(individual.Genes[0], individual.Genes[1], individual.Genes[2], individual.Genes[3]) : std::numeric_limits<int>::max();
		mutation.Quality = mutation.Usable ? getDiffenceOfEvolutionStrategyEquation(mutation.Genes[0], mutation.Genes[1], mutation.Genes[2], mutation.Genes[3]) : std::numeric_limits<int>::max();

		//if mutation is better
		if (mutation.Quality < individual.Quality)
			individual = mutation;

		//save new best quality level
		qualityOverIterations.push_back(individual.Quality);
	}

	qualityOverIterations.push_back(0);
	qualityOverIterations.shrink_to_fit();

	if (iterationCounter < maxIterations)
		printSolution(solution, iterationCounter, 0);
	else
		std::cout << "Exceeded maximum iterations of " << maxIterations << "! -> Couldn't find a solution!\n";
	saveToFile(qualityOverIterations, "1+1.csv");
}

void EquationSolver::muPlusLambdaEvolutionStrategy()
{
	Individual solution;
	size_t sizeOfGenes = _countof(solution.Genes);

	std::vector<Individual> parents, children, individuals;
	parents.resize(m_mu);
	children.resize(m_lambda); 
	individuals.resize(m_mu + m_lambda);

	std::uniform_int_distribution<> randomParentDistribution(0, m_mu - 1);

	int iterationCounter = 0;
	int maxIterations = 500000;

	std::vector<int> qualityOverIterations;
	qualityOverIterations.reserve(maxIterations);

	/* -------------------------------------- RANDOM START PARENTS -------------------------------------- */
	for (int i = 0; i < parents.size(); ++i)
		for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
			parents[i].Genes[geneIndex] = m_randomIndividualDistribution(m_mersenneTwisterEngine);

	/* -------------------------------------- START EVOLUTION -------------------------------------- */
	for(; iterationCounter <= maxIterations; ++iterationCounter)
	{
		//add all new parents to a summary array
		for(int i = 0; i < parents.size(); ++i)
			individuals[i] = parents[i];

		for(int i = 0; i < children.size(); ++i)
		{
			/* -------------------------------------- SELF-REPLICATION -------------------------------------- */
			children[i] = parents[randomParentDistribution(m_mersenneTwisterEngine)];

			/* -------------------------------------- RANDOM MUTATION -------------------------------------- */
			for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
				children[i].Genes[geneIndex] += m_randomMutationDistribution(m_mersenneTwisterEngine);

			//add to a summary array
			individuals[parents.size()+ i] = children[i];
		}

		/* -------------------------------------- SELECTION -------------------------------------- */
		if (checkConditionsAndReturnTrueIfSolutionFound(individuals, solution))
			break;

		plusSelection(individuals, parents, qualityOverIterations);
	}

	qualityOverIterations.push_back(0);
	qualityOverIterations.shrink_to_fit();

	if (iterationCounter < maxIterations)
		printSolution(solution, iterationCounter, 0);
	else
		std::cout << "Exceeded maximum iterations of " << maxIterations << "! -> Couldn't find a solution!\n";
	saveToFile(qualityOverIterations, "mu+lambda.csv");
}

bool EquationSolver::foundSolution(const Individual* individualsArray, const size_t& amount, Individual& outSolution) const
{
	for (int i = 0; i < amount; ++i)
	{
		if (individualsArray[i].Usable && isEvolutionStrategyCondition1Fulfilled(individualsArray[i]))
		{
			outSolution = individualsArray[i];
			return true;
		}
	}
	return false;
}

void EquationSolver::muCommaLambdaEvolutionStrategy()
{
	Individual solution;
	size_t sizeOfGenes = _countof(solution.Genes);

	std::vector<Individual> parents, children, individuals;
	parents.resize(m_mu);
	children.resize(m_lambda);
	individuals.resize(m_mu + m_lambda);

	std::uniform_int_distribution<> randomParentDistribution(0, m_mu - 1);

	int iterationCounter = 0;
	int maxIterations = 500000;

	std::vector<int> qualityOverIterations;
	qualityOverIterations.reserve(maxIterations);

	/* -------------------------------------- RANDOM START PARENTS -------------------------------------- */
	for (int i = 0; i < parents.size(); ++i)
		for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
			parents[i].Genes[geneIndex] = m_randomIndividualDistribution(m_mersenneTwisterEngine);

	/* -------------------------------------- START EVOLUTION -------------------------------------- */
	for (; iterationCounter <= maxIterations; ++iterationCounter)
	{
		//add all new parents to a summary array
		for (int i = 0; i < parents.size(); ++i)
			individuals[i] = parents[i];

		for (int i = 0; i < children.size(); ++i)
		{
			/* -------------------------------------- SELF-REPLICATION -------------------------------------- */
			children[i] = parents[randomParentDistribution(m_mersenneTwisterEngine)];

			/* -------------------------------------- RANDOM MUTATION -------------------------------------- */
			for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
				children[i].Genes[geneIndex] += m_randomMutationDistribution(m_mersenneTwisterEngine);

			//add to a summary array
			individuals[parents.size() + i] = children[i];
		}

		/* -------------------------------------- SELECTION -------------------------------------- */
		if (checkConditionsAndReturnTrueIfSolutionFound(individuals, solution))
			break;

		commaSelection(parents, children, individuals, qualityOverIterations);
	}

	qualityOverIterations.push_back(0);
	qualityOverIterations.shrink_to_fit();

	if (iterationCounter < maxIterations)
		printSolution(solution, iterationCounter, 0);
	else
		std::cout << "Exceeded maximum iterations of " << maxIterations << "! -> Couldn't find a solution!\n";
	saveToFile(qualityOverIterations, "mu,lambda.csv");

}

void EquationSolver::muSlashRohSharpLambdaEvolutionStrategy(EquationSolverStrategy::Enum strategy)
{
	Individual solution;
	const size_t sizeOfGenes = _countof(solution.Genes);

	std::vector<Individual> parents, children, individuals;
	parents.resize(m_mu);
	children.resize(m_lambda);
	individuals.resize(m_mu + m_lambda);

	std::uniform_int_distribution<> randomParentDistribution(0, m_mu - 1);
	std::uniform_int_distribution<> randomGeneDistribution(0, m_roh - 1);

	int iterationCounter = 0;
	int maxIterations = 500000;

	std::vector<int> qualityOverIterations;
	qualityOverIterations.reserve(maxIterations);

	/* -------------------------------------- RANDOM START PARENTS -------------------------------------- */
	for (int i = 0; i < parents.size(); ++i)
		for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
			parents[i].Genes[geneIndex] = m_randomIndividualDistribution(m_mersenneTwisterEngine);

	/* -------------------------------------- START EVOLUTION -------------------------------------- */
	for (; iterationCounter <= maxIterations; ++iterationCounter)
	{
		//add all new parents to a summary array
		for (int i = 0; i < parents.size(); ++i)
			individuals[i] = parents[i];

		for (int i = 0; i < children.size(); ++i)
		{
			/* -------------------------------------- RECOMBINATION -------------------------------------- */
			muSlashRohSharpLambaRecombination(parents, children[i], randomParentDistribution, randomGeneDistribution);

			/* -------------------------------------- RANDOM MUTATION -------------------------------------- */
			for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
				children[i].Genes[geneIndex] += m_randomMutationDistribution(m_mersenneTwisterEngine);

			//add to a summary array
			individuals[parents.size() + i] = children[i];
		}

		/* -------------------------------------- SELECTION -------------------------------------- */
		if (checkConditionsAndReturnTrueIfSolutionFound(individuals, solution))
			break;

		if (strategy == EquationSolverStrategy::muSlashRohPlusLambda)
			plusSelection(individuals, parents, qualityOverIterations);
		else if(strategy == EquationSolverStrategy::muSlashRohCommaLambda)
			commaSelection(parents, children, individuals, qualityOverIterations);
	}

	qualityOverIterations.push_back(0);
	qualityOverIterations.shrink_to_fit();

	if (iterationCounter < maxIterations)
		printSolution(solution, iterationCounter, 0);
	else
		std::cout << "Exceeded maximum iterations of " << maxIterations << "! -> Couldn't find a solution!\n";

	std::string type = strategy == EquationSolverStrategy::muSlashRohPlusLambda ? ("+") : (",");
	saveToFile(qualityOverIterations, "mu_p" + type + "lambda.csv");
}

void EquationSolver::muSlashRohSharpLambaRecombination(std::vector<Individual>& parents, Individual& outChild,  const std::uniform_int_distribution<>& randomParentDistribution, const std::uniform_int_distribution<>& randomGeneDistribution)
{
	int sizeOfGenes = _countof(parents[0].Genes);
	if (m_muSlashRohSharpLambdaRecombination == MuSlashRohSharpLambdaRecombination::Combination)
	{
		std::vector<Individual*> randomParents;
		randomParents.reserve(m_roh);
		for (int p = 0; p < m_roh; ++p)
		{
			randomParents.push_back(&parents[randomParentDistribution(m_mersenneTwisterEngine)]);
		}
		for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
		{
			outChild.Genes[geneIndex] = randomParents[randomGeneDistribution(m_mersenneTwisterEngine)]->Genes[geneIndex];
		}
	}
	else if (m_muSlashRohSharpLambdaRecombination == MuSlashRohSharpLambdaRecombination::Melting)
	{
		outChild.Reset();
		for (int p = 0; p < m_roh; ++p)
		{
			for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
			{
				outChild.Genes[geneIndex] += parents[randomParentDistribution(m_mersenneTwisterEngine)].Genes[geneIndex];
			}
		}
		for (int geneIndex = 0; geneIndex < sizeOfGenes; ++geneIndex)
			outChild.Genes[geneIndex] /= m_roh;
	}
}

bool EquationSolver::checkConditionsAndReturnTrueIfSolutionFound(std::vector<Individual>& individuals, Individual& solution) const
{	
	//which of the individuals fulfill the condition 2?
	for (int i = 0; i < individuals.size(); ++i)
		individuals[i].Usable = isEvolutionStrategyCondition2Fulfilled(individuals[i]);

	//is one of the individuals the solution?
	if (foundSolution(&individuals[0], individuals.size(), solution))
		return true;
	return false;
}

void EquationSolver::plusSelection(std::vector<Individual>& inOutIndividuals, std::vector<Individual>& inOutParents, std::vector<int>& inOutQualityOverIterations)
{			
	//determine quality
	for (int i = 0; i < inOutIndividuals.size(); ++i)
		inOutIndividuals[i].Quality = inOutIndividuals[i].Usable ? getDiffenceOfEvolutionStrategyEquation(inOutIndividuals[i].Genes[0], inOutIndividuals[i].Genes[1], inOutIndividuals[i].Genes[2], inOutIndividuals[i].Genes[3]) : std::numeric_limits<int>::max();

	std::random_shuffle(inOutIndividuals.begin(), inOutIndividuals.end());
	std::sort(inOutIndividuals.begin(), inOutIndividuals.end(), [](const Individual& lhs, const Individual& rhs) { return lhs.Quality < rhs.Quality; });

	//save best quality level
	inOutQualityOverIterations.push_back(inOutIndividuals[0].Quality);

	//choose mu best individuals from all individuals
	for (int i = 0; i < inOutParents.size(); ++i)
		inOutParents[i] = inOutIndividuals[i];
}

void EquationSolver::commaSelection(std::vector<Individual>& inOutParents, std::vector<Individual>& inOutChildren, const std::vector<Individual>& individuals, std::vector<int>& inOutQualityOverIterations)
{			
	//determine quality
	for (int i = 0; i < inOutChildren.size(); ++i)
		inOutChildren[i].Quality = individuals[inOutParents.size() + i].Usable ? getDiffenceOfEvolutionStrategyEquation(inOutChildren[i].Genes[0], inOutChildren[i].Genes[1], inOutChildren[i].Genes[2], inOutChildren[i].Genes[3]) : std::numeric_limits<int>::max();

	std::random_shuffle(inOutChildren.begin(), inOutChildren.end());
	std::sort(inOutChildren.begin(), inOutChildren.end(), [](const Individual& lhs, const Individual& rhs) { return lhs.Quality < rhs.Quality; });

	//save best quality level
	inOutQualityOverIterations.push_back(inOutChildren[0].Quality);

	//choose mu best individuals from all individuals
	for (int i = 0; i < inOutParents.size(); ++i)
		inOutParents[i] = inOutChildren[i % inOutChildren.size()];
}
