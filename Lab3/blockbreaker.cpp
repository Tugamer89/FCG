#include <SFML/Graphics.hpp>
#include "textures.hpp"



//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "blockbreaker";
const unsigned window_width = 800;
const unsigned window_height = 600;
const float max_frame_rate = 60;

// ball
const float ball_radius = 10.f;

// paddle
const sf::Vector2f paddle_size = {100.f, 16.f};



/////////////
// Classes //
/////////////

struct Ball
{
    float radius;
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Texture texture;

    Ball();
    void draw(sf::RenderWindow& window);
    void update(float dt);
};

struct Paddle
{
    sf::Vector2f size;
    sf::Vector2f pos;
    sf::Texture texture;
    
    Paddle();
    void draw(sf::RenderWindow& window);
};

struct State
{
    Ball ball;
    Paddle paddle;
    sf::Clock clock;

    State();
    void draw(sf::RenderWindow& window);
    void update();
};



//////////////////
// Constructors //
//////////////////

Ball::Ball()
    : radius(ball_radius)
    , pos({window_width/2.f, window_height - paddle_size.y - ball_radius})
    , vel({0.f, -100.f})
    , texture(ball_png, ball_png_len)
{
    texture.setSmooth(true);
}

Paddle::Paddle()
    : size(paddle_size)
    , pos({(window_width - paddle_size.x)/2.f, window_height - paddle_size.y})
    , texture(paddle_png, paddle_png_len)
{
    texture.setSmooth(true);
}

State::State() {}



//////////
// Draw //
//////////

void Ball::draw(sf::RenderWindow& window)
{
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void Paddle::draw(sf::RenderWindow& window)
{
    sf::RectangleShape shape(size);
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void State::draw(sf::RenderWindow& window)
{
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
    float new_aspect = static_cast<float>(ws.x)/static_cast<float>(ws.y);
    if (new_aspect < aspect)
        ws = {ws.x,static_cast<unsigned>(ws.x/aspect)};
    else
        ws = {static_cast<unsigned>(ws.y*aspect),ws.y};
    window.setSize(ws);
}

void handle_key_pressed(const sf::Event::KeyPressed &keyPressed, sf::RenderWindow& window)
{
    if (keyPressed.code == sf::Keyboard::Key::Escape)
        window.close();
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
            [&window](const sf::Event::KeyPressed& event) { handle_key_pressed(event, window); }
        );

        // display
        window.clear(sf::Color::Black);
        state.update();
        state.draw(window);
        window.display();
    }
}
