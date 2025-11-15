#include "TextEditor.hpp"

TE::TE() :   m_textSpacing(2.f), FirstVisibleLineVtxt(0), LastVisibleLineVtxt(0),
             allowVtxtConstructing(true), textScaling(1.f), BiggestLineIdx(0),
             transparentCursor(true), ShowStar(false), isSaveAsOpened(false), isOpenFromFileOpened(false),
             CursorColor(sf::Color::White), NumColor(sf::Color::White),
             TextColor(sf::Color::White), isSettingsOpened(false), m_filename("Untitled.txt"), version("alpha"),
             BackGround({ 0.f, 0.f }), textSize(16),
             font(), allowScrolling(false), cursor({ 0.f, 0.f }), lineSpacing(23.f), numberSpacing(0),
             cursorColumn(0), cursorLine(0), lastCharSizeX(0), text(font), m_selected(false), m_begin_selected_x(0),
             m_end_selected_x(0), m_begin_selected_y(0), m_end_selected_y(0), m_selected_area(), m_selectedLines_list(),
             m_char_width(0.f), m_line_height(0.f), m_stopedSelecting(true), c_cfg_buffer(nullptr), m_CFG_MODULE(), m_cfgmap(),
             m_BackGroundColor(sf::Color::Black), m_CursorSize(2.f, static_cast<float>(textSize)), m_BG_picpath(""),
             m_Fontpath("font\\Consolas.ttf"), m_LastUsedBgColor(), m_BottomPanelHeight(0.f), tabSpaces(0), m_preffered_cfg()
{
    text.setCharacterSize(textSize);
    Vtxt.reserve(1);
    lines.resize(1);
    lines = { "" };
    ReadOrWriteModule.setFileName(m_filename);
    TextInitialPos = { 130.f, 10.f };
    cfg_settings = { &m_Fontpath, &textSize, &textScaling, &m_CursorSize, &m_BG_picpath, &CursorColor, &NumColor, &TextColor, &m_BackGroundColor, &tabSpaces, &m_preffered_cfg};

    loadCFG(CE_STARTUP_NAME, 0);
    loadCFG(m_preffered_cfg, 1);
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

// if failed returns "".
// if succed returns selected filepath.
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

// if failed returns "".
// if succed returns selected filepath.
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
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        std::cout << "OPEN FILE " << fileName << '\n';
        return std::string(fileName); // User selected a file
    }

    return "";
}

std::string TE::getSelectedString(std::size_t Sx, std::size_t Sy, std::size_t Ex, std::size_t Ey)
{
    m_selectedLines_list.clear();
    std::string selected_str;

    if (Sy > Ey || (Sy == Ey && Sx > Ex))
    {
        std::swap(Sx, Ex);
        std::swap(Sy, Ey);
    }

    //int size = std::abs(static_cast<int>(Sx) - static_cast<int>(Ex));
    for (int i = Sy; i <= Ey; ++i)
    {
        std::size_t start = (i == Sy ? Sx : 0);
        std::size_t end = (i == Ey ? Ex : lines[i].size());

        if (start > lines[i].size()) start = lines[i].size();
        if (end > lines[i].size()) end = lines[i].size();

        selected_str += lines[i].substr(start, end - start);
        if (i != Ey) selected_str += '\n';

        float fallbackY = TextInitialPos.y + i * lineSpacing * textScaling;
        float fallbackX = Vtxt[0].getPosition().x + start * m_char_width * textScaling;

        sf::RectangleShape _line;
        _line.setScale({ 1.f, 1.f });
        _line.setSize({ static_cast<float>((end - start) * m_char_width * textScaling), static_cast<float>(textSize) * textScaling });
        _line.setPosition({ fallbackX, fallbackY });
        _line.setFillColor(sf::Color(0, 255, 255, 100));

        m_selectedLines_list.push_back(_line);
    }
    

    return selected_str;
}

void TE::show_settings()
{
    isSettingsOpened = true;
    sf::RenderWindow settingsWindow(sf::VideoMode({ 1000, 500 }), "Settings", sf::Style::Close | sf::Style::Titlebar);

    settingsWindow.setSize({ 1000, 500 });
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

    gui.removeAllWidgets();
}

void TE::saveSettings()
{
    std::filesystem::current_path(m_base_path);
    std::cout << std::filesystem::current_path();
    m_CFG_MODULE.setFileName("startup.cfg");
    if (m_CFG_MODULE.ReadFile() != RWFM::StatusCodes::STATUS_OK)
    {
        std::cout << "\n#LOG# coudln`t find startup using startup.cfg path, trying settings\\startup.cfg path\n";
        m_CFG_MODULE.setFileName("settings\\startup.cfg");
        if (m_CFG_MODULE.ReadFile() != RWFM::StatusCodes::STATUS_OK)
        {
            std::cout << "\n#LOG# couldn`t find c_settings using settings\\startup.cfg path. Terminating.\n";
            return;
        }
        std::cout << "\n#LOG# startup.cfg found!\n";
    }

    std::vector<std::string> buffer = m_CFG_MODULE.getBuffer();
    std::vector<std::string> new_startup_buffer;
    new_startup_buffer.reserve(buffer.size());
    bool is_cfg_not_damaged = true;
    int i = 0;

    for (auto& line : buffer)
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            new_startup_buffer.push_back(line);
            ++i;
            continue;
        }
        std::size_t equ = buffer[i].find_first_of('=');
        std::string key = buffer[i].substr(0, equ);
        std::cout << "\nkey " << key << ";";
        std::size_t semi = buffer[i].find_first_of(';');
        std::string val = buffer[i].substr(equ+1, semi - equ - 1);
        std::cout << "\nval " << val << ";";

        if (deleteSpaces(key) != "FORCELOAD_CFG")
        {
            is_cfg_not_damaged = false;
            continue;
        }
        
        if (m_preffered_cfg == "D_SETTINGS.cfg" || m_preffered_cfg.empty())
        {
            val = "0";
        }
        else if (m_preffered_cfg == "C_SETTINGS.cfg")
        {
            val = "1";
        }

        std::string new_line = key + "=" + val + ";";
        new_startup_buffer.push_back(new_line);
        std::cout << "\n" << new_startup_buffer[i];
    }
    
    m_CFG_MODULE.WriteToFile(new_startup_buffer);

    m_CFG_MODULE.setFileName("C_SETTINGS.cfg");
    if (m_CFG_MODULE.ReadFile() != RWFM::StatusCodes::STATUS_OK)
    {
        std::cout << "\n#LOG# coudln`t find c_settings using C_SETTINGS.cfg path, trying settings\\C_SETTINGS.cfg path\n";
        m_CFG_MODULE.setFileName("settings\\C_SETTINGS.cfg");
        if (m_CFG_MODULE.ReadFile() != RWFM::StatusCodes::STATUS_OK)
        {
            std::cout << "\n#LOG# couldn`t find c_settings using settings\\C_SETTINGS.cfg path. Terminating.\n";
            return;
        }
        std::cout << "\n#LOG# c_settings found!\n";
    }

    buffer = m_CFG_MODULE.getBuffer(); 
    new_startup_buffer.clear();
    is_cfg_not_damaged = true;
    i = 0;

    for (auto& line : buffer)
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            new_startup_buffer.push_back(line);
            ++i;
            continue;
        }
        std::size_t equ = buffer[i].find_first_of('=');
        std::string key = buffer[i].substr(0, equ);
        std::cout << "\nkey " << key << ";";
        
        std::string new_line;
        std::string processed_key = deleteSpaces(key);
        if (processed_key == "BGCOLOR")
        {
            new_line = processed_key + '=' + 
                '('
                + std::to_string(m_BackGroundColor.r) + ',' + std::to_string(m_BackGroundColor.g) +
                ',' + std::to_string(m_BackGroundColor.b) + ',' + std::to_string(m_BackGroundColor.a) + 
                ')' + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "TXTCOLOR")
        {
            new_line = processed_key + '=' +
                '('
                + std::to_string(TextColor.r) + ',' + std::to_string(TextColor.g) +
                ',' + std::to_string(TextColor.b) + ',' + std::to_string(TextColor.a) +
                ')' + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "NUMCOLOR")
        {
            new_line = processed_key + '=' +
                '('
                + std::to_string(NumColor.r) + ',' + std::to_string(NumColor.g) +
                ',' + std::to_string(NumColor.b) + ',' + std::to_string(NumColor.a) +
                ')' + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "CURSCOLOR")
        {
            new_line = processed_key + '=' +
                '('
                + std::to_string(CursorColor.r) + ',' + std::to_string(CursorColor.g) +
                ',' + std::to_string(CursorColor.b) + ',' + std::to_string(CursorColor.a) +
                ')' + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "BGPICPATH")
        {
            new_line = processed_key + '=' + m_BG_picpath + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "CURWIDTH")
        {
            new_line = processed_key + '=' + std::to_string(m_CursorSize.x) + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "CURHEIGHT")
        {
            new_line = processed_key + '=' + std::to_string(m_CursorSize.y) + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "SCAL")
        {
            new_line = processed_key + '=' + std::to_string(textScaling) + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "TXTSIZE")
        {
            new_line = processed_key + '=' + std::to_string(textSize) + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "TABSPACE")
        {
            new_line = processed_key + '=' + std::to_string(tabSpaces) + ';';
            std::cout << "\n" << new_line;
        }
        else if (processed_key == "FONTPATH")
        {
            new_line = processed_key + '=' + m_Fontpath + ';';
            std::cout << "\n" << new_line;
        }
  
        if(new_line != "")
            new_startup_buffer.push_back(new_line);
        ++i;
    }

    m_CFG_MODULE.WriteToFile(new_startup_buffer);
}

void TE::spritesMenu(tgui::Gui& gui)
{
    gui.removeAllWidgets();
    
    auto SetBgSprite = tgui::Button::create("Change background image");
    auto ResetBgSprite = tgui::Button::create("Reset background image");
    auto setCustomSettings = tgui::Button::create("Set custom settings");
    auto ResetToDefault = tgui::Button::create("Reset to defaults");
    auto SpritePickPanel = tgui::Panel::create({ (double)gui.getWindow()->getSize().x ,(double)gui.getWindow()->getSize().y });
    auto changeToColorButton = tgui::Button::create("Color menu");
    auto SetFont = tgui::Button::create("Change fonts");
    auto SetPrefferedCfgButton = tgui::Button::create("Change what configuration to load everytime");

    auto SetCursorSize = tgui::EditBoxSlider::create(2.f, 10.f, cursor.getSize().x, 1, 0.1f);
    auto TextSizeSlider = tgui::EditBoxSlider::create(16, 999, textSize, 0, 1);
    auto TabSpacesSlider = tgui::EditBoxSlider::create(0, 999, tabSpaces, 0, 1);

    auto CursorTextLabel = tgui::Label::create("Change cursor size: ");
    auto TextSizeLabel = tgui::Label::create("Change text size: ");
    auto TabSpacesLabel = tgui::Label::create("Change how many spaces is used with tab: ");

    gui.add(SpritePickPanel);
    SpritePickPanel->getRenderer()->setBackgroundColor({ 127, 127, 127, 255});
    SpritePickPanel->getRenderer()->setPadding(5);

    // adding to panel
    SpritePickPanel->add(changeToColorButton);
    SpritePickPanel->add(CursorTextLabel);
    SpritePickPanel->add(SetBgSprite);
    SpritePickPanel->add(SetCursorSize);
    SpritePickPanel->add(TextSizeSlider);
    SpritePickPanel->add(TextSizeLabel);
    SpritePickPanel->add(SetFont);
    SpritePickPanel->add(ResetBgSprite);
    SpritePickPanel->add(ResetToDefault);
    SpritePickPanel->add(setCustomSettings);
    SpritePickPanel->add(TabSpacesLabel);
    SpritePickPanel->add(TabSpacesSlider);
    SpritePickPanel->add(SetPrefferedCfgButton);

    // setting position and adjusting
    CursorTextLabel->setSize({150.f, 25.f});
    CursorTextLabel->setTextSize(13);
    CursorTextLabel->getRenderer()->setTextColor(tgui::Color::White);

    TextSizeLabel->setSize({ 150.f, 25.f });
    TextSizeLabel->setTextSize(13);
    TextSizeLabel->getRenderer()->setTextColor(tgui::Color::White);

    TabSpacesLabel->setSize({ 150.f, 25.f });
    TabSpacesLabel->setTextSize(13);
    TabSpacesLabel->getRenderer()->setTextColor(tgui::Color::White);

    changeToColorButton->setWidth(SpritePickPanel->getSize().x);

    SetBgSprite->setPosition({ 0.f, SetBgSprite->getSize().y + changeToColorButton->getPosition().y });

    ResetBgSprite->setPosition({ 0.f, SetBgSprite->getPosition().y + ResetBgSprite->getSize().y });

    CursorTextLabel->setPosition({ 0.f, ResetBgSprite->getPosition().y + CursorTextLabel->getSize().y});

    SetCursorSize->setPosition({ 0.f, SetCursorSize->getSize().y + CursorTextLabel->getPosition().y });

    TextSizeLabel->setPosition({ 0.f, SetCursorSize->getPosition().y + TextSizeLabel->getSize().y + 5.f });

    TextSizeSlider->setPosition({ 0.f, TextSizeLabel->getPosition().y + TextSizeSlider->getSize().y });

    TabSpacesLabel->setPosition({ 0.f, TextSizeSlider->getPosition().y + TabSpacesLabel->getSize().y + 5.f});

    TabSpacesSlider->setPosition({0.f, TabSpacesLabel->getPosition().y + TabSpacesSlider->getSize().y + 5.f});

    SetFont->setPosition({ 0.f, SetFont->getSize().y + TabSpacesSlider->getPosition().y + 10.f });

    ResetToDefault->setPosition({ 0.f, ResetToDefault->getSize().y + SetFont->getPosition().y + 10.f });

    setCustomSettings->setPosition({ 0.f, setCustomSettings->getSize().y + ResetToDefault->getPosition().y + 10.f });

    SetPrefferedCfgButton->setPosition({ 0.f, setCustomSettings->getPosition().y + SetPrefferedCfgButton->getSize().y + 10.f });


    // setting button function`s
    SetFont->onClick([&]() {
        setNewFont();
        });
    changeToColorButton->onPress([&]() {
        colorMenu(gui);
        });

    ResetBgSprite->onPress([&]() {
        BackGround.setTexture(nullptr);
        m_BackGroundColor = m_LastUsedBgColor;
        });

    SetBgSprite->onPress([&]() {
        m_LastUsedBgColor = BackGround.getFillColor();
        setNewBgTexture();
        });

    SetCursorSize->onValueChange([&](const float value) {
        m_CursorSize.x = value;
        });

    TextSizeSlider->onValueChange([&](const int value) {
        textSize = value;
        lineSpacing = static_cast<float>(value) * 1.5f;
        allowVtxtConstructing = true;
        });

    TabSpacesSlider->onValueChange([&](const int value) {
        tabSpaces = value;
        });

    ResetToDefault->onPress([&]() {
        if (m_CFG_MODULE.getFileName() != CE_DEFAULT_CONFIG_NAME)
        {
            loadCFG(CE_DEFAULT_CONFIG_NAME, 1);
        }
        });

    setCustomSettings->onPress([&]() {
        if (m_CFG_MODULE.getFileName() != CE_CUSTOM_CONFIG_NAME)
        {
            loadCFG(CE_CUSTOM_CONFIG_NAME, 1);
        }
        });

    SetPrefferedCfgButton->onPress([&]() {
        std::string cfgfilepath = show_LoadFromDialog("cfg files(*.cfg)\0*.cfg\0All files (*.*)\0*.*\0\0");
        if (cfgfilepath.size() >= 23)
        {
            const std::string c_IS_THIS_THE_CE_DEFAULT_CONFIG_NAME_OR_ISNT_IT = cfgfilepath.substr(cfgfilepath.size() - 23);

            if (c_IS_THIS_THE_CE_DEFAULT_CONFIG_NAME_OR_ISNT_IT == CE_DEFAULT_CONFIG_NAME)
            {
                m_preffered_cfg = "D_SETTINGS.cfg";
            }
            else if (c_IS_THIS_THE_CE_DEFAULT_CONFIG_NAME_OR_ISNT_IT == CE_CUSTOM_CONFIG_NAME)
            {
                m_preffered_cfg = "C_SETTINGS.cfg";
            }
        }
        
        });
}

void TE::colorMenu(tgui::Gui& gui)
{
    gui.removeAllWidgets();

    auto ColorPickerPanel = tgui::Panel::create({ (double)gui.getWindow()->getSize().x + 100 ,(double)gui.getWindow()->getSize().y + 100 });
    auto BGColorPicker = tgui::ColorPicker::create("Set text background color", m_BackGroundColor);
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
        {m_BackGroundColor = { color.getRed(),color.getGreen(),color.getBlue(),color.getAlpha() };});
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
    const char* filter = "Png files(*.png)\0*.png\0Jpeg files(*.jpeg)\0*.jpeg\0Jpg files(*.jpg)\0*.jpg\0All files (*.*)\0*.*\0\0";
    m_BG_picpath = show_LoadFromDialog(filter);
    bgTexture = {};
    if (m_BG_picpath.size() > 0 && !bgTexture.loadFromFile(m_BG_picpath))
    {
        MessageBoxA(NULL, "Something went wrong while trying to open this file", "ERROR", MB_OK | MB_ICONERROR);
        return;
    }
    BackGround.setTexture(&bgTexture);
    m_BackGroundColor = sf::Color::White;
    BackGround.setFillColor(m_BackGroundColor);
}

void TE::setNewFont()
{
    const std::string_view filter = "Font`s (*.ttf)\0 * .ttf\0All Files\0 * .*\0\0";
    m_Fontpath = show_LoadFromDialog(filter);

    if (m_Fontpath.size() > 1 || GetFileAttributesA(m_Fontpath.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        font = sf::Font();
        if (!font.openFromFile(m_Fontpath))
        {
            MessageBoxA(NULL, "Something went wrong while trying to open ttf file", "ERROR", MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        MessageBoxA(NULL, "INVALID_FILE_ATTRIBUTES maybe you declined", "ERROR", MB_OK | MB_ICONERROR);
    }

    m_char_width = font.getGlyph('M', textSize, false).advance;
    m_line_height = font.getLineSpacing(textSize);
}

void TE::constructVtxt(const sf::View& v)
{
    lineNumbers.clear();
    Vtxt.clear();
    lineNumbers.reserve(lines.size());
    Vtxt.reserve(lines.size());

    BiggestLineIdx = 0;

    std::size_t tempidx = 0;
    float lineSize = 0;
    
    const float viewTop = v.getCenter().y - v.getSize().y / 2.f;
    const float viewBottom = v.getCenter().y + v.getSize().y / 2.f;

    FirstVisibleLineVtxt = std::max(0, (int)((viewTop - TextInitialPos.y) / (lineSpacing * textScaling)));
    LastVisibleLineVtxt = std::min(lines.size(), (std::size_t)((viewBottom - TextInitialPos.y) / (lineSpacing * textScaling)) + 1);

    for (std::size_t i = FirstVisibleLineVtxt; i < LastVisibleLineVtxt; ++i)
    {
        // Line numbers
        sf::Text num(font);
        num.setCharacterSize(textSize);
        num.setString(std::to_string(i + 1));
        num.setScale({ textScaling, textScaling });
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
        t.setScale({ textScaling, textScaling });

        const float size = t.getGlobalBounds().size.x;
        Vtxt.push_back(t);

        const float oldLineSize = lineSize;
        lineSize = std::max(lineSize, size);

        if (oldLineSize != lineSize)
        {
            tempidx = i;
        }
    }

    BiggestLineIdx = tempidx;

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

void TE::pasteFromClipBoard()
{
    if (!OpenClipboard(NULL))
    {
        return;
    }
    else
    {
        HANDLE hndl = GetClipboardData(CF_TEXT);
        if (hndl != NULL)
        {
            char* buff = static_cast<char*>(GlobalLock(hndl));
            if (buff != NULL)
            {
                int linescount = 0;
                for (char* i = buff; *i != '\0'; ++i)
                {
                    if (*i == '\n')
                    {
                        ++linescount;
                    }
                }
                if (linescount > 0)
                {
                    std::vector<std::string> pasted_lines;
                    pasted_lines.reserve(linescount);
                    std::string accum;
                    char* j = buff;
                    for (; *j != '\0'; ++j)
                    {
                        accum += *j;
                        if (*j == '\n')
                        {
                            pasted_lines.push_back(accum);
                            accum.clear();
                        }
                    }
                    if (!accum.empty())
                        pasted_lines.push_back(accum);

                    std::string left = lines[cursorLine].substr(0, cursorColumn);
                    std::string right = lines[cursorLine].substr(cursorColumn);
                    lines[cursorLine] = left;
                    for (int i = 0; i < pasted_lines.size(); ++i)
                    {
                        lines.insert(lines.begin() + cursorLine, pasted_lines[i]);
                        ++cursorLine;
                    }

                    lines.insert(lines.begin() + cursorLine - 1, right);
                }
                else
                {
                    lines[cursorLine].insert(cursorColumn, std::string(buff));
                }

                GlobalUnlock(hndl);
                allowVtxtConstructing = true;
            }
        }
        CloseClipboard();
    }
}


void TE::copyToClipboard()
{
    if (m_selected_area.empty())
        return;
    if (!OpenClipboard(NULL))
    {
        return;
    }
    else
    {
        EmptyClipboard();

        std::size_t b_size = m_selected_area.size() / sizeof(char) + 1;

        HGLOBAL allocated_chunk = GlobalAlloc(GMEM_MOVEABLE, b_size);
        if (allocated_chunk == NULL)
        {
            CloseClipboard();
            return;
        }

        char* pointer_to_chunk = static_cast<char*>(GlobalLock(allocated_chunk));
        if (pointer_to_chunk == NULL)
        {
            GlobalFree(allocated_chunk);
            CloseClipboard();
            return;
        }

        strcpy_s(pointer_to_chunk, b_size, m_selected_area.c_str());

        GlobalUnlock(allocated_chunk);
        
        if (SetClipboardData(CF_TEXT, allocated_chunk) == NULL) {
            GlobalFree(allocated_chunk);
        }
        CloseClipboard();
    }
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
    
        ReadOrWriteModule.setFileName(m_filename);
    }
    else
    {
        // user refused to save the file
        return;
    }
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
            if (m_selected)
            {
                int begl = std::max(m_begin_selected_y, m_end_selected_y);
                int endl = std::min(m_begin_selected_y, m_end_selected_y);

                int fchar_pos = (m_begin_selected_x < m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;
                int end_pos = (fchar_pos == m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;

                if (endl == begl)
                {
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].begin() + end_pos);
                    cursorColumn = fchar_pos;
                }
                else if (endl != begl)
                {
                    std::string copy = lines[begl].substr(end_pos);
                    int i = begl;
                    for (; i > endl; --i)
                    {
                        lines.erase(lines.begin() + i);
                    }
                    endl = i;
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].end());
                    lines[endl].insert(fchar_pos, copy);
                    cursorLine = endl;
                    cursorColumn = fchar_pos;
                    allowVtxtConstructing = true;
                }
            }
            if (cursorColumn > 0)
            {
                if (!m_selected)
                {
                    cursorColumn--;
                    lines[cursorLine].erase(cursorColumn, 1);
                }
                allowVtxtConstructing = true;
                m_selected = false;
            }
            else if (cursorLine > 0)
            {
                // Merge with previous line
                cursorColumn = lines[cursorLine - 1].size();
                lines[cursorLine - 1] += lines[cursorLine];
                lines.erase(lines.begin() + cursorLine);
                cursorLine--;
                allowVtxtConstructing = true;
                m_selected = false;
            }

            allowScrolling = true;
            ShowStar = true;
            m_selected = false;
        }
        else if (typed == 13 || typed == '\n') // Enter
        {
            if (m_selected)
            {
                int begl = std::max(m_begin_selected_y, m_end_selected_y);
                int endl = std::min(m_begin_selected_y, m_end_selected_y);

                int fchar_pos = (m_begin_selected_x < m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;
                int end_pos = (fchar_pos == m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;

                if (endl == begl)
                {
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].begin() + end_pos);
                    cursorColumn = fchar_pos;
                }
                else if (endl != begl)
                {
                    std::string copy = lines[begl].substr(end_pos);
                    int i = begl;
                    for (; i > endl; --i)
                    {
                        lines.erase(lines.begin() + i);
                    }
                    endl = i;
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].end());
                    lines[endl].insert(fchar_pos, copy);
                    cursorLine = endl;
                    cursorColumn = fchar_pos;
                }
            }
            std::string left = lines[cursorLine].substr(0, cursorColumn);
            std::string right = lines[cursorLine].substr(cursorColumn);

            lines[cursorLine] = left;
            cursorLine += 1;
            cursorColumn = 0;
            lines.insert(lines.begin() + cursorLine, right);

            allowScrolling = true;
            allowVtxtConstructing = true;
            ShowStar = true;
            m_selected = false;
        }
        else if (typed == 9 || typed == '\t') // Tab
        {
            if (m_selected)
            {
                int begl = std::max(m_begin_selected_y, m_end_selected_y);
                int endl = std::min(m_begin_selected_y, m_end_selected_y);

                int fchar_pos = (m_begin_selected_x < m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;
                int end_pos = (fchar_pos == m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;

                if (endl == begl)
                {
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].begin() + end_pos);
                    cursorColumn = fchar_pos;
                }
                else if (endl != begl)
                {
                    std::string copy = lines[begl].substr(end_pos);
                    int i = begl;
                    for (; i > endl; --i)
                    {
                        lines.erase(lines.begin() + i);
                    }
                    endl = i;
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].end());
                    lines[endl].insert(fchar_pos, copy);
                    cursorLine = endl;
                    cursorColumn = fchar_pos;
                }
            }

            lines[cursorLine].insert(lines[cursorLine].begin() + cursorColumn, tabSpaces, ' ');
            cursorColumn += tabSpaces;

            allowScrolling = true;
            allowVtxtConstructing = true;
            ShowStar = true;
            m_selected = false;
        }
        else if (typed >= 32 && typed < 127) // chars
        {
            if(m_selected)
            {
                int begl = std::max(m_begin_selected_y, m_end_selected_y);
                int endl = std::min(m_begin_selected_y, m_end_selected_y);

                int fchar_pos = (m_begin_selected_x < m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;
                int end_pos = (fchar_pos == m_end_selected_x) ? m_begin_selected_x : m_end_selected_x;

                if (endl == begl)
                {
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].begin() + end_pos);
                    cursorColumn = fchar_pos;
                }
                else if(endl != begl)
                {
                    std::string copy = lines[begl].substr(end_pos);
                    int i = begl;
                    for (; i > endl; --i)
                    {
                        lines.erase(lines.begin() + i);
                    }
                    endl = i;
                    lines[endl].erase(lines[endl].begin() + fchar_pos, lines[endl].end());
                    lines[endl].insert(fchar_pos, copy);
                    cursorLine = endl;
                    cursorColumn = fchar_pos;
                }
            }
            lines[cursorLine].insert(lines[cursorLine].begin() + cursorColumn, typed);
            cursorColumn += 1;
            allowVtxtConstructing = true;
            allowScrolling = true;
            ShowStar = true;
            m_selected = false;
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
    if (!Vtxt.empty() && BiggestLineIdx < Vtxt.size())
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
        float scrollAmount = -mouseWheelScrolled->delta * 60.f; 
        v.move({ 0.f, scrollAmount });
        allowScrolling = false;
    }

    sf::Vector2f center = v.getCenter();
    float halfViewHeight = v.getSize().y / 2.f;

    float minY = halfViewHeight;
    float maxY = std::max(minY, (lines.size() * lineSpacing * textScaling + 23.f) - halfViewHeight);

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
    if (m_selected) {
        m_end_selected_x = cursorColumn;
        m_end_selected_y = cursorLine;
    }
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

std::string TE::deleteSpaces(const std::string& str)
{
    std::string s = str;
    s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
    s.erase(std::remove_if(s.begin(), s.end(), [=](char c) {return c == '\n'; }), s.end());
    return s;
}

void TE::cfgParser(const std::vector<std::string>& buffer)
{
    m_cfgmap.clear();

    if (buffer.empty())
        return;

    // tokenizer
    for(std::size_t y = 0;y < buffer.size();++y)
    {
        bool commentflag = false;
        std::string insides = deleteSpaces(buffer[y]);
        std::string key, val;

        // processing key
        for (std::size_t x1 = 0;x1 < insides.size() && insides[x1] != '='; ++x1)
        {
            if (insides[x1] == '#')
            {
                commentflag = true;
                break;
            }
            key += insides[x1];
        }

        if (commentflag)
            continue;

        // processing value
        for (std::size_t x2 = key.size() + 1; x2 < insides.size() && insides[x2] != ';'; ++x2)
        {
            val += insides[x2];
        }

        if(!key.empty() && !val.empty())
            m_cfgmap[key] = val;
    }

    if (m_cfgmap.empty())
    {
        return;
    }

    // parser
    for (auto& e : m_cfgmap)
    {
        if (e.first == "FORCELOAD_CFG")
        {
            *reinterpret_cast<std::string*>(cfg_settings[10]) = (std::stoi(e.second) == 0 ? CE_DEFAULT_CONFIG_NAME : CE_CUSTOM_CONFIG_NAME);
        }
        else if (e.first == "FONTPATH")
        {
            *reinterpret_cast<std::string*>(cfg_settings[0]) = (e.second == "null" || e.second == "NULL") ? "" : e.second;
        }
        else if (e.first == "TXTSIZE")
        {
            *reinterpret_cast<int*>(cfg_settings[1]) = std::stoi(e.second);
        }
        else if (e.first == "TABSPACE")
        {
            *reinterpret_cast<int*>(cfg_settings[9]) = std::stoi(e.second);
        }
        else if (e.first == "SCAL")
        {
            *reinterpret_cast<float*>(cfg_settings[2]) = std::stof(e.second);
        }
        else if (e.first == "CURHEIGHT")
        {
            (*reinterpret_cast<sf::Vector2f*>(cfg_settings[3])).y = std::stod(e.second);
        }
        else if (e.first == "CURWIDTH")
        {
            (*reinterpret_cast<sf::Vector2f*>(cfg_settings[3])).x = std::stod(e.second);
        }
        else if (e.first == "BGPICPATH")
        {
            *reinterpret_cast<std::string*>(cfg_settings[4]) = (e.second == "null" || e.second == "NULL") ? "" : e.second;
        }
        else 
        {
            auto l_Init_Colors = [&](std::array<void*, 11>& table, std::size_t index, std::string& val) {
                std::string colorT = deleteSpaces(val);
                uint8_t colorTable[4] = { 0, 0, 0, 255 };

                char c[4] = { '\0', '\0', '\0', '\0'};
                for (int i = 1, j = 0, x = 0; i < colorT.size(); ++i)
                {
                    if (colorT[i] == ',' || colorT[i] == ')')
                    {
                        c[3] = '\0';
                        colorTable[x] = std::stoi(c);
                        ++x;
                        j = 0;
                        std::fill(c, c + 4, '\0');
                        continue;
                    }
                    else
                    {
                        c[j] = colorT[i];
                        ++j;
                    }
                }

                (*reinterpret_cast<sf::Color*>(table[index])).r = colorTable[0];
                (*reinterpret_cast<sf::Color*>(table[index])).g = colorTable[1];
                (*reinterpret_cast<sf::Color*>(table[index])).b = colorTable[2];
                (*reinterpret_cast<sf::Color*>(table[index])).a = colorTable[3];
            };

            if (e.first == "CURSCOLOR")
            {
                l_Init_Colors(cfg_settings, 5, e.second);
            }
            else if (e.first == "NUMCOLOR")
            {
                l_Init_Colors(cfg_settings, 6, e.second);
            }
            else if (e.first == "TXTCOLOR")
            {
                l_Init_Colors(cfg_settings, 7, e.second);
            }
            else if (e.first == "BGCOLOR")
            {
                l_Init_Colors(cfg_settings, 8, e.second);
            }
        }
    }
}

void TE::loadCFG(const std::string& path, unsigned int type)
{
    m_CFG_MODULE.setFileName(path);
    m_CFG_MODULE.ReadFile();
    c_cfg_buffer = nullptr;

    if (m_CFG_MODULE.getStatusCode() == RWFM::StatusCodes::STATUS_OK)
    {
        c_cfg_buffer = &m_CFG_MODULE.getBuffer();
        cfgParser(*c_cfg_buffer);
        m_CFG_MODULE.getBuffer().clear();

        // X_SETTINGS
        if(type == 1)
        {
            if (!m_BG_picpath.empty()) {
                bgTexture = sf::Texture();
                if (!bgTexture.loadFromFile(m_BG_picpath))
                {
                    MessageBoxA(NULL, "Something went wrong while trying to open this file", "ERROR", MB_OK | MB_ICONERROR);
                }
                else
                {
                    BackGround.setTexture(&bgTexture);
                    m_BackGroundColor = sf::Color::White;
                    m_LastUsedBgColor = m_BackGroundColor;
                    BackGround.setFillColor(m_BackGroundColor);
                }
            }
            else {
                bgTexture = sf::Texture();
                BackGround.setTexture(nullptr);
            }
        }
    }
    else
    {
        MessageBoxA(NULL, "Something went wrong while trying to open configuration file", "ERROR", MB_OK | MB_ICONERROR);
    }
}

void TE::Start()
{
    std::cout << std::filesystem::current_path();
    sf::RenderWindow window(sf::VideoMode({ 1000, 700 }), "MLTE v" + version + " : " + m_filename, sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize, sf::State::Windowed);
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

    OptionButton->onClick([&]() {
        if (!isSettingsOpened)
            show_settings();
        });

    OpenButton->onClick([&]() {
        if (!isOpenFromFileOpened)
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
    OpenButton->setPosition({ SaveAsButton->getPosition().x + SaveAsButton->getSize().x, SaveAsButton->getPosition().y });

    m_BottomPanelHeight = buttonPanel->getSize().y;

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
    //BackGround.setFillColor(m_BackGroundColor);

    if (!font.openFromFile(m_Fontpath))
    {
        int res = MessageBoxA(0, "Font not found or failed to load.\nLoad manually please", "Error", MB_OKCANCEL | MB_ICONWARNING);

        switch (res)
        {
        case IDCANCEL:
            return;
            break;
        case IDOK:
            setNewFont();
            break;
        default:
            break;
        }
    }

    m_char_width = font.getGlyph('M', textSize, false).advance;
    m_line_height = font.getLineSpacing(textSize);

    text.setPosition(TextInitialPos);
    text.setFillColor(sf::Color::White);

    cursor.setSize({ 2.f, static_cast<float>(textSize) });
    cursor.setPosition({ text.getPosition().x - text.getCharacterSize(), text.getPosition().y });
    cursor.setFillColor(sf::Color::White);

    std::string WindowTitle = "MLTE v" + version + " : " + m_filename;
    std::string strapedFilename = m_filename.substr(m_filename.find_last_of('\\') + 1);

    //--------/init clocks\--------

    constexpr float LastFileCallDelay = 0.5f;

    sf::Clock ClipBoardAccessClock;
    sf::Clock SelectAllAccessClock;

    SelectAllAccessClock.start(); // using for select all lock
    ClipBoardAccessClock.start(); // for clipboard access lock
    SaveAsDelayClock.start(); // using for ctrl + s lock and unlock
    blinkingCursorDelayClock.start(); // using for blinking with cursor anim
    CursorIdleTime.start(); // cursor idle anim
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
                    if (m_stopedSelecting)
                    {
                        m_selected = false;
                    }
                }
            }

            getInput(*event);
        }

        // LShift + Lctrl + S \ RShift + Rctrl + S
        // save to file
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && !isSaveAsOpened) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift) &&
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) && !isSaveAsOpened)
        {
            SaveAs();
        }

        // Lctrl + S \ Rctrl + S
        // quick save
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && SaveAsDelayClock.getElapsedTime().asSeconds() >= LastFileCallDelay) ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && SaveAsDelayClock.getElapsedTime().asSeconds() >= LastFileCallDelay))
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
        // Lctr + P \ Rctrl + P
        // parameters
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::P)) && !isSettingsOpened ||
            (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::P)) && !isSettingsOpened)
        {
            show_settings();
        }

        // Lctr + V \ Rctrl + V
        // paste from clipboard
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::V) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::V)) && ClipBoardAccessClock.getElapsedTime().asSeconds() >= 0.6f)
        {
            ClipBoardAccessClock.restart();
            pasteFromClipBoard();
            m_selected = false;
        }

        // Lctr + C \ Rctrl + C
        // copy to clipboard
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::C) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::C)) && ClipBoardAccessClock.getElapsedTime().asSeconds() >= 0.6f)
        {
            ClipBoardAccessClock.restart();
            copyToClipboard();
        }

        // Lctr + A \ Rctrl + A
        // select all
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) && SelectAllAccessClock.getElapsedTime().asSeconds() >= 0.8f)
        {
            SelectAllAccessClock.restart();
            m_selected_area = getSelectedString(0, 0, lines[lines.size() - 1].size(), lines.size() - 1);
            m_selected = true;
        }

        //LShift \ RShift - text selection
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RShift))
        {
            if (!m_selected) {
                m_selected = true;
                m_stopedSelecting = false;
                m_begin_selected_x = cursorColumn;
                m_begin_selected_y = cursorLine;
            }

            if (m_selected)
            {
                m_end_selected_x = cursorColumn;
                m_end_selected_y = cursorLine;

                m_selected_area = getSelectedString(m_begin_selected_x, m_begin_selected_y, m_end_selected_x, m_end_selected_y);
            }
        }
        else
        {
            m_stopedSelecting = true;
        }

        if (allowVtxtConstructing)
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
            const float cursorTop = cursor.getPosition().y;
            const float cursorBottom = cursor.getPosition().y + cursor.getSize().y;
            const float cursorRight = cursor.getSize().x + cursor.getPosition().x;
            const float cursorLeft = cursor.getPosition().x - cursor.getSize().x;

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
                center.y += (cursorBottom - viewBottom) + lineSpacing + m_BottomPanelHeight;
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
        BackGround.setFillColor(m_BackGroundColor);
        window.draw(BackGround);

       
        // nums
        for (auto& num : lineNumbers)
        {
            const float numTopCord = num.getPosition().y + lineSpacing * textScaling;
            const float numRightCord = num.getPosition().x + num.getLocalBounds().size.x;
            const float numBottomCord = numTopCord + num.getLocalBounds().size.y - lineSpacing * textScaling;

            if (numBottomCord > viewTop || numTopCord < viewBottom && numRightCord > viewLeft)
            {
                const float difference = std::abs(cursor.getLocalBounds().position.y - num.getLocalBounds().position.y) * textScaling;
                if (cursor.getGlobalBounds().position.y + difference == num.getGlobalBounds().position.y)
                {
                    num.setFillColor(sf::Color::Yellow);
                }
                else
                {
                    num.setFillColor(NumColor);
                }
                window.draw(num);
            }
        }

        // selected area
        if (m_selected)
        {
            for (const auto& rect : m_selectedLines_list)
            {
                window.draw(rect);
            }
        }

        // text
        for (auto& lineText : Vtxt)
        {
            float tempY = lineText.getPosition().y;
            lineText.setPosition({ TextInitialPos.x * textScaling, tempY });

            const float TextTopCord = lineText.getPosition().y + lineSpacing * textScaling;
            const float TextBottomCord = TextTopCord + lineText.getLocalBounds().size.y;
            const float TextRightCord = lineText.getGlobalBounds().size.x;

            if (TextBottomCord > viewTop || TextTopCord < viewBottom)
            {
                if (TextRightCord > viewRight)
                {
                    sf::Text Temp(lineText);
                    std::string accumulativeStr = "";

                    for (const char32_t& c : lineText.getString())
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
            {
                const sf::Vector2f position = Vtxt[relativeIndex].findCharacterPos(cursorColumn);
                const float cursorRightBound = cursor.getSize().x;
                cursor.setPosition({ position.x - cursorRightBound, position.y });
            }
        }
        else
        {
            const float fallbackY = TextInitialPos.y + cursorLine * lineSpacing * textScaling;
            const float fallbackX = TextInitialPos.x + cursorColumn * lastCharSizeX;
            cursor.setPosition({ fallbackX, fallbackY });
        }

        cursor.setScale({ textScaling, textScaling });
        cursor.setSize({ cursor.getSize().x, static_cast<float>(textSize) });
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
    saveSettings();
    //-----------------------------
}