#include <queue>
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
using namespace std;
#define W '%'
#define E '_'
#define D '.'
#define V '$'
#define Q 'Q'
#define MAZE_VAL(a,c) a[c.y][c.x]
int M,N;
struct coord {
    int x;
    int y;
    int cost ;
    public:
    coord(int a , int b, int c=0):x(a),y(b),cost(c) {}
    bool operator==(const coord& c) {
        return (x==c.x)&&(y==c.y);
    }
    bool operator!=(const coord& c) {
        return !(this->operator==(c));
    }
    void print(){
        cout<<"node("<<x<<","<<y<<","<<cost<<")\n";
    }
};

int man_dist(coord c1, coord c2){
    return abs(c1.x-c2.x) + abs(c1.y-c2.y);
}

struct manhattan_comp{
    public:
    coord dest;
    manhattan_comp(coord& c):dest(c){};
    bool operator()(coord& a , coord& b ){
        return ((man_dist(a,dest))<(man_dist(b,dest)))?false:true;
    }
};
struct manhattan_a_star_comp{
    public:
    coord dest;
    manhattan_a_star_comp(coord& c):dest(c){
    }
    bool operator()(coord& a, coord& b){
        return (man_dist(a,dest)+a.cost)<(man_dist(b,dest)+b.cost)?false:true;
    }

};
enum direction {UP,DOWN,LEFT,RIGHT};
vector<coord> get_neighbors(coord c){
    vector<coord> neighbors;
    if(c.x+1 < N ) neighbors.push_back(coord(c.x+1,c.y));
    if(c.y+1 < M ) neighbors.push_back(coord(c.x,c.y+1));
    if(c.x-1 >= 0) neighbors.push_back(coord(c.x-1,c.y));
    if(c.y-1 >= 0) neighbors.push_back(coord(c.x,c.y-1));
    return neighbors;
}
direction get_direction(coord p, coord c){
    if(p.x==c.x) return (p.y-c.y)>0?DOWN:UP;
    else return (p.x-c.x)>0 ? RIGHT:LEFT;
}
void print_maze(char** maze){

    for(int i=0;i<M;++i){
        for(int j=0;j<N;++j)
            cout<<maze[i][j];
        cout<<"\n";
    }

}


void bfs(char **maze, coord start, coord dest, int& numnodes, int& path_cost){
    
    char ** parent = new char*[M];
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
            cout<<"Found sentinel\n";
            path_cost++;
            if(!frontier.empty()) frontier.push(sentinel) ;
            continue;
        }
        cout<<"At position\n";pos.print();
        MAZE_VAL(maze,pos) = V;
        if(pos==dest){
            found = true;
            cout<<"Found the destination\n";
            break;
        }
        numnodes++;
        vector<coord> neighbors = get_neighbors(pos);
        //cout<<"printing the neighbors of size "<<neighbors.size()<<"\n";
        for (int i=0; i< neighbors.size(); ++i){
            if(MAZE_VAL(maze,neighbors[i])==E) {
                cout<<"\t";neighbors[i].print();
                frontier.push(neighbors[i]);
                MAZE_VAL(parent,neighbors[i]) = get_direction(pos,neighbors[i]);
            }
        }
    }

    if(found){
        coord trav = dest;
        while(trav!=start){
            MAZE_VAL(maze,trav) = D;
            direction dir  = static_cast<direction>(MAZE_VAL(parent,trav));
            switch(dir){case UP: trav.y--;break; case DOWN:trav.y++;break; case LEFT: trav.x--;break; case RIGHT:trav.x++;break;}
        }
        MAZE_VAL(maze,trav) = (char)D;
    }


    for(int i=0;i<M;++i)
        delete[] parent[i];
    delete[] parent;    

}

bool dfs(char ** maze, coord start, coord dest , int& numnodes, int& path_cost){


    cout<<"DFS search at node\n";start.print();
    numnodes++;
    if(start==dest){
        path_cost++;
        return true;
    }

    MAZE_VAL(maze,start)=V;
    vector<coord> neighbors = get_neighbors(start);
    for (int i=0; i< neighbors.size(); ++i){
        if(MAZE_VAL(maze,neighbors[i])==E){
            if(dfs(maze,neighbors[i],dest,numnodes,path_cost)){
                MAZE_VAL(maze,neighbors[i])=D;
                path_cost++;
                return true;
            }
        }
    }
    
    return false;
}


bool greedy_best_fit(char** maze,coord start, coord dest, int& numnodes, int& path_cost){
    
    char ** parent = new char*[M];
    for( int i=0 ; i<M;++i){
        parent[i] = new char[N];
        memset(parent[i],0,sizeof(char)*N);
    }
    
    bool found = false;
    manhattan_comp mcomp(dest);
    priority_queue<coord,vector<coord>,manhattan_comp> pq(dest);
    pq.push(start);
    while(!pq.empty()){
        coord pos = pq.top();
        pq.pop();
        cout<<"At position\n";pos.print();
        MAZE_VAL(maze,pos) = V;
        if(pos==dest){
            found = true;
            cout<<"Found the destination\n";
            break;
        }
        numnodes++;
        vector<coord> neighbors = get_neighbors(pos);
        for(int i=0;i<neighbors.size();++i){
            if(MAZE_VAL(maze,neighbors[i])==E) {
                cout<<"\t";neighbors[i].print();
                cout<<"\tqmetric:"<<man_dist(dest,neighbors[i])<<"\n";
                pq.push(neighbors[i]);
                MAZE_VAL(parent,neighbors[i]) = get_direction(pos,neighbors[i]);
            }
        }
    }

    if(found){
        coord trav = dest;
        while(trav!=start){
            MAZE_VAL(maze,trav) = D;
            direction dir  = static_cast<direction>(MAZE_VAL(parent,trav));
            switch(dir){case UP: trav.y--;break; case DOWN:trav.y++;break; case LEFT: trav.x--;break; case RIGHT:trav.x++;break;}
        }
        MAZE_VAL(maze,trav) = (char)D;
    }

    for(int i=0;i<M;++i)
        delete[] parent[i];
    delete[] parent;    
}

bool a_star(char** maze,coord start, coord dest, int& numnodes, int& path_cost){
    
    char ** parent = new char*[M];
    for( int i=0 ; i<M;++i){
        parent[i] = new char[N];
        memset(parent[i],0,sizeof(char)*N);
    }
    
    bool found = false;
    
    manhattan_a_star_comp mcomp(dest);
    priority_queue<coord,vector<coord>,manhattan_a_star_comp> pq(mcomp);
    pq.push(start);
    while(!pq.empty()){
        coord pos = pq.top();
        pq.pop();
        cout<<"At position\n";pos.print();
        MAZE_VAL(maze,pos) = V;
        if(pos==dest){
            found = true;
            cout<<"Found the destination\n";
            break;
        }
        numnodes++;
        vector<coord> neighbors = get_neighbors(pos);
        for(int i=0;i<neighbors.size();++i){
            if(MAZE_VAL(maze,neighbors[i])==E) {
                //as pos is the parent of neighbor i                    
                neighbors[i].cost = pos.cost +1;
                cout<<"\t";neighbors[i].print();
                cout<<"\tqmetric:"<<man_dist(dest,neighbors[i])<<"\n";
                pq.push(neighbors[i]);
                MAZE_VAL(parent,neighbors[i]) = get_direction(pos,neighbors[i]);
            }
        }
    }

    if(found){
        coord trav = dest;
        while(trav!=start){
            MAZE_VAL(maze,trav) = D;
            direction dir  = static_cast<direction>(MAZE_VAL(parent,trav));
            switch(dir){case UP: trav.y--;break; case DOWN:trav.y++;break; case LEFT: trav.x--;break; case RIGHT:trav.x++;break;}
        }
        MAZE_VAL(maze,trav) = (char)D;
    }

    for(int i=0;i<M;++i)
        delete[] parent[i];
    delete[] parent;    

}

int main(){

    int numnodes, path_cost;
    cin>>M>>N;
    cout<<"The values are "<<M<<" "<<N<<"\n";
    coord start(0,0), dest(0,0);
    char ** maze = new char*[M];
    for (int i=0;i<N;++i) 
        maze[i] = new char[N];
    for (int i=0; i<M;++i){
        for(int j=0; j<N; ++j){
            cin>>maze[i][j];
            if(maze[i][j]=='P'){
                start.x = j; start.y = i;
            }
            if(maze[i][j]==D){
                dest.x = j; dest.y=i;
                maze[i][j]=E;
            }
            cout<<maze[i][j];
        }
        cout<<"\n";
    }
    cout << "Running the DFS search on this\n";
    greedy_best_fit(maze,start,dest,numnodes,path_cost);
    print_maze(maze);
}
