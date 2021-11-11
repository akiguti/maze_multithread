#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <iomanip>
using namespace std;

// IGNACIO GUTIERREZ MARTINEZ, 14206
// ISE 2020-2021 MII (UPM)
//
//
// Coordinates:
// Top left corner      ==> (00,00)
// Bottom right corner  ==> (99,24)
int maze (int Ix, int Iy, char dir, char mat[][25], int steps) {
    // Arguments: 
    // Initial position, direction, labyrinth 2D array, number of steps previously given
    int id = 0;
    struct point{
        int x;
        int y;
    };
    if (Ix < 0 || Iy < 0){
        cout << "Error: negative coordinates " << Ix << "," << Iy << "\n";
        return 1;
    }
    // To avoid infinite loops, we place a rock into an already used bifurcation
    mat[Ix][Iy] = '#';
    // Start at I coordinates ==> go in the selected direction 
    // up:      negative y
    // down:    positive y
    // right:   positive x
    // left:    negative x
    point pos;
    point pos_prev;
    pos_prev.x=Ix;
    pos_prev.y=Iy;
    switch (dir)
    {
    case 'r':
        pos.x=Ix + 1;
        pos.y=Iy;
        break;
    case 'l':
        pos.x=Ix - 1;
        pos.y=Iy;
        break;
    case 'u':
        pos.x=Ix;
        pos.y=Iy - 1;
        break;
    case 'd':
        pos.x=Ix;
        pos.y=Iy + 1;
        break;
    default:
        break;
    }
    //number of bifurcations
    int n_bifurcations = 0;
    //flags of direction
    int up = 0;
    int down = 0;
    int right = 0;
    int left = 0;
    int f_out = 0;
    // Bifurcation flag:
    //   1: there is a bifurcation
    //   0: no bifurcation
    int f_bifurcation = 0;
    while (!f_bifurcation){ // While there is not a bifurcation: keep going forward
        //LOOK AROUND
        //up 
        if (mat[pos.x][pos.y - 1] != '#' && pos_prev.y != pos.y - 1){
            n_bifurcations++;
            up++;
        }
        //down
        if (mat[pos.x][pos.y + 1] != '#' && pos_prev.y != pos.y + 1){
            n_bifurcations++;
            down++;
        }
        //right
        if (mat[pos.x + 1][pos.y] != '#' && pos_prev.x != pos.x + 1){
            n_bifurcations++;
            right++;
            if (mat[pos.x + 1][pos.y] == 'O'){
                // end of the maze
                pos.x ++;
                steps += 2;
                std::stringstream display;
                display        << "  " << setw(5) << setfill('0') << std::this_thread::get_id()
                          << "   |    " << setw(2) << setfill('0') << Ix 
                          << "," << setw(2) << setfill('0') << Iy 
                          << "   |"
                          << "    " << setw(2) << setfill('0') << pos.x 
                          << "," << setw(2) << setfill('0')<< pos.y
                          << "   |"
                          << "    "
                          << setw(4) << setfill('0') << steps
                          << "   |" << " ==> OUT SUCCESSFUL" << '\n';
                std::cout << display.str();
                // Sleep until the end of the program to print the number of steps
                // 0,5 seconds
                usleep(500000);
                std::cout << '\n';
                cout << "Minimum number of steps to get out of labyrinth: " << steps << "\n";
                std::cout << '\n';

                f_out++;
            }
        }
        //left
        if (mat[pos.x - 1][pos.y] != '#' && pos_prev.x != pos.x - 1){
            n_bifurcations++;
            left++;
        }
        if(f_out) {
            //end of the maze
            return 0;
        }
        // Next step is not a bifurcation ==> calculate next position
        if (n_bifurcations == 1){
            pos_prev = pos;
            pos.x = pos.x + right - left;
            pos.y = pos.y - up + down;
            up = 0;
            down = 0;
            right = 0;
            left = 0;
            n_bifurcations = 0;
            steps ++;
        }
        else{
            if (n_bifurcations == 0){
                std::stringstream display;
                display        << "  " << setw(5) << setfill('0') << std::this_thread::get_id()
                          << "   |    " << setw(2) << setfill('0') << Ix 
                          << "," << setw(2) << setfill('0') << Iy 
                          << "   |"
                          << "    " << setw(2) << setfill('0') << pos.x 
                          << "," << setw(2) << setfill('0')<< pos.y
                          << "   |"
                          << "    "
                          << setw(4) << setfill('0') << steps
                          << "   |" << " ==> DEAD END" << '\n';
                std::cout << display.str();
                // Dead end ==> destroy 
                return 0;
                
            }
            steps ++;
            // Bifurcations ==> threading
            f_bifurcation = 1;
            // n_bifurcations = 0 : dead end
            // n_bifurcations = 1 : no bifurcation
            // n_bifurcations = 2 : bifurcation
            // n_bifurcations = 3 : tri-furcation
            // n_bifurcations = 4 : not possible (prev position does not increase n_bifurcations)
            std::stringstream display;
            display     << "  " << setw(5) << setfill('0') << std::this_thread::get_id()
                        << "   |    " << setw(2) << setfill('0') << Ix 
                        << "," << setw(2) << setfill('0') << Iy 
                        << "   |"
                        << "    " << setw(2) << setfill('0') << pos.x 
                        << "," << setw(2) << setfill('0')<< pos.y
                        << "   |"
                        << "    "
                        << setw(4) << setfill('0') << steps
                        << "   |" << " ==> BIFURCATION (" << n_bifurcations << ")" << '\n';
            std::cout << display.str();
            // thread vector (maximum 1000)
            std :: thread threads [ 1000 ];
            // number of threads
            int n_thr = 0;
            if (right && pos_prev.x != pos.x + 1){
                threads [n_thr]= thread (maze,pos.x, pos.y, 'r', mat, steps);
                n_thr ++;    
            }
            if (left && pos_prev.x != pos.x - 1){
                threads [n_thr]= thread (maze,pos.x, pos.y, 'l', mat, steps);
                n_thr ++;
            }
            if (up && pos_prev.y != pos.y + 1){
                threads [n_thr]= thread (maze,pos.x, pos.y, 'u', mat, steps);
                n_thr ++; 
            }
            if (down && pos_prev.y != pos.y + 1){
                threads [n_thr]= thread (maze,pos.x, pos.y, 'd', mat, steps);
                n_thr ++;
            }
            for (int i = 0 ; i < n_thr ; i++){
                threads [i].join(); 
            }
            return 0;
        }
    }
    return 0;
}

int main(int argc, char ** argv) { 
    // This program needs an imput argument
    // It must be a .txt file placed in the same directory as the executable
    if (argv[1] == NULL){
        cout << "ERROR: Empty argument\n";
        cout << "Type the name of a .txt file after the program name\n";
        return 1;
    }else{
        cout << "Argument: " << argv[1] << "\n";
    }
    struct point{
        int x;
        int y;
    };
    // number of lines and columns of the maze
    int lines = 25;
    int cols = 100;
    // matrix including the data from the file
    char mat[100][25] = { };
    // cols:    0-->99
    // lines:   0-->24
    point I;
    point O;
    std::string line;
    // READ THE FILE (NAME FROM ARGUMENT)
    std::ifstream myfile(argv[1]);
    //put the file into a 2D array (mat)
    for (int j = 0; getline(myfile, line) ; j++){
        for (int i = 0; i < cols && j< lines; i++){
            mat[i][j] = line[i];
            if (line[i] == 'I'){
                I.x = i;
                I.y = j;
            }
            if (line[i] == 'O'){
                O.x = i;
                O.y = j;
            } 
        }
    }
    std::cout << '\n';
    std::cout << "| MAZE REPORT |" << '\n';
    std::cout << '\n';
    std::cout << "IN  position: "  << setw(2) << setfill('0') << I.x << ',';
    std::cout << setw(2) << setfill('0') << I.y << '\n';
    std::cout << "OUT position: "  << setw(2) << setfill('0') << O.x << ',';
    std::cout << setw(2) << setfill('0') << O.y << '\n';
    std::cout << '\n';
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::cout << "Main thread id: ";
    std::cout << setw(4) << setfill('0') << std::this_thread::get_id() << '\n';
    std::cout << '\n';
    std::cout << "Thread id | Start pos. |  End pos.  | No. steps |"<< '\n';

    // Execute the maze function 
    // Input parameters:
    // Initial position, direction of next step, maze matrix, number of steps already done
    maze (I.x, I.y, 'r', mat, 0 );
    // The report is printed by the maze function.
    // End of the program
    return 0;
}