#include <SFML/Graphics.hpp>
#include "textures.hpp"



//////////////////////
// Initial defaults //
//////////////////////

// window
const std::string window_title = "blockbreaker";
const unsigned window_width = 800;
const unsigned window_height = 600;
const unsigned max_frame_rate = 60;

// ball
const float ball_radius = 10.f;

// paddle
const sf::Vector2f paddle_size = {100.f, 16.f};



/////////////
// Classes //
/////////////

struct Ball
{
    float radius = ball_radius;
    sf::Vector2f pos = {window_width/2.f, window_height - paddle_size.y - ball_radius};
    sf::Vector2f vel = {0.f, 0.f};
    sf::Texture texture = sf::Texture(ball_png, ball_png_len);

    Ball() = default;
    void draw(sf::RenderWindow& window) const;
    void update(float dt);
};

struct Paddle
{
    sf::Vector2f size = paddle_size;
    sf::Vector2f pos = {(window_width - paddle_size.x)/2.f, window_height - paddle_size.y};
    sf::Texture texture = sf::Texture(paddle_png, paddle_png_len);

    Paddle() = default;
    void draw(sf::RenderWindow& window) const;
};

struct State
{
    Ball ball;
    Paddle paddle;
    sf::Clock clock;

    State() = default;
    void draw(sf::RenderWindow& window) const;
    void update();
};



//////////
// Draw //
//////////

void Ball::draw(sf::RenderWindow& window) const
{
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void Paddle::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape shape(size);
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void State::draw(sf::RenderWindow& window) const
{
    if (ball.vel == sf::Vector2f{0.f, 0.f})
    {
        sf::Font font("resources/tuffy.ttf");
        sf::Text text(font, "Press space to start", 24);
        text.setFillColor(sf::Color::White);
        text.setPosition({(window_width - text.getLocalBounds().size.x)/2.f, (window_height - text.getLocalBounds().size.y)/2.f});
        window.draw(text);
    }

    ball.draw(window);
    paddle.draw(window);
}



////////////
// Update //
////////////

void Ball::update(float dt)
{
    pos += vel * dt;
}

void State::update()
{
    ball.update(clock.restart().asSeconds());
}



////////////
// Events //
////////////

void handle_close(sf::RenderWindow& window)
{
    window.close();
}

void handle_resize(const sf::Event::Resized& resized, sf::RenderWindow& window)
{   // constrain aspect ratio and map always the same portion of the world
    float aspect = static_cast<float>(window_width)/static_cast<float>(window_height);
    sf::Vector2u ws = resized.size;
    if (static_cast<float>(ws.x)/static_cast<float>(ws.y) < aspect)
        ws = {ws.x,ws.x/static_cast<unsigned>(aspect)};
    else
        ws = {ws.y*static_cast<unsigned>(aspect),ws.y};
    window.setSize(ws);
}

void handle_key_pressed(const sf::Event::KeyPressed &keyPressed, sf::RenderWindow& window, State& gs)
{
    if (keyPressed.code == sf::Keyboard::Key::Escape)
        window.close();
    if (keyPressed.code == sf::Keyboard::Key::Space)
        gs.ball.vel = {0.f, -100.f};
}


//////////
// Loop //
//////////

int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i centerPosition(
        (desktop.size.x - window_width) / 2,
        (desktop.size.y - window_height) / 2
    );

    sf::RenderWindow window(sf::VideoMode ({window_width, window_height}), window_title);
    window.setFramerateLimit(max_frame_rate);
    window.setMinimumSize(window.getSize());
    window.setPosition(centerPosition);

    State state;

    while (window.isOpen())
    {
        // events
        window.handleEvents(
            [&window](const sf::Event::Closed&) { handle_close(window); },
            [&window](const sf::Event::Resized& event) { handle_resize(event, window); },
            [&window, &state](const sf::Event::KeyPressed& event) { handle_key_pressed(event, window, state); }
        );

        // display
        window.clear(sf::Color::Black);
        state.update();
        state.draw(window);
        window.display();
    }
}
