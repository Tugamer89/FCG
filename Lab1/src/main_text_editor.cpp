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
        int cols = std::max(1u, window.getSize().x / FONT_WIDTH - 2);
        int rows = std::max(1u, window.getSize().y / FONT_SIZE - 2);
        text_view.size = {cols, rows};
        
        if (cursor_pos.x < text_view.position.x) {
            text_view.position.x = cursor_pos.x; // Esce a sx
        } 
        else if (cursor_pos.x >= text_view.position.x + text_view.size.x) {
            text_view.position.x = cursor_pos.x - text_view.size.x + 1; // Esce a dx
        }

        if (cursor_pos.y < text_view.position.y) {
            text_view.position.y = cursor_pos.y; // Esce in alto
        } 
        else if (cursor_pos.y >= text_view.position.y + text_view.size.y) {
            text_view.position.y = cursor_pos.y - text_view.size.y + 1; // Esce in basso
        }
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
    if (textEnter.unicode == '\b') // backspace
    {
        if (gs.cursor_pos.x > 0)
        {
            --gs.cursor_pos.x;
            gs.log[gs.cursor_pos.y].erase(gs.cursor_pos.x, 1);
        }
        else if (gs.cursor_pos.y > 0)
        {
            --gs.cursor_pos.y;
            gs.cursor_pos.x = static_cast<int>(gs.log[gs.cursor_pos.y].size());
            
            gs.log[gs.cursor_pos.y].append(gs.log[gs.cursor_pos.y + 1]);
            gs.log.erase(gs.log.begin() + gs.cursor_pos.y + 1);
        }
    }
    else if (textEnter.unicode == '\n' || textEnter.unicode == '\r') // enter
    {
        std::string remainder = gs.log[gs.cursor_pos.y].substr(gs.cursor_pos.x);
        
        gs.log[gs.cursor_pos.y].erase(gs.cursor_pos.x);
        
        ++gs.cursor_pos.y;
        gs.cursor_pos.x = 0;
        
        gs.log.emplace(gs.log.begin() + gs.cursor_pos.y, remainder);
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

void handle(const sf::Event::KeyPressed &keyPressed, State &gs)
{
    if (keyPressed.code == sf::Keyboard::Key::Left)
    {
        if (gs.cursor_pos.x > 0)
            --gs.cursor_pos.x;
        else if (gs.cursor_pos.y > 0)
        {
            --gs.cursor_pos.y;
            gs.cursor_pos.x = static_cast<int>(gs.log[gs.cursor_pos.y].size());
        }
    }
    else if (keyPressed.code == sf::Keyboard::Key::Right)
    {
        if (gs.cursor_pos.x < gs.log[gs.cursor_pos.y].size())
            ++gs.cursor_pos.x;
        else if (gs.cursor_pos.y < gs.log.size() - 1)
        {
            ++gs.cursor_pos.y;
            gs.cursor_pos.x = 0;
        }
    }
    else if (keyPressed.code == sf::Keyboard::Key::Up && gs.cursor_pos.y > 0)
    {
        --gs.cursor_pos.y;
        gs.cursor_pos.x = std::min(gs.cursor_pos.x, static_cast<int>(gs.log[gs.cursor_pos.y].size()));
    }
    else if (keyPressed.code == sf::Keyboard::Key::Down && gs.cursor_pos.y < gs.log.size() - 1)
    {
        ++gs.cursor_pos.y;
        gs.cursor_pos.x = std::min(gs.cursor_pos.x, static_cast<int>(gs.log[gs.cursor_pos.y].size()));
    }

    gs.adjustView();
}

template <typename T>
void handle(const T &, State &)
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
                         static_cast<float>((gs.cursor_pos.y - gs.text_view.position.y + 1) * FONT_SIZE - 3)});
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
        gs.window.handleEvents([&gs](const auto &event) { handle(event, gs); });

        // Show log
        doGraphics(gs);
    }
}
