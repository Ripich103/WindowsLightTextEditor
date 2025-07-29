#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H
#include<iostream> // for debuging
#include<vector>
#include<string>
#include<utility>
#include<algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <stdexcept>
#include "RWFM.h"
#include <Windows.h>
#include <thread>
#include <string_view>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

class TE
{
private:
	sf::Clock SaveAsDelayClock;
	sf::Clock blinkingCursorDelayClock;
	sf::Clock CursorIdleTime;

	std::vector<std::string> lines;
	std::vector<sf::Text> Vtxt;
	std::vector<sf::Text> textLines;
	std::vector<sf::Text> lineNumbers;
	sf::Vector2f TextInitialPos;
	
	sf::Text text;

	sf::Texture bgTexture;

	sf::Color TextColor;
	sf::Color NumColor;
	sf::Color CursorColor;

	sf::RectangleShape BackGround;
	sf::Font font;
	sf::RectangleShape cursor;

	std::size_t cursorColumn;
	std::size_t cursorLine;
	std::size_t lastCharSizeX;
	std::size_t BiggestLineIdx;
	std::size_t FirstVisibleLineVtxt;
	std::size_t LastVisibleLineVtxt;

	int textSize;

	bool isSettingsOpened;
	bool isOpenFromFileOpened;
	bool isSaveAsOpened;
	bool ShowStar;
	bool transparentCursor;
	bool allowScrolling;
	bool allowVtxtConstructing;

	void OpenFromFIle();
	void SaveAs();
	void Save();

	
	std::string m_filename;
	const std::string version;

	RWFM ReadOrWriteModule;

	float lineSpacing;
	float numberSpacing;
	float textScaling;

	void getInput(const sf::Event& event);
	void scrollVertical(const sf::Event& event, sf::View& v, const sf::RenderWindow& w);
	void scrollHoryzontal(const sf::Event& event, sf::View& v, const sf::RenderWindow& w);
	void handleCursor(const std::vector<std::string>& t, sf::Keyboard::Scancode keyCode);
	void shiftUp();
	void shiftDown(std::size_t size);
	void shiftLeft();
	void shiftRight();
	void show_settings();
	void spritesMenu(tgui::Gui& gui);
	void colorMenu(tgui::Gui& gui);
	void setNewBgTexture();
	void setNewNumBgTexture();
	void setNewFont();
	void constructVtxt(const sf::View& v);
	void setCursorVisible();
	void setCursorInvisible();
	void resizeWindow(const sf::Event& event, sf::View& view, sf::RenderWindow& window);

	bool checkIfThereIsTxTEnding(const std::string& s);

	std::string show_SaveAsDialog() noexcept;
	std::string show_LoadFromDialog(std::string_view filter) noexcept;
public:
	TE();

	void Start();

	// dont need these
	TE(TE&& te) noexcept = delete;
	TE(const TE& te) = delete;
	TE& operator = (TE&& te) noexcept = delete;
	TE& operator = (const TE& te) = delete;
	~TE() = default;
};


#endif // !TEXTEDITOR_H