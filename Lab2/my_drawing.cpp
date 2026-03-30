////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <float.h>
#include <algorithm>

////////////////////////////////////////////////////////////
/// Utility

template <typename T1, typename T2>
double dist(sf::Vector2<T1> p1, sf::Vector2<T2> p2)
{
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

const sf::Color LIGHT_GRAY(180, 180, 180, 255);
const sf::Color DARK_GRAY(85, 85, 85, 255);


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

    Shape() = default;
    Shape(size_t type, size_t color, sf::Vector2f position) : type(type), color(color), position(position) {}
};

/// GUI state

struct State
{
    const std::vector<sf::Color> colors = {
        sf::Color::White,
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Cyan,
        sf::Color::Yellow,
        sf::Color::Magenta
    };

    sf::RenderWindow window;
    float menu_height = 50;

    sf::Vector2f mouse_pos;

    size_t active_shape = 0;
    size_t active_color = 0;
    long long selected_shape = -1;

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

    sf::Vector2f enforceTopLeftBounds(const sf::Vector2f &pos, float radius) const
    {
        return {
            std::max(pos.x, radius),
            std::max(pos.y, menu_height + radius)
        };
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

void handle(const sf::Event::KeyPressed &keyPressed, State &gs)
{
    if (keyPressed.code == sf::Keyboard::Key::Escape)
        gs.window.close();
    
    else if (gs.selected_shape != -1 && (keyPressed.code == sf::Keyboard::Key::Delete ||
                                         keyPressed.code == sf::Keyboard::Key::Backspace))
    {
        gs.shapes.erase(gs.shapes.begin() + gs.selected_shape);
        gs.selected_shape = -1;
    }
}

void handle(const sf::Event::MouseMoved &mouseMoved, State &gs)
{
    if (gs.selected_shape == -1) {
        return;
    }

    Shape &selected_shape = gs.shapes[gs.selected_shape];
    const auto mouse_pos = static_cast<sf::Vector2f>(mouseMoved.position);
    const sf::Vector2f offset = mouse_pos - gs.mouse_pos;

    gs.mouse_pos = mouse_pos;
    selected_shape.position = gs.enforceTopLeftBounds(selected_shape.position + offset, selected_shape.size / 2.f);
}

void handle(const sf::Event::MouseButtonPressed &mouseBP, State &gs)
{
    auto wSize = static_cast<sf::Vector2f>(gs.window.getSize());
    auto mouse_pos = static_cast<sf::Vector2f>(mouseBP.position);

    sf::FloatRect menuArea({0.f, 0.f}, {wSize.x, gs.menu_height});
    sf::FloatRect canvasArea({0.f, gs.menu_height}, wSize - sf::Vector2f{0.f, gs.menu_height});

    if (menuArea.contains(mouse_pos))
    {   // Menu
        float button_size = gs.menu_height;

        auto index = static_cast<size_t>(mouse_pos.x / button_size);

        if (index < 5)
        {
            gs.active_shape = index;
            return;
        }

        index = static_cast<size_t>((wSize.x - mouse_pos.x) / button_size);

        if (index < gs.colors.size())
        {
            gs.active_color = gs.colors.size() - index - 1;
            return;
        }
    }
    else if (!canvasArea.contains(mouse_pos))
        return;
    // Canvas
    else if (mouseBP.button == sf::Mouse::Button::Left)
    {
        const float radius = Shape().size / 2.f;
        sf::Vector2f spawn_pos = gs.enforceTopLeftBounds(mouse_pos, radius);

        gs.shapes.emplace_back(gs.active_shape, gs.active_color, spawn_pos);
    }
    else if (mouseBP.button == sf::Mouse::Button::Right)
    {
        auto last_dist = DBL_MAX;
        gs.mouse_pos = mouse_pos;

        for (size_t i = 0; i < gs.shapes.size(); ++i)
        {
            double distance = dist(gs.shapes[i].position, mouse_pos);

            if (distance < gs.shapes[i].size / 2.f && distance < last_dist)
            {
                last_dist = distance;
                gs.selected_shape = i;
            }
        }
    }
}

void handle(const sf::Event::MouseWheelScrolled &mouseWheelScrolled, State &gs)
{
    if (gs.selected_shape == -1)
        return;

    Shape &selected_shape = gs.shapes[gs.selected_shape];

    if (mouseWheelScrolled.delta > 0)
        selected_shape.size *= 1.1f;
    else
        selected_shape.size /= 1.1f;
    
    const auto wSize = static_cast<sf::Vector2f>(gs.window.getSize());
    const float max_radius = std::min(wSize.x / 2.f, (wSize.y - gs.menu_height) / 2.f);
    selected_shape.size = std::clamp(selected_shape.size, 10.f, max_radius * 2.f);

    const float radius = selected_shape.size / 2.f;
    selected_shape.position = gs.enforceTopLeftBounds(selected_shape.position, radius);
}

void handle(const sf::Event::MouseButtonReleased &mouseBP, State &gs)
{
    if (mouseBP.button == sf::Mouse::Button::Right)
        gs.selected_shape = -1;
}

void handle(const sf::Event::Resized &resized, State &gs)
{
    sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized.size));
    gs.window.setView(sf::View(visibleArea));
}

template <typename T>
void handle(const T &, State &)
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
    float shape_radius = button_size * 0.3f;

    sf::RectangleShape button({button_size, button_size});
    button.setFillColor(sf::Color::Transparent);
    button.setOutlineThickness(-3.f);
    
    sf::CircleShape shape(shape_radius);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(3.f);
    shape.setOrigin({shape_radius, shape_radius});
    
    // Shapes
    for (size_t i = 0; i < 5; ++i)
    {
        button.setOutlineColor(i == gs.active_shape ? DARK_GRAY : LIGHT_GRAY);
        button.setPosition({button_size * static_cast<float>(i), 0.f});
        gs.window.draw(button);

        shape.setPointCount(i > 0 ? i+2 : 30);
        shape.setPosition({button_size * (static_cast<float>(i) + 0.5f), button_size / 2.f});
        gs.window.draw(shape);
    }

    // Colors
    for (size_t i = 0; i < gs.colors.size(); ++i)
    {
        const size_t color_index = gs.colors.size() - i - 1;

        button.setOutlineColor(color_index == gs.active_color ? DARK_GRAY : LIGHT_GRAY);
        button.setFillColor(gs.colors[color_index]);
        button.setPosition({static_cast<float>(gs.window.getSize().x) - button_size * (static_cast<float>(i) + 1), 0.f});
        gs.window.draw(button);
    }
}

void doGraphics(State &gs)
{
    gs.window.clear();
    doGUI(gs);

    sf::CircleShape drawable_shape(0.f);

    for (size_t i = 0; i < gs.shapes.size(); ++i)
    {
        const Shape &shape = gs.shapes[i];
        const float shape_radius = shape.size / 2.f;

        drawable_shape.setRadius(shape_radius);
        drawable_shape.setPointCount(shape.type > 0 ? shape.type + 2 : 30);
        drawable_shape.setFillColor(gs.colors[shape.color]);
        drawable_shape.setOrigin({shape_radius, shape_radius});
        drawable_shape.setPosition(shape.position);

        if (i == gs.selected_shape)
        {
            drawable_shape.setOutlineColor(LIGHT_GRAY);
            drawable_shape.setOutlineThickness(-3.f);
        }
        else
            drawable_shape.setOutlineThickness(0.f);

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
        gs.window.handleEvents([&gs](const auto &event) {handle(event, gs);});
        // Show update
        doGraphics(gs);
    }
}
