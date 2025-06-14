#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H
#include<iostream> // for debuging
#include<vector>
#include<string>
#include<utility>
#include<algorithm>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "RWFM.h"
#include <Windows.h>
#include <commdlg.h>



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

	std::size_t cursorColumn;
	std::size_t cursorLine;
	std::size_t lastCharSizeX;

	sf::Vector2f TextInitialPos;

	std::string filename;

	RWFM ReadOrWriteModule;

	bool cursorInControl;

	float Linespacing;

	void getInput(const sf::Event& event);
	void scroll(const sf::Event& event, sf::View& v, const sf::RenderWindow& w);
	void handleCursor(const std::vector<sf::Text>& t, sf::Keyboard::Scancode keyCode);
	void shiftUp(const sf::Text& t);
	void shiftDown(const sf::Text& t, std::size_t size);
	void shiftLeft(const sf::Text& t);
	void shiftRight(const sf::Text& t);

	bool checkIfThereIsTxTEnding(const std::string& s);

	std::string show_SaveAsDialog() noexcept;

	std::string show_LoadFromDialog() noexcept;

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