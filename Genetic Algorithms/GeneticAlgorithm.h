#pragma once
#include <random>

class GeneticAlgorithm
{
public:
	GeneticAlgorithm();
	~GeneticAlgorithm();

	std::vector<int> SolveNQueensPuzzle(const int& queensAmount, int& outNeededIterations, bool& outSuccess);

private:
	struct Individual 
	{
		std::vector<int> Genes; // queen positions in rows
		int Fitness; //number of collisions

		void ResetFitness();
		void Reset();
		void Initialize(const int& queensAmount, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine);
		int EvaluateFitness();
		void OnePointCrossover(Individual& inOutOther, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine);
		void Mutate(const float& probability, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, const std::uniform_real_distribution<>& randomProbabilityDistirbution, std::mt19937_64& mersenneTwisterEngine);
	};

	struct Population 
	{
		std::vector<Individual> Individuals; 
		std::vector<Individual> SelectionTmpBuffer;
		Individual* BestIndividual;

		void Initialize(const int& queensAmount, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine);
		int EvaluateFitness();
		void OnePointCrossoverWithWedding(const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine);
		void Mutate(const float& probability, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, const std::uniform_real_distribution<>& randomProbabilityDistirbution, std::mt19937_64& mersenneTwisterEngine);
		//determin new children
		void Selection(int torunamentSize, bool preserveBestIndividual, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine);
		//find the best individual in a randomly choosen individuals tournament with torunamentSize rounds. a kind of survival of the fittest
		Individual TournamentSelection(int torunamentSize, const std::uniform_int_distribution<>& randomChromosomeGeneDistribution, std::mt19937_64& mersenneTwisterEngine);
	};

	void initalizeRandomDistributions(const int& genes);

	std::random_device m_randomDevice;
	std::mt19937_64 m_mersenneTwisterEngine;
	std::uniform_int_distribution<int> m_randomChromosomeGeneDistribution;
	std::uniform_real_distribution<double> m_randomProbabilityDistirbution;
	Population m_population;
};

