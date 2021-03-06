#include "Utils.h"

float g_DistLookUpTable[NUM_NODES][NUM_NODES] = { 0 };
vector<Individual*> g_currentPopulation;

void ClearPopulation()
{
	for (size_t i = 0; i < g_currentPopulation.size(); i++)
	{
		delete g_currentPopulation[i];
	}

	g_currentPopulation.clear();

}

void GenerateRandomPopulation()
{
	ClearPopulation();

	for (size_t i = 0; i < POPULATION_SIZE; i++)
	{
		Individual* newGuy = new Individual();

		//Create pairs to swap in genome
		for (size_t j = 0; j < NUM_NODES; j++)
		{
			//Chance for no swap to occur
			newGuy->swaps[j]._index1 = rand() % NUM_NODES;
			newGuy->swaps[j]._index2 = rand() % NUM_NODES;
		}

		g_currentPopulation.push_back(newGuy);
	}
}

float GetDistanceOfPath(Individual* individual)
{
	float totalDistance = 0;
	//Initialize path index
	int pathIndex[NUM_NODES];
	for (size_t i = 0; i < NUM_NODES; i++)
	{
		pathIndex[i] = i;
	}

	//Apply all swaps in order
	for (size_t i = 0; i < NUM_NODES; i++)
	{

		int temp = pathIndex[individual->swaps[i]._index1];
		pathIndex[individual->swaps[i]._index1] = pathIndex[individual->swaps[i]._index2];
		pathIndex[individual->swaps[i]._index2] = temp;
	}

	//Accumulate distance
	for (size_t i = 0; i < NUM_NODES-1; i++)
	{
		totalDistance += g_DistLookUpTable[pathIndex[i]][pathIndex[i+1]];
	}

	//add distance from last destination to start.
	totalDistance += g_DistLookUpTable[pathIndex[NUM_NODES - 1]][pathIndex[0]];

	return totalDistance;
}

Individual* SelectFitIndividual()
{
	float rouletteMax = 0;
	float pathDistances[POPULATION_SIZE];
	float greatestDistance = 0;

	//Get total distance of all children (for roulette)
	for (size_t i = 0; i < POPULATION_SIZE; i++)
	{
		pathDistances[i] = GetDistanceOfPath(g_currentPopulation[i]);
		if (pathDistances[i] > greatestDistance)
		{
			greatestDistance = pathDistances[i];
		}
	}

	//Create rouletteMax. Lower distance = higher chance of winning.
	for (size_t i = 0; i < POPULATION_SIZE; i++)
	{
		rouletteMax += (greatestDistance - pathDistances[i]);
	}

	float rouletteOutcome = 0;
	if (rouletteMax > 0) 
	{
		rouletteOutcome = (float)(rand() % (int)ceilf(rouletteMax)); 
	}


	//If somehow rouletteOutcome is zero, won't enter while loop
	int chosenIndex = (rouletteOutcome == 0.0f) ? 0 : -1;

	//Individual is chosen when value goes under 0
	while (rouletteOutcome > 0 && chosenIndex < (POPULATION_SIZE-1))
	{
		chosenIndex++;
		rouletteOutcome -= (greatestDistance - pathDistances[chosenIndex]);
	}

	return g_currentPopulation[chosenIndex];
}

void GenerateNewPopulation()
{
	//Genetic algorithm variables
	float crossOverChance = 0.7f; // 70% chance
	float mutationChance = 0.05f; //  5% chance
	vector<Individual*> newPopulation;

	for (size_t i = 0; i < POPULATION_SIZE; i++)
	{
		Individual* newIndividual = new Individual();
		newPopulation.push_back(newIndividual);

		//Select which 2 parents to read from
		Individual* parent1 = SelectFitIndividual(); 
		Individual* parent2 = SelectFitIndividual();

		while (parent1 == parent2)
		{
			parent2 = SelectFitIndividual(); //Make sure parents are different individuals
		}

		bool readingFromFirstParent = true;
		//Start giving child the parent's genes
		for (size_t j = 0; j < NUM_NODES; j++)
		{
			if (readingFromFirstParent)	//Read from first parent
			{
				newIndividual->swaps[j]._index1 = parent1->swaps[j]._index1;
				newIndividual->swaps[j]._index2 = parent1->swaps[j]._index2;
			}
			else //Read from second parent
			{
				newIndividual->swaps[j]._index1 = parent2->swaps[j]._index1;
				newIndividual->swaps[j]._index2 = parent2->swaps[j]._index2;
			}

			//Chance to crossover
			if ((rand() / RAND_MAX) < crossOverChance)
			{
				readingFromFirstParent = !readingFromFirstParent; //Start reading from other parent
			}

			//Chance for mutation
			if ((rand() / RAND_MAX) < mutationChance)
			{
				//Random new swap
				newIndividual->swaps[j]._index1 = rand() % NUM_NODES;
				newIndividual->swaps[j]._index2 = rand() % NUM_NODES;
			}
		}

	}


	//Replace old population with new population
	ClearPopulation();

	for (size_t i = 0; i < POPULATION_SIZE; i++)
	{
		g_currentPopulation.push_back(newPopulation[i]);
	}
}

float DistanceBetweenNodes(int index1, int index2)
{
	float xDiff = _PositionX [index1] - _PositionX[index2];
	float yDiff = _PositionY[index1] - _PositionY[index2];

	return sqrtf(xDiff*xDiff + yDiff*yDiff);
}

void Initialize()
{
	srand((unsigned int)time(NULL));

	//Populate look up table
	for (size_t i = 0; i < NUM_NODES; i++)
	{
		for (size_t j = i+1; j < NUM_NODES; j++)
		{
			//Same distance from either direction
			g_DistLookUpTable[i][j] = g_DistLookUpTable[j][i] = DistanceBetweenNodes(i, j);
		}
	}

	//Create initial population
	GenerateRandomPopulation();
}

void PrintPath(Individual* ind)
{
	//Initialize path index
	int pathIndex[NUM_NODES];
	for (size_t i = 0; i < NUM_NODES; i++)
	{
		pathIndex[i] = i;
	}

	//Apply all swaps in order
	for (size_t i = 0; i < NUM_NODES; i++)
	{
		int temp = pathIndex[ind->swaps[i]._index1];
		pathIndex[ind->swaps[i]._index1] = pathIndex[ind->swaps[i]._index2];
		pathIndex[ind->swaps[i]._index2] = temp;
	}

	//Print out path
	for (size_t i = 0; i < NUM_NODES; i++)
	{
		cout << "[" << (char)(65 + pathIndex[i]) << "]-";
	}

	//Print first node again (returning to start)
	cout << "[" << (char)(65 + pathIndex[0]) << "]";
}

int main()
{
	Initialize();	//Get the simulation ready

	cout << "Starting Simulation" << endl;;
	cout << "===================" << endl << endl;

	for (size_t i = 0; i < NUM_ITERATIONS; i++)	//One loop cycle = one genetic algorithm cycle
	{
		GenerateNewPopulation();
	}

	//Get best individual
	Individual* bestOne = g_currentPopulation[0];
	float bestDistance = GetDistanceOfPath(bestOne);

	for (size_t i = 1; i < g_currentPopulation.size(); i++)
	{
		if (GetDistanceOfPath(g_currentPopulation[i]) < bestDistance)
		{
			bestDistance = (GetDistanceOfPath(g_currentPopulation[i]));
			bestOne = g_currentPopulation[i];
		}
	}

	//Print study results
	cout << "Results: " << endl << endl;;
	cout << "Population Size: " << POPULATION_SIZE  << " individuals"<< endl;
	cout << "Number of generations (cycles): " << NUM_ITERATIONS << endl;
	cout << "Lowest distance: " << bestDistance << endl;
	cout << "Path: ";
	PrintPath(bestOne);

	ClearPopulation();	//Clear memory

	_getch();

	return 0;
}