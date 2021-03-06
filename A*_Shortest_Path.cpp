#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <deque>
#include <iomanip>

using namespace std;

// Map class
class Map {

public:
    const static int mapWidth = 6;
    const static int mapHeight = 5;
    vector<vector<int> > grid = {
            {0, 1, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 0}
    };

    vector<vector<int> > manhattan_heuristic = {
            { 9, 8, 7, 6, 5, 4 },
            { 8, 7, 6, 5, 4, 3 },
            { 7, 6, 5, 4, 3, 2 },
            { 6, 5, 4, 3, 2, 1 },
            { 5, 4, 3, 2, 1, 0 }
    };
};

// Planner class
class Planner : Map {
public:
    int start[2] = {0, 0};
    int goal[2] = {mapHeight - 1, mapWidth - 1};
    int cost = 1;

    string movements_arrows[4] = {"^", "<", "v", ">",};

    vector<vector<int> > movements{
            {-1, 0},
            {0,  -1},
            {1,  0},
            {0,  1}
    };

};

// Template function to print 2D vectors of any type
template<typename T>
void print2DVector(T Vec) {
    for (int i = 0; i < Vec.size(); ++i) {
        for (int j = 0; j < Vec[0].size(); ++j) {
            cout << setw(2) << Vec[i][j] << ' ';
        }
        cout << endl;
    }
}



/*#### TODO: Code the search function which will generate the expansion list ####*/
// You are only required to print the final triplet values
void search(Map map, Planner planner) {

    int y = 0;
    int x = 0;
    int new_x, new_y;
    int cost = 0;
    int count = 0;
    int direction_num = sizeof(planner.movements_arrows) / sizeof(planner.movements_arrows[0]);
    deque<vector<int>> frontier_queue;
    vector<vector<int>> footprint(map.mapHeight, vector<int>(map.mapWidth));
    vector<vector<string> > grid_paths(map.mapHeight, vector<string>(map.mapWidth, "-"));

    string mark;
    int directrion;
    int parent_y, parent_x;
    int f;


    // init start point
    frontier_queue.push_back({map.manhattan_heuristic[planner.start[0]][planner.start[1]], cost,
                              planner.start[0], planner.start[1], 0, planner.start[0], planner.start[1]});
    footprint[planner.start[0]][planner.start[1]] = 1;  //mark this as to avoid duplicate expansion
    grid_paths[planner.start[0]][planner.start[1]] = 1;  //mark this as to avoid duplicate expansion


    // replace 1 with -1
    for (auto &v : map.grid)
        replace(v.begin(), v.end(), 1, -1);

//    print2DVector(map.grid);  //debug

    for (;;) {
        // print

        print2DVector(map.grid);  //debug
        cout << "Expansion #:" << count << endl;
        cout << "OpenList: ";
        for (auto &cell : frontier_queue) {
            cout << "[" << cell[0] << " " << cell[1] << " " << cell[2] <<  " " << cell[3] << "]";
        }
        cout << endl;

        // move
        if (frontier_queue.size() == 0) {
            cout << "loop all the grid, but haven't found the goal??" << endl;
            return;
        }
        // move: get the element with lowest f from the list, as a new start
        sort(begin(frontier_queue), end(frontier_queue));
        vector<int> new_start = frontier_queue.front();
        frontier_queue.pop_front();

        f = new_start[0];
        cost = new_start[1];
        y = new_start[2];
        x = new_start[3];
        directrion = new_start[4];
        parent_y = new_start[5];
        parent_x = new_start[6];

        cout << "Cell Picked: ";
        cout << "[" << f << " " << cost << " " << y << " " << x << " " << directrion << "]" << endl;
        cout << endl;

        // reach Goal??
        if (x == planner.goal[1] && y == planner.goal[0]) {
            cout << "Arrive the goal!" << endl;
            grid_paths[y][x] = planner.movements_arrows[directrion];
//            grid_paths[y][x] = "*";
            map.grid[y][x] = cost;
            break;
        }

        // mark the last pos
        grid_paths[y][x] = planner.movements_arrows[directrion];
        map.grid[y][x] = cost;


        // loop "^ < v >" to add the frontier to the queue
        for (int i = 0; i < planner.movements.size(); i++) {
            new_y = y + planner.movements[i][0];
            new_x = x + planner.movements[i][1];
            if (new_x < 0 || new_x > map.mapWidth - 1)
                continue;
            if (new_y < 0 || new_y > map.mapHeight - 1)
                continue;

            // add the available cell to the grid
            if (map.grid[new_y][new_x] == 0) {
                // In fact, to avoid push duplicate cell,
                // I can either search in the frontier queue or maintain another "footprint" queue.
                // I chose search way, but actually "done" queue should be better(faster)
                if (footprint[new_y][new_x] == 0) {
                    // planner.cost mean the cost of each movement
                    // 'i' represents the diction
                    // f = h + g(cost)
                    f = map.manhattan_heuristic[new_y][new_x] + cost + planner.cost;
                    frontier_queue.push_back({f, cost + planner.cost, new_y, new_x, i, y, x});
                    footprint[new_y][new_x] = 1;
                }
            }
        }

        // next
        count++;
    }

    cout << "Done:" << cost << " " << y << " " << x << " " << parent_y << " " << parent_x << endl << endl;
    print2DVector(map.grid);
    cout << endl;
    print2DVector(grid_paths);  // print all the paths

    //  reverse traverse based on grid_paths to find the shortest_path to the goal
    vector<vector<string> > shortest_path(map.mapHeight, vector<string>(map.mapWidth, "-"));
    cout << "============== reverse traverse based ==============" << endl;

    for (;;) {
        // find parents' direction symbol
        auto it = find(begin(planner.movements_arrows), end(planner.movements_arrows), grid_paths[y][x]);
        int index = distance(begin(planner.movements_arrows), it);
        if (index >= direction_num) {
            cout << "Error" << endl;
            return;
        }
        parent_y = y - planner.movements[index][0];
        parent_x = x - planner.movements[index][1];
        shortest_path[parent_y][parent_x] = planner.movements_arrows[index];

        if (parent_y == planner.start[0] && parent_x == planner.start[1]) {
            shortest_path[planner.goal[0]][planner.goal[1]] = "*";
            break;
        }

        cout << endl;
        print2DVector(shortest_path);

        y = parent_y;
        x = parent_x;
    }
    cout << "----------- Final shortest_path output -------------" << endl;
    print2DVector(shortest_path);
}

int main() {
    // Instantiate map and planner objects
    Map map;
    Planner planner;

    // Search for the expansions
    search(map, planner);

    return 0;
}
