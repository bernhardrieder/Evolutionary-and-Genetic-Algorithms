#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

int main()
{
	unsigned int queensAmount = 50;
	unsigned int queensPixel = 20;
	unsigned int gridPixel = 1;
	unsigned int pixelAmount = (queensAmount * (queensPixel + gridPixel))- gridPixel; // -gridPixel for the last one where no grid is needed
	sf::Vector2u textureDimension = { pixelAmount, pixelAmount };
	sf::RenderTexture renderTexture;

	if (!renderTexture.create(textureDimension.x, textureDimension.y))
	{
		std::cerr << "Cannot create rendertexture";
		getchar();
		return 0;
	}
	renderTexture.clear(sf::Color::White);

	//create grid
	{
		sf::VertexArray vertices(sf::PrimitiveType::Lines);
		for(int x = 0; x < queensAmount; ++x)
		{
			int xPos = x*queensPixel + x*gridPixel;
			sf::Vertex v1(sf::Vector2f(xPos, 0), sf::Color::Black);
			sf::Vertex v2(sf::Vector2f(xPos, pixelAmount), sf::Color::Black);
			vertices.append(v1);
			vertices.append(v2);
		}
		for (int y = 0; y < queensAmount; ++y)
		{
			int yPos = y*queensPixel + y*gridPixel;
			sf::Vertex v1(sf::Vector2f(0, yPos), sf::Color::Black);
			sf::Vertex v2(sf::Vector2f(pixelAmount, yPos), sf::Color::Black);
			vertices.append(v1);
			vertices.append(v2);
		}
		renderTexture.draw(vertices);
	}
	renderTexture.display();
	if (renderTexture.getTexture().copyToImage().saveToFile(std::to_string(queensAmount) + "_queens_puzzle.jpg"))
		std::cout << "created image on filesystem";

	return 0;
}