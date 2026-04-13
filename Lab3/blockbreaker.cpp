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
const float ball_speed = 500.f;

// paddle
const sf::Vector2f paddle_size = {100.f, 16.f};
const float paddle_speed = 600.f;



/////////////
// Classes //
/////////////

struct Paddle
{
    sf::Vector2f size = paddle_size;
    sf::Vector2f pos = {(window_width - paddle_size.x)/2.f, window_height - paddle_size.y};
    sf::Texture texture = sf::Texture(paddle_png, paddle_png_len);

    Paddle() = default;
    void draw(sf::RenderWindow& window) const;
    void update(float dt);
};

struct Ball
{
    float radius = ball_radius;
    sf::Vector2f pos = {window_width/2.f, window_height - paddle_size.y - ball_radius};
    sf::Vector2f vel = {0.f, 0.f};
    sf::Texture texture = sf::Texture(ball_png, ball_png_len);

    Ball() = default;
    void draw(sf::RenderWindow& window) const;
    void update(float dt, const Paddle& paddle);
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

void Paddle::update(float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        pos.x -= paddle_speed * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        pos.x += paddle_speed * dt;
    }

    pos.x = std::clamp(pos.x, 0.f, static_cast<float>(window_width) - size.x);
}

void Ball::update(float dt, const Paddle& paddle)
{
    pos += vel * dt;

    // Rimbalzo sui bordi laterali
    if (pos.x - radius < 0.f)
    {
        pos.x = radius;
        vel.x = -vel.x;
    }
    else if (pos.x + radius > window_width)
    {
        pos.x = window_width - radius;
        vel.x = -vel.x;
    }

    // Rimbalzo sul bordo superiore
    if (pos.y - radius < 0.f)
    {
        pos.y = radius;
        vel.y = -vel.y;
    }

    // Collisione con il Paddle
    if (pos.y + radius >= paddle.pos.y &&
        pos.x >= paddle.pos.x &&
        pos.x <= paddle.pos.x + paddle.size.x)
    {
        // 1. Trova il centro del paddle e calcola la distanza dell'impatto dal centro
        float paddle_center_x = paddle.pos.x + paddle.size.x / 2.f;
        float hit_offset = pos.x - paddle_center_x;

        // 2. Normalizza l'offset tra -1.0 (tutto a sx) e 1.0 (tutto a dx)
        float normalized_offset = hit_offset / (paddle.size.x / 2.f);
        
        // Per sicurezza (se colpisce millimetricamente fuori) limitiamo il valore
        normalized_offset = std::clamp(normalized_offset, -1.f, 1.f);

        // 3. Calcola l'angolo di rimbalzo. 
        // L'angolo 0 è dritto verso l'alto. L'angolo massimo è 60 gradi (circa 1.047 radianti).
        const float MAX_BOUNCE_ANGLE = 1.0472f; 
        float bounce_angle = normalized_offset * MAX_BOUNCE_ANGLE;

        // 4. Calcola la velocità attuale (modulo del vettore) per non farla rallentare/accelerare
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        // 5. Applica la nuova direzione vettoriale
        // Moltiplichiamo per il seno (X) e il coseno (Y). La Y è negativa perché in SFML 0 è in alto.
        vel.x = speed * std::sin(bounce_angle);
        vel.y = -speed * std::cos(bounce_angle);

        // Risolvi compenetrazione per evitare che si incastri nel paddle
        pos.y = paddle.pos.y - radius;
    }
}

void State::update()
{
    if (ball.vel == sf::Vector2f{0.f, 0.f})
        return;

    float dt = clock.restart().asSeconds();

    paddle.update(dt);
    ball.update(dt, paddle);

    if (ball.pos.y + ball.radius > window_height)
    {
        ball.vel = {0.f, 0.f};
        ball.pos = {window_width/2.f, window_height - paddle_size.y - ball_radius};
        paddle.pos = {(window_width - paddle_size.x)/2.f, window_height - paddle_size.y};
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
{   // constrain aspect ratio and map always the same portion of the world
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
    if (keyPressed.code == sf::Keyboard::Key::Space && gs.ball.vel == sf::Vector2f{0.f, 0.f})
    {
        gs.clock.restart();
        gs.ball.vel = {ball_speed / 3.f, -ball_speed * 2.f / 3.f};
    }
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
            [&window, &state](const sf::Event::KeyPressed &event) { handle_key_pressed(event, window, state); }
        );

        // display
        window.clear(sf::Color::Black);
        state.update();
        state.draw(window);
        window.display();
    }
}
