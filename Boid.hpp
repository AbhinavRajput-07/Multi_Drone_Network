#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Boid {
public:
    // --- Properties ---
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float r;        // Radius/size of the boid
    float maxforce; // Maximum steering force
    float maxspeed; // Maximum speed

    // --- Constructor ---
    Boid(float x, float y);

    // --- Methods ---
    void applyForce(const sf::Vector2f& force);
    sf::Vector2f seek(const sf::Vector2f& target);
    sf::Vector2f flee(const sf::Vector2f& target);

    // The main logic function that combines all behaviors
    void flock(const std::vector<Boid>& boids, sf::Vector2f predatorPos, sf::Vector2f targetWaypoint);

    void update();
    void borders(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

private:
    // --- Individual Flocking Behaviors ---
    sf::Vector2f align(const std::vector<Boid>& boids);
    sf::Vector2f cohesion(const std::vector<Boid>& boids);
    sf::Vector2f separation(const std::vector<Boid>& boids);
};
