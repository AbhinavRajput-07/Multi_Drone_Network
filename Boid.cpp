#include "Boid.hpp"
#include <SFML/System/Angle.hpp>
#include <cmath>

// Define PI if it's not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor: Initializes a new boid
Boid::Boid(float x, float y) {
    acceleration = sf::Vector2f(0.f, 0.f);
    velocity = sf::Vector2f(static_cast<float>(rand() % 4 - 2), static_cast<float>(rand() % 4 - 2));
    position = sf::Vector2f(x, y);
    r = 3.0f; // Boid size
    maxspeed = 4.0f;
    maxforce = 0.05f;
}

// The main logic function that combines all steering behaviors
void Boid::flock(const std::vector<Boid>& boids, sf::Vector2f predatorPos, sf::Vector2f targetWaypoint) {
    // --- Rule 1: Standard Flocking ---
    sf::Vector2f sep = separation(boids); // Keep distance from neighbors
    sf::Vector2f ali = align(boids);      // Steer in the same direction as neighbors
    sf::Vector2f coh = cohesion(boids);   // Move toward the center of neighbors

    // --- Rule 2: Goal Seeking ---
    // All boids feel a pull towards the current target waypoint
    sf::Vector2f goal = seek(targetWaypoint);

    // --- Rule 3: Predator Avoidance ---
    // Flee from the mouse cursor if it's too close
    sf::Vector2f fear(0.f, 0.f);
    float d = std::sqrt(std::pow(position.x - predatorPos.x, 2) + std::pow(position.y - predatorPos.y, 2));
    if (d < 150.f) {
        fear = flee(predatorPos);
    }

    // -- Apply all forces with weights to set priorities --
    sep *= 2.0f;  // Separation is a high priority to avoid collisions
    ali *= 1.0f;
    coh *= 1.0f;
    goal *= 0.5f; // The pull of the goal is a suggestion, not a command
    fear *= 5.0f; // Fleeing is the HIGHEST priority

    applyForce(sep);
    applyForce(ali);
    applyForce(coh);
    applyForce(goal);
    applyForce(fear);
}

// Draws the boid as a blue square
void Boid::draw(sf::RenderWindow& window) {
    sf::RectangleShape square(sf::Vector2f(r * 2.f, r * 2.f));
    square.setFillColor(sf::Color::Blue);
    square.setOrigin({ r, r }); // Set origin to the center for proper rotation
    square.setPosition(position);

    float angle = static_cast<float>(std::atan2(velocity.y, velocity.x) * 180 / M_PI);
    square.setRotation(sf::degrees(angle));

    window.draw(square);
}

// --- Helper and Physics Functions ---

// Adds a force to the boid's acceleration
void Boid::applyForce(const sf::Vector2f& force) {
    acceleration += force;
}

// Updates the boid's position based on its velocity and acceleration
void Boid::update() {
    velocity += acceleration;
    float mag = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (mag > maxspeed) {
        velocity = (velocity / mag) * maxspeed; // Limit speed
    }
    position += velocity;
    acceleration = sf::Vector2f(0.f, 0.f); // Reset acceleration each frame
}

// Wraps the boid around the screen edges
void Boid::borders(sf::RenderWindow& window) {
    if (position.x < -r) position.x = static_cast<float>(window.getSize().x) + r;
    if (position.y < -r) position.y = static_cast<float>(window.getSize().y) + r;
    if (position.x > window.getSize().x + r) position.x = -r;
    if (position.y > window.getSize().y + r) position.y = -r;
}

// Calculates a steering force towards a target
sf::Vector2f Boid::seek(const sf::Vector2f& target) {
    sf::Vector2f desired = target - position;
    float mag = std::sqrt(desired.x * desired.x + desired.y * desired.y);
    if (mag > 0) { // Check to avoid division by zero
        desired = (desired / mag) * maxspeed;
        sf::Vector2f steer = desired - velocity;
        float steerMag = std::sqrt(steer.x * steer.x + steer.y * steer.y);
        if (steerMag > maxforce) {
            steer = (steer / steerMag) * maxforce; // Limit force
        }
        return steer;
    }
    return sf::Vector2f(0.f, 0.f);
}

// Calculates a steering force away from a target
sf::Vector2f Boid::flee(const sf::Vector2f& target) {
    sf::Vector2f desired = position - target; // Opposite of seek
    float d = std::sqrt(desired.x * desired.x + desired.y * desired.y);
    if (d > 0) {
        desired = (desired / d) * maxspeed;
        sf::Vector2f steer = desired - velocity;
        float steerMag = std::sqrt(steer.x * steer.x + steer.y * steer.y);
        if (steerMag > maxforce) {
            steer = (steer / steerMag) * maxforce;
        }
        return steer;
    }
    return sf::Vector2f(0.f, 0.f);
}

// Calculates force to steer away from neighbors
sf::Vector2f Boid::separation(const std::vector<Boid>& boids) {
    float desiredseparation = 25.0f;
    sf::Vector2f steer(0.f, 0.f);
    int count = 0;
    for (const auto& other : boids) {
        float d = std::sqrt(std::pow(position.x - other.position.x, 2) + std::pow(position.y - other.position.y, 2));
        if ((d > 0) && (d < desiredseparation)) {
            sf::Vector2f diff = position - other.position;
            diff /= d; // Weight by distance
            steer += diff;
            count++;
        }
    }
    if (count > 0) {
        steer /= static_cast<float>(count);
    }
    float mag = std::sqrt(steer.x * steer.x + steer.y * steer.y);
    if (mag > 0) {
        steer = (steer / mag) * maxspeed;
        steer -= velocity;
        float steerMag = std::sqrt(steer.x * steer.x + steer.y * steer.y);
        if (steerMag > maxforce) {
            steer = (steer / steerMag) * maxforce;
        }
    }
    return steer;
}

// Calculates force to steer toward average heading of neighbors
sf::Vector2f Boid::align(const std::vector<Boid>& boids) {
    float neighbordist = 50.0f;
    sf::Vector2f sum(0.f, 0.f);
    int count = 0;
    for (const auto& other : boids) {
        float d = std::sqrt(std::pow(position.x - other.position.x, 2) + std::pow(position.y - other.position.y, 2));
        if ((d > 0) && (d < neighbordist)) {
            sum += other.velocity;
            count++;
        }
    }
    if (count > 0) {
        sum /= static_cast<float>(count);
        float mag = std::sqrt(sum.x * sum.x + sum.y * sum.y);
        if (mag > 0) {
            sum = (sum / mag) * maxspeed;
            sf::Vector2f steer = sum - velocity;
            float steerMag = std::sqrt(steer.x * steer.x + steer.y * steer.y);
            if (steerMag > maxforce) {
                steer = (steer / steerMag) * maxforce;
            }
            return steer;
        }
    }
    return sf::Vector2f(0.f, 0.f);
}

// Calculates force to steer toward average position of neighbors
sf::Vector2f Boid::cohesion(const std::vector<Boid>& boids) {
    float neighbordist = 50.0f;
    sf::Vector2f sum(0.f, 0.f);
    int count = 0;
    for (const auto& other : boids) {
        float d = std::sqrt(std::pow(position.x - other.position.x, 2) + std::pow(position.y - other.position.y, 2));
        if ((d > 0) && (d < neighbordist)) {
            sum += other.position;
            count++;
        }
    }
    if (count > 0) {
        sum /= static_cast<float>(count);
        return seek(sum);
    }
    return sf::Vector2f(0.f, 0.f);
}