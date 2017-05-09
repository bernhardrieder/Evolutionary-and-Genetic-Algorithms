#include "NQueensPuzzle.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>
#include "Helper.h"


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
	std::cout << "INFO: Solved successfully!\n";

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

	drawGridInResultTextureAndCreateQueenPosLUT();
	
	PuzzleResult result;
	result.Positions.push_back(sf::Vector2i(5, 5));
	//result.Positions.push_back(sf::Vector2i(m_queensAmount-1, m_queensAmount-1));
	if (!drawQueensInResultTexture(result))
		return false;

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

void NQueensPuzzle::drawGridInResultTextureAndCreateQueenPosLUT()
{
	int queenPixelHalf = m_queensPixel / 2;

	m_queenMiddlePointPixelXCoordLUT.resize(m_queensAmount);
	m_queenMiddlePointPixelYCoordLUT.resize(m_queensAmount);
	
	sf::VertexArray vertices(sf::PrimitiveType::Lines);
	for (int x = 0; x < m_queensAmount; ++x)
	{
		int xPos = x * m_queensPixel + x * m_gridPixel;
		sf::Vertex v1(sf::Vector2f(xPos, 0), sf::Color::Black);
		sf::Vertex v2(sf::Vector2f(xPos, m_pixelAmount), sf::Color::Black);
		vertices.append(v1);
		vertices.append(v2);

		m_queenMiddlePointPixelXCoordLUT[x] = xPos + queenPixelHalf;
	}
	for (int y = 0; y < m_queensAmount; ++y)
	{
		int yPos = y * m_queensPixel + y * m_gridPixel;
		sf::Vertex v1(sf::Vector2f(0, yPos), sf::Color::Black);
		sf::Vertex v2(sf::Vector2f(m_pixelAmount, yPos), sf::Color::Black);
		vertices.append(v1);
		vertices.append(v2);

		m_queenMiddlePointPixelYCoordLUT[y] = yPos + queenPixelHalf + 1;
	}
	m_resultTexture.draw(vertices);
}

bool NQueensPuzzle::drawQueensInResultTexture(const PuzzleResult& puzzleResult)
{
	int queenPixelHalf = m_queensPixel / 2;
	for(int i = 0; i < puzzleResult.Positions.size(); ++i)
	{
		auto& queenPos = puzzleResult.Positions[i];
		if (queenPos.x >= m_queensAmount || queenPos.y >= m_queensAmount)
		{
			std::cerr << "ERROR: Queen position is out of texture range!";
			return false;
		}

		sf::Vector2i queenMiddlePointPixelPos = { m_queenMiddlePointPixelXCoordLUT[queenPos.x], m_queenMiddlePointPixelYCoordLUT[queenPos.y] };

		sf::Color queenColor = sf::Color::Green;
		sf::VertexArray vertices(sf::PrimitiveType::Quads);
		vertices.append({ sf::Vector2f(queenMiddlePointPixelPos.x - queenPixelHalf, queenMiddlePointPixelPos.y + queenPixelHalf), queenColor }); // lower left
		vertices.append({ sf::Vector2f(queenMiddlePointPixelPos.x - queenPixelHalf, queenMiddlePointPixelPos.y - queenPixelHalf), queenColor }); // upper left
		vertices.append({ sf::Vector2f(queenMiddlePointPixelPos.x + queenPixelHalf, queenMiddlePointPixelPos.y - queenPixelHalf), queenColor }); // upper right
		vertices.append({ sf::Vector2f(queenMiddlePointPixelPos.x + queenPixelHalf, queenMiddlePointPixelPos.y + queenPixelHalf), queenColor }); // lower right
		m_resultTexture.draw(vertices);


		sf::VertexArray verticesLineVertical(sf::PrimitiveType::Lines);
		verticesLineVertical.append({ sf::Vector2f(queenMiddlePointPixelPos.x, 0), queenColor });
		verticesLineVertical.append({ sf::Vector2f(queenMiddlePointPixelPos.x, m_pixelAmount), queenColor }); 
		m_resultTexture.draw(verticesLineVertical);

		sf::VertexArray verticesLineHorizontal(sf::PrimitiveType::Lines);
		verticesLineHorizontal.append({ sf::Vector2f(0, queenMiddlePointPixelPos.y), queenColor });
		verticesLineHorizontal.append({ sf::Vector2f(m_pixelAmount, queenMiddlePointPixelPos.y), queenColor });
		m_resultTexture.draw(verticesLineHorizontal);

		int low = queenPos.x;
		if (low > queenPos.y)
			low = queenPos.y;
		sf::Vector2i lowPos = { m_queenMiddlePointPixelXCoordLUT[queenPos.x-low], m_queenMiddlePointPixelYCoordLUT[queenPos.y-low] };

		sf::Vector2i diagonalLeftUpVec = sf::Vector2i{ -1, -1 };
		sf::VertexArray verticesDiagLeftUp(sf::PrimitiveType::Lines);
		sf::Vector2f vec = sf::Vector2f(lowPos - queenMiddlePointPixelPos);
		verticesDiagLeftUp.append({ vec, queenColor });
		verticesDiagLeftUp.append({ sf::Vector2f(queenMiddlePointPixelPos.x + diagonalLeftUpVec.x * -m_pixelAmount, queenMiddlePointPixelPos.y + diagonalLeftUpVec.y * -m_pixelAmount), queenColor });
		m_resultTexture.draw(verticesDiagLeftUp);


	}
	return true;
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
