#pragma once
#include <SFML/Graphics/RenderTexture.hpp>

class NQueensPuzzle
{
	//just a wrapper due to early function declarations
	struct PuzzleResult
	{
		std::vector<sf::Vector2i> Positions;
		int NeededIterations;
	};
public:
	NQueensPuzzle();
	~NQueensPuzzle();

	int Execute(int argc, char** argv);

private:
	bool parseCommandLine(int argc, char** argv);
	static void showUsage(char* appExecutionPath);
	bool solvePuzzle(PuzzleResult& outResult);

	bool createAndSaveResultTextureOnFileSystem(const PuzzleResult& puzzleResult);
	bool createResultRenderTexture();
	void calculateResultTexturePixelWidthHeightAndDimension();
	void drawGridInResultTextureAndCreateQueenPosLUT();
	bool drawQueensInResultTexture(const PuzzleResult& puzzleResult);
	bool saveResultTextureOnFileSystem() const;
	

	sf::RenderTexture m_resultTexture;
	sf::Vector2i m_textureDimension;
	std::vector<int> m_queenMiddlePointPixelXCoordLUT;
	std::vector<int> m_queenMiddlePointPixelYCoordLUT;
	int m_queensAmount;
	int m_pixelAmount;
	const int m_queensPixel = 20;
	const int m_gridPixel = 1;
};

