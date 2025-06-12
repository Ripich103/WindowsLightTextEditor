#include "TextEditor.h"

TE::TE() : version("alpha"), BackGround({ 0.f, 0.f }), textSize(18),
font(), allowScrolling(false), cursor({ 0.f, 0.f }), Linespacing(28.f), cursorColumn(0), cursorLine(0), lastCharSizeX(0), cursorInControl(false), text(font)
{
    text.setCharacterSize(textSize);
    Vtxt.reserve(1);
    lines = { "" };
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
            }
            if (cursorColumn > 0)
            {
                lines[cursorLine].erase(cursorColumn - 1, 1);
                cursorColumn--;
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
        }
        else if (typed >= 32 && typed < 127) // Printable ASCII
        {
            lines[cursorLine].insert(lines[cursorLine].begin() + cursorColumn, typed);
            cursorColumn += 1;
            allowScrolling = true;
        }
    }


}

void TE::scroll(const sf::Event& event, sf::View& v, const sf::RenderWindow& w)
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
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "MLTE v" + version, sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize, sf::State::Windowed);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    BackGround.setSize({ (float)window.getSize().x, (float)window.getSize().y });
    BackGround.setPosition({ 50.f, 0.f });
    BackGround.setFillColor({ 100, 100, 100 });

    if (!font.openFromFile("Font\\pixel.ttf"))
    {
        return;
    }

    text.setPosition({ 60.f, 10.f });
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
        Vtxt = std::move(textLines);
        lastCharSizeX = 0;
        if (!Vtxt.empty()) {
            const auto& lastText = Vtxt.back();
            std::size_t len = lastText.getString().getSize();
            if (len > 0) {
                sf::Vector2f lastCharPos = lastText.findCharacterPos(len);
                sf::Vector2f prevCharPos = lastText.findCharacterPos(len - 1);
                lastCharSizeX = lastCharPos.x - prevCharPos.x;
            }
        }



        BackGround.setPosition({ n + lineSpacing, 0.0f });

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

        BackGround.setSize({ BackGround.getSize().x, std::max(BackGround.getSize().y, (lines.size() * lineSpacing + 20.f) * 2.f) });


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
        window.clear(sf::Color::White);
        window.draw(BackGround);

        for (const auto& num : lineNumbers)
            window.draw(num);

        for (const auto& lineText : Vtxt)
            window.draw(lineText);

        window.draw(cursor);

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
}