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
const unsigned max_frame_rate = 1200;

// ball
const float ball_radius = 10.f;
const float ball_initial_speed = 500.f;
const sf::Angle ball_initial_angle = sf::degrees(-60.f);

const float ball_speed_increment = 25.f;
const float ball_max_speed = 1200.f;

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

struct Ball;

struct Paddle
{
    sf::Vector2f size = paddle_size;
    sf::Vector2f pos = {(window_width - paddle_size.x)/2.f, window_height - paddle_size.y};
    float speed = paddle_initial_speed; 
    sf::Texture texture = sf::Texture(paddle_png, paddle_png_len);

    Paddle() { texture.setSmooth(true); }
    void draw(sf::RenderWindow& window) const;
    void move_left(float dt);
    void move_right(float dt);
    bool hit(const Ball& ball) const;
    bool strike(Ball& ball) const;
};

struct Ball
{
    float radius = ball_radius;
    sf::Vector2f pos = {window_width/2.f, window_height - paddle_size.y - ball_radius};    
    float speed = ball_initial_speed;
    sf::Angle angle = ball_initial_angle;
    sf::Texture texture = sf::Texture(ball_png, ball_png_len);

    Ball() { texture.setSmooth(true); }
    void draw(sf::RenderWindow& window) const;
    void move(float dt);
};

struct State
{
    Ball ball;
    Paddle paddle;
    sf::Clock clock;
    bool pause = true; 
    bool move_paddle_left = false;
    bool move_paddle_right = false;
    unsigned int score = 0;

    State() = default;
    void draw(sf::RenderWindow& window) const;
    void update();
    void restart();
    void field_limits();
    void collisions();
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
    sf::Font font("resources/dejavu-sans-mono-font/DejavuSansMono-5m7L.ttf");

    sf::Text score_text(font, "Score: " + std::to_string(score), 24);
    score_text.setFillColor(sf::Color::White);
    score_text.setPosition({10.f, 10.f});
    window.draw(score_text);

    if (pause)
    {
        sf::Text text(font, "Press SPACE to start", 24);
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

void Paddle::move_left(float dt)
{
    pos.x -= speed * dt;
}

void Paddle::move_right(float dt)
{
    pos.x += speed * dt;
}

bool Paddle::hit(const Ball& ball) const
{
    if (sf::Vector2f(1.f, ball.angle).y < 0.f) return false;

    return ball.pos.y + ball.radius >= pos.y &&
           ball.pos.y - ball.radius <= pos.y + size.y &&
           ball.pos.x + ball.radius >= pos.x &&
           ball.pos.x - ball.radius <= pos.x + size.x;
}

bool Paddle::strike(Ball& ball) const
{
    if (!hit(ball)) return false;

    float paddle_center_x = pos.x + size.x / 2.f;
    float hit_offset = ball.pos.x - paddle_center_x;
    float normalized_offset = hit_offset / (size.x / 2.f);
    normalized_offset = std::clamp(normalized_offset, -1.f, 1.f);

    const float MAX_BOUNCE_ANGLE = sf::degrees(60.f).asRadians(); 
    float bounce_angle = normalized_offset * MAX_BOUNCE_ANGLE;

    sf::Vector2f new_dir(std::sin(bounce_angle), -std::cos(bounce_angle));
    ball.angle = new_dir.angle();

    float paddle_center_y = pos.y + size.y / 2.f;
    
    if (ball.pos.y > paddle_center_y) 
    {
        ball.angle = reflect_vertical(ball.angle);
        return false;
    }
    else 
    {
        ball.pos.y = pos.y - ball.radius;
        return true;
    }
}

void Ball::move(float dt)
{
    pos += sf::Vector2f(speed * dt, angle);
}

void State::restart()
{
    pause = true;
    ball.speed = ball_initial_speed;
    ball.angle = ball_initial_angle;
    ball.pos = {window_width/2.f, window_height - paddle_size.y - ball_radius};
    paddle.pos = {(window_width - paddle_size.x)/2.f, window_height - paddle_size.y};
    
    move_paddle_left = false;
    move_paddle_right = false;
    score = 0;
}

void State::field_limits()
{
    // Correzione bordi racchetta
    if (paddle.pos.x < 0.f) 
        paddle.pos.x = 0.f;
    if (paddle.pos.x > window_width - paddle.size.x) 
        paddle.pos.x = window_width - paddle.size.x;

    // Componenti della traiettoria pallina per collisioni robuste
    sf::Vector2f ball_dir(1.f, ball.angle);

    // Bordo Sinistro
    if (ball.pos.x - ball.radius < 0.f && ball_dir.x < 0.f)
    {
        ball.pos.x = ball.radius;
        ball.angle = reflect_horizontal(ball.angle);
    }
    // Bordo Destro
    else if (ball.pos.x + ball.radius > window_width && ball_dir.x > 0.f)
    {
        ball.pos.x = window_width - ball.radius;
        ball.angle = reflect_horizontal(ball.angle);
    }

    // Soffitto
    if (ball.pos.y - ball.radius < 0.f && ball_dir.y < 0.f)
    {
        ball.pos.y = ball.radius;
        ball.angle = reflect_vertical(ball.angle);
    }

    // Pavimento: Riavvio del livello
    if (ball.pos.y + ball.radius > window_height)
    {
        restart();
    }
}

void State::collisions()
{
    field_limits();

    if (paddle.strike(ball))
    {
        score++;

        ball.speed = std::min(ball.speed + ball_speed_increment, ball_max_speed);
    }
}

void State::update()
{
    if (pause)
        return;

    float dt = clock.restart().asSeconds();

    if (move_paddle_left) paddle.move_left(dt);
    if (move_paddle_right) paddle.move_right(dt);

    ball.move(dt);
    
    collisions();
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
    switch (keyPressed.code)
    {
        case sf::Keyboard::Key::Escape:
            window.close();
            return;
        case sf::Keyboard::Key::Space:
            gs.clock.restart();
            gs.pause = !gs.pause;
            return;
        case sf::Keyboard::Key::Left:
            gs.move_paddle_left = true;
            return;
        case sf::Keyboard::Key::Right:
            gs.move_paddle_right = true;
            return;
        default:
            return;
    }
}

void handle_key_released(const sf::Event::KeyReleased &keyReleased, State &gs)
{
    switch (keyReleased.code)
    {
        case sf::Keyboard::Key::Left:
            gs.move_paddle_left = false;
            return;
        case sf::Keyboard::Key::Right:
            gs.move_paddle_right = false;
            return;
        default:
            return;
    }
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

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode ({window_width, window_height}), window_title, sf::State::Windowed, settings);
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
