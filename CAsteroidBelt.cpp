//CAsteroidBelt.cpp
#include "CAsteroidBelt.h"

AsteroidBelt::AsteroidBelt(int maximum) :
    _leave(false), _userInput('0'), _maxQttyAsteroids(maximum), _guiSize(700,640) // should eventually be "_qtty(maximum)"
{
    // Generate the maximum number of asteroids
    for (int i = 0; i < _maxQttyAsteroids; ++i)
        generateAsteroid();

    _canvas = cv::Mat(_guiSize, CV_8UC3, BKGRD_COLOR); // (size(width, height), type)
}

void AsteroidBelt::run()
{
    while(!_leave)
    {
        draw();
        update();
    }
}

void AsteroidBelt::draw()
{
    //check for key input
    if( (_userInput = cv::waitKey(INPUT_DELAY)) ) {}

    // clear the _canvas
    _canvas = BKGRD_COLOR;

    //draws the updated objects screen, based on updated values
    for (const asteroid& ast : _Asteroids)
        cv::circle(_canvas, ast.getPosition(), ast.getRadius(), ast.getColor(), 5); // Draw each asteroid
    cv::putText(_canvas,"Press 'l' to leave",cv::Point(30,30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255,255,255),1);

    // Display the canvas
    cv::imshow(WINDOW_NAME, _canvas);
}

void AsteroidBelt::update()
{
    if(_userInput == 'l')
        _leave = true;

    moveAsteroids();
    detectCollisions();
}

void AsteroidBelt::generateAsteroid()
{
    // Create a random number generator
    cv::RNG rng(cv::getTickCount());

    // Generate random radius between 20 and 40
    int rad = rng.uniform(MIN_RADIUS, MAX_RADIUS + 1);

    // Generate random position
    int posX = rng.uniform(0, _guiSize.width);
    int posY = HORIZON_Y;
    cv::Point pos = cv::Point(posX, posY);

    // Generate random velocity
    int velX = rng.uniform(MIN_VELOCITY, MAX_VELOCITY + 1);
    int velY = rng.uniform(MIN_VELOCITY, MAX_VELOCITY + 1);
    cv::Point vel = cv::Point(velX, velY);

    // Generate random color
    cv::Scalar col = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

    // Create the asteroid object
    asteroid newAsteroid(rad, pos, vel, col);

    // Add the new asteroid to the vector
    _Asteroids.push_back(newAsteroid);
}

void AsteroidBelt::moveAsteroids()
{
    // Update the position of each asteroid
    for (asteroid& ast : _Asteroids) {
        cv::Point pos = ast.getPosition();
        cv::Point vel = ast.getVelocity();

        // Update the position based on the velocity
        pos.x += vel.x;
        pos.y += vel.y;

        // If the asteroid goes off the edge of the screen, make it appear on the opposite side
        if (pos.x < 0) pos.x = _guiSize.width;
        if (pos.y < 0) pos.y = _guiSize.height;
        if (pos.x > _guiSize.width) pos.x = 0;
        if (pos.y > _guiSize.height) pos.y = 0;

        // Set the new position
        ast.setPosition(pos);
    }
}

void AsteroidBelt::detectCollisions()
{
    int collisionCount = 0;

    // Check for collisions between asteroids
    for (size_t i = 0; i < _Asteroids.size(); ++i)
    {
        for (size_t j = i + 1; j < _Asteroids.size(); ++j)
        {
            cv::Point pos1 = _Asteroids[i].getPosition();
            cv::Point pos2 = _Asteroids[j].getPosition();
            int rad1 = _Asteroids[i].getRadius();
            int rad2 = _Asteroids[j].getRadius();

            // Calculate the distance between the two asteroids
            double distance = cv::norm(pos1 - pos2);

            // Check if the distance is less than the sum of their radii (collision detected)
            if (distance < rad1 + rad2)
            {
                collisionCount++;
                _Asteroids.erase(_Asteroids.begin() + j);
                _Asteroids.erase(_Asteroids.begin() + i);
            }
        }
    }

    // Generate new asteroids
    for (int i = 0; i < (collisionCount * 2); ++i)
        generateAsteroid();

    // Optionally, you can add a delay for a few cycles if needed
    if(collisionCount) {cv::waitKey(DELAY_TIME);}
}
