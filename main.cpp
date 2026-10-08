#include <SFML/Graphics.hpp>
#include <string> 

#include <iostream> 
using namespace std;

// It seems that SFML have updated their APIs so you can no longer pass separate x and y values into many of the calls – instead you need to pass sf::Vector2f()
// (or make sure you are using sf like we are in this file). This will no doubt be an optimisation, but it means that some ‘legacy’ code (like what you will get later in this module) will not work by default.
//Thankfully it’s an easy fix, and I’ll be fixing it for most of the code we give you in the labs, but just remember this for if you find errors later.After all, I might miss one!

const sf::Keyboard::Key controls[4] = {
    sf::Keyboard::A,   // Player1 UP
    sf::Keyboard::Z,   // Player1 Down
    sf::Keyboard::Up,  // Player2 UP
    sf::Keyboard::Down // Player2 Down
};

//Parameters

sf::Font font;
sf::Text text;

const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const float time_step = 0.017f; //60 fps

int score = 0;

sf::Vector2f ball_velocity;
bool is_player1_serving = false;
const float initial_velocity_x = 100.f; //horizontal velocity
const float initial_velocity_y = 60.f; //vertical velocity

const float velocity_multiplier = 1.1f; //how much the ball will speed up everytime it hits a paddle. Here, 10% every time.

//Objects of the game
sf::CircleShape ball;
sf::RectangleShape paddles[2];

void init() {
    // Set size and origin of paddles
    for (sf::RectangleShape& p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius); //Should be half the ball width and height
    // reset paddle position
    paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
    paddles[1].setPosition(gameWidth - (paddleOffsetWall*2) , gameHeight / 2.f);
    // reset Ball Position
    ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);

    ball_velocity = { (is_player1_serving ? initial_velocity_x : -initial_velocity_x), initial_velocity_y };
}

void reset() {
    score++;
    is_player1_serving = !is_player1_serving;
    ball_velocity = { (is_player1_serving ? initial_velocity_x : -initial_velocity_x), initial_velocity_y };

    paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
    paddles[1].setPosition(gameWidth - paddleOffsetWall * 2, gameHeight / 2.f); // potentially just: gamewidth - (paddleOffsetWall + paddleSize.x / 2.f)
    // reset Ball Position
    ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);

    // Update Score Text
    text.setString(std::to_string(score));
    // Keep Score Text Centered
    text.setPosition((gameWidth * .5f) - (text.getLocalBounds().width * .5f), 0);
}
void update(float dt) {
    // handle paddle movement
    float directionP1 = 0.0f;
    float directionP2 = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[0])) {
        directionP1--;
    }
    if (sf::Keyboard::isKeyPressed(controls[1])) {
        directionP1++;
    }

    if (sf::Keyboard::isKeyPressed(controls[2])) {
        directionP2--;
        
    }
    if (sf::Keyboard::isKeyPressed(controls[3])) {
        directionP2++;
    }

    const float P1x = paddles[0].getPosition().x;
    const float P1y = paddles[0].getPosition().y;
    const float P2x = paddles[1].getPosition().x;
    const float P2y = paddles[1].getPosition().y;
    if (P1y > gameHeight) { //bottom wall
        // bottom wall
        directionP1 = 0;
        paddles[0].move(sf::Vector2f(0.f, -1.f));
    }
    if (P1y < 0) { //Top wall
        // bottom wall
        directionP1 = 0;
        paddles[0].move(sf::Vector2f(0.f, 1.f));
    }
    if (P2y > gameHeight) { //bottom wall
    // bottom wall
        directionP2 = 0;
        paddles[1].move(sf::Vector2f(0.f, -1.f));
    }

    if (P2y < 0) { //Top wall
    // bottom wall
        directionP2 = 0;
        paddles[1].move(sf::Vector2f(0.f, 10.f));
    }
    paddles[0].move(sf::Vector2f(0.f, directionP1 * paddleSpeed * dt));
    paddles[1].move(sf::Vector2f(0.f, directionP2 * paddleSpeed * dt));
    
    ball.move(ball_velocity * dt);

    // check ball collision
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;
    if (by > gameHeight) { //bottom wall
        // bottom wall
        ball_velocity.x *= velocity_multiplier;
        ball_velocity.y *= -velocity_multiplier;
        ball.move(sf::Vector2f(0.f, -10.f));
    }
    else if (by < 0) { //top wall
        // top wall
        ball_velocity.x *= velocity_multiplier;
        ball_velocity.y *= -velocity_multiplier;
        ball.move(sf::Vector2f(0.f, 10.f));
    }
    else if (bx > gameWidth) {
        // right wall
        reset();
    }
    else if (bx < 0) {
        // left wall
        reset();
    }
    else if (
        //ball is inline or behind paddle AND
        bx < paddleSize.x + paddleOffsetWall &&
        //ball is below top edge of paddle AND
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5)) {
                ball_velocity.x *= -velocity_multiplier;
                ball_velocity.y *= velocity_multiplier;
                ball.move(sf::Vector2f(0.f, -10.f));
            }
    else if (//ball is inline or behind paddle AND
        bx > gameWidth - (paddleOffsetWall*5) &&  // #########works but position of right paddle needs to be changed because ive just put its hitbox way infront of it
        //ball is below top edge of paddle AND
        by > paddles[1].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[1].getPosition().y + (paddleSize.y * 0.5)) {
                // bounce off right paddle
                ball_velocity.x *= -velocity_multiplier;
                ball_velocity.y *= velocity_multiplier;
                ball.move(sf::Vector2f(0.f, +10.f));
            }

    // check paddle collision 
}

void render(sf::RenderWindow& window) {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    window.draw(text);
}

int main() {
    if (!font.loadFromFile("Users\goodw\Downloads\Main\Comp_Sci_Bsc\Third_Year\games_engenieering\Pong\resources\BitcountSingle-Regular.ttf")) {
        cout << "bruh";
    }
    // Set text element to use font
    text.setFont(font);
    // set the character size to 24 pixels
    text.setCharacterSize(100);
    text.setFillColor(sf::Color::White);
    text.setString("test");

    //create the window
    sf::RenderWindow window(sf::VideoMode({ gameWidth, gameHeight }), "PONG");
    //initialise and load
    init();
    while (window.isOpen()) {
        //Calculate dt
        //...
        static sf::Clock clock;
        const float dt = clock.restart().asSeconds();

        window.clear();
        update(dt);
        render(window);
        //wait for the time_step to finish before displaying the next frame.
        sf::sleep(sf::seconds(time_step));
        //Wait for Vsync
        window.display();
    }
    //Unload and shutdown
    //clean();
}