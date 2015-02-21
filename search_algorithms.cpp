#include <queue>
#include <iostream>
#include <cstring>
#include <vector>
using namespace std;
#define W '%'
#define E ' '
#define D '.'
#define V '$'
#define Q 'Q'
#define MAZE_VAL(a,c) a[c.x][c.x]
struct coord {
    int x;
    int y;
    public:
    coord(int a , int b):x(a),y(b) {}
    bool operator==(const coord& c) {
        return (x==c.x)&&(y==c.y);
    }
    bool operator!=(const coord& c) {
        return !(this->operator==(c));
    }
};
enum direction {UP,DOWN,LEFT,RIGHT};
vecor<coord> get_neighbors(coord c){
    vector<coord> neighbors;
    if(c.x+1 < N ) neighbors.push_back(coord(c.x+1,c.y));
    if(c.y+1 < M ) neighbors.push_back(coord(c.x,c.y+1));
    if(c.x-1 >= 0) neighbors.push_back(coord(c.x-1,c.y));
    if(c.y-1 >= 0 ) neighbors.push_back(coord(c.x,c.y-1));
    return neighbors;
}
direction get_direction(coord p, coord c){
    if(p.x==c.x) return (p.y-c.y)>0?DOWN:UP;
    else return (p.x-c.x)>0 ? RIGHT:LEFT;
}
void bfs(char **maze, coord start, coord dest, int& numnodes, int& path_cost){
    
    char ** parent = new char[M];
    for( int i=0 ; i<M;++i){
        parent[i] = new char[N];
        memset(parent[i],0,sizeof(char)*N);
    }
    
    bool found = false;
    std::queue<coord> frontier;
    coord sentinel(-1,-1);
    frontier.push(start);
    frontier.push(sentinel);
    while(!frontier.empty()){
        coord pos = frontier.front();
        frontier.pop();
        if(pos==sentinel) {
            path_cost++;
            if(!frontier.empty()) frontier.push(sentinel) ;
            continue;
        }
        if(pos==dest){
            found = true;
            break;
        }
        numnodes++;
        vector<coord> neighbors = adjacent_cells(pos);
        for (int i=0; i< neighbors.size(); ++i){
            if(MAZE_VAL(maze,neighbors[i])==E) {
                frontier.push_back(neighbors[i]);
                MAZE_VAL(parent,neighbors[i]) = get_direction(pos,neighbors[i]);
            }
        }
    }

    if(found){
        coord trav = dest;
        while(trav!=start){
            MAZE_VAL(maze,trav) = D;
            direction dir  = MAZE_VAL(parent,trav);
            switch(dir){case UP: trav.y--;break; case DOWN:trav.y++;break; case LEFT: trav.x--;break; case RIGHT:trav.x++;break;}
        }
        MAZE_VAL(maze,trav) = D;
    }

    for(int i=0;i<M;++i)
        delete[] parent[i];
    delete parent[];    

}
/*
bool dfs(char ** maze, coord start, coord dest , int& numnodes, int& path_cost){


    numnodes++;
    if(start==dest){
        path_cost++;
        return true;
    }
 
    MAZE_VAL(maze,start)=V;
    vector<coord> neighbors = adjacent_cells(start);
    for (int i=0; i< neighbors.size(); ++i){
        if(MAZE_VAL(maze,neighbors[i])==E){
            if(dfs(maze,neighbors[i],dest,numnodes,path_cost)){
                MAZE_VAL(maze,neighbors[i])=D;
                path_cost++;
                return true;
            }
        }
    }

}


bool greedy_best_fit(char* maze,coord start, coord dest, int& numnodes, int& path_cost){
    
    coord pos = start;
    while(pos!=dest){
        MAZE_VAL(maze,pos) = V;
        vector<coord> neighbors = adjacent_cells(pos);
        int min_val = man_dist(pos,dest);
        for(int i=0;i<neighbors.size();++i){
            if(MAZE_VAL(maze,neighbors[i])!=E) continue;
            if(man_dist(neighbors[i],dest)<min_val){
                min_val = man_dist(neighbors[i],dest);
                pos = neighbors[i];
            }
        }
    }

}
*/
int main(){

    int numnodes, path_cost;
    cin>>M>>N;
    coord start, dest;
    char ** maze = new char*[M];
    for (int i=0;i<N;++i) 
        maze[i] = new char[N];
    for (int =0; i<M;++i)
        for(int j=0; J<N; ++j){
            cin>>maze[i][j];
            if(maze[i][j]=='P'){
                start.x = i; start.y = j;
            }
            if(maze[i][j]==D){
                dest.x = i; dest.y=j;
                maze[i][j]=E;
            }
        }
    
    cout << "Running the BFS search on this\n";
    
    bfs(maze,start,dest,numnodes,path_cost);
}
