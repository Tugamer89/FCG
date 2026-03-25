////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <math.h>

////////////////////////////////////////////////////////////
/// Utility

template <typename T1, typename T2>
double dist(sf::Vector2<T1> p1, sf::Vector2<T2> p2)
{
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

const sf::Color GRAY(128, 128, 128, 255);
const sf::Color LIGHT_GRAY(170, 170, 170, 255);
const sf::Color DARK_GRAY(85, 85, 85, 255);

const std::vector<sf::Color> colors = {
    sf::Color::White,
    sf::Color::Red,
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Cyan,
    sf::Color::Yellow,
    sf::Color::Magenta
};

///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Shape data

struct Shape
{
    size_t type = 0;
    size_t color = 0;
    float size = 30.f;
    sf::Vector2f position = {0.f, 0.f};

    Shape(size_t type, size_t color, sf::Vector2f position) : type(type), color(color), position(position) {}
};

/// GUI state

struct State
{
    sf::RenderWindow window;
    float menu_height = 50;

    size_t active_shape = 0;
    size_t active_color = 0;

    std::vector<Shape> shapes;

    State(unsigned w, unsigned h, const std::string &title)
    {
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        sf::Vector2i centerPosition(
            (desktop.size.x - w) / 2,
            (desktop.size.y - h) / 2
        );

        window = sf::RenderWindow(sf::VideoMode({w, h}), title);
        window.setPosition(centerPosition);
        window.setVerticalSyncEnabled(true);
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
    // TODO
}

void handle(const sf::Event::KeyPressed &keyPressed, State &gs)
{
    // TODO
}

void handle(const sf::Event::MouseButtonPressed &mouseBP, State &gs)
{
    float button_size = gs.menu_height;
    sf::FloatRect button_area({0.f, 0.f}, {button_size, button_size});
    auto mouse_pos = static_cast<sf::Vector2f>(mouseBP.position);

    for (size_t i = 0; i < 5; ++i)
    {
        button_area.position = {button_size * static_cast<float>(i), 0.f};

        if (button_area.contains(mouse_pos))
        {
            gs.active_shape = i;
            return;
        }
    }

    for (size_t i = 0; i < colors.size(); ++i)
    {
        button_area.position = {static_cast<float>(gs.window.getSize().x) - button_size * (static_cast<float>(i) + 1), 0.f};

        if (button_area.contains(mouse_pos))
        {
            gs.active_color = colors.size() - i - 1;
            return;
        }
    }

    if (mouse_pos.y < gs.menu_height)
        return;

    if (mouseBP.button == sf::Mouse::Button::Left)
        gs.shapes.emplace_back(gs.active_shape, gs.active_color, mouse_pos);
}

void handle(const sf::Event::MouseButtonReleased &mouseBP, State &gs)
{
    // TODO
}

void handle(const sf::Event::Resized &resized, State &gs)
{
    sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized.size));
    gs.window.setView(sf::View(visibleArea));
}

template <typename T>
void handle(const T &, State &gs)
{
    // All unhandled events will end up here
}
///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Graphics
void doGUI(State &gs)
{
    float button_size = gs.menu_height;
    float shape_radius = button_size * 3.f / 10.f;

    sf::RectangleShape button({button_size, button_size});
    button.setFillColor(sf::Color::Transparent);
    button.setOutlineThickness(-3.f);
    
    sf::CircleShape shape(shape_radius);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(3.f);
    shape.setOrigin({shape_radius, shape_radius});
    
    for (size_t i = 0; i < 5; ++i)
    {
        button.setOutlineColor(i == gs.active_shape ? DARK_GRAY : LIGHT_GRAY);
        button.setPosition({button_size * static_cast<float>(i), 0.f});
        gs.window.draw(button);

        shape.setPointCount(i > 0 ? i+2 : 30);
        shape.setPosition({button_size * (static_cast<float>(i) + 0.5f), button_size / 2.f});
        gs.window.draw(shape);
    }

    for (size_t i = 0; i < colors.size(); ++i)
    {
        size_t color_index = colors.size() - i - 1;

        button.setOutlineColor(color_index == gs.active_color ? DARK_GRAY : LIGHT_GRAY);
        button.setFillColor(colors[color_index]);
        button.setPosition({static_cast<float>(gs.window.getSize().x) - button_size * (static_cast<float>(i) + 1), 0.f});
        gs.window.draw(button);
    }
}

void doGraphics(State &gs)
{
    gs.window.clear();
    doGUI(gs);

    sf::CircleShape drawable_shape(0.f);

    for (const auto &shape : gs.shapes)
    {
        float shape_radius = shape.size * 3.f / 10.f;

        drawable_shape.setRadius(shape_radius);
        drawable_shape.setPointCount(shape.type > 0 ? shape.type + 2 : 30);
        drawable_shape.setFillColor(colors[shape.color]);
        drawable_shape.setOrigin({shape_radius, shape_radius});
        drawable_shape.setPosition(shape.position);
        gs.window.draw(drawable_shape);
    }

    gs.window.display();
}
///
////////////////////////////////////////////////////////////

int main()
{
    State gs(800, 600, "Draw shapes");
    gs.window.setFramerateLimit(50);

    while (gs.window.isOpen()) // main loop
    {
        // event loop and handler through callbacks
        gs.window.handleEvents([&gs](const auto &event) { handle(event, gs); });
        // Show update
        doGraphics(gs);
    }
}
