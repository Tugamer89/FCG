#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <format>
#include <numbers>
#include <random>
#include <ranges>
#include <string>
#include <vector>

#include "textures.hpp"

//////////////////////
// Initial defaults //
//////////////////////

// window
const std::string window_title = "Block Breaker";
constexpr unsigned window_width = 800;
constexpr unsigned window_height = 600;
constexpr unsigned max_frame_rate = 120;

// wall
const sf::Vector2f wall_displacement = {5.f, 50.f};
const sf::Vector2f wall_size = {window_width - 2 * wall_displacement.x,
                                window_height - 2 * wall_displacement.y};
const sf::Vector2f block_size = {59.f, 20.f};
const sf::Vector2i block_num = {13, 8};

const sf::Color block_outline_color = sf::Color(30, 30, 30);
constexpr float block_outline_thickness = -2.f;

// ball
constexpr float ball_radius = 8.f;
constexpr float ball_initial_speed = 350.f;

constexpr float ball_speed_increment = 15.f;
constexpr float ball_max_speed = 850.f;

// paddle
const sf::Vector2f paddle_size = {100.f, 16.f};
constexpr float paddle_initial_speed = 700.f;

//////////////////
// Random & Utils //
//////////////////

float get_random_float(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

sf::Angle get_random_start_angle() {
    bool go_right = get_random_float(0.f, 1.f) > 0.5f;
    float angle_deg = go_right ? get_random_float(-75.f, -45.f) : get_random_float(-135.f, -105.f);
    return sf::degrees(angle_deg);
}

//////////////////
// Level Design //
//////////////////

// 0: Vuoto, 1: Giallo (1 colpo), 2: Arancione (2 colpi), 3: Rosso (3 colpi), 4: Grigio
// (Indistruttibile)
const std::vector<std::vector<std::string>> levels_data = {
    {// Livello 1: Semplice
     "0000000000000", "0011111111100", "0111111111110", "0011111111100", "0000000000000"},
    {// Livello 2: Mattoni a due colpi
     "0000000000000", "0222222222220", "0111111111110", "0022200022200", "0011100011100"},
    {// Livello 3: Muro solido e indistruttibili
     "0333333333330", "0220022200220", "0110011100110", "0440000000440", "0110000000110"},
    {// Livello 4: The Gauntlet
     "4333333333334", "4222244422224", "4111114111114", "0040004000400", "4440000000444"},
};

//////////////////////
// Helper functions //
//////////////////////

sf::Angle reflect_horizontal(sf::Angle angle) {
    sf::Vector2f v(1.f, angle);
    v.x = -v.x;
    return v.angle();
}

sf::Angle reflect_vertical(sf::Angle angle) {
    sf::Vector2f v(1.f, angle);
    v.y = -v.y;
    return v.angle();
}

sf::Color get_color_for_hp(int hp) {
    switch (hp) {
        case 1:
            return sf::Color::Yellow;
        case 2:
            return sf::Color(255, 128, 0);  // Arancione
        case 3:
            return sf::Color::Red;
        case 4:
            return sf::Color(120, 120, 120);  // Grigio
        default:
            return sf::Color::Transparent;
    }
}

/////////////
// Classes //
/////////////

struct Ball;

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Color color;
    float life = 1.0f;
    float size;

    void update(float dt) {
        pos += vel * dt;
        life -= dt * 1.5f;
    }

    void draw(sf::RenderWindow& window) const {
        if (life <= 0) return;
        sf::RectangleShape p({size, size});
        p.setPosition(pos);
        sf::Color c = color;
        c.a = static_cast<uint8_t>(255 * life);
        p.setFillColor(c);
        window.draw(p);
    }
};

struct Block {
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Color color;
    int hp;  // 0 = distrutto, 4 = indistruttibile

    Block(sf::Vector2f p, sf::Vector2f s, int health) : pos(p), size(s), hp(health) {
        color = get_color_for_hp(hp);
    }

    void draw(sf::RenderWindow& window) const;
    [[nodiscard]] bool is_inside(sf::Vector2f point) const;
    bool hit(Ball& ball, std::vector<Particle>& particles);
    void spawn_particles(std::vector<Particle>& particles) const;
};

struct Wall {
    std::vector<Block> blocks;

    Wall() = default;
    void load_level(size_t level_idx);
    void draw(sf::RenderWindow& window) const;
    int hit(Ball& ball, std::vector<Particle>& particles);
    [[nodiscard]] bool is_cleared() const;
};

struct Paddle {
    sf::Vector2f size = paddle_size;
    sf::Vector2f pos = {(window_width - paddle_size.x) / 2.f, window_height - paddle_size.y - 10.f};
    float speed = paddle_initial_speed;
    sf::Texture texture = sf::Texture(paddle_png, paddle_png_len);

    Paddle() { texture.setSmooth(true); }
    void draw(sf::RenderWindow& window) const;
    void move_left(float dt);
    void move_right(float dt);
    [[nodiscard]] bool hit(const Ball& ball) const;
    bool strike(Ball& ball) const;
};

struct Ball {
    float radius = ball_radius;
    sf::Vector2f pos = {window_width / 2.f, window_height - paddle_size.y - ball_radius - 10.f};
    float speed = ball_initial_speed;
    sf::Angle angle = get_random_start_angle();
    sf::Texture texture = sf::Texture(ball_png, ball_png_len);

    Ball() { texture.setSmooth(true); }
    void draw(sf::RenderWindow& window) const;
    void move(float dt);
};

struct State {
    Ball ball;
    Paddle paddle;
    Wall wall;
    sf::Clock clock;
    sf::Font font = sf::Font("resources/dejavu-sans-mono-font/DejavuSansMono-5m7L.ttf");

    std::vector<Particle> particles;

    bool pause = true;
    bool game_over = false;
    bool game_won = false;

    bool move_paddle_left = false;
    bool move_paddle_right = false;

    unsigned int score = 0;
    unsigned int lives = 3;
    unsigned int current_level = 0;

    State() { wall.load_level(current_level); }

    void draw(sf::RenderWindow& window) const;
    void update();
    void restart_game();
    void reset_ball();
    void advance_level();
    void field_limits();
    void collisions();
};

//////////////////
// Block & Wall //
//////////////////

void Block::spawn_particles(std::vector<Particle>& particles) const {
    int num_particles = 12;
    for (int i = 0; i < num_particles; ++i) {
        Particle p;
        p.pos = pos + size / 2.f;
        float angle = get_random_float(0.f, 360.f) * std::numbers::pi_v<float> / 180.f;
        float speed = get_random_float(50.f, 200.f);
        p.vel = {std::cos(angle) * speed, std::sin(angle) * speed};
        p.color = color;
        p.size = get_random_float(2.f, 5.f);
        particles.push_back(p);
    }
}

void Block::draw(sf::RenderWindow& window) const {
    if (hp <= 0) return;

    sf::RectangleShape shape(size);
    shape.setPosition(pos);
    shape.setFillColor(color);
    shape.setOutlineColor(block_outline_color);
    shape.setOutlineThickness(block_outline_thickness);
    window.draw(shape);
}

bool Block::is_inside(sf::Vector2f point) const {
    return point.x >= pos.x && point.x <= pos.x + size.x && point.y >= pos.y &&
           point.y <= pos.y + size.y;
}

bool Block::hit(Ball& ball, std::vector<Particle>& particles) {
    if (hp <= 0) return false;

    sf::Vector2f top = {ball.pos.x, ball.pos.y - ball.radius};
    sf::Vector2f bottom = {ball.pos.x, ball.pos.y + ball.radius};
    sf::Vector2f left = {ball.pos.x - ball.radius, ball.pos.y};
    sf::Vector2f right = {ball.pos.x + ball.radius, ball.pos.y};

    sf::Vector2f ball_dir(1.f, ball.angle);

    bool hit_vertical =
        (is_inside(top) && ball_dir.y < 0.f) || (is_inside(bottom) && ball_dir.y > 0.f);
    bool hit_horizontal =
        (is_inside(left) && ball_dir.x < 0.f) || (is_inside(right) && ball_dir.x > 0.f);

    if (hit_vertical) {
        ball.angle = reflect_vertical(ball.angle);
    } else if (hit_horizontal) {
        ball.angle = reflect_horizontal(ball.angle);
    }

    if (hit_vertical || hit_horizontal) {
        if (hp < 4) {  // 4 = indistruttibile
            hp--;
            if (hp > 0) {
                color = get_color_for_hp(hp);
            } else {
                spawn_particles(particles);
            }
        }
        return true;
    }

    return false;
}

void Wall::load_level(size_t level_idx) {
    blocks.clear();
    if (level_idx >= levels_data.size()) return;

    const auto& layout = levels_data[level_idx];

    float start_x = wall_displacement.x +
                    (wall_size.x - (static_cast<float>(layout[0].size()) * block_size.x)) / 2.f;
    float start_y = wall_displacement.y;

    for (size_t r = 0; r < layout.size(); ++r) {
        for (size_t c = 0; c < layout[r].size(); ++c) {
            char type = layout[r][c];
            if (type != '0') {
                int health = type - '0';
                sf::Vector2f p(start_x + static_cast<float>(c) * block_size.x,
                               start_y + static_cast<float>(r) * block_size.y);
                blocks.emplace_back(p, block_size, health);
            }
        }
    }
}

void Wall::draw(sf::RenderWindow& window) const {
    for (const auto& block : blocks) block.draw(window);
}

int Wall::hit(Ball& ball, std::vector<Particle>& particles) {
    for (auto& block : blocks) {
        if (block.hp > 0 && block.hit(ball, particles)) return block.hp < 4 ? 10 : 0;
    }
    return -1;
}

bool Wall::is_cleared() const {
    return std::ranges::none_of(blocks,
                                [](const Block& block) { return block.hp > 0 && block.hp < 4; });
}

//////////
// Draw //
//////////

void Paddle::draw(sf::RenderWindow& window) const {
    sf::RectangleShape shape(size);
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void Ball::draw(sf::RenderWindow& window) const {
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition(pos);
    shape.setTexture(&texture);
    window.draw(shape);
}

void State::draw(sf::RenderWindow& window) const {
    std::string ui_string =
        std::format("Score: {}   Lives: {}   Level: {}", score, lives, current_level + 1);
    sf::Text ui_text(font, ui_string, 20);
    ui_text.setFillColor(sf::Color::White);
    ui_text.setPosition({10.f, 10.f});
    window.draw(ui_text);

    wall.draw(window);

    for (const auto& p : particles) {
        p.draw(window);
    }

    if (!game_over && !game_won) {
        ball.draw(window);
        paddle.draw(window);
    }

    if (game_over) {
        sf::Text text(font, "GAME OVER\nPress SPACE to restart", 30);
        text.setFillColor(sf::Color::Red);
        text.setPosition(
            {(window_width - text.getLocalBounds().size.x) / 2.f, window_height / 2.f});
        window.draw(text);
    } else if (game_won) {
        sf::Text text(font, "VICTORY!\nYou have completed all levels!\nPress SPACE", 30);
        text.setFillColor(sf::Color::Green);
        text.setPosition(
            {(window_width - text.getLocalBounds().size.x) / 2.f, window_height / 2.f});
        window.draw(text);
    } else if (pause) {
        sf::Text text(font, "Press SPACE to start", 24);
        text.setFillColor(sf::Color::White);
        text.setPosition({(window_width - text.getLocalBounds().size.x) / 2.f,
                          (window_height - text.getLocalBounds().size.y) / 2.f});
        window.draw(text);
    }
}

////////////
// Update //
////////////

void Paddle::move_left(float dt) {
    pos.x -= speed * dt;
}
void Paddle::move_right(float dt) {
    pos.x += speed * dt;
}

bool Paddle::hit(const Ball& ball) const {
    if (sf::Vector2f(1.f, ball.angle).y < 0.f) return false;
    return ball.pos.y + ball.radius >= pos.y && ball.pos.y - ball.radius <= pos.y + size.y &&
           ball.pos.x + ball.radius >= pos.x && ball.pos.x - ball.radius <= pos.x + size.x;
}

bool Paddle::strike(Ball& ball) const {
    if (!hit(ball)) return false;

    float paddle_center_x = pos.x + size.x / 2.f;
    float hit_offset = ball.pos.x - paddle_center_x;
    float normalized_offset = hit_offset / (size.x / 2.f);
    normalized_offset = std::clamp(normalized_offset, -1.f, 1.f);

    const sf::Angle MAX_BOUNCE_ANGLE = sf::degrees(75.f);
    sf::Angle bounce_angle = MAX_BOUNCE_ANGLE * normalized_offset;
    ball.angle = bounce_angle - sf::degrees(90.f);

    float paddle_center_y = pos.y + size.y / 2.f;

    if (ball.pos.y > paddle_center_y) {
        ball.angle = reflect_vertical(ball.angle);
        return false;
    } else {
        ball.pos.y = pos.y - ball.radius;
        return true;
    }
}

void Ball::move(float dt) {
    pos += sf::Vector2f(speed * dt, angle);
}

void State::reset_ball() {
    pause = true;
    ball.speed = ball_initial_speed;
    ball.angle = get_random_start_angle();
    ball.pos = {window_width / 2.f, window_height - paddle_size.y - ball_radius - 10.f};
    paddle.pos = {(window_width - paddle_size.x) / 2.f, window_height - paddle_size.y - 10.f};
    move_paddle_left = false;
    move_paddle_right = false;
}

void State::restart_game() {
    game_over = false;
    game_won = false;
    score = 0;
    lives = 3;
    current_level = 0;
    wall.load_level(current_level);
    particles.clear();
    reset_ball();
}

void State::advance_level() {
    current_level++;
    if (current_level >= levels_data.size()) {
        game_won = true;
        pause = true;
    } else {
        wall.load_level(current_level);
        reset_ball();
    }
}

void State::field_limits() {
    if (paddle.pos.x < 0.f) paddle.pos.x = 0.f;
    if (paddle.pos.x > window_width - paddle.size.x) paddle.pos.x = window_width - paddle.size.x;

    sf::Vector2f ball_dir(1.f, ball.angle);

    if (ball.pos.x - ball.radius < 0.f && ball_dir.x < 0.f) {
        ball.pos.x = ball.radius;
        ball.angle = reflect_horizontal(ball.angle);
    } else if (ball.pos.x + ball.radius > window_width && ball_dir.x > 0.f) {
        ball.pos.x = window_width - ball.radius;
        ball.angle = reflect_horizontal(ball.angle);
    }

    if (ball.pos.y - ball.radius < 0.f && ball_dir.y < 0.f) {
        ball.pos.y = ball.radius;
        ball.angle = reflect_vertical(ball.angle);
    }

    if (ball.pos.y + ball.radius > window_height) {
        lives--;
        if (lives > 0) {
            reset_ball();
        } else {
            game_over = true;
            pause = true;
        }
    }
}

void State::collisions() {
    field_limits();

    if (paddle.strike(ball)) {
        ball.speed = std::min(ball.speed + ball_speed_increment * 0.1f, ball_max_speed);
    }

    int points = wall.hit(ball, particles);

    if (points >= 0) {
        score += points;
        ball.speed = std::min(ball.speed + ball_speed_increment, ball_max_speed);

        if (wall.is_cleared()) {
            advance_level();
        }
    }
}

void State::update() {
    float dt = clock.restart().asSeconds();

    for (auto& p : particles) {
        p.update(dt);
    }

    std::erase_if(particles, [](const Particle& p) { return p.life <= 0; });

    if (pause || game_over || game_won) return;

    if (move_paddle_left) paddle.move_left(dt);
    if (move_paddle_right) paddle.move_right(dt);

    ball.move(dt);
    collisions();
}

////////////
// Events //
////////////

void handle_close(sf::RenderWindow& window) {
    window.close();
}

void handle_resize(const sf::Event::Resized& resized, sf::RenderWindow& window) {
    float aspect = static_cast<float>(window_width) / static_cast<float>(window_height);
    sf::Vector2u ws = resized.size;
    if (static_cast<float>(ws.x) / static_cast<float>(ws.y) < aspect)
        ws = {ws.x, ws.x / static_cast<unsigned>(aspect)};
    else
        ws = {ws.y * static_cast<unsigned>(aspect), ws.y};
    window.setSize(ws);
}

void handle_key_pressed(const sf::Event::KeyPressed& keyPressed, sf::RenderWindow& window,
                        State& gs) {
    switch (keyPressed.code) {
        case sf::Keyboard::Key::Escape:
            window.close();
            return;
        case sf::Keyboard::Key::Space:
            if (gs.game_over || gs.game_won) {
                gs.restart_game();
            } else {
                gs.clock.restart();
                gs.pause = !gs.pause;
            }
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

void handle_key_released(const sf::Event::KeyReleased& keyReleased, State& gs) {
    switch (keyReleased.code) {
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

void handle_lost_focus(State& gs) {
    if (!gs.game_over && !gs.game_won) gs.pause = true;
    gs.move_paddle_left = false;
    gs.move_paddle_right = false;
}

//////////
// Loop //
//////////

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i centerPosition((desktop.size.x - window_width) / 2,
                                (desktop.size.y - window_height) / 2);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), window_title,
                            sf::State::Windowed, settings);
    window.setFramerateLimit(max_frame_rate);
    window.setMinimumSize(window.getSize());
    window.setPosition(centerPosition);

    State state;

    while (window.isOpen()) {
        window.handleEvents(
            [&window](const sf::Event::Closed&) { handle_close(window); },
            [&window](const sf::Event::Resized& event) { handle_resize(event, window); },
            [&window, &state](const sf::Event::KeyPressed& event) {
                handle_key_pressed(event, window, state);
            },
            [&state](const sf::Event::KeyReleased& event) { handle_key_released(event, state); },
            [&state](const sf::Event::FocusLost&) { handle_lost_focus(state); });

        window.clear(sf::Color(15, 15, 20));
        state.update();
        state.draw(window);
        window.display();
    }
}
