#include "TextEditor.h"

TE::TE() :  ShowStar(false), isSaveAsOpened(false), isOpenFromFileOpened(false), NumBGColor(sf::Color::White), NumColor(sf::Color(80, 80, 80)), TextColor(sf::Color::Black), isSettingsOpened(false), m_filename("Untitled.txt"), version("alpha"), BackGround({0.f, 0.f}), textSize(18),
font(), allowScrolling(false), cursor({ 0.f, 0.f }), Linespacing(28.f), cursorColumn(0), cursorLine(0), lastCharSizeX(0), cursorInControl(false), text(font)
{
    text.setCharacterSize(textSize);
    //Vtxt.resize(1);
    Vtxt.reserve(1);
    lines.resize(1);
    lines = { "" };
    ReadOrWriteModule.setFileName(m_filename);
    TextInitialPos = { 60.f, 10.f };
}

void TE::shiftUp(const sf::Text& t)
{
    if (cursorLine > 0)
        cursorLine -= 1;
}

void TE::shiftDown(const sf::Text& t, std::size_t size)
{
    if (cursorLine + 1 < size)
        cursorLine += 1;
}

void TE::shiftLeft(const sf::Text& t)
{
    if (cursorColumn > 0)
        cursorColumn -= 1;
}

void TE::shiftRight(const sf::Text& t)
{
    cursorColumn += 1;
}

bool TE::checkIfThereIsTxTEnding(const std::string& s)
{
    std::size_t i = s.find_last_of('.');
    if (i == std::string::npos)
        return false;

    std::string ending = s.substr(i);
    return ending == ".txt";
}

std::string TE::show_SaveAsDialog() noexcept
{
    char fileName[MAX_PATH] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;  
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Save As";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileNameA(&ofn)) {
        return std::string(fileName); // User selected a file
    }

    return "";
}

std::string TE::show_LoadFromDialog(const char* filter) noexcept
{
    char fileName[MAX_PATH] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;  
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Open From";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(fileName); // User selected a file
    }

    return "";
}

void TE::show_settings()
{
    isSettingsOpened = true;
    sf::RenderWindow settingsWindow(sf::VideoMode({ 1000, 500 }), "Settings");
    tgui::Gui gui(settingsWindow);

    colorMenu(gui);

    while (settingsWindow.isOpen())
    {
        while (const std::optional event = settingsWindow.pollEvent())
        {
            gui.handleEvent(*event);

            if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape))
            {
                gui.removeAllWidgets();
                settingsWindow.close();
            }
        }

        settingsWindow.clear(sf::Color::White);

        gui.draw();

        settingsWindow.display();
        isSettingsOpened = false;
    }
}

void TE::spritesMenu(tgui::Gui& gui)
{
    gui.removeAllWidgets();
    // cuz i want to have window have the size of a button
    auto SetNumBgSpriteButton = tgui::Button::create("Change numbers background image");
    
    gui.getWindow()->setSize({static_cast<unsigned int>(SetNumBgSpriteButton->getSize().x + 10),static_cast<unsigned int>(SetNumBgSpriteButton->getSize().y * 7 + 10)});

    auto SpritePickPanel = tgui::Panel::create({ (double)gui.getWindow()->getSize().x ,(double)gui.getWindow()->getSize().y });
    auto changeToColorButton = tgui::Button::create("Color menu");
    auto SetBgSprite = tgui::Button::create("Change background image");
    auto SetFont = tgui::Button::create("Change fonts");

    sf::Font N_font;

    gui.add(SpritePickPanel);
    SpritePickPanel->getRenderer()->setBackgroundColor({ 127, 127, 127, 255});
    SpritePickPanel->getRenderer()->setPadding(5);

    // adding to panel
    SpritePickPanel->add(changeToColorButton);
    SpritePickPanel->add(SetBgSprite);
    SpritePickPanel->add(SetNumBgSpriteButton);
    SpritePickPanel->add(SetFont);

    // setting position
    changeToColorButton->setWidth(SpritePickPanel->getSize().x);
    SetBgSprite->setPosition({ 0, SetBgSprite->getSize().y + changeToColorButton->getPosition().y });
    SetNumBgSpriteButton->setPosition({ 0, SetNumBgSpriteButton->getSize().y + SetBgSprite->getPosition().y });
    SetFont->setPosition({ 0, SetFont->getSize().y + SetNumBgSpriteButton->getPosition().y });

    // setting button function`s
    SetFont->onClick([&]() {
        setNewFont();
        });
    changeToColorButton->onPress([&]() {
        colorMenu(gui);
        });

    SetBgSprite->onPress([&]() {
        setNewBgTexture();
        });

    SetNumBgSpriteButton->onPress([&]()
        {
        setNewNumBgTexture();
        });

}

void TE::colorMenu(tgui::Gui& gui)
{
    gui.removeAllWidgets();

    auto ColorPickerPanel = tgui::Panel::create({ (double)gui.getWindow()->getSize().x + 100 ,(double)gui.getWindow()->getSize().y + 100 });
    auto BGColorPicker = tgui::ColorPicker::create("Set text background color", BackGround.getFillColor());
    auto TextColorPicker = tgui::ColorPicker::create("Set text color", TextColor);
    auto NumColorPicker = tgui::ColorPicker::create("Set number color", NumColor);
    auto NumBgColorPicker = tgui::ColorPicker::create("Set number background color", NumBGColor);
    auto changeToSpritesButton = tgui::Button::create("Sprites menu");

    ColorPickerPanel->setSize({ BGColorPicker->getSize().x * 2, BGColorPicker->getSize().y * 2 + changeToSpritesButton->getSize().y });
    gui.getWindow()->setSize( { (unsigned int)ColorPickerPanel->getSize().x, (unsigned int)ColorPickerPanel->getSize().y });

    gui.add(ColorPickerPanel);
    ColorPickerPanel->add(changeToSpritesButton);
    ColorPickerPanel->add(BGColorPicker);
    ColorPickerPanel->add(TextColorPicker);
    ColorPickerPanel->add(NumColorPicker);
    ColorPickerPanel->add(NumBgColorPicker);

    // configuring gui
    ColorPickerPanel->getRenderer()->setPadding(5);
    ColorPickerPanel->getRenderer()->setBackgroundColor({80, 80, 80, 255});
    

    BGColorPicker->setPositionLocked(true);
    TextColorPicker->setPositionLocked(true);
    NumColorPicker->setPositionLocked(true);
    NumBgColorPicker->setPositionLocked(true);

    BGColorPicker->onOkPress([=](const tgui::Color& color)
        {BackGround.setFillColor({ color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() });});
    TextColorPicker->onOkPress([=](const tgui::Color& color)
        {TextColor = { color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() };});
    NumColorPicker->onOkPress([=](const tgui::Color& color)
        {NumColor = { color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() };});
    NumBgColorPicker->onOkPress([=](const tgui::Color& color)
        {NumBGColor = { color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() };});

    changeToSpritesButton->setWidth(ColorPickerPanel->getSize().x);
    
    changeToSpritesButton->onPress([&]() {
        spritesMenu(gui);
        });

    TextColorPicker->setPosition(TextColorPicker->getPosition().x, changeToSpritesButton->getSize().y);
    BGColorPicker->setPosition(TextColorPicker->getPosition().x + BGColorPicker->getSize().x, TextColorPicker->getPosition().y + NumBgColorPicker->getSize().y);
    NumColorPicker->setPosition(TextColorPicker->getPosition().x + NumColorPicker->getSize().x, TextColorPicker->getPosition().y);
    NumBgColorPicker->setPosition(TextColorPicker->getPosition().x, TextColorPicker->getPosition().y + NumBgColorPicker->getSize().y);
}

void TE::setNewBgTexture()
{
    const char* filter = "Png files(*.png)\0*.PNG\0Jpeg files(*.jpeg)\0*.jpeg\0All files (*.*)\0*.*\0\0";
    const std::string location = show_LoadFromDialog(filter);

    if (location.size() > 0 && !bgTexture.loadFromFile(location))
    {
        throw std::runtime_error("Run time error can`t load from this file!");
        return;
    }
    
    BackGround.setTexture(&bgTexture);
}

void TE::setNewNumBgTexture()
{
}

void TE::setNewFont()
{
    const char* filter = "Font`s (*.ttf)\0 * .ttf\0All Files\0 * .*\0\0";
    std::string location = show_LoadFromDialog(filter);

    if (location.size() > 1 || GetFileAttributesA(location.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        if (!font.openFromFile(location))
        {
            MessageBoxA(NULL, "Something went wrong while trying to open ttf file", "ERROR", MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        MessageBoxA(NULL, "INVALID_FILE_ATTRIBUTES try with existing file", "ERROR", MB_OK | MB_ICONERROR);
    }
}

void TE::OpenFromFIle()
{
    isOpenFromFileOpened = true;

    // i need to ask the client from where should i read
    // i will use the win api way almost like
    // with save as
    const char* filter = "Text Files(*.txt)\0 * .txt\0All Files\0 * .*\0\0";
    std::string FromWhereToRead = show_LoadFromDialog(filter);
    if (GetFileAttributesA(FromWhereToRead.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        if (!FromWhereToRead.empty()) {
            m_filename = FromWhereToRead;
            ReadOrWriteModule.setFileName(m_filename);
            ReadOrWriteModule.ReadFile(); // reads from file and saves to buffer

            if (ReadOrWriteModule.getStatusCode() == RWFM::StatusCodes::STATUS_EMPTYBUF)
            {
                std::cout << "EMPTY BUF";
                lines.clear();
                lines = { "" };
            }
            else
            {
                lines = ReadOrWriteModule.getBuffer();
            }
            // now we need to adjsut cursorLine and cursorColumn
            // i will set them at 0 and 0
            // so the text in initial pos
            cursorColumn = 0;
            cursorLine = 0;
            text.setPosition(TextInitialPos);
            cursor.setPosition({ text.getPosition().x - text.getCharacterSize(), text.getPosition().y }); // also reset the cursor
        }
    }
    else
    {
        MessageBoxA(NULL, "INVALID_FILE_ATTRIBUTES try with existing file", "ERROR", MB_OK | MB_ICONERROR);
    }
    isOpenFromFileOpened = false;
}

void TE::SaveAs()
{
    // we should request new filename from user
    // for this i will use windows API 
    std::string newFileName = show_SaveAsDialog();
    if (!newFileName.empty()) {
    
        m_filename = newFileName;
        if (!checkIfThereIsTxTEnding(m_filename))
            m_filename += ".txt";
    
        ReadOrWriteModule.setFileName(m_filename);
    }
    else
    {
        // user refused to save the file
        return;
    }
    //std::cout << "#saved to " << filename << "!\n";
    ReadOrWriteModule.WriteToFile(lines);

    ShowStar = false;
}

void TE::Save()
{
    if (GetFileAttributesA(m_filename.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        // file exists
        ReadOrWriteModule.WriteToFile(lines);
        ShowStar = false;
    }
    else if(m_filename == "Untitled.txt" || m_filename != "Untitled.txt*")
    {
        SaveAs();
    }
}

void TE::getInput(const sf::Event& event)
{
    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>())
    {
        char typed = static_cast<char>(textEntered->unicode);

        if (typed == 8) // Backspace
        {
            if (!lines[cursorLine].empty())
            {
                if (cursorColumn > 0)
                {
                    cursorColumn--;
                    lines[cursorLine].erase(cursorColumn, 1);
                }
                else if (cursorLine > 0)
                {
                    // Merge with previous line
                    cursorColumn = lines[cursorLine - 1].size();
                    lines[cursorLine - 1] += lines[cursorLine];
                    lines.erase(lines.begin() + cursorLine);
                    cursorLine--;
                }

                allowScrolling = true;
                ShowStar = true;
            }
            else if (cursorColumn > 0)
            {
                cursorColumn--;
                lines[cursorLine].erase(cursorColumn, 1);
            }
            else if (cursorLine > 0)
            {
                // Merge with previous line
                cursorColumn = lines[cursorLine - 1].size();
                lines[cursorLine - 1] += lines[cursorLine];
                lines.erase(lines.begin() + cursorLine);
                cursorLine--;
            }

            allowScrolling = true;
            ShowStar = true;
        }
        else if (typed == 13 || typed == '\n') // Enter
        {
            std::string current = lines[cursorLine];
            std::string left = current.substr(0, cursorColumn);
            std::string right = current.substr(cursorColumn);

            lines[cursorLine] = left;
            lines.insert(lines.begin() + cursorLine + 1, right);

            cursorLine += 1;
            cursorColumn = 0;

            allowScrolling = true;
            ShowStar = true;
        }
        else if (typed >= 32 && typed < 127) // Printable ASCII
        {
            lines[cursorLine].insert(lines[cursorLine].begin() + cursorColumn, typed);
            cursorColumn += 1;

            allowScrolling = true;
            ShowStar = true;
        }
    }
}

void TE::scroll(const sf::Event& event, sf::View& v, const sf::RenderWindow& w)
{
    if (const auto* mouseWheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        float scrollAmount = -mouseWheelScrolled->delta * 60.f; // fix scroll direction
        v.move({ 0.f, scrollAmount });
    }

    sf::Vector2f center = v.getCenter();
    float halfViewHeight = v.getSize().y / 2.f;

    // Top scroll limit
    float minY = halfViewHeight;

    // --- Bottom scroll limit ---
    float maxY = minY; // fallback value
    if (!Vtxt.empty())
    {
        const auto& lastLine = Vtxt.back();
        float lastLineBottom = lastLine.getPosition().y + lastLine.getCharacterSize();
        maxY = std::max(minY, lastLineBottom - halfViewHeight + 20.f);
    }

    // Clamp the Y
    if (center.y < minY)
    {
        center.y = minY;
    }
    else if (center.y > maxY)
    {
        center.y = maxY;
    }

    v.setCenter(center);
}

void TE::handleCursor(const std::vector<sf::Text>& t, sf::Keyboard::Scancode keyCode)
{
    if (t.empty()) return;

    if (keyCode == sf::Keyboard::Scancode::Up)
    {
        shiftUp(t.back());
        std::cout << "Up";
    }
    else if (keyCode == sf::Keyboard::Scancode::Down)
    {
        shiftDown(t.back(), t.size());
        std::cout << "Down";
    }
    else if (keyCode == sf::Keyboard::Scancode::Left)
    {
        shiftLeft(t.back());
        std::cout << "Left";
    }
    else if (keyCode == sf::Keyboard::Scancode::Right)
    {
        shiftRight(t.back());
        std::cout << "Right";
    }
    cursorColumn = std::min(cursorColumn, t[cursorLine].getString().getSize());
    cursorLine = std::min(cursorLine, t.size() - 1);

    cursor.setPosition(t[cursorLine].findCharacterPos(cursorColumn));
}

void TE::Start()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "MLTE v" + version + " : " + m_filename, sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize, sf::State::Windowed);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    tgui::Gui TEgui(window);
    auto OptionButton = tgui::Button::create("Open settings.");
    auto SaveButton = tgui::Button::create("Save file.");
    auto OpenButton = tgui::Button::create("Open from file.");
    auto SaveAsButton = tgui::Button::create("Save as.");
    auto buttonPanel = tgui::Panel::create({ OptionButton->getSize().x * 3, OptionButton->getSize().y });

    TEgui.add(buttonPanel);
    buttonPanel->add(OptionButton);
    buttonPanel->add(SaveButton);
    buttonPanel->add(OpenButton);
    buttonPanel->add(SaveAsButton);

    OptionButton->onClick([&](){
        if (!isSettingsOpened)
        {
            show_settings();
        }
    });

    OpenButton->onClick([&]() {
        if(!isOpenFromFileOpened)
            OpenFromFIle();
        });

    SaveAsButton->onClick([&]() {
        SaveAs();
        });

    SaveButton->onClick([&]() {
        Save();
        });

    buttonPanel->setAutoLayout(tgui::AutoLayout::Bottom);

    SaveButton->setPosition({ OptionButton->getPosition().x + OptionButton->getSize().x, OptionButton->getPosition().y });
    SaveAsButton->setPosition({ SaveButton->getPosition().x + SaveAsButton->getSize().x, SaveButton->getPosition().y });
    OpenButton->setPosition({ SaveAsButton->getPosition().x + SaveAsButton->getSize().x, SaveAsButton->getPosition().y});

    BackGround.setSize({ (float)window.getSize().x, (float)window.getSize().y });
    BackGround.setPosition({ 50.f, 0.f });
    BackGround.setFillColor({ 100, 100, 100 });

    if (!font.openFromFile("Font\\pixel.ttf"))
    {
        return;
    }

    text.setPosition(TextInitialPos);
    text.setFillColor(sf::Color::Color::Black);

    float n{ 0 };

    sf::View view;
    view.setSize({ (float)window.getSize().x, (float)window.getSize().y });
    view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });


    cursor.setSize({ 2.f, (float)text.getCharacterSize() });
    cursor.setPosition({ text.getPosition().x - text.getCharacterSize(), text.getPosition().y });
    cursor.setFillColor(sf::Color::White);
    bool transparentCursor = true;
    cursorLine = 0;
    cursorColumn = 0;

    std::string WindowTitle = "MLTE v" + version + " : " + m_filename;
    std::string strapedFilename = m_filename.substr(m_filename.find_last_of('\\') + 1);

    //--------/init clocks\--------
    clock.start(); // using for blinking cursor animation

    constexpr float FileLastCallDelay = 0.5f;
    
    clock2.start(); // using for ctrl + s lock and unlock

    //clock3.start(); // using for ctrl + o lock and unlock
    //-----------------------------

    std::string previous_filename = m_filename;

    //---------/main-loop\---------
    while (window.isOpen())
    {
        // if user changed the file name
        if (previous_filename != m_filename)
        {
            previous_filename = m_filename;

            strapedFilename = m_filename.substr(m_filename.find_last_of('\\') + 1);
            window.setTitle("MLTE v" + version + " : " + strapedFilename);
        }
        if (ShowStar)
        {
            window.setTitle("MLTE v" + version + " : " + '*' + strapedFilename);
        }
        else
        {
            window.setTitle("MLTE v" + version + " : " + strapedFilename);
        }
        while (const std::optional event = window.pollEvent())
        {
            TEgui.handleEvent(*event);

            if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape))
            {
                TEgui.removeAllWidgets();
                window.close();
            }
            if (event->is<sf::Event::Resized>())
            {   // Update view size, preserve center
                sf::Vector2f center = view.getCenter();
                view.setSize({ (float)window.getSize().x, (float)window.getSize().y });
                view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });

                window.setView(view);

                // Also resize background
                BackGround.setSize({ (float)view.getSize().x, (float)view.getSize().y });
            }
            if (event->is<sf::Event::MouseWheelScrolled>())
            {
                scroll(*event, view, window);
                window.setView(view);
            }

            getInput(*event);
        }

        // LShift + Lctrl + s \ RShift + Rctrl + s 
        // save as
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && !isSaveAsOpened) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift) && 
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl) 
                    && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) && !isSaveAsOpened)
        {
            SaveAs();
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && clock2.getElapsedTime().asSeconds() >= FileLastCallDelay) ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && clock2.getElapsedTime().asSeconds() >= FileLastCallDelay))
        {
            clock2.restart();
            Save();
        }

        // Lctr + O \ Rctrl + O
        // load from file
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::O) && !isOpenFromFileOpened) ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::O) && !isOpenFromFileOpened) )
        {
            OpenFromFIle();
        }
        // ctr + p parameters
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::P)) && !isSettingsOpened ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::P)) && !isSettingsOpened)
        {
            show_settings();
        }

        // Prepare texts
        std::vector<sf::Text> textLines;
        std::vector<sf::Text> lineNumbers;
        float lineSpacing = text.getLineSpacing() + 23.f;

        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            // Line number
            sf::Text num(font);
            num.setCharacterSize(textSize);
            num.setString(std::to_string(i + 1));
            num.setFillColor(NumColor);
            num.setPosition({ 10.f, 10.f + i * lineSpacing });
            lineNumbers.push_back(num);

            sf::FloatRect f = num.getGlobalBounds();

            n = std::max(n, f.size.x);

            // Actual text
            sf::Text t(font);
            t.setString(lines[i]);
            t.setCharacterSize(textSize);
            t.setFillColor(TextColor);
            t.setPosition({ TextInitialPos.x, TextInitialPos.y + i * lineSpacing });
            textLines.push_back(t);
        }

        Vtxt = std::move(textLines);
        lastCharSizeX = 0;
        if (!Vtxt.empty()) {
            const auto& lastText = Vtxt.back();
            std::size_t len = lastText.getString().getSize();
            if (len > 0) {
                sf::Vector2f lastCharPos = lastText.findCharacterPos(len);
                sf::Vector2f prevCharPos = lastText.findCharacterPos(len - 1);
                lastCharSizeX = static_cast<std::size_t>(lastCharPos.x - prevCharPos.x);
            }
        }

        //BackGround.setPosition({ n + lineSpacing, 0.0f });

        if (!Vtxt.empty() && allowScrolling == true)
        {
            float viewTop = view.getCenter().y - view.getSize().y / 2.f;
            float viewBottom = view.getCenter().y + view.getSize().y / 2.f;

            const auto& lastLine = Vtxt.back();
            float lastLineBottom = lastLine.getPosition().y + lastLine.getCharacterSize();
            if (lastLineBottom > viewBottom - 10.f)
            {
                sf::Vector2f center = view.getCenter();
                center.y += lineSpacing;
                view.setCenter(center);
            }
            allowScrolling = false;
        }

        window.setView(view); // Apply updated view

        //BackGround.setSize({ BackGround.getSize().x, std::max(BackGround.getSize().y, (lines.size() * lineSpacing + 20.f) * 2.f) });

        BackGround.setPosition({view.getCenter().x - BackGround.getSize().x / 2.f + n + lineSpacing, view.getCenter().y - BackGround.getSize().y / 2.f });

        for (auto& e : Vtxt)
        {
            float tempY = e.getPosition().y;
            e.setPosition({ n + lineSpacing + 10.f, tempY });
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up))
        {
            cursorInControl = true;
            handleCursor(Vtxt, sf::Keyboard::Scancode::Up);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))
        {
            cursorInControl = true;
            handleCursor(Vtxt, sf::Keyboard::Scancode::Down);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left))
        {
            cursorInControl = true;
            handleCursor(Vtxt, sf::Keyboard::Scancode::Left);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right))
        {
            cursorInControl = true;
            handleCursor(Vtxt, sf::Keyboard::Scancode::Right);
        }
        else
        {
            cursorInControl = false;
            handleCursor(Vtxt, sf::Keyboard::Scancode::Unknown);
        }

        window.clear(NumBGColor);
        window.draw(BackGround);

        for (const auto& num : lineNumbers)
            window.draw(num);

        for (const auto& lineText : Vtxt)
            window.draw(lineText);

        window.draw(cursor);

        TEgui.draw();

        window.display();

        // cursor blinking animation
        if (clock.getElapsedTime().asSeconds() >= 0.5f && !transparentCursor)
        {
            cursor.setFillColor(sf::Color::Transparent);
            clock.restart();
            transparentCursor = true;
        }
        else if (clock.getElapsedTime().asSeconds() >= 0.5f && transparentCursor)
        {
            cursor.setFillColor(sf::Color::White);
            clock.restart();
            transparentCursor = false;
        }
    }
    //-----------------------------

    return;
}