#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H
#include<iostream> // for debuging
#include<vector>
#include<string>
#include<utility>
#include<algorithm>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

class TE
{
private:
	std::vector<std::string> lines;
	std::vector<sf::Text> Vtxt;
	const std::string version;
	sf::Text text;

	sf::RectangleShape BackGround;
	int textSize;
	sf::Font font;
	bool allowScrolling;
	sf::RectangleShape cursor;

	float Linespacing;

	void getInput(const sf::Event& event);
	void scroll(const sf::Event& event, sf::View& v,const sf::RenderWindow& w);
	void handleCursor(const std::vector<sf::Text>& t);
public:
	TE();


	void Start();

	// dont need these
	TE(TE&& te) = delete;
	TE(const TE& te) = delete;
	TE& operator = (TE&& te) = delete;
	TE& operator = (const TE& te) = delete;
	~TE() = default;
};


#endif // !TEXTEDITOR_H