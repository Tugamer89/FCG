////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>

#include <vector>
#include <string>

////////////////////////////////////////////////////////////
/// GUI State
const int FONT_SIZE = 24;
const int FONT_WIDTH = 14;
const std::string FONT_NAME = "resources/dejavu-sans-mono-font/DejavuSansMono-5m7L.ttf";

struct State
{
    // General resources
    sf::RenderWindow window;
    const sf::Font font{FONT_NAME};
    // Text buffer
    std::vector<std::string> log;
    // View of the text buffer
    sf::IntRect text_view;
    // Cursor position in the text buffer
    sf::Vector2i cursor_pos{0, 0};

    State(unsigned w, unsigned h, const std::string &title)
    {
        window = sf::RenderWindow(sf::VideoMode({w, h}), title);
        window.setFramerateLimit(60);

        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        sf::Vector2i centerPosition(
            (desktop.size.x - w) / 2,
            (desktop.size.y - h) / 2
        );
        window.setPosition(centerPosition);

        log.resize(1, "");
        text_view = sf::IntRect({0, 0}, sf::Vector2i(w / FONT_WIDTH - 2, h / FONT_SIZE - 2));
    }

    void adjustView()
    {
        text_view.position = {0, 0};
        
        if (cursor_pos.y >= text_view.position.y + text_view.size.y)
            text_view.position.y = cursor_pos.y - text_view.size.y + 1;
        if (cursor_pos.x >= text_view.position.x + text_view.size.x)
            text_view.position.x = cursor_pos.x - text_view.size.x + 1;

        text_view.size = sf::Vector2i(window.getSize().x - 2, window.getSize().y - 2);
    }
};
///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Callback functions
void handle(const sf::Event::Closed &, State &gs)
{
    gs.window.close();
}

void handle(const sf::Event::TextEntered &textEnter, State &gs)
{
    if (textEnter.unicode == '\n' || textEnter.unicode == '\r') // enter
    {
        ++gs.cursor_pos.y;
        gs.cursor_pos.x = 0;
        gs.log.emplace(gs.log.begin() + gs.cursor_pos.y, "");
    }    
    else if (textEnter.unicode >= ' ' && textEnter.unicode <= '~') // printable char
    {
        gs.log[gs.cursor_pos.y].insert(gs.cursor_pos.x, 1, static_cast<char>(textEnter.unicode));
        ++gs.cursor_pos.x;
    }

    gs.adjustView();
}

void handle(const sf::Event::Resized &resized, State &gs)
{
    sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized.size));
    gs.window.setView(sf::View(visibleArea));
    gs.adjustView();
}

template <typename T>
void handle(const T &, State &gs)
{
    // All unhandled events will end up here
}
///
////////////////////////////////////////////////////////////

void doGraphics(State &gs)
{
    unsigned lines_to_print = std::min(static_cast<unsigned>(gs.log.size() - gs.text_view.position.y),
                                       static_cast<unsigned>(gs.text_view.size.y));

    sf::Text logText{gs.font, "", FONT_SIZE};

    gs.window.clear();
    for (std::size_t i = 0; i < lines_to_print; ++i)
    {
        if (gs.text_view.position.x >= gs.log[gs.text_view.position.y + i].size())
            continue;

        logText.setPosition({FONT_WIDTH, static_cast<float>(i * FONT_SIZE) + FONT_SIZE});
        logText.setString(gs.log[gs.text_view.position.y + i].substr(
            (size_t)gs.text_view.position.x, (size_t)gs.text_view.size.x)
        );
        gs.window.draw(logText);
    }

    logText.setPosition({static_cast<float>((gs.cursor_pos.x - gs.text_view.position.x + 1) * FONT_WIDTH),
                         static_cast<float>((gs.cursor_pos.y - gs.text_view.position.y + 1) * FONT_SIZE + 3)});
    logText.setString("_");
    logText.setFillColor(sf::Color::Green);
    gs.window.draw(logText);

    gs.window.display();
}

int main()
{
    State gs(800, 600, "Text echo");
    while (gs.window.isOpen()) // main loop
    {
        // event loop and handler through callbacks
        gs.window.handleEvents([&](const auto &event) { handle(event, gs); });

        // Show log
        doGraphics(gs);
    }
}
