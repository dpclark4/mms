#include "danAlgo.h"
#include <iostream>
#include <vector>

using namespace std;

void danAlgo::solve(sim::MouseInterface* mouse){
    mouse->declareInterfaceType(sim::DISCRETE);
    test(mouse);
}
void danAlgo::test(sim::MouseInterface* mouse){
    vector<vector<struct cell>> grid = init(16,16);
    tremaux(mouse, grid);
}

vector<vector<danAlgo::cell>> danAlgo::init(int length, int width){
    vector<vector<struct cell>> grid;
    for(int i = 0; i < length/2; i++){
        vector<struct cell> row;
        for(int j = 0; j < width; j++){
            struct cell pos;
            pos.visited = 0;
            j < width/2 ? pos.distance = width-2-j-i : pos.distance = j-i-1;
            row.push_back(pos);
        }
        grid.push_back(row);
    } 
    for(int i = length/2; i < length; i++){
        vector<struct cell> row;
        for(int j = 0; j < width; j++){
            struct cell pos;
            pos.visited = 0;
            j < width/2 ? pos.distance = i-1-j : pos.distance = -1*(width-2-j-i)-2;
            row.push_back(pos);
        }
        grid.push_back(row);
    }
    return grid; 
} 

void danAlgo::print(vector<vector<struct cell>> grid){
    for(int i = 0; i < grid.size(); i++){
        for(int j = 0; j < grid[i].size(); j++){
            cout << grid[i][j].distance << "\t";
        }
        cout << endl;
    }
}

void danAlgo::tremaux(sim::MouseInterface* mouse, vector<vector<danAlgo::cell>> grid){
    print(grid);
    int compass = 1; //N:1, E:2, S:3, W:4;
    int x = 0; //horizontal 
    int y = 15; //vertical
    while(1){
        if(!mouse->wallRight() && grid[x][y].visited==0){

            grid[x][y].visited=1; 
            if(compass == 1){
                x++;
                compass = 2;
            }
            else if (compass == 2){
                y++;
                compass = 3;
            }
            else if(compass == 3){
                x--;
                compass = 4;
            }
            else if(compass == 4){
                y--;
                compass = 1;
            }
            mouse->turnRight();
            mouse->moveForward();
            cout << "R x: " << x << " y: " << y << " compass: " << compass << endl;
        }
        else if (!mouse->wallLeft() && grid[x][y].visited==0){
            grid[x][y].visited=1; 
            if(compass == 1){
                x--;
                compass = 4;
            }
            else if (compass == 2){
                y--;
                compass = 1;
            }
            else if(compass == 3){
                x++;
                compass = 2;
            }
            else if(compass==4){
                y++;
                compass = 3;
            }
            mouse->turnLeft();
            mouse->moveForward();
            cout << "L x: " << x << " y: " << y << " compass: " << compass << endl;
        }
        else if(!mouse->wallFront() && grid[x][y].visited==0){
            grid[x][y].visited=1; 
            if(compass==1)
                y--;
            else if (compass==2)
                x++;
            else if(compass==3)
                y++;
            else if(compass==4)
                x--;
            mouse->moveForward();
            cout << "F x: " << x << " y: " << y << " compass: " << compass << endl;
        }
        else{

            //cout << "backtrace" << endl;
        }

    }
}
