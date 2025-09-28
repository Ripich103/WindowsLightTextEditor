#ifndef TEXTEDITOR_HPP
#define TEXTEDITOR_HPP

// for refactoring add member prefix - 'm_...'

#include<iostream> // for debuging
#include<vector>
#include<string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "../read_write_file_module/RWFM.hpp"
#include <string_view>
#include <Windows.h>
#include <WinUser.h>
#include <list>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#define DEBUG 1

class TE
{
private:
	using ui = unsigned int;
	sf::Clock SaveAsDelayClock;
	sf::Clock blinkingCursorDelayClock;
	sf::Clock CursorIdleTime;

	std::vector<std::string> lines;
	std::vector<sf::Text> Vtxt;
	std::vector<sf::Text> lineNumbers;
	sf::Vector2f TextInitialPos;

	std::list<sf::RectangleShape> m_selectedLines_list;
	
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

	std::size_t m_begin_selected_x;
	std::size_t m_end_selected_x;
	std::size_t m_begin_selected_y;
	std::size_t m_end_selected_y;

	int textSize;
	
	bool isSettingsOpened;
	bool isOpenFromFileOpened;
	bool isSaveAsOpened;
	bool ShowStar;
	bool transparentCursor;
	bool allowScrolling;
	bool allowVtxtConstructing;
	bool m_selected;
	bool m_stopedSelecting; // bottleneck for cursor

	void OpenFromFIle();
	void SaveAs();
	void Save();

	
	std::string m_filename;
	std::string m_selected_area;

	const std::string version;

	RWFM ReadOrWriteModule;

	float lineSpacing;
	float numberSpacing;
	float textScaling;
	float m_textSpacing;
	float m_char_width;
	float m_line_height;

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
	void setNewFont();
	void constructVtxt(const sf::View& v);
	void setCursorVisible();
	void setCursorInvisible();
	void resizeWindow(const sf::Event& event, sf::View& view, sf::RenderWindow& window);
	void pasteFromClipBoard();
	void copyToClipboard();

	bool checkIfThereIsTxTEnding(const std::string& s);

	std::string show_SaveAsDialog() noexcept;
	std::string show_LoadFromDialog(std::string_view filter) noexcept;

	std::string getSelectedString(std::size_t Sx, std::size_t Sy, std::size_t Ex, std::size_t Ey);
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