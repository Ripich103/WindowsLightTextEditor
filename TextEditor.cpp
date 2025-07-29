#include "TextEditor.h"

TE::TE() : FirstVisibleLineVtxt(0), LastVisibleLineVtxt(0), allowVtxtConstructing(true), textScaling(1.f), BiggestLineIdx(0), transparentCursor(true), ShowStar(false), isSaveAsOpened(false), isOpenFromFileOpened(false), CursorColor(sf::Color::White), NumColor(sf::Color::White), TextColor(sf::Color::White), isSettingsOpened(false), m_filename("Untitled.txt"), version("alpha"), BackGround({0.f, 0.f}), textSize(18),
font(), allowScrolling(false), cursor({ 0.f, 0.f }), lineSpacing(0), numberSpacing(0), cursorColumn(0), cursorLine(0), lastCharSizeX(0), text(font)
{
    text.setCharacterSize(textSize);
    Vtxt.reserve(1);
    lines.resize(1);
    lines = { "" };
    ReadOrWriteModule.setFileName(m_filename);
    TextInitialPos = { 50.f, 10.f };
}

void TE::shiftUp()
{
    if (cursorLine > 0)
        cursorLine -= 1;
}

void TE::shiftDown(std::size_t size)
{
    if (cursorLine + 1 < size)
        cursorLine += 1;
}

void TE::shiftLeft()
{
    if (cursorColumn > 0)
        cursorColumn -= 1;
}

void TE::shiftRight()
{
    cursorColumn += 1;
}

bool TE::checkIfThereIsTxTEnding(const std::string& s)
{
    const std::size_t i = s.find_last_of('.');
    if (i == std::string::npos)
        return false;

    std::string_view ending = std::string_view(s).substr(i);
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

    return std::string("");
}

std::string TE::show_LoadFromDialog(std::string_view filter) noexcept
{
    char fileName[MAX_PATH] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;  
    ofn.lpstrFilter = filter.data();
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Open From";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(fileName); // User selected a file
    }

    return std::string("");
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
    auto CursorColorPicker = tgui::ColorPicker::create("Set cursor color", CursorColor);
    auto changeToSpritesButton = tgui::Button::create("Sprites menu");

    ColorPickerPanel->setSize({ BGColorPicker->getSize().x * 2, BGColorPicker->getSize().y * 2 + changeToSpritesButton->getSize().y });
    gui.getWindow()->setSize( { (unsigned int)ColorPickerPanel->getSize().x, (unsigned int)ColorPickerPanel->getSize().y });

    gui.add(ColorPickerPanel);
    ColorPickerPanel->add(changeToSpritesButton);
    ColorPickerPanel->add(BGColorPicker);
    ColorPickerPanel->add(TextColorPicker);
    ColorPickerPanel->add(NumColorPicker);
    ColorPickerPanel->add(CursorColorPicker);

    // configuring gui
    ColorPickerPanel->getRenderer()->setPadding(5);
    ColorPickerPanel->getRenderer()->setBackgroundColor({80, 80, 80, 255});
    

    BGColorPicker->setPositionLocked(true);
    TextColorPicker->setPositionLocked(true);
    NumColorPicker->setPositionLocked(true);
    CursorColorPicker->setPositionLocked(true);

    BGColorPicker->onOkPress([=](const tgui::Color& color)
        {BackGround.setFillColor({ color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() });});
    TextColorPicker->onOkPress([=](const tgui::Color& color)
        {TextColor = { color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() };});
    NumColorPicker->onOkPress([=](const tgui::Color& color)
        {NumColor = { color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() };});
    CursorColorPicker->onOkPress([=](const tgui::Color& color)
        {CursorColor = { color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() };});

    changeToSpritesButton->setWidth(ColorPickerPanel->getSize().x);
    
    changeToSpritesButton->onPress([&]() {
        spritesMenu(gui);
        });

    TextColorPicker->setPosition(TextColorPicker->getPosition().x, changeToSpritesButton->getSize().y);
    BGColorPicker->setPosition(TextColorPicker->getPosition().x + BGColorPicker->getSize().x, TextColorPicker->getPosition().y + CursorColorPicker->getSize().y);
    NumColorPicker->setPosition(TextColorPicker->getPosition().x + NumColorPicker->getSize().x, TextColorPicker->getPosition().y);
    CursorColorPicker->setPosition(TextColorPicker->getPosition().x, TextColorPicker->getPosition().y + CursorColorPicker->getSize().y);
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
    const std::string_view filter = "Font`s (*.ttf)\0 * .ttf\0All Files\0 * .*\0\0";
    const std::string location = show_LoadFromDialog(filter);

    if (location.size() > 1 || GetFileAttributesA(location.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        if (!font.openFromFile(location))
        {
            MessageBoxA(NULL, "Something went wrong while trying to open ttf file", "ERROR", MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        MessageBoxA(NULL, "INVALID_FILE_ATTRIBUTES maybe you declined", "ERROR", MB_OK | MB_ICONERROR);
    }
}

void TE::constructVtxt(const sf::View& v)
{
    // Prepare texts
    lineNumbers.clear();
    textLines.clear();
    Vtxt.clear();
    lineNumbers.reserve(lines.size());
    textLines.reserve(lines.size());
    Vtxt.reserve(lines.size());

    BiggestLineIdx = 0;
    lineSpacing = text.getLineSpacing() + 23.f;

    std::size_t tempidx = 0;
    float lineSize = 0;
    
    const float viewTop = v.getCenter().y - v.getSize().y / 2.f;
    const float viewBottom = v.getCenter().y + v.getSize().y / 2.f;

    FirstVisibleLineVtxt = std::max(0, (int)((viewTop - TextInitialPos.y) / (lineSpacing * textScaling)));
    LastVisibleLineVtxt = std::min(lines.size(), (std::size_t)((viewBottom - TextInitialPos.y) / (lineSpacing * textScaling)) + 5);

    for (std::size_t i = FirstVisibleLineVtxt; i < LastVisibleLineVtxt; ++i)
    {
        // Line number
        sf::Text num(font);
        num.setCharacterSize(textSize);
        num.setString(std::to_string(i + 1));
        num.setFillColor(NumColor);
        num.setScale({textScaling, textScaling});
        num.setPosition({ 10.f, 10.f + i * lineSpacing * textScaling });
        lineNumbers.push_back(num);

        const sf::FloatRect f = num.getGlobalBounds();

        if (textScaling == 0.9f || textScaling == 1.f)
            numberSpacing = std::max(numberSpacing, f.size.x);
        else
            numberSpacing = f.size.x;

        sf::Text t(font);
        t.setString(lines[i]);
        t.setPosition({ TextInitialPos.x, TextInitialPos.y + i * lineSpacing * textScaling });
        t.setCharacterSize(textSize);
        t.setFillColor(TextColor);
        t.setScale({textScaling, textScaling});
        const float size = t.getGlobalBounds().size.x;
        textLines.push_back(t);

        const float oldLineSize = lineSize;
        lineSize = std::max(lineSize, size);

        if (oldLineSize != lineSize)
        {
            tempidx = i;
        }
    }

    BiggestLineIdx = tempidx;

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
}

void TE::setCursorVisible()
{
    cursor.setFillColor(CursorColor);
    transparentCursor = false;
}

void TE::setCursorInvisible()
{
    cursor.setFillColor(sf::Color::Transparent);
    transparentCursor = true;
}

void TE::resizeWindow(const sf::Event& event, sf::View& view, sf::RenderWindow& window)
{
    sf::Vector2f center = view.getCenter();
    view.setSize({ (float)window.getSize().x, (float)window.getSize().y });
    view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });

    window.setView(view);

    BackGround.setSize({ (float)view.getSize().x, (float)view.getSize().y });
}

void TE::OpenFromFIle()
{
    isOpenFromFileOpened = true;

    // i need to ask the client from where should i read
    // i will use the win api way almost like
    // with save as
    const char* filter = "Text Files(*.txt)\0 * .txt\0All Files\0 * .*\0\0";
    const std::string FromWhereToRead = show_LoadFromDialog(filter);
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
            allowVtxtConstructing = true;
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
        CursorIdleTime.restart();
        setCursorVisible();

        char typed = static_cast<char>(textEntered->unicode);
        if (typed == 8) // Backspace
        {
            if (cursorColumn > 0)
            {
                cursorColumn--;
                lines[cursorLine].erase(cursorColumn, 1);
                allowVtxtConstructing = true;
            }
            else if (cursorLine > 0)
            {
                // Merge with previous line
                cursorColumn = lines[cursorLine - 1].size();
                lines[cursorLine - 1] += lines[cursorLine];
                lines.erase(lines.begin() + cursorLine);
                cursorLine--;
                allowVtxtConstructing = true;
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
            allowVtxtConstructing = true;
            ShowStar = true;
        }
        else if (typed >= 32 && typed < 127) // Printable ASCII
        {
            lines[cursorLine].insert(lines[cursorLine].begin() + cursorColumn, typed);
            cursorColumn += 1;
            allowVtxtConstructing = true;
            allowScrolling = true;
            ShowStar = true;
        }
        
    }
}

void TE::scrollHoryzontal(const sf::Event& event, sf::View& v, const sf::RenderWindow& w)
{
    if (const auto* mouseWheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        const float scrollAmount = -mouseWheelScrolled->delta * 60.f;
        v.move({ scrollAmount, 0.f });
        allowScrolling = false;
    }

    sf::Vector2f center = v.getCenter();
    const float ViewLeftBorder = v.getSize().x / 2.f;

    float minX = ViewLeftBorder;

    float max_X = minX;
    if (!Vtxt.empty())
    {
        const sf::Text& LineWithBiggestX = Vtxt[BiggestLineIdx];
        const float lastLineWidth = LineWithBiggestX.getPosition().x + LineWithBiggestX.getGlobalBounds().size.x;
        max_X = std::max(max_X, lastLineWidth - ViewLeftBorder + 20.f);
    }

    if (center.x < minX)
    {
        center.x = minX;
    }
    if (center.x > max_X)
    {
        center.x = max_X;
    }

    v.setCenter(center);
}

void TE::scrollVertical(const sf::Event& event, sf::View& v, const sf::RenderWindow& w)
{
    if (const auto* mouseWheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        float scrollAmount = -mouseWheelScrolled->delta * 60.f; // fix scroll direction
        v.move({ 0.f, scrollAmount });
        allowScrolling = false;
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

void TE::handleCursor(const std::vector<std::string>& t, sf::Keyboard::Scancode keyCode)
{
    CursorIdleTime.restart();
    setCursorVisible();
    if (t.empty()) return;
    if (keyCode == sf::Keyboard::Scancode::Up)
    {
        shiftUp();
    }
    else if (keyCode == sf::Keyboard::Scancode::Down)
    {
        shiftDown(t.size());
    }
    else if (keyCode == sf::Keyboard::Scancode::Left)
    {
        shiftLeft();
    }
    else if (keyCode == sf::Keyboard::Scancode::Right)
    {
        shiftRight();
    }
    cursorColumn = std::min(cursorColumn, t[cursorLine].size());
    cursorLine = std::min(cursorLine, t.size() - 1);
}

void TE::Start()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "MLTE v" + version + " : " + m_filename, sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize, sf::State::Windowed);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    sf::View view;
    int viewZoomModifier = 0;
    view.setSize({ (float)window.getSize().x, (float)window.getSize().y });
    view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });

    tgui::Gui TEgui(window);
    auto OptionButton = tgui::Button::create("Open settings.");
    auto SaveButton = tgui::Button::create("Save file.");
    auto OpenButton = tgui::Button::create("Open from file.");
    auto SaveAsButton = tgui::Button::create("Save as.");
    auto buttonPanel = tgui::Panel::create({ OptionButton->getSize().x * 3, OptionButton->getSize().y });

    auto VerticalScrollBar = tgui::Scrollbar::create(tgui::Orientation::Vertical);

    TEgui.add(buttonPanel);
    TEgui.add(VerticalScrollBar);
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

    VerticalScrollBar->setPolicy(tgui::Scrollbar::Policy::Automatic);
    VerticalScrollBar->setHeight(static_cast<unsigned int>(BackGround.getSize().y));
    VerticalScrollBar->setScrollAmount(static_cast<unsigned int>(lastCharSizeX));
    VerticalScrollBar->setPosition({ "&.width - 18", "0" });
    VerticalScrollBar->setSize({ 18, "100%" });
    VerticalScrollBar->setMaximum(static_cast<unsigned int>(Vtxt.size()));
    VerticalScrollBar->setValue(0);     
    VerticalScrollBar->getRenderer()->setThumbColor(tgui::Color::Black);

    VerticalScrollBar->onValueChange([&](unsigned int val) {
        const float minY = view.getSize().y / 2.f;
        const float newY = val + minY;
        float centerY = newY;
        view.setCenter({ view.getCenter().x, centerY });
        window.setView(view);
        allowVtxtConstructing = true;
        });


    BackGround.setSize({ (float)window.getSize().x, (float)window.getSize().y });
    BackGround.setPosition({ 0.f, 0.f });
    BackGround.setFillColor(sf::Color::Black);

    if (!font.openFromFile("Font\\pixel.ttf"))
    {
        return;
    }

    text.setPosition(TextInitialPos);
    text.setFillColor(sf::Color::White);

    cursor.setSize({ 2.f, (float)text.getCharacterSize() });
    cursor.setPosition({ text.getPosition().x - text.getCharacterSize(), text.getPosition().y });
    cursor.setFillColor(sf::Color::White);

    std::string WindowTitle = "MLTE v" + version + " : " + m_filename;
    std::string strapedFilename = m_filename.substr(m_filename.find_last_of('\\') + 1);

    //--------/init clocks\--------
    blinkingCursorDelayClock.start(); // using for blinking cursor animation

    constexpr float FileLastCallDelay = 0.5f;
    
    SaveAsDelayClock.start(); // using for ctrl + s lock and unlock

    CursorIdleTime.start();
    //-----------------------------

    std::string previous_filename = m_filename;
    constexpr const float zoomInVal = 0.9f;
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
            {
                resizeWindow(*event, view, window);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Equal))
            {
                if (textScaling + 0.1f < 3.f)
                {
                    textScaling += 0.1f;
                    allowVtxtConstructing = true;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Hyphen))
            {
                if (textScaling - 0.1f >= 0.f)
                {
                    textScaling -= 0.1f;
                    allowVtxtConstructing = true;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::R))
            {
                textScaling = 1.f;
                allowVtxtConstructing = true;
            }
            if (event->is<sf::Event::MouseWheelScrolled>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift))
            {
                scrollHoryzontal(*event, view, window);
                window.setView(view);
                allowVtxtConstructing = true;
            }
            else if (event->is<sf::Event::MouseWheelScrolled>())
            {
                scrollVertical(*event, view, window);
                window.setView(view);
                allowVtxtConstructing = true;
            }
            
            if (event->is<sf::Event::KeyPressed>()) {
                const auto sc = event->getIf<sf::Event::KeyPressed>()->scancode;
                if (sc == sf::Keyboard::Scancode::Up || sc == sf::Keyboard::Scancode::Down ||
                    sc == sf::Keyboard::Scancode::Left || sc == sf::Keyboard::Scancode::Right)
                {
                    allowScrolling = true;
                    handleCursor(lines, sc);
                }
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
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && SaveAsDelayClock.getElapsedTime().asSeconds() >= FileLastCallDelay) ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && SaveAsDelayClock.getElapsedTime().asSeconds() >= FileLastCallDelay))
        {
            SaveAsDelayClock.restart();
            Save();
        }

        // Lctr + O \ Rctrl + O
        // load from file
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::O) && !isOpenFromFileOpened) ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::O) && !isOpenFromFileOpened))
        {
            OpenFromFIle();
        }
        // ctr + p parameters
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::P)) && !isSettingsOpened ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::P)) && !isSettingsOpened)
        {
            show_settings();
        }
        if(allowVtxtConstructing)
        {
            constructVtxt(view);
            allowVtxtConstructing = false; 
        }

        const float visibleHeight = view.getSize().y;

        if (!lines.empty()) {
            VerticalScrollBar->setMaximum(static_cast<int>(lines.size() * lineSpacing * textScaling + 23.f));
        }

        VerticalScrollBar->setViewportSize(static_cast<unsigned int>(visibleHeight));
        VerticalScrollBar->setValue(static_cast<unsigned int>(view.getCenter().y - visibleHeight / 2.f));
        
        const float viewTop = view.getCenter().y - view.getSize().y / 2.f;
        const float viewBottom = view.getCenter().y + view.getSize().y / 2.f;
        const float viewLeft = view.getCenter().x - view.getSize().x / 2.f;
        const float viewRight = view.getCenter().x + view.getSize().x / 2.f;

        if (allowScrolling)
        {
            const float cursorTop    = cursor.getPosition().y;
            const float cursorBottom = cursor.getPosition().y + cursor.getSize().y;
            const float cursorRight  = cursor.getSize().x + cursor.getPosition().x;
            const float cursorLeft   = cursor.getPosition().x - cursor.getSize().x;

            if (cursorTop < viewTop)
            {
                sf::Vector2f center = view.getCenter();
                center.y -= (viewTop - cursorTop);
                view.setCenter(center);

                allowScrolling = false;
            }
            else if (cursorBottom > viewBottom)
            {
                sf::Vector2f center = view.getCenter();
                center.y += (cursorBottom - viewBottom) + lineSpacing;
                view.setCenter(center);

                allowScrolling = false;
            }
            if (cursorRight > viewRight)
            {
                sf::Vector2f center = view.getCenter();
                center.x -= (viewRight - cursorRight) - lastCharSizeX;
                view.setCenter(center);

                allowScrolling = false;
            }
            if (cursorLeft < viewLeft)
            {
                sf::Vector2f center = view.getCenter();
                center.x -= (viewLeft - cursorLeft);
                if (center.x <= window.getSize().x)
                    center.x = window.getSize().x / 2.f;

                view.setCenter(center);

                allowScrolling = false;
            }
        }
        window.clear();

        window.setView(view);

        BackGround.setPosition
        ({
          view.getCenter().x - BackGround.getSize().x / 2.f,
          view.getCenter().y - BackGround.getSize().y / 2.f
            });

        window.draw(BackGround);

        for (const auto& num : lineNumbers)
        {
            const float numTopCord = num.getPosition().y;
            const float numRightCord = num.getPosition().x + num.getLocalBounds().size.x;
            const float numBottomCord = numTopCord + num.getLocalBounds().size.y;
            if (numBottomCord > viewTop && numTopCord < viewBottom && numRightCord > viewLeft)
            {
                window.draw(num);
            }
        }

        for (auto& lineText : Vtxt)
        {
            float tempY = lineText.getPosition().y;
            lineText.setPosition({ numberSpacing + lineSpacing, tempY });

            const float TextTopCord = lineText.getPosition().y;
            const float TextBottomCord = TextTopCord + lineText.getLocalBounds().size.y;
            const float TextRightCord = lineText.getGlobalBounds().size.x;

            if (TextBottomCord > viewTop && TextTopCord < viewBottom)
            {
                if (TextRightCord > viewRight)
                {
                    sf::Text Temp(lineText);
                    std::string accumulativeStr = "";

                    for (const char& c : lineText.getString())
                    {
                        accumulativeStr += c;
                        Temp.setString(accumulativeStr);
                        if (Temp.getGlobalBounds().size.x >= viewRight)
                        {
                            break;
                        }
                    }

                    const float TempTop = Temp.getPosition().y;
                    const float TempBot = TempTop + Temp.getLocalBounds().size.x;
                    window.draw(Temp);
                }
                else
                {
                    window.draw(lineText);
                }
            }
        }

        // if in bounds
        if (cursorLine >= FirstVisibleLineVtxt && cursorLine < LastVisibleLineVtxt)
        {
            const std::size_t relativeIndex = cursorLine - FirstVisibleLineVtxt;
            if (relativeIndex < Vtxt.size())
                cursor.setPosition(Vtxt[relativeIndex].findCharacterPos(cursorColumn));
        }
        else
        {
            // Fallback for safety: estimate position even if line is offscreen
            const float fallbackY = TextInitialPos.y + cursorLine * lineSpacing * textScaling;
            const float fallbackX = TextInitialPos.x + cursorColumn * lastCharSizeX;
            cursor.setPosition({ fallbackX, fallbackY });
        }

        cursor.setScale({ textScaling, textScaling });
        window.draw(cursor);

        TEgui.draw();

        window.display();

        // cursor blinking
        if (CursorIdleTime.getElapsedTime().asSeconds() > 0.15f)
        {
            if (blinkingCursorDelayClock.getElapsedTime().asSeconds() >= 0.5f && !transparentCursor)
            {
                setCursorInvisible();
                blinkingCursorDelayClock.restart();
            }
            else if (blinkingCursorDelayClock.getElapsedTime().asSeconds() >= 0.5f && transparentCursor)
            {
                setCursorVisible();
                blinkingCursorDelayClock.restart();
            }

            CursorIdleTime.restart();
        }
    }
    //-----------------------------

    return;
}