#include "GeneticAlgorithm.h"



GeneticAlgorithm::GeneticAlgorithm()
{
	m_mersenneTwisterEngine = std::mt19937_64(m_randomDevice());
}


GeneticAlgorithm::~GeneticAlgorithm()
{
}

std::vector<int> GeneticAlgorithm::SolveNQueensPuzzle(const int& queensAmount, int& outNeededIterations, bool& outSuccess)
{	
	/*
	* Wähle problemspezifische Individuencodierung
	* Initialisiere Individuen der Startpopulation zufällig
	* do {
	* Bewerte Individuen mit Bewertungs-/Fitnessfunktion
	* Selektiere Elterpaare nach Heiratsschema
	* Erzeuge Nachkommen durch Rekombination
	* Mutiere die erzeugten Nachkommen
	* Ersetze Individuen der akt. Generation nach Ersetzungsschema
	* } while Abbruchbedingung trifft nicht zu
	*/
	initalizeRandomDistributions(queensAmount);

	m_population.Initialize(queensAmount, m_randomChromosomeGeneDistribution, m_mersenneTwisterEngine);
	int maxIterations = 1000000;
	outNeededIterations = 0;
	while(m_population.EvaluateFitness() < 0 && ++outNeededIterations <= maxIterations)
	{
		m_population.OnePointCrossoverWithWedding(m_randomChromosomeGeneDistribution, m_mersenneTwisterEngine);
		m_population.Mutate(0.5f, m_randomChromosomeGeneDistribution, m_randomProbabilityDistirbution, m_mersenneTwisterEngine);
		m_population.Selection(3, true, m_randomChromosomeGeneDistribution, m_mersenneTwisterEngine);
	}
	outSuccess = outNeededIterations < maxIterations;
	return m_population.BestIndividual->Genes;
}

void GeneticAlgorithm::Individual::ResetFitness()
{
	Fitness = 1; //fitness goes along minus
}

void GeneticAlgorithm::Individual::Reset()
{
	for (int i = 0; i < Genes.size(); ++i)
		Genes[i] = -1;
	ResetFitness();
}

void GeneticAlgorithm::Individual::Initialize(const int& queensAmount, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine)
{
	Reset();
	Genes.resize(queensAmount);

	for (auto& gene : Genes)
		gene = randomChromosomeGeneDistribution(mersenneTwisterEngine);
}

int GeneticAlgorithm::Individual::EvaluateFitness()
{
	int numberOfCollisions = 0;

	//fitness already set?
	if (Fitness <= 0)
		return Fitness;

	for (int geneIndex = Genes.size(); --geneIndex > 0;)
	{
		//iterate through pairs of queens
		for (int otherGeneIndex = geneIndex; --otherGeneIndex >= 0;)
		{
			//horizontal distance between queens
			int distance = abs(Genes[geneIndex] - Genes[otherGeneIndex]);

			//count the number of pairs of queens in the same column or diagonal
			if ((distance == 0) || (distance == geneIndex - otherGeneIndex))
				numberOfCollisions++;
		}
	}

	Fitness = -numberOfCollisions;
	return Fitness;
}

void GeneticAlgorithm::Individual::OnePointCrossover(Individual& inOutOther, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine)
{
	int current = -1; //start on the bottom
	int crossoverPoint = randomChromosomeGeneDistribution(mersenneTwisterEngine);

	while (++current < crossoverPoint)
		std::swap(Genes[current], inOutOther.Genes[current]);

	ResetFitness();
	inOutOther.ResetFitness();
}

void GeneticAlgorithm::Individual::Mutate(const float& probability, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, const std::uniform_real_distribution<>& randomProbabilityDistirbution, std::mt19937_64& mersenneTwisterEngine)
{
	if (randomProbabilityDistirbution(mersenneTwisterEngine) >= probability)
		return;

	do
		Genes[randomChromosomeGeneDistribution(mersenneTwisterEngine)] = randomChromosomeGeneDistribution(mersenneTwisterEngine);
	while (randomProbabilityDistirbution(mersenneTwisterEngine) < probability);

	ResetFitness();
}

void GeneticAlgorithm::Population::Initialize(const int& queensAmount, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine)
{
	Individuals.resize(queensAmount);
	SelectionTmpBuffer.resize(queensAmount);

	for (auto& individual : Individuals)
		individual.Initialize(queensAmount, randomChromosomeGeneDistribution, mersenneTwisterEngine);
}

int GeneticAlgorithm::Population::EvaluateFitness()
{
	Individual& bestIndividual = Individuals[0];
	bestIndividual.EvaluateFitness();

	//evaluate remaining individuals and check if better
	for (int i = 1; i < Individuals.size(); ++i)
		if (Individuals[i].EvaluateFitness() >= bestIndividual.Fitness)
			bestIndividual = Individuals[i];

	BestIndividual = &bestIndividual;
	return bestIndividual.Fitness;
}

void GeneticAlgorithm::Population::OnePointCrossoverWithWedding(const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine)
{
	// one point crossover of pairs of individuals
	for (int i = 0; i < Individuals.size(); i += 2)
		Individuals[i].OnePointCrossover(Individuals[i + 1], randomChromosomeGeneDistribution, mersenneTwisterEngine);
}

void GeneticAlgorithm::Population::Mutate(const float& probability, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, const std::uniform_real_distribution<>& randomProbabilityDistirbution, std::mt19937_64& mersenneTwisterEngine)
{
	for (auto& individual : Individuals)
		individual.Mutate(probability, randomChromosomeGeneDistribution, randomProbabilityDistirbution, mersenneTwisterEngine);
}

void GeneticAlgorithm::Population::Selection(int torunamentSize, bool preserveBestIndividual, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine)
{
	int i = Individuals.size();

	if (preserveBestIndividual)
		SelectionTmpBuffer[--i] = *BestIndividual;

	while (--i >= 0)
		SelectionTmpBuffer[i] = TournamentSelection(torunamentSize, randomChromosomeGeneDistribution, mersenneTwisterEngine);

	Individuals.swap(SelectionTmpBuffer); //new parents
	BestIndividual = nullptr;
}

GeneticAlgorithm::Individual GeneticAlgorithm::Population::TournamentSelection(int torunamentSize, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine)
{
	Individual bestIndividual = Individuals[randomChromosomeGeneDistribution(mersenneTwisterEngine)];

	while (--torunamentSize > 0)
	{
		Individual individual = Individuals[randomChromosomeGeneDistribution(mersenneTwisterEngine)];

		if (individual.Fitness > bestIndividual.Fitness)
			bestIndividual = individual;
	}
	return bestIndividual;
}

void GeneticAlgorithm::initalizeRandomDistributions(const int& genes)
{
	m_randomChromosomeGeneDistribution = std::uniform_int_distribution<int>(0, genes-1);
	m_randomProbabilityDistirbution = std::uniform_real_distribution<double>(0, 1);
}
