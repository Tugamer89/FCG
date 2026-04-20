#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
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
const float ball_initial_speed = 500.f;
const sf::Angle ball_initial_angle = sf::degrees(-60.f);

// paddle
const sf::Vector2f paddle_size = {100.f, 16.f};
const float paddle_initial_speed = 600.f;



//////////////////////
// Helper functions //
//////////////////////

sf::Angle reflect_horizontal(sf::Angle angle)
{
    sf::Vector2f v(1.f, angle);
    v.x = -v.x;
    return v.angle();
}

sf::Angle reflect_vertical(sf::Angle angle)
{
    sf::Vector2f v(1.f, angle);
    v.y = -v.y;
    return v.angle();
}

/////////////
// Classes //
/////////////

struct Paddle
{
    sf::Vector2f size = paddle_size;
    sf::Vector2f pos = {(window_width - paddle_size.x)/2.f, window_height - paddle_size.y};
    float speed = paddle_initial_speed;
    sf::Texture texture = sf::Texture(paddle_png, paddle_png_len);

    Paddle() = default;
    void draw(sf::RenderWindow& window) const;
    void move_left(float dt);
    void move_right(float dt);
};

struct Ball
{
    float radius = ball_radius;
    sf::Vector2f pos = {window_width/2.f, window_height - paddle_size.y - ball_radius};    
    float speed = ball_initial_speed;
    sf::Angle angle = ball_initial_angle;
    sf::Texture texture = sf::Texture(ball_png, ball_png_len);

    Ball() = default;
    void draw(sf::RenderWindow& window) const;
    void move(float dt, const Paddle& paddle);
};

struct State
{
    Ball ball;
    Paddle paddle;
    sf::Clock clock;
    
    bool pause = true;
    bool move_paddle_left = false;
    bool move_paddle_right = false;

    State() = default;
    void draw(sf::RenderWindow& window) const;
    void update();
};



//////////
// Draw //
//////////

void Paddle::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape shape(size);
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void Ball::draw(sf::RenderWindow& window) const
{
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void State::draw(sf::RenderWindow& window) const
{
    if (pause)
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



//////////
// Move //
//////////

void Paddle::move_left(float dt)
{
    pos.x -= speed * dt;
    pos.x = std::max(pos.x, 0.f);
}

void Paddle::move_right(float dt)
{
    pos.x += speed * dt;
    pos.x = std::min(pos.x, static_cast<float>(window_width) - size.x);
}



////////////
// Update //
////////////

void Ball::move(float dt, const Paddle& paddle)
{
    pos += sf::Vector2f(speed * dt, angle);

    if (pos.x - radius < 0.f)
    {
        pos.x = radius;
        angle = reflect_horizontal(angle);
    }
    else if (pos.x + radius > window_width)
    {
        pos.x = window_width - radius;
        angle = reflect_horizontal(angle);
    }

    if (pos.y - radius < 0.f)
    {
        pos.y = radius;
        angle = reflect_vertical(angle);
    }

    if (pos.y + radius >= paddle.pos.y &&
        pos.x >= paddle.pos.x &&
        pos.x <= paddle.pos.x + paddle.size.x)
    {
        // Trova il centro del paddle e calcola la distanza dell'impatto dal centro
        float paddle_center_x = paddle.pos.x + paddle.size.x / 2.f;
        float hit_offset = pos.x - paddle_center_x;

        // Normalizza l'offset
        float normalized_offset = hit_offset / (paddle.size.x / 2.f);
        normalized_offset = std::clamp(normalized_offset, -1.f, 1.f);

        // Calcola l'angolo di rimbalzo in radianti (0 dritto in alto)
        const float MAX_BOUNCE_ANGLE = sf::degrees(60.f).asRadians(); 
        float bounce_angle = normalized_offset * MAX_BOUNCE_ANGLE;

        // Convertiamo il bounce angle direzionale in un sf::Angle assegnabile.
        // Y è negativa perché in SFML 0 è in alto.
        sf::Vector2f new_dir(std::sin(bounce_angle), -std::cos(bounce_angle));
        angle = new_dir.angle();

        // Risolvi compenetrazione
        pos.y = paddle.pos.y - radius;
    }
}

void State::update()
{
    if (pause)
        return;

    float dt = clock.restart().asSeconds();

    if (move_paddle_left)
        paddle.move_left(dt);
    if (move_paddle_right)
        paddle.move_right(dt);

    ball.move(dt, paddle);

    if (ball.pos.y + ball.radius > window_height)
    {
        pause = true;
        ball.speed = ball_initial_speed;
        ball.angle = ball_initial_angle;
        ball.pos = {window_width/2.f, window_height - paddle_size.y - ball_radius};
        paddle.pos = {(window_width - paddle_size.x)/2.f, window_height - paddle_size.y};
        
        move_paddle_left = false;
        move_paddle_right = false;
    }
}



////////////
// Events //
////////////

void handle_close(sf::RenderWindow &window)
{
    window.close();
}

void handle_resize(const sf::Event::Resized &resized, sf::RenderWindow &window)
{   
    float aspect = static_cast<float>(window_width)/static_cast<float>(window_height);
    sf::Vector2u ws = resized.size;
    if (static_cast<float>(ws.x)/static_cast<float>(ws.y) < aspect)
        ws = {ws.x,ws.x/static_cast<unsigned>(aspect)};
    else
        ws = {ws.y*static_cast<unsigned>(aspect),ws.y};
    window.setSize(ws);
}

void handle_key_pressed(const sf::Event::KeyPressed &keyPressed, sf::RenderWindow &window, State &gs)
{
    if (keyPressed.code == sf::Keyboard::Key::Escape)
        window.close();
    
    if (keyPressed.code == sf::Keyboard::Key::Space)
    {
        gs.clock.restart();
        gs.pause = !gs.pause;
    }

    if (keyPressed.code == sf::Keyboard::Key::Left)
        gs.move_paddle_left = true;
    if (keyPressed.code == sf::Keyboard::Key::Right)
        gs.move_paddle_right = true;
}

void handle_key_released(const sf::Event::KeyReleased &keyReleased, State &gs)
{
    if (keyReleased.code == sf::Keyboard::Key::Left)
        gs.move_paddle_left = false;
    if (keyReleased.code == sf::Keyboard::Key::Right)
        gs.move_paddle_right = false;
}

void handle_lost_focus(State &gs)
{
    gs.pause = true;
    gs.move_paddle_left = false;
    gs.move_paddle_right = false;
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
            [&window](const sf::Event::Resized &event) { handle_resize(event, window); },
            [&window, &state](const sf::Event::KeyPressed &event) { handle_key_pressed(event, window, state); },
            [&state](const sf::Event::KeyReleased &event) { handle_key_released(event, state); },
            [&state](const sf::Event::FocusLost&) { handle_lost_focus(state); }
        );

        // display
        window.clear(sf::Color::Black);
        state.update();
        state.draw(window);
        window.display();
    }
}
