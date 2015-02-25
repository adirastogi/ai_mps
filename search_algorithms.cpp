#include <queue>
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <map>
#include <set>
#include <string>
#include <cassert>
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
        //cout<<"At position\n";pos.print();
        MAZE_VAL(maze,pos) = V;
        if(pos==dest){
            found = true;
            //cout<<"Found the destination\n";
            break;
        }
        numnodes++;
        vector<coord> neighbors = get_neighbors(pos,M,N);
        for(int i=0;i<neighbors.size();++i){
            if(MAZE_VAL(maze,neighbors[i])==E) {
                //as pos is the parent of neighbor i                    
                neighbors[i].cost = pos.cost +1;
                //cout<<"\t";neighbors[i].print();
                //cout<<"\tqmetric:"<<man_dist(dest,neighbors[i])<<"\n";
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
    int misplaced_tiles_count(const state8& s){
        int count=0;
        for(int i=1;i<10;++i)
            if(s.state[i-1]-'0'!=i)count++;
        return count;
    }
    bool operator()(const state8 &a, const state8& b){
        int ca =  (misplaced_tiles_count(a)+a.cost);
        int cb =  (misplaced_tiles_count(b)+b.cost);
        if (ca!=cb) return ca<cb;
        else return a.state<b.state;
    }
};
struct manhattan_dist_tiles{
    public:
    int manhattan_dist_sum(const state8& s){
        int sum =0;
        for(int i=0;i<9;++i){
            coord c1(i/3,i%3),c2((s.state[i]-'1')/3,(s.state[i]-'1')%3);
            int mdist = man_dist(c1,c2);
            sum += mdist;
        }
        //std::cout<<"Manhattan Distance for "<<s.state <<" "<<sum<<"\n";
        return sum;
    } 
    //reqires a total order so they must also be lexicographically compared 
    bool operator()(const state8 &a,const state8& b){
        int ca =  (manhattan_dist_sum(a)+a.cost);
        int cb =  (manhattan_dist_sum(b)+b.cost);
        if(ca!=cb) return ca<cb;
        else return a.state < b.state;
    }

};
struct gasching_hr{
    public:
    int gasching_hr_val(const state8 &a){
        string base_str = "123456789";
        int count = 0;
        string state = a.state;
        int x=0;
        while(state!=base_str){
            //cout<<"State:"<<state<<"\n";
            for(int i=1;i<10;++i){
                //if not at its proper state
                if(state[i-1]-'0'!=i){
                    swap(state[i-1],state[state[i-1]-'0'-1]);
                    count++;
                    break;
                }
            }
        }
        //cout<<"\nThe gasching heuristic count for "<<a.state<<" is "<<count;
        return count;
    }
    bool operator()(state8 &a, state8& b){
        return (gasching_hr_val(a)+a.cost)<(gasching_hr_val(b)+b.cost)?false:true;
    }
};
template<class T>
bool astar_8_puzle(state8 start, int& numnodes, int& path_cost, T heuristic){
    

    /* the starting position has no parent and no move needs to be 
    executed */ 
    bool found = false;
    path_cost = 0;
    numnodes=0;
    map<string,string> visited;
    typedef typename set<state8,T>::iterator setiterator;
    map<string,setiterator> open; 
    set<state8,T> pq;
    std::pair<setiterator,bool> f = pq.insert(start);
    assert(f.second==true); 
    open[start.state]=f.first;
    state8 pos;
    while(!pq.empty()){
        typename set<state8,T>::iterator iter = pq.begin();
        pos = *(iter);
        pq.erase(iter);
        open.erase(open.find(pos.state));
        cout<<"At position "<<pos.state<<"\n";
        cout<<"With cost "<<pos.cost<<" \n";
        //cout<<"With Parent "<<pos.parent<<"\n";
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
            typename map<string,setiterator>::iterator openit; 
            setiterator setit ;
            neighbors[i].cost = pos.cost+1;
            cout<<"\tneighbor "<<neighbors[i].state<<" with hr "<<heuristic.manhattan_dist_sum(neighbors[i])<<"\n";
            //neighbour in closed set
            if(visited.find(neighbors[i].state)!=visited.end()){ 
                cout<<"\tvisited state "<<neighbors[i].state<<"\n";
                continue;
            //in openset, update the cost estimate by removing from set and adding again
            }else if((openit=open.find(neighbors[i].state))!=open.end()){
                setit = openit->second;
                if(setit->cost>neighbors[i].cost){
                    cout<<"\tupdated state "<<neighbors[i].state<<","<<"parent:"<<neighbors[i].parent<<","<<neighbors[i].cost<<" against "<<setit->state 
                    <<","<<setit->parent<<","<<setit->cost<<" \n";
                    pq.erase(setit);
                    open.erase(openit);
                    std::pair < typename set<state8,T>::iterator, bool > retval;
                    retval = pq.insert(neighbors[i]);
                    assert(retval.second==true);
                    setit = retval.first;
                    open[neighbors[i].state]=setit;
                }
            }else {
                std::pair < typename set<state8,T>::iterator, bool > retval;
                retval = pq.insert(neighbors[i]);
                assert(retval.second==true);
                setit = retval.first;
                open[neighbors[i].state]=setit;
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
            path_cost++;
        }
    }
    
}

void solve_maze_single();
void solve_maze_multiple();
void solve_8puzzle(){

    cout<<"Solving the 8 puzzle for 50 random runs\n";
    int avg_misplaced_path =0;
    int avg_misplaced_nodes =0;
    int avg_manhattan_path =0;
    int avg_manhattan_nodes =0;
    int avg_gasching_path =0;
    int avg_gasching_nodes =0;
    
    for(int i=0;i<1;++i){
        int numshuffle = rand()%10;
        int path_cost,numnodes;
        string source = "123456789";
        for(int j=0;j<numshuffle;++j)
            swap(source[rand()%9],source[rand()%9]);
        cout<<"Solving for string "<<source<<"\n";
        state8 start = {source,0,""};
        misplaced_tiles mt;
        manhattan_dist_tiles mh;
        gasching_hr gh;
        path_cost=0;numnodes=0;
        astar_8_puzle(start,numnodes,path_cost,mt);
        cout<<"With misplaced heuristic pathcost:"<<path_cost<<" numnodes "<<numnodes<<"\n";
        /*
        avg_misplaced_path += path_cost;
        avg_misplaced_nodes += numnodes;
        path_cost=0;numnodes=0;
        astar_8_puzle(start,numnodes,path_cost,mh);
        cout<<"With manhattan heuristic pathcost:"<<path_cost<<" numnodes "<<numnodes<<"\n";
        avg_manhattan_path += path_cost;
        avg_manhattan_nodes += numnodes;
        path_cost=0;numnodes=0;
        astar_8_puzle(start,numnodes,path_cost,gh);
        cout<<"With gasching heuristic pathcost:"<<path_cost<<" numnodes "<<numnodes<<"\n";
        avg_gasching_path += path_cost;
        avg_gasching_nodes += numnodes;
        */
    }
    cout<<"Average Misplaced pathcost and numnodes:"<<float(avg_misplaced_path)/50<<" "<<float(avg_misplaced_nodes)/50<<"\n";
    cout<<"Average Misplaced pathcost and numnodes:"<<float(avg_manhattan_path)/50<<" "<<float(avg_manhattan_nodes)/50<<"\n";
    cout<<"Average Misplaced pathcost and numnodes:"<<float(avg_gasching_path)/50<<" "<<float(avg_gasching_nodes)/50<<"\n";
    
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
    state8 start = {std::string("296134758"),0,string("124896753")};
    cout << "Running the DFS search on this\n";
    //misplaced_tiles mt;
    gasching_hr mt;
    astar_8_puzle(start,numnodes,path_cost,mt);
}
