#include "World.h"

#include <cmath>
#include <thread>

#include "GeometryUtilities.h"
#include "Param.h"
#include "SimUtilities.h"
#include "State.h"

namespace sim {

World::World(Maze* maze, Mouse* mouse) : m_maze(maze), m_mouse(mouse), m_collision(false) {
}

void World::simulate() {

    // Wait until the interface type is declared
    while (S()->interfaceType() == UNDECLARED) {
        sim::SimUtilities::sleep(Seconds(P()->glutInitDuration()));
    }

    // Start a separate collision detection thread
    std::thread collisionDetector(&World::checkCollision, this);

    // Use this thread to perform mouse position updates
    while (true) {

        // In order to ensure we're sleeping the correct amount of time, we time
        // the mouse position update operation and take it into account when we sleep.
        double start(sim::SimUtilities::getHighResTime());

        // If we've crashed, let this thread exit
        if (S()->crashed()) {
            collisionDetector.join();
            return;
        }

        // Update the position of the mouse
        m_mouse->update(Seconds(1.0/P()->mousePositionUpdateRate()));

        // Get the duration of the mouse position update, in seconds. Note that this duration
        // is simply the total number of real seconds that have passed, which is exactly
        // what we want (since the framerate is perceived in real-time and not CPU time).
        double end(sim::SimUtilities::getHighResTime());
        double duration = end - start;
/*
        // Notify the use of a late mouse position update
        if (P()->printLateMousePositionUpdates() && duration > 1.0/sim::P()->mousePositionUpdateRate()) {
            sim::SimUtilities::print(std::string("A mouse position update was late by ")
                + std::to_string(duration - 1.0/sim::P()->mousePositionUpdateRate())
                + std::string(" seconds, which is ")
                + std::to_string((duration - 1.0/P()->mousePositionUpdateRate())/(1.0/P()->mousePositionUpdateRate()) * 100)
                + std::string(" percent late."));
        }
*/
        // Sleep the appropriate amout of time, based on the mouse update duration
        sim::SimUtilities::sleep(sim::Seconds(std::max(0.0, 1.0/sim::P()->mousePositionUpdateRate() - duration)));
    }
}

void World::checkCollision() {

    // If the interface type is not continuous, let this thread exit
    if (S()->interfaceType() != CONTINUOUS) {
        return;
    }

    while (true) {

        // In order to ensure we're sleeping the correct amount of time, we time
        // the collision detection operation and take it into account when we sleep.
        double start(sim::SimUtilities::getHighResTime());

        // For each line segment in the mouse polygon ...
        for (std::pair<Cartesian, Cartesian> B : GeometryUtilities::getLineSegments(m_mouse->getCollisionPolygon())) {

            // ... and for each tile the segment could be intersecting with ...
            for (const Tile* tile : GeometryUtilities::lineSegmentTileCover(B.first, B.second, *m_maze)) {

                // ... iterate through all of the tile's polygons ...
                for (std::vector<Polygon> group : {tile->getActualWallPolygons(), tile->getCornerPolygons()}) {
                    for (Polygon obstacle : group) {
                        for (std::pair<Cartesian, Cartesian> A : GeometryUtilities::getLineSegments(obstacle)) {

                        // TODO Is there a way we can join the line segments so as to do less work

                            // ... and check for a collision
                            if (GeometryUtilities::linesIntersect(A, B)) {
                                S()->setCrashed();
                                return; // If we've crashed, let this thread exit
                            }
                        }
                    }
                }
            }
        }

        // Get the duration of the collision detection, in seconds. Note that this duration
        // is simply the total number of real seconds that have passed, which is exactly
        // what we want (since the framerate is perceived in real-time and not CPU time).
        double end(sim::SimUtilities::getHighResTime());
        double duration = end - start;

        // Notify the use of a late collision detection
        if (P()->printLateCollisionDetections() && duration > 1.0/sim::P()->collisionDetectionRate()) {
            sim::SimUtilities::print(std::string("A collision detection was late by ")
                + std::to_string(duration - 1.0/sim::P()->collisionDetectionRate())
                + std::string(" seconds, which is ")
                + std::to_string((duration - 1.0/P()->collisionDetectionRate())/(1.0/P()->collisionDetectionRate()) * 100)
                + std::string(" percent late."));
        }

        // Sleep the appropriate amout of time, based on the collision detection duration
        sim::SimUtilities::sleep(sim::Seconds(std::max(0.0, 1.0/sim::P()->collisionDetectionRate() - duration)));
    }
}

} // namespace sim
