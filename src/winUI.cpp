#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <vector>

using namespace sf;

/* ---------------- GRID SETTINGS ---------------- */
const int N = 30;
const int M = 20;
const int SIZE = 24; // Upped grid size slightly for better visibility

const int WIDTH = N * SIZE;
const int HEIGHT = M * SIZE;

enum class GameState { PLAYING, WON };

/* ---------------- ENTITY CLASS ---------------- */
class Entity {
public:
    int x, y;
    Entity() { respawn(); }

    void respawn() {
        x = rand() % N;
        y = rand() % M;
    }
};

/* ---------------- PLAYER (KNIGHT) CLASS ---------------- */
class Knight {
public:
    int x, y;

    Knight() {
        x = N / 2;
        y = M / 2;
    }

    void move(int dx, int dy) {
        x += dx;
        y += dy;

        // Boundary checks
        if (x >= N) x = N - 1;
        if (x < 0)  x = 0;
        if (y >= M) y = M - 1;
        if (y < 0)  y = 0;
    }
};

/* ---------------- GAME CLASS ---------------- */
class Game {
private:
    RenderWindow window;
    GameState state;

    Knight knight;
    Entity trophy;
    Entity enemy;

    // Graphics shapes used for drawing your Custom UI Elements
    RectangleShape gridCell;
    RectangleShape winBanner;
    
    // Custom drawing objects for the win screen layout
    CircleShape princessHead;
    ConvexShape princessDress;
    RectangleShape knightBody;
    ConvexShape knightHelmet;
    ConvexShape trophyCup;

    Font font;
    Text winText;

    Clock clock;
    float timer = 0;
    float delay = 0.2; // Movement delay for enemy

public:
    Game() : window(VideoMode(Vector2u(WIDTH, HEIGHT)), "Save the Princess!") 
    {
        srand(time(0));
        state = GameState::PLAYING;

        // Initialize grid cell shape
        gridCell.setSize(Vector2f(SIZE - 1, SIZE - 1));

        // Load a system font for the "WIN!" text
        // Note: Ensure you have a valid font file path or place one in your project folder
        if (!font.openFromFile("resources/sansation.ttf")) {
            // Fallback to system default or ignore if font isn't found immediately
        }

        winText.setFont(font);
        winText.setString("WIN!");
        winText.setCharacterSize(45);
        winText.setFillColor(Color::Black);
        winText.setStyle(Text::Bold);

        // Position enemy away from player initially
        while(abs(enemy.x - knight.x) < 5 && abs(enemy.y - knight.y) < 5) {
            enemy.respawn();
        }
        
        setupWinScreenSprites();
    }

    // Helper setup to geometrically draw your sketch using SFML shapes
    void setupWinScreenSprites() {
        // "WIN!" Banner Box
        winBanner.setSize(Vector2f(250, 70));
        winBanner.setFillColor(Color::White);
        winBanner.setOutlineColor(Color::Black);
        winBanner.setOutlineThickness(3);
        winBanner.setOrigin(Vector2f(125, 35));
        winBanner.setPosition(Vector2f(WIDTH / 2, HEIGHT / 4));

        // Center text on banner
        FloatRect textRect = winText.getLocalBounds();
        winText.setOrigin(Vector2f(textRect.position.x + textRect.size.x / 2.0f,
                                   textRect.position.y + textRect.size.y / 2.0f));
        winText.setPosition(winBanner.getPosition());

        // Simple Geometric Trophy Shape
        trophyCup.setPointCount(4);
        trophyCup.setPoint(0, Vector2f(0, 0));
        trophyCup.setPoint(1, Vector2f(80, 0));
        trophyCup.setPoint(2, Vector2f(60, 60));
        trophyCup.setPoint(3, Vector2f(20, 60));
        trophyCup.setFillColor(Color::Yellow);
        trophyCup.setOutlineColor(Color::Black);
        trophyCup.setOutlineThickness(2);
        trophyCup.setPosition(Vector2f(WIDTH / 4, HEIGHT / 2));
    }

    void handleInput(KeyEvent keyEvent) {
        if (state == GameState::PLAYING) {
            if (keyEvent.code == Keyboard::Key::Left)  knight.move(-1, 0);
            if (keyEvent.code == Keyboard::Key::Right) knight.move(1, 0);
            if (keyEvent.code == Keyboard::Key::Up)    knight.move(0, -1);
            if (keyEvent.code == Keyboard::Key::Down)  knight.move(0, 1);
        } else if (state == GameState::WON) {
            // Restart game on Enter key press
            if (keyEvent.code == Keyboard::Key::Enter) {
                knight = Knight();
                trophy.respawn();
                enemy.respawn();
                state = GameState::PLAYING;
            }
        }
    }

    void update() {
        if (state != GameState::PLAYING) return;

        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        // Enemy AI movement logic (Chases player slowly)
        if (timer > delay) {
            timer = 0;
            if (enemy.x < knight.x) enemy.x++;
            else if (enemy.x > knight.x) enemy.x--;
            
            if (enemy.y < knight.y) enemy.y++;
            else if (enemy.y > knight.y) enemy.y--;
        }

        // Win Condition: Knight reaches Trophy
        if (knight.x == trophy.x && knight.y == trophy.y) {
            state = GameState::WON;
        }

        // Lose Condition: Enemy catches Knight (Resets position)
        if (knight.x == enemy.x && knight.y == enemy.y) {
            knight = Knight();
        }
    }

    void drawGameplay() {
        // Draw Background Grid
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                gridCell.setPosition(Vector2f(i * SIZE, j * SIZE));
                gridCell.setFillColor((i + j) % 2 == 0 ? Color(30, 30, 30) : Color(40, 40, 40));
                window.draw(gridCell);
            }
        }

        // Draw Trophy (Objective)
        gridCell.setPosition(Vector2f(trophy.x * SIZE, trophy.y * SIZE));
        gridCell.setFillColor(Color::Yellow);
        window.draw(gridCell);

        // Draw Enemy
        gridCell.setPosition(Vector2f(enemy.x * SIZE, enemy.y * SIZE));
        gridCell.setFillColor(Color::Red);
        window.draw(gridCell);

        // Draw Knight
        gridCell.setPosition(Vector2f(knight.x * SIZE, knight.y * SIZE));
        gridCell.setFillColor(Color::Cyan);
        window.draw(gridCell);
    }

    void drawWinScreen() {
        window.clear(Color(150, 220, 150)); // Cozy light green victory background

        // Draw the WIN! banner
        window.draw(winBanner);
        window.draw(winText);

        // Draw Trophy representation from layout sketch
        window.draw(trophyCup);

        // Custom drawn Knight Character using simple vector shapes
        RectangleShape knightBase(Vector2f(40, 70));
        knightBase.setFillColor(Color(180, 180, 180));
        knightBase.setOutlineColor(Color::Black);
        knightBase.setOutlineThickness(2);
        knightBase.setPosition(Vector2f(WIDTH / 2 - 20, HEIGHT / 2));
        window.draw(knightBase);

        // Custom Princess Dress using shapes
        ConvexShape dress;
        dress.setPointCount(3);
        dress.setPoint(0, Vector2f(WIDTH * 3/4, HEIGHT / 2));
        dress.setPoint(1, Vector2f(WIDTH * 3/4 - 40, HEIGHT / 2 + 80));
        dress.setPoint(2, Vector2f(WIDTH * 3/4 + 40, HEIGHT / 2 + 80));
        dress.setFillColor(Color(255, 105, 180)); // Pink Dress
        dress.setOutlineColor(Color::Black);
        dress.setOutlineThickness(2);
        window.draw(dress);

        CircleShape head(18);
        head.setFillColor(Color(255, 220, 180));
        head.setPosition(Vector2f(WIDTH * 3/4 - 18, HEIGHT / 2 - 30));
        window.draw(head);
    }

    void run() {
        while (window.isOpen()) {
            while (const auto event = window.pollEvent()) {
                if (event->is<Event::Closed>())
                    window.close();
                
                if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
                    handleInput(*keyEvent);
                }
            }

            update();

            window.clear();
            if (state == GameState::PLAYING) {
                drawGameplay();
            } else if (state == GameState::WON) {
                drawWinScreen();
            }
            window.display();
        }
    }
};

/* ---------------- MAIN ---------------- */
int main() {
    Game game;
    game.run();
    return 0;
}