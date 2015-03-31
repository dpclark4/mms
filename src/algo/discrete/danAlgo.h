#pragma once

#include "../IAlgorithm.h"


class danAlgo : public IAlgorithm{
    typedef struct cell{
        int distance;
        int visited;
        bool wallTop;
        bool wallRight;
        bool wallLeft;
        bool wallBot;
    }cell;
public:
    void solve(sim::MouseInterface* mouse);

private:
    void test(sim::MouseInterface* mouse);
    std::vector<std::vector<cell>> init(int length, int width);
    void print(std::vector<std::vector<cell>> grid);
    void tremaux(sim::MouseInterface* mouse, std::vector<std::vector<cell>> grid);
};

