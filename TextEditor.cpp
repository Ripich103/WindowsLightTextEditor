#include "TextEditor.h"

TE::TE() : substr(""), version("alpha"), BackGround({ 0.f, 0.f }), textSize(18),
font(), allowScrolling(false), cursor({ 0.f, 0.f }), Linespacing(28.f), cursorColumn(0), cursorLine(0), lastCharSizeX(0), cursorInControl(false), text(font)
{
    text.setCharacterSize(textSize);
    Vtxt.reserve(1);
}

void TE::shiftUp(const sf::Text& t)
{
    if (cursorLine > 0)
        cursorLine -= 1;
}

void TE::getInput(const sf::Event& event)
{
    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>())
    {
        char typed = static_cast<char>(textEntered->unicode);

        if (typed == 8) // Backspace
        {
            if (!substr.empty())
            {
                cursorColumn = std::max(0, (int)cursorColumn - 1);
                if (substr.back() == '\n')
                {
                    cursorLine = std::max(0, (int)cursorLine - 1);
                    if (cursorLine < Vtxt.size())
                        cursorColumn = Vtxt[cursorLine].getString().getSize();
                    else
                        cursorColumn = 0;
                }

                substr.pop_back();
            }
            allowScrolling = true;
        }
        else if (typed == 13 || typed == '\n') // Enter
        {
            substr += '\n';
            cursorLine += 1;
            allowScrolling = true;
        }
        else if (typed >= 32 && typed < 127) // Printable ASCII
        {
            substr += typed;
            cursorColumn += 1;
            allowScrolling = true;
        }
    }

    
}

void TE::scroll(const sf::Event& event, sf::View& v,const sf::RenderWindow& w)
{
    sf::Vector2f center = v.getCenter();
    if (const auto* mouseWheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        switch (mouseWheelScrolled->wheel)
        {
        case sf::Mouse::Wheel::Vertical:
            center.y -= mouseWheelScrolled->delta * 30.f; // scroll sensitivity
            break;
        }
    }
    if ((center.y * 2.f) > BackGround.getSize().y)
    {
        center.y = BackGround.getSize().y / 2.f;
    }
    else if (center.y * 2.f < w.getSize().y)
    {
        center.y = w.getSize().y / 2.f;
    }

    v.setCenter(center); // Keep both X and Y unchanged except for scroll
}

void TE::handleCursor(const std::vector<sf::Text>& t)
{
    if (t.empty()) return;

    const sf::Text& lastLine = t.back();

    cursor.setPosition({ lastLine.getPosition().x + lastLine.getGlobalBounds().size.x, lastLine.getPosition().y});
}


void TE::Start()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "MLTE v" + version, sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize, sf::State::Windowed);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    BackGround.setSize({ (float)window.getSize().x, (float)window.getSize().y});
    BackGround.setPosition({50.f, 0.f});
    BackGround.setFillColor({ 100, 100, 100 });

    if (!font.openFromFile("Font\\pixel.ttf"))
    {
        return;
    }

    sf::Text text(font);
    text.setPosition({ 60.f, 10.f });
    text.setFillColor(sf::Color::Color::Black);
    text.setCharacterSize(textSize);

    float n{ 0 };
    
    sf::View view;
    view.setSize({ (float)window.getSize().x, (float)window.getSize().y });
    view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });

    
    cursor.setSize({ 2.f, (float)text.getCharacterSize()});
    cursor.setPosition({ text.getPosition().x - text.getCharacterSize(), text.getPosition().y});
    cursor.setFillColor(sf::Color::White);
    bool transparentCursor = true;

    sf::Clock clock;

    clock.start();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::Resized>())
            {
                // Update view size, preserve center
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
                //std::cout << "DEBUG[] v.y" << view.getCenter().y << " DEBUG[] bg.x and y " << BackGround.getSize().x << " " << BackGround.getSize().y << "\n";
            }
            getInput(*event);
        }


        //keyScroll(view, window);

        std::vector<std::string> lines;
        std::string line;

        for (char c : substr)
        {
            if (c == '\n')
            {
                lines.push_back(line);
                line.clear();
            }
            else
            {
                line += c;
            }

        }
        lines.push_back(line); // push final line

        //keyScroll(view, window);A
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
            num.setFillColor(sf::Color(80, 80, 80));
            num.setPosition({ 10.f, 10.f + i * lineSpacing });
            lineNumbers.push_back(num);

            sf::FloatRect f = num.getGlobalBounds();
            
            n = std::max(n, f.size.x);

            // Actual text
            sf::Text t(font);
            t.setString(lines[i]);
            t.setCharacterSize(textSize);
            t.setFillColor(sf::Color::Black);
            t.setPosition({ 60.f, 10.f + i * lineSpacing });
            textLines.push_back(t); 
        }

        BackGround.setPosition({ n + lineSpacing, 0.0f });
        
        if (!textLines.empty() && allowScrolling == true)
        {
            float viewTop = view.getCenter().y - view.getSize().y / 2.f;
            float viewBottom = view.getCenter().y + view.getSize().y / 2.f;

            const auto& lastLine = textLines.back();
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

        BackGround.setSize({ BackGround.getSize().x, std::max(BackGround.getSize().y, (lines.size() * lineSpacing + 20.f) * 2.f) });


        for (auto& e : textLines)
        {
            float tempY = e.getPosition().y;
            e.setPosition({ n + lineSpacing + 10.f, tempY });
        }
        
        handleCursor(textLines);
        

        window.clear(sf::Color::White);
        window.draw(BackGround);

        for (const auto& num : lineNumbers)
            window.draw(num);

        for (const auto& lineText : textLines)
            window.draw(lineText);

        window.draw(cursor);

        window.display();

        if(clock.getElapsedTime().asSeconds() >= 0.5f && !transparentCursor)
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
}