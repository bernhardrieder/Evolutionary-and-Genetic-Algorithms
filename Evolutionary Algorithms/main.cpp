#include <iostream>
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
void OnePlusOneEvolutionStrategy();
int main(int argc, char** argv)
{

	OnePlusOneEvolutionStrategy();
	OnePlusOneEvolutionStrategy();
	OnePlusOneEvolutionStrategy();
	OnePlusOneEvolutionStrategy();
	OnePlusOneEvolutionStrategy();


	getchar();

	return 0;
}



int getDiffenceOfEvolutionStrategyEquation(const int& x, const int& y, const int& a, const int& b)
{
	int left = 3 * std::pow(x, 2) + 5 * std::pow(y, 3);
	int right = 7 * a + 3 * std::pow(b, 2);
	return std::abs(left - right); // difference
}
bool isEvolutionStrategyCondition1Fulfilled(const Individual& individual)
{
	return getDiffenceOfEvolutionStrategyEquation(individual.Genes[0], individual.Genes[1], individual.Genes[2], individual.Genes[3]) == 0;
}
bool isEvolutionStrategyCondition2Fulfilled(const int& a, const int& b)
{
	return a > b;
}

void printSolution(Individual solution, int iterationCounter, int deathCounter, int* randomIndividualRange, int* randomMutationRange);
void OnePlusOneEvolutionStrategy()
{
	int randomIndividualRange[2] = { -50, 50 };
	int randomMutationRange[2] = { -5, 5 };
	std::random_device randomDevice;
	std::mt19937_64 mersenneTwisterEngine(randomDevice());
	std::uniform_int_distribution<> randomIndividualDistribution(randomIndividualRange[0], randomIndividualRange[1]);
	std::uniform_int_distribution<> randomMutationDistribution(randomMutationRange[0], randomMutationRange[1]);

	Individual individual, solution;
	size_t sizeOfGenes = _countof(individual.Genes);

	int deathCounter = 0;
	int iterationCounter = 0;
	int maxIterations = 500000;
	bool initNewIndividual = true;
	while(true && iterationCounter <= maxIterations)
	{
		++iterationCounter;
		/* -------------------------------------- SELF-REPLICATION -------------------------------------- */
		if(initNewIndividual)
		{
			for (int i = 0; i < sizeOfGenes; ++i)
				individual.Genes[i] = randomIndividualDistribution(mersenneTwisterEngine);
			initNewIndividual = false; 
		}

		Individual mutation = individual;

		/* -------------------------------------- RANDOM MUTATION -------------------------------------- */
		for (int i = 0; i < sizeOfGenes; ++i)
			mutation.Genes[i] += randomMutationDistribution(mersenneTwisterEngine);


		/* -------------------------------------- SELECTION -------------------------------------- */
		bool individualUsable = isEvolutionStrategyCondition2Fulfilled(individual.Genes[2], individual.Genes[3]);
		bool mutationUsable = isEvolutionStrategyCondition2Fulfilled(mutation.Genes[2], mutation.Genes[3]);

		//both are shit!
		if(!individualUsable && !mutationUsable)
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
		if(mutationUsable && !individualUsable)
		{
			individual = mutation;
			continue;
		}

		if(individualUsable || mutationUsable)
		{
			//determine better
			int individualQuality = getDiffenceOfEvolutionStrategyEquation(individual.Genes[0], individual.Genes[1], individual.Genes[2], individual.Genes[3]);
			int mutationQuality = getDiffenceOfEvolutionStrategyEquation(mutation.Genes[0], mutation.Genes[1], mutation.Genes[2], mutation.Genes[3]);

			//std::cout << "Quality of Parent: " << parentQuality << ", Quality of Child: " << childQuality << std::endl;
			//std::cout << ".";

			//if mutation is better
			if (mutationQuality < individualQuality)
				individual = mutation;
		}
	}

	if (iterationCounter < maxIterations)
		printSolution(solution, iterationCounter, deathCounter, randomIndividualRange, randomMutationRange);
	else
		std::cout << "Exceeded maximum of iterations! -> Couldn't find a solution!\n";
}

void printSolution(Individual solution, int iterationCounter, int deathCounter, int* randomIndividualRange, int* randomMutationRange)
{
	int x = solution.Genes[0], y = solution.Genes[1], a = solution.Genes[2], b = solution.Genes[3];

	std::cout << "\n ############ SOLUTION ############\n"
		<< "(<x,y,a,b>) ==> (<" << x << ", " << y << ", " << a << ", " << b << ">)" << std::endl
		<< "\n ############ CONDITION 1 ############\n"
		<< "3x^2 + 5y^3 = 7a + 3b^2" << std::endl
		<< "3 * " << x << "^2 + 5 * " << y << "^3 = 7 * " << a << " + 3 " << b << "^2" << std::endl
		<< 3 * std::pow(x, 2) + 5 * std::pow(y, 3) << " = " << 7 * a + 3 * std::pow(b, 2) << std::endl
		<< "\n ############ CONDITION 2 ############\n"
		<< "a > b ==> " << a << " > " << b << std::endl
		<< "\n ############ SUMMARY ############\n"
		<< "It took " << iterationCounter << " generations and " << deathCounter << " death" << ((deathCounter > 0) ? "s" : "") << "!" << std::endl
		<< "Individual random range: (" << randomIndividualRange[0] << ", " << randomIndividualRange[1] << ")" << std::endl
		<< "Mutation random range: (" << randomMutationRange[0] << ", " << randomMutationRange[1] << ")" << std::endl;
}