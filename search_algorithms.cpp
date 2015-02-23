#include <queue>
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <map>
#include <string>
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
    coord(int a , int b, int c=0):x(a),y(b),cost(c){}
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
vector<coord> get_neighbors(coord c,int M, int N){
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
        vector<coord> neighbors = get_neighbors(pos,M,N);
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
    vector<coord> neighbors = get_neighbors(start,M,N);
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
        vector<coord> neighbors = get_neighbors(pos,M,N);
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
        vector<coord> neighbors = get_neighbors(pos,M,N);
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
/*
void execute_puzzle_move(int ** puzzle, char** parent, cood pos){

    coord pcoord(-1,-1);
    switch(MAZE_VAL(parent,pos)){
        case UP : { pcoord.y = pos.y-1; pccordd.x = pos.x;break; }
        case DOWN : { pcoord.y = pos.y+1; pccordd.x = pos.x;break; }
        case LEFT : { pcoord.y = pos.y; pccordd.x = pos.x-1;break; }
        case RIGHT : { pcoord.y = pos.y; pccordd.x = pos.x+1;break; }
        default : return;
    }
    MAZE_VAL(puzzle,pcoord) = MAZE_VAL(puzzle,pos);
    MAZE_VAL(puzzle,pos) = 9;
}

bool check_solution(int ** puzzle){
    int number = 1;
    for(int i=0;i<8;++i)
        for(int j=0;j<8;++j)
            if(puzzle[i][j]!=number++)
                return false;
    return true;
}
*/

struct state8{
    string state;   
    int cost;
    string parent;
};
state8 execute_move(state8 s,direction dir){
    
    int pos,i,j,newpos;
    string parent = s.state;
    //find the curent
    for(pos=0;pos<9 && s.state[pos]!='9';++pos);
    i = pos/3;
    j = pos%3;
    switch(dir){
        case UP: i--;break; case DOWN:i++;break; case LEFT:j--;break; case RIGHT:j++;break;
    }
    newpos = 3*i+j;
    swap(s.state[newpos],s.state[pos]);
    s.cost++;
    s.parent = parent;
    return s;
}
vector<state8> get_8neighbors(state8 s){
    vector<state8> neighbors;
    int i,j,pos;
    for(pos=0;pos<9 && s.state[pos]!='9';++pos);
    i = pos/3;
    j = pos%3;
    if(i+1<3) neighbors.push_back(execute_move(s,DOWN));
    if(j+1<3) neighbors.push_back(execute_move(s,RIGHT));
    if(i-1>=0) neighbors.push_back(execute_move(s,UP));
    if(j-1>=0) neighbors.push_back(execute_move(s,LEFT));
    return neighbors;
}
bool check_solution(state8 s){
    for(int i=1;i<10;++i)
        if(s.state[i-1]-'0'!=i)
            return false;
    return true;

}
void print_puzzle(string s){
    for(int i=0;i<3;++i){
        for(int j=0;j<3;++j)
            cout<<s[3*i+j]<<" ";
        cout<<"\n";
    }
}

/* heuristic for # misplaced tiles */
struct misplaced_tiles{
    public:
    int misplaced_tiles_count(state8& s){
        int count=0;
        for(int i=1;i<10;++i)
            if(s.state[i-1]-'0'!=i)count++;
        return count;
    }
    bool operator()(state8 &a, state8& b){
        return (misplaced_tiles_count(a)+a.cost)< (misplaced_tiles_count(b)+b.cost)?false:true;
    }
};
struct manhattan_dist_tiles{
    public:
    int manhattan_dist_sum(state8& s){
        int sum =0;
        for(int i=0;i<9;++i){
            coord c1(i/3,i%3),c2((s.state[i]-'0')/3,(s.state[i]-'1')%3);
            int mdist = man_dist(c1,c2);
            sum += mdist;
        }
        return sum;
    } 
    bool operator()(state8 &a, state8& b){
        return (manhattan_dist_sum(a)+a.cost)< (manhattan_dist_sum(b)+b.cost)?false:true;
    }

};
template<class T>
bool astar_8_puzle(state8 start, int& numnodes, int& path_cost, T heuristic){
    

    /* the starting position has no parent and no move needs to be 
    executed */ 
    bool found = false;
    map<string,string> visited;
    priority_queue<state8,vector<state8>,T> pq(heuristic);
    pq.push(start);
    state8 pos;
    while(!pq.empty()){
        pos = pq.top();
        pq.pop();
        cout<<"At position\n";print_puzzle(pos.state);
        cout<<"With Parent "<<pos.parent<<"\n";
        visited[pos.state]=pos.parent;
        if(check_solution(pos)){
            found = true;
            cout<<"Found the destination\n";
            print_puzzle(pos.state);
            break;
        }
        numnodes++;
        vector<state8> neighbors = get_8neighbors(pos);
        for(int i=0;i<neighbors.size();++i){
            if(visited.find(neighbors[i].state)==visited.end()) {
                cout<<"\t"<<neighbors[i].state<<"\n";
                //cout<<"\tqmetric:"<<man_dist(dest,neighbors[i])<<"\n";
                pq.push(neighbors[i]);
            }
        }
    }
    if(found){
        cout<<"The sequence of operations is \n";
        string trav = pos.state;
        while(trav!=start.state){
            cout<<"\n";
            print_puzzle(trav);
            trav=visited[trav];
        }
    }
    
}

int main(){
    int numnodes, path_cost;
    /*
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
    }*/
    state8 start = {std::string("124896753"),0,string("124896753")};
    cout << "Running the DFS search on this\n";
    //misplaced_tiles mt;
    manhattan_dist_tiles mt;
    astar_8_puzle(start,numnodes,path_cost,mt);
}
