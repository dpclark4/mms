#include "danAlgo.h"
#include <iostream>
#include <vector>
#include <stack>

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
    //    print(grid);
    int compass = 1; //N:1, E:2, S:3, W:4;
    int x = 0; //horizontal 
    int y = 15; //vertical
    int xright, xleft, xstraight, yright, yleft, ystraight; 
    stack<int> moves;
    while(1){
        xright = xleft = xstraight = x;
        yright = yleft = ystraight = y;
        //calculates next block to move
        if(compass == 1){
            xright = x+1;
            xleft = x-1;
            ystraight = y-1;
        }
        if (compass == 2){
            yright = y+1;
            yleft = y-1;
            xstraight = x+1;
        }
        if (compass == 3){
            xright = x-1;
            xleft = x+1;
            ystraight = y+1;
        }
        if (compass == 4){
            yright = y-1;
            yleft = y+1;
            xstraight = x-1;
        }
        //check to see if next block has been visited
        if(!mouse->wallRight() && grid[xright][yright].visited == 0){
            grid[x][y].visited=1;
            mouse->turnRight();
            mouse->moveForward();
            moves.push(3);
            x = xright;
            y = yright;
            compass == 4 ? compass = 1 : compass++;
        }
        else if (!mouse->wallLeft() && grid[xleft][yleft].visited == 0){
            grid[x][y].visited=1;
            mouse->turnLeft();
            moves.push(2);
            mouse->moveForward();
            x = xleft;
            y = yleft;
            compass == 1 ? compass = 4 : compass--;

        }
        else if(!mouse->wallFront() && grid[xstraight][ystraight].visited == 0){
            grid[x][y].visited=1; 
            x = xstraight;
            y = ystraight;
            mouse->moveForward();
            moves.push(1);
        }
        else{
            bool madeMove = false;
            while(1){
                madeMove = false;
                if(mouse->wallFront()) {
                    cout << "wall in front, turning around" << endl;
                    compass > 2 ? compass-=2 : compass+=2;
                    mouse->turnLeft();
                    mouse->turnLeft();   
                    madeMove = true;   
                    grid[x][y].visited==2;
                }
               // cout << "while compass: " << compass << endl; 
                xright = xleft = xstraight = x;
                yright = yleft = ystraight = y;
                //calculates next block to move
                if(compass == 1){
                    xright = x+1;
                    xleft = x-1;
                    ystraight = y-1;
                }
                if (compass == 2){
                    yright = y+1;
                    yleft = y-1;
                    xstraight = x+1;
                }
                if (compass == 3){
                    xright = x-1;
                    xleft = x+1;
                    ystraight = y+1;
                }
                if (compass == 4){
                    yright = y-1;
                    yleft = y+1;
                    xstraight = x-1;
                }
               
                /*
                   if(!mouse->wallFront() && grid[xstraight][ystraight].visited != 0){
                   cout << "can't move forward: " <<x << "," <<y << endl;
                   compass > 2 ? compass-=2 : compass+=2;
                   mouse->turnLeft();
                   mouse->turnLeft();      
                   grid[x][y].visited==2;
                   }
                 */
                if (!mouse->wallLeft() && grid[xleft][yleft].visited == 1){
                    //grid[x][y].visited=2;
                    mouse->turnLeft();
                    mouse->moveForward();
                    x = xleft;
                    y = yleft;
                    madeMove = true;
                    compass == 1 ? compass = 4 : compass--;

                }

                else if(!mouse->wallRight() && grid[xright][yright].visited==1){
                    //grid[x][y].visited=2;
                    mouse->turnRight();
                    mouse->moveForward();
                    x = xright;
                    y = yright;
                    madeMove = true;
                    compass == 4 ? compass = 1 : compass++;
                }

                else if(!mouse->wallFront() && grid[xstraight][ystraight].visited == 1){
                    //grid[x][y].visited=2; 
                    x = xstraight;
                    y = ystraight;
                    madeMove = true;
                    mouse->moveForward();
                }
                if(!mouse->wallRight() && grid[xright][yright].visited==0){
                    grid[x][y].visited=2;
                    cout << "Found right " << x << "," << y << endl;
                    break;
                }
                if(!mouse->wallLeft() && grid[xleft][yleft].visited==0){
                    grid[x][y].visited=2;
                    cout << "Found left " << x << "," << y << endl;
                    break;
                }
                if(!mouse->wallFront() && grid[xstraight][ystraight].visited==0){
                    grid[x][y].visited=2;
                    cout << "Found front " << x << "," << y<< "c: " << compass << endl;
                    break;
                }
                if(madeMove == false){
                    cout << "didn't make move this round" << endl;
                    compass > 2 ? compass-=2 : compass+=2;
                    mouse->turnLeft();
                    mouse->turnLeft();      
                    grid[x][y].visited==2;
                    cout << grid[x][y].visited << endl;
                    madeMove = true;
                }
            }
            //cout << "backtrace" << compass <<  endl;
        }

    }
}
