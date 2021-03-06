#include "RandomizedWallFollow.h"

#include <cstdlib>

namespace wallFollow {

void RandomizedWallFollow::solve(sim::MouseInterface* mouse){
    mouse->initializeMouse("mouse.xml");
    mouse->declareInterfaceType(sim::DISCRETE);
    while (true){
        if (rand() % 2 == 0){
            rightWallFollowStep(mouse);
        }
        else{
            leftWallFollowStep(mouse);
        }
    }
}

void RandomizedWallFollow::rightWallFollowStep(sim::MouseInterface* mouse){
    if (!mouse->wallRight()){
        mouse->turnRight();
    }
    while (mouse->wallFront()){
        mouse->turnLeft();
    }
    mouse->moveForward();
}

void RandomizedWallFollow::leftWallFollowStep(sim::MouseInterface* mouse){
    if (!mouse->wallLeft()){
        mouse->turnLeft();
    }
    while (mouse->wallFront()){
        mouse->turnRight();
    }
    mouse->moveForward();
}

} // namespace wallFollow
