#pragma once
#include <SFML/Graphics/RenderTexture.hpp>

class NQueensPuzzle
{
	struct PuzzleResult
	{
		
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
	void drawGridInResultTexture();
	void drawQueensInResultTexture(const PuzzleResult& puzzleResult);
	bool saveResultTextureOnFileSystem() const;
	

	sf::RenderTexture m_resultTexture;
	sf::Vector2u m_textureDimension;
	unsigned m_queensAmount;
	unsigned m_pixelAmount;
	const unsigned m_queensPixel = 20;
	const unsigned m_gridPixel = 1;
};

