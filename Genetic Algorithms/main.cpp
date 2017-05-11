#include "NQueensPuzzle.h"
#include <ctime>
#include <iostream>

int main(int argc, char** argv)
{
	srand(time(nullptr));
	NQueensPuzzle puzzleSolver;
	return puzzleSolver.Execute(argc, argv);
}
