#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include <conio.h>

using namespace std;

//Genetic algorithm variables
#define POPULATION_SIZE 50
#define NUM_ITERATIONS 100

//These have to be updated together
#define NUM_NODES 15
const static float _PositionX[NUM_NODES] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140 };
const static float _PositionY[NUM_NODES] = { 20, 10, 40, 120, 100, 0, 60, 80, 70, 90, 50, 140, 30, 130, 110 };
//============

//Structures
struct Pair
{
	int _index1;
	int _index2;
};

struct Individual	//Number of swaps = same as number of nodes
{
	Pair swaps[NUM_NODES];
};