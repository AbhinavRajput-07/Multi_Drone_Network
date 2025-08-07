#include <SFML/Graphics.hpp>
#include "Boid.hpp"
#include <vector>
#include <cmath> // Required for std::sqrt and std::pow

// Helper function to calculate distance between two points
float distance(sf::Vector2f p1, sf::Vector2f p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Drone Swarm Coverage Simulation");
    window.setFramerateLimit(60);

    // Create the flock of boids
    std::vector<Boid> flock;
    for (int i = 0; i < 75; i++) {
        flock.push_back(Boid(10.f, 10.f)); // Start them all in the top-left corner
    }

    // --- Create a grid of waypoints to define the search area ---
    std::vector<sf::Vector2f> waypoints;
    for (int x = 100; x < window.getSize().x; x += 150) {
        for (int y = 100; y < window.getSize().y; y += 150) {
            waypoints.push_back(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
        }
    }
    int currentWaypointIndex = 0;

    // --- Visuals for debugging waypoints ---
    sf::CircleShape waypointMarker(5);
    waypointMarker.setFillColor(sf::Color(255, 255, 255, 100)); // Dim white for visited
    waypointMarker.setOrigin({ 5.f, 5.f }); // CORRECTED: Use Vector2f for origin

    sf::CircleShape targetMarker(8);
    targetMarker.setFillColor(sf::Color::Green); // Bright green for current target
    targetMarker.setOrigin({ 8.f, 8.f }); // CORRECTED: Use Vector2f for origin


    // Main application loop
    while (window.isOpen()) {
        // Event handling
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Get mouse position to act as the predator
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // --- Waypoint Management Logic ---
        sf::Vector2f target = waypoints[currentWaypointIndex];

        // Calculate the center of the flock to see if it has reached the target
        sf::Vector2f flockCenter(0.f, 0.f);
        if (!flock.empty()) {
            for (const auto& boid : flock) {
                flockCenter += boid.position;
            }
            flockCenter /= static_cast<float>(flock.size());
        }

        // If the flock's center is close enough to the target, move to the next waypoint
        if (distance(flockCenter, target) < 50.f && currentWaypointIndex < waypoints.size() - 1) {
            currentWaypointIndex++;
        }

        // --- Drawing ---
        window.clear();

        // Draw the waypoints for visualization
        for (const auto& wp : waypoints) {
            waypointMarker.setPosition(wp);
            window.draw(waypointMarker);
        }
        // Draw the current target waypoint in a different color
        targetMarker.setPosition(target);
        window.draw(targetMarker);

        // Update and draw all the boids in the flock
        for (auto& boid : flock) {
            // Pass all necessary info to the main flocking function
            boid.flock(flock, mousePos, target);
            boid.update();
            boid.borders(window);
            boid.draw(window);
        }

        window.display();
    }

    return 0;
}