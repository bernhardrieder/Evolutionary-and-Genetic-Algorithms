#include "NQueensPuzzle.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>


NQueensPuzzle::NQueensPuzzle() : m_queensAmount(0), m_pixelAmount(0)
{
}


NQueensPuzzle::~NQueensPuzzle()
{
}

int NQueensPuzzle::Execute(int argc, char** argv)
{
	if (!parseCommandLine(argc, argv))
		return 0;

	std::cout << "INFO: Start solving '" + std::to_string(m_queensAmount) + " Queens Puzzle'\n";
	PuzzleResult result;
	if (!solvePuzzle(result))
	{
		std::cerr << "ERROR: Couldn't solve Puzzle! :(\n";
		return 0;
	}
	std::cout << "INFO: Finished solving successfully!\n";

	if(!createAndSaveResultTextureOnFileSystem(result))
	{
		std::cerr << "ERROR: Couldn't save result on filesystem! :(\n";
		return 0;
	}

#if _DEBUG
	getchar();
#endif
	
	return 1;
}

bool NQueensPuzzle::parseCommandLine(int argc, char** argv)
{
	const unsigned requiredArgv = 2;
	if (argc != requiredArgv + 1)
	{
		showUsage(argv[0]);
		return false;
	}

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if ((arg == "-h") || (arg == "--help"))
		{
			showUsage(argv[0]);
			return false;
		}
		if ((arg == "-n") && (i + 1 < argc))
			m_queensAmount = std::stoi(argv[++i]);
	}

	if(m_queensAmount <= 0)
	{
		std::cerr << "ERROR: Input Error! Give at least a positive number for the amount of Queens with the parameter '-n <NUM>'!\n";
		return false;
	}
	return true;
}

void NQueensPuzzle::showUsage(char* appExecutionPath)
{
	std::cerr << "Usage: " << appExecutionPath << " <option(s)>\n\n"
		<< "Options:\n"
		<< "\t-h,--help\tShow this help message\n"
		<< "\t-n <NUM>\t\tAmount of queens in puzzle.\n"
		<< std::endl;
}

bool NQueensPuzzle::solvePuzzle(PuzzleResult& outResult)
{
	return true;
}

bool NQueensPuzzle::createAndSaveResultTextureOnFileSystem(const PuzzleResult& puzzleResult)
{
	calculateResultTexturePixelWidthHeightAndDimension();
	if (!createResultRenderTexture())
		return false;

	m_resultTexture.clear(sf::Color::White);

	drawGridInResultTexture();
	drawQueensInResultTexture(puzzleResult);

	m_resultTexture.display();

	if (!saveResultTextureOnFileSystem())
		return false;
	return true;
}

bool NQueensPuzzle::createResultRenderTexture()
{
	if (!m_resultTexture.create(m_textureDimension.x, m_textureDimension.y))
	{
		std::cerr << "ERROR: Cannot create rendertexture!";
		return false;
	}
	return true;
}

void NQueensPuzzle::calculateResultTexturePixelWidthHeightAndDimension()
{
	m_pixelAmount = (m_queensAmount * (m_queensPixel + m_gridPixel)) - m_gridPixel; // -gridPixel for the last one where no grid is needed
	m_textureDimension = {m_pixelAmount, m_pixelAmount};
}

void NQueensPuzzle::drawGridInResultTexture()
{
	sf::VertexArray vertices(sf::PrimitiveType::Lines);
	for (int x = 0; x < m_queensAmount; ++x)
	{
		int xPos = x * m_queensPixel + x * m_gridPixel;
		sf::Vertex v1(sf::Vector2f(xPos, 0), sf::Color::Black);
		sf::Vertex v2(sf::Vector2f(xPos, m_pixelAmount), sf::Color::Black);
		vertices.append(v1);
		vertices.append(v2);
	}
	for (int y = 0; y < m_queensAmount; ++y)
	{
		int yPos = y * m_queensPixel + y * m_gridPixel;
		sf::Vertex v1(sf::Vector2f(0, yPos), sf::Color::Black);
		sf::Vertex v2(sf::Vector2f(m_pixelAmount, yPos), sf::Color::Black);
		vertices.append(v1);
		vertices.append(v2);
	}
	m_resultTexture.draw(vertices);
}

void NQueensPuzzle::drawQueensInResultTexture(const PuzzleResult& puzzleResult)
{
}

bool NQueensPuzzle::saveResultTextureOnFileSystem() const
{
	const std::string fileName = std::to_string(m_queensAmount) + "_queens_puzzle.jpg";
	if (!m_resultTexture.getTexture().copyToImage().saveToFile(fileName))
	{
		std::cerr << "ERROR: Result image creation failed!!";
		return false;
	}
	std::cout << "INFO: Result of '" + std::to_string(m_queensAmount) + " Queens Puzzle' saved in execution path!\n";
	std::cout << "INFO: Filename: '" + fileName + "'\n";
	return true;
}
