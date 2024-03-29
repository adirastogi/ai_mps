#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <climits>
#include <string>
#include <set>
#include <queue>
#include <sys/time.h>
using namespace std;

#define BLUE 0
#define GREEN 1
#define CHANCE 2
#define EMPTY 3
#define MAX_X 6
#define MAX_Y 6
#define MAX_DEPTH 4
#define MAX_DEPTH_AB 5
#define MAX_DEPTH_EM 4

#define DEBUG 0
#define myprintf(fmt, ...) if(DEBUG) printf(fmt, ##__VA_ARGS__);

typedef std::pair<int,int> coord;

/* struct to hold the state of the game, takes classes that define Algo, Order and Util */
class GameBoard{
   
    public:
    int (*board)[2];        //board that contains the points with each position;
    int* total_util;    //tracking the total utility of each player
    int numrows;        //num rows in board
    int numcols;        //num cols in board
    int numplayers;     //number of players in the game
    int maxagent;       //color for the maxagent
    int minagent;       //color for the minagent
    
    GameBoard(int,int,int,int,int);
    ~GameBoard();
    int move_normal(coord& pos,int curragent,vector<coord>& s);
    void undo_move_normal(coord& pos,int curragent,int otheragent,vector<coord>& s);


    void undo_move_sabotage(coord& movepos,int curr_agent,int other_agent,std::vector<coord>& sabotage_pos,bool sabotage);
    int move_sabotage(coord& movepos,int curr_agent,std::vector<coord>& sabotage_pos,bool sabotage);

    void identify_blitz_pos(coord& pos, int curragent, vector<coord>& blitzpos);
    void identify_sabotage_pos(coord& pos, int curragent, vector<coord>& sabotage_pos);
    bool terminal_pos();
    void print_board(int);
    void print_board();
    void reset();
};
/*initialization of the game */
GameBoard::GameBoard(int n,int m, int mnagent, int mxagent, int nump=2):
numrows(n),numcols(m),minagent(mnagent),maxagent(mxagent),numplayers(nump){
    board = new int[numrows*numcols][2];
    for(int i=0;i<numrows;++i){
        for (int j=0;j<numcols;++j){
            board[i*numcols+j][0] = 0;
            board[i*numcols+j][1] = EMPTY;
        }
    }
    total_util = new int[numplayers]; 
    for (int i=0;i<numplayers;++i)
        total_util[i]=0;
}

/* clean up the memory */
GameBoard::~GameBoard(){
    delete[] board;
}
/* check if the board is full*/
bool GameBoard::terminal_pos(){
    bool full=true;
    for(int i=0;i<numrows && full ;++i)
        for(int j=0;j<numcols && full ;++j)
            full = full & (board[i*numcols+j][1]==EMPTY)?false:true;
    return full;
}
/* identify the blitz positions ,basically those positions where you have atleast one self neighbor, otherwise
it defaults to a commando para dropn*/
void GameBoard::identify_blitz_pos(coord& movepos,int curr_agent,std::vector<coord>& blitz_pos){

    int dirs[] = {-1,0,1,0};
    int num_same=0;
    for(int i=0;i<4;++i){
        int iidx = movepos.first+dirs[i];
        int jidx = movepos.second+dirs[(i+1)%4];
        if((iidx<numrows && iidx>=0) && (jidx<numcols && jidx>=0)){
            if(board[iidx*numcols+jidx][1]==curr_agent) 
                num_same++;
            else if(board[iidx*numcols+jidx][1]!=EMPTY)
                blitz_pos.push_back(coord(iidx,jidx));
        }
    }
    if(num_same==0)
        blitz_pos.clear();
}
/* Identify the sabotage positions - basically all the positions where you have atleast one opposite neighbor*/
void GameBoard::identify_sabotage_pos(coord& movepos,int curr_agent,std::vector<coord>& sabotage_pos){
    
    int dirs[] = {-1,0,1,0};
    for(int i=0;i<4;++i){
        int iidx = movepos.first+dirs[i];
        int jidx = movepos.second+dirs[(i+1)%4];
        if((iidx<numrows && iidx>=0) && (jidx<numcols && jidx>=0)){
            if(board[iidx*numcols+jidx][1]!=curr_agent && board[iidx*numcols+jidx][1]!=EMPTY) 
                sabotage_pos.push_back(coord(iidx,jidx));
        }
    }
}
/* function to change the Board state for the given player and using the given next move position*/
int GameBoard::move_normal(coord& movepos,int curr_agent,std::vector<coord>& blitz_pos){
    //check if you can blitz, if so , do it
    identify_blitz_pos(movepos,curr_agent,blitz_pos);
    //should be adjacent to alteast one node of same color
    int other_agent = EMPTY;
    for(int i=0;i<blitz_pos.size();++i){
        other_agent = board[blitz_pos[i].first*numcols+blitz_pos[i].second][1];
        board[blitz_pos[i].first*numcols+blitz_pos[i].second][1]  = curr_agent;
        total_util[curr_agent] += board[blitz_pos[i].first*numcols+blitz_pos[i].second][0];
        total_util[other_agent] -= board[blitz_pos[i].first*numcols+blitz_pos[i].second][0];
    } 
    //now make the move on the requested space
    board[movepos.first*numcols+movepos.second][1]  = curr_agent;
    total_util[curr_agent] += board[movepos.first*numcols+movepos.second][0];
    return other_agent;
}

int GameBoard::move_sabotage(coord& movepos,int curr_agent,std::vector<coord>& sabotage_pos,bool sabotage){
    //check if you can sabotage. logic similar to blitz
    int other_agent = EMPTY;
    if(sabotage) {
    //should be adjacent to alteast one node of same color
        for(int i=0;i<sabotage_pos.size();++i){
            other_agent = board[sabotage_pos[i].first*numcols+sabotage_pos[i].second][1];
            board[sabotage_pos[i].first*numcols+sabotage_pos[i].second][1]  = curr_agent;
            total_util[curr_agent] += board[sabotage_pos[i].first*numcols+sabotage_pos[i].second][0];
            total_util[other_agent] -= board[sabotage_pos[i].first*numcols+sabotage_pos[i].second][0];
        } 
        //now make the move on the requested space
        board[movepos.first*numcols+movepos.second][1]  = curr_agent;
        total_util[curr_agent] += board[movepos.first*numcols+movepos.second][0];
        return other_agent;
    }else{
        //sabotage failed, your move position is converted to enemy's
        other_agent = (curr_agent+1)%2;
        board[movepos.first*numcols+movepos.second][1]  = other_agent;
        total_util[other_agent] += board[movepos.first*numcols+movepos.second][0];
        return other_agent;
    }
}

/* undo the change to the board state */
void GameBoard::undo_move_normal(coord& movepos,int curr_agent,int other_agent,std::vector<coord>& blitz_pos){
    //check if blitz was done
    for(int i=0;i<blitz_pos.size();++i){
        board[blitz_pos[i].first*numcols+blitz_pos[i].second][1]  = other_agent;
        total_util[curr_agent] -= board[blitz_pos[i].first*numcols+blitz_pos[i].second][0];
        total_util[other_agent] += board[blitz_pos[i].first*numcols+blitz_pos[i].second][0];
    } 
    board[movepos.first*numcols+movepos.second][1]  = EMPTY;
    total_util[curr_agent] -= board[movepos.first*numcols+movepos.second][0];
}

/* undo the change to the board state */
void GameBoard::undo_move_sabotage(coord& movepos,int curr_agent,int other_agent,std::vector<coord>& sabotage_pos,bool sabotage){
    //check if sabotage was done
    if(sabotage){
        //restore the enemy positions
        for(int i=0;i<sabotage_pos.size();++i){
            board[sabotage_pos[i].first*numcols+sabotage_pos[i].second][1]  = other_agent;
            total_util[curr_agent] -= board[sabotage_pos[i].first*numcols+sabotage_pos[i].second][0];
            total_util[other_agent] += board[sabotage_pos[i].first*numcols+sabotage_pos[i].second][0];
        } 
        board[movepos.first*numcols+movepos.second][1]  = EMPTY;
        total_util[curr_agent] -= board[movepos.first*numcols+movepos.second][0];
    }else{
        //sabotage had failed
        board[movepos.first*numcols+movepos.second][1]  = EMPTY;
        total_util[other_agent] -= board[movepos.first*numcols+movepos.second][0];
    }
}

/* print the current state of the board for debug */
void GameBoard::print_board(int indent){
    if(indent<0)indent = 0;
    else if(!DEBUG) return;
    for(int i=0;i<numrows;++i){
        cout<<std::string(indent,'\t');
        for(int j=0;j<numcols;++j){
            cout<<board[i*numcols+j][0]<<"|";
            switch(board[i*numcols+j][1]){
                case BLUE: cout<<"B";break;
                case GREEN: cout<<"G";break;
                case EMPTY: cout<<"E";break;
            }
            cout<<" ";
        }
        cout<<"\n";
    }
}
void GameBoard::reset(){
    for(int i=0;i<numrows;++i){
        for (int j=0;j<numcols;++j){
            board[i*numcols+j][1] = EMPTY;
        }
    }
    for (int i=0;i<numplayers;++i)
        total_util[i]=0;
}
/* For normal output printing */
void GameBoard::print_board(){
    print_board(-1);
}

/*===============================================================================*/
/* class that returns the utility at a given state(depth terminal) of the game for the max player*/
class DefaultUtility{
    GameBoard* board;
    public:
    DefaultUtility(GameBoard* b):
    board(b){}
    /* simply returns the difference between total utilities of the max and min agent */
    int get_utility(coord& movepos){
        return board->total_util[board->maxagent]-board->total_util[board->minagent];
    }
};
/*==================================================================================*/

class DefaultMoveOrder{
    
    typedef int (*board_type)[2];
    GameBoard * board;      //the sate of the board for which the move order is being evaluated
    int curr_agent;         //agent for which the move order is being evaluated
    std::vector<board_type> vec;
    std::vector<board_type>::iterator it;
    public:
    DefaultMoveOrder(GameBoard* b, int cagent):board(b),curr_agent(cagent){
    /*store the pointers to each position in the board in a vector and then do as you please(sort, pq etc)*/
        for(int i=0;i<board->numrows;++i)
            for(int j=0;j<board->numcols;++j)
                vec.push_back(&board->board[i*(board->numcols)+j]);
        it = vec.begin();
    }
    /* returns the offset from the pointer*/
    coord calculate_offsets(board_type pos){
        int rowpos = (pos-board->board)/(board->numcols);
        int colpos = (pos-board->board)%(board->numcols);
        return coord(rowpos,colpos);
    } 
    /* right now, just returns the next avaialable pos*/
    coord next_pos(coord& curpos,int agent){
        /* find the next empty position */
        while(it!=vec.end() && (*(*it))[1]!=EMPTY) ++it; 
        if(it!=vec.end()) {
            coord retcord = calculate_offsets(*it);
            ++it;
            return retcord;
        }
        else return end();
    }

    coord end(){
        return coord(-1,-1);
    }
};

/* class for implementing the move ordering,right now implements greedy move ordering*/
class PriorityMoveOrder{
    typedef int (*board_type)[2];
    struct priority_comp{
        public:
        //prioirty order for returning the positions in decreasing value
        bool operator()(const board_type  a, const board_type b) const{
            return (*a)[0]<(*b)[0];
        }
    };

    GameBoard * board;      //the sate of the board for which the move order is being evaluated
    int curr_agent;         //agent for which the move order is being evaluated
    std::priority_queue<board_type,std::vector<board_type>,priority_comp> pq;   //priority queue for ordering moves
    public:
    PriorityMoveOrder(GameBoard* b,int cagent):board(b),curr_agent(cagent){
    /*store the pointers to each position in the board in a vector and then do as you please(sort, pq etc)*/
        for(int i=0;i<board->numrows;++i)
            for(int j=0;j<board->numcols;++j)
                pq.push(&board->board[i*(board->numcols)+j]);
    }
    /* returns the offset from the pointer*/
    coord calculate_offsets(board_type pos){
        int rowpos = (pos-board->board)/(board->numcols);
        int colpos = (pos-board->board)%(board->numcols);
        return coord(rowpos,colpos);
    } 
    /* right now, just returns the next avaialable pos in prority order*/
    coord next_pos(coord& curpos,int agent){
        while(!pq.empty() && (*pq.top())[1]!=EMPTY)
            pq.pop();
        if(!pq.empty()){
            coord retcord = calculate_offsets(pq.top());
            pq.pop();
            return retcord;
        }else return end();
        /*
        while(it!=board_pos.end() && **it!=EMPTY) ++it;
        if(it!=board_pos.end()) return calculate_offsets(*it); else return coord(-1,-1);
        */
    }
    /* singals end of iterator*/
    coord end(){
        return coord(-1,-1);
    }
};


template <class Order,class Util>
class MiniMax{
    protected:
    GameBoard* board;   //reference to the gameboard state
    Util util;          //the utility function

    public:
    MiniMax(GameBoard* b);
    int minimax(coord& curpos, std::vector<coord>& path_list, int depth, int curr_agent,int& numnodes);
    int  play(coord& next_move,int move_color,int& numnodes);
};

/* constructor */
template <class Order,class Util>
MiniMax<Order,Util>::MiniMax(GameBoard* b):
board(b),util(b){
}
/* return the next move based on minimax stratgy for the game board */
template<class Order,class Util>
int MiniMax<Order,Util>::play(coord& next_move,int move_color,int& numnodes){
    coord startpos(0,0);
    std::vector<coord> next_moves;
    int util = minimax(startpos,next_moves,0,move_color,numnodes);
    next_move =  *next_moves.rbegin();
    return util;
}

/* calculates the minimax value of each state and builds the optimal game path */
template <class Order,class Util>
int  MiniMax<Order,Util>::minimax(coord& curpos, std::vector<coord>& path_list,int depth,int curr_agent,int& numnodes){
    //max depth hit
    if(depth==MAX_DEPTH){
        return util.get_utility(curpos);
    }
    //terminal state reached
    if(board->terminal_pos()){
        return util.get_utility(curpos);;
    }else if(curr_agent==board->maxagent){ 
        //every time the function is called a node is expanded
        numnodes++;
        coord nextpos;
        int max_util=INT_MIN;
        coord max_pos=curpos;
        std::vector<coord> best_path_list;
        Order iterator(board,curr_agent);
        //evaluate all possible successor states, without pruning 
        while((nextpos=iterator.next_pos(curpos,board->minagent))!=iterator.end()){
            //change the board state(including blitzing) 
            std::vector<coord> blitz_pos;
            int other_agent = board->move_normal(nextpos,curr_agent,blitz_pos);
            if(blitz_pos.size()>0) myprintf("\n%sBlitzing!",std::string(depth,'\t').c_str());
            //try out the move
            std::vector<coord> path_list_child;
            myprintf("\n%sThe board position tried for player max at depth %d is %d,%d\n",std::string(depth,'\t').c_str(),depth,nextpos.first,nextpos.second);
            board->print_board(depth);
            int util = minimax(nextpos,path_list_child,depth+1,board->minagent,numnodes);
            myprintf("\n%sreceived utility %d\n",std::string(depth,'\t').c_str(),util);
            if (util>max_util){
                max_util = util;
                max_pos = nextpos;
                best_path_list = path_list_child;
            }
            //restore the board state
            board->undo_move_normal(nextpos,curr_agent,other_agent,blitz_pos);
        }
        best_path_list.push_back(max_pos);
        path_list = best_path_list;
        return max_util;
    }else if(curr_agent==board->minagent){
        //evaluate all possible successor states, without pruning
        numnodes++;
        coord nextpos;
        coord min_pos = curpos;
        int min_util=INT_MAX;
        std::vector<coord> best_path_list;
        Order iterator(board,curr_agent);
        while((nextpos=iterator.next_pos(curpos,board->maxagent))!=iterator.end()){
            //change the board state(including blitzing) 
            std::vector<coord> blitz_pos;
            int other_agent = board->move_normal(nextpos,curr_agent,blitz_pos);
            if(blitz_pos.size()>0) myprintf("\n%sBlitzing!",std::string(depth,'\t').c_str());
            //try out the move
            std::vector<coord> path_list_child;
            myprintf("\n%sThe board position tried for player min at depth %d is %d,%d\n",std::string(depth,'\t').c_str(),depth,nextpos.first,nextpos.second);
            board->print_board(depth);
            int util = minimax(nextpos,path_list_child,depth+1,board->maxagent,numnodes);
            myprintf("\n%sreceived utility %d\n",std::string(depth,'\t').c_str(),util);
            if (util<min_util){
                min_util = util;
                min_pos = nextpos;
                best_path_list = path_list_child;
            }
            //restore the board state
            board->undo_move_normal(nextpos,curr_agent,other_agent,blitz_pos);
        }
        best_path_list.push_back(min_pos);
        path_list = best_path_list;
        return min_util;
    }else {
        myprintf("Error state of game");
        exit(1);
    }
}
/* =====================================================================*/
template<class Order, class Util>
class AlphaBeta{
    protected:
    GameBoard* board;   //reference to gameboard state
    Util util;       //the utility function
    public:

    AlphaBeta(GameBoard* b);
    int alphabeta(coord& curpos, std::vector<coord>& path_list,int depth, int curr_agent,int alpha, int beta,int& numnodes);
    int play(coord& next_move,int move_color,int& numnodes);

};
template<class Order, class Util>
AlphaBeta<Order, Util>::AlphaBeta(GameBoard* b):board(b),util(b){
}
/* return the next move based on minimax stratgy for the game board */
template<class Order,class Util>
int AlphaBeta<Order,Util>::play(coord& next_move,int move_color,int& numnodes){
    coord startpos(0,0);
    std::vector<coord> next_moves;
    int util = alphabeta(startpos,next_moves,0,move_color,INT_MIN,INT_MAX,numnodes);
    next_move =  *next_moves.rbegin();
    return util;
}

/* calculates the minimax value of each state and builds the optimal game path */
/* function to calculate minimax value using alpha beta */
template <class Order,class Util>
int  AlphaBeta<Order,Util>::alphabeta(coord& curpos, std::vector<coord>& path_list,int depth,int curr_agent,int alpha, int beta, int& numnodes){
    //max depth hit, should I use a different utility function ?
    if(depth==MAX_DEPTH_AB){
        return util.get_utility(curpos);
    }
    //terminal state reached
    if(board->terminal_pos()){
        return util.get_utility(curpos);;
    }else if(curr_agent==board->maxagent){ 
        numnodes++;
        coord nextpos;
        int max_util=alpha;
        coord max_pos=curpos;
        std::vector<coord> best_path_list;
        Order iterator(board,curr_agent);
        //evaluate all possible successor states, without pruning 
        while((nextpos=iterator.next_pos(curpos,board->minagent))!=iterator.end()){
            //change the board state(including blitzing) 
            std::vector<coord> blitz_pos;
            int other_agent = board->move_normal(nextpos,curr_agent,blitz_pos);
            if(blitz_pos.size()>0) myprintf("\n%sBlitzing!",std::string(depth,'\t').c_str());
            //try out the move
            std::vector<coord> path_list_child;
            myprintf("\n%sThe board position tried for player max at depth %d is %d,%d\n",std::string(depth,'\t').c_str(),depth,nextpos.first,nextpos.second);
            board->print_board(depth);
            int util = alphabeta(nextpos,path_list_child,depth+1,board->minagent,alpha,beta,numnodes);
            myprintf("\n%sreceived utility %d\n",std::string(depth,'\t').c_str(),util);
            if (util>max_util){
                max_util = util;
                max_pos = nextpos;
                best_path_list = path_list_child;
            }
            //restore the board state
            board->undo_move_normal(nextpos,curr_agent,other_agent,blitz_pos);
            //prune against beta
            if(max_util >= beta) {
                //just return after setting the appropriate variables
                myprintf("\n%sPruning against beta on util %d and {%d,%d}\n",std::string(depth,'\t').c_str(),max_util,alpha,beta);
                best_path_list.push_back(max_pos);
                path_list = best_path_list;
                return max_util;         
            }
            alpha = max(alpha,max_util);
        }
        best_path_list.push_back(max_pos);
        path_list = best_path_list;
        return max_util;
    }else if(curr_agent==board->minagent){
        //evaluate all possible successor states, without pruning
        numnodes++;
        coord nextpos;
        coord min_pos = curpos;
        int min_util=beta;
        std::vector<coord> best_path_list;
        Order iterator(board,curr_agent);
        while((nextpos=iterator.next_pos(curpos,board->maxagent))!=iterator.end()){
            //change the board state(including blitzing) 
            std::vector<coord> blitz_pos;
            int other_agent = board->move_normal(nextpos,curr_agent,blitz_pos);
            if(blitz_pos.size()>0) myprintf("\n%sBlitzing!",std::string(depth,'\t').c_str());
            //try out the move
            std::vector<coord> path_list_child;
            myprintf("\n%sThe board position tried for player min at depth %d is %d,%d\n",std::string(depth,'\t').c_str(),depth,nextpos.first,nextpos.second);
            board->print_board(depth);
            int util = alphabeta(nextpos,path_list_child,depth+1,board->maxagent,alpha,beta,numnodes);
            myprintf("\n%sreceived utility %d\n",std::string(depth,'\t').c_str(),util);
            if (util<min_util){
                min_util = util;
                min_pos = nextpos;
                best_path_list = path_list_child;
            }
            //restore the board state
            board->undo_move_normal(nextpos,curr_agent,other_agent,blitz_pos);
            //prune against alpha
            if(min_util <= alpha) {
                //just return after setting the appropriate variables
                myprintf("\n%sPruning against alpha on util %d and {%d,%d}\n",std::string(depth,'\t').c_str(),min_util,alpha,beta);
                best_path_list.push_back(min_pos);
                path_list = best_path_list;
                return min_util;         
            }
            beta = min(beta,min_util);
        }
        best_path_list.push_back(min_pos);
        path_list = best_path_list;
        return min_util;
    }else {
        myprintf("Error state of game");
        exit(1);
    }
}

/*================================================*/
template <class Order,class Util>
class ExpectiMiniMax{
    protected:
    GameBoard* board;   //reference to the gameboard state
    Util util;          //the utility function
    double gamma;

    public:
    ExpectiMiniMax(GameBoard* b,double gamma);
    double expectiminimax(coord& curpos, coord& best_move, int depth, int curr_agent,int &numnodes,bool& sabotage_move);
    double  play(coord& next_move,int move_color,int& numnodes,bool& sabotage_move);
};

/* constructor */
template <class Order,class Util>
ExpectiMiniMax<Order,Util>::ExpectiMiniMax(GameBoard* b,double g=0.2):
board(b),util(b),gamma(g){
}
/* return the next move based on minimax stratgy for the game board */
template<class Order,class Util>
double ExpectiMiniMax<Order,Util>::play(coord& next_move,int move_color,int& numnodes,bool& sabotage_move){
    coord startpos(0,0);
    double util = expectiminimax(startpos,next_move,0,move_color,numnodes,sabotage_move);
    return util;
}

/* calculates the minimax value of each state and builds the optimal game path */
template <class Order,class Util>
double  ExpectiMiniMax<Order,Util>::expectiminimax(coord& curpos, coord& best_move,int depth,int curr_agent,int& numnodes,bool& sabotage_move){
    //max depth hit (double the max depth here because of the chance nodes */
    if(depth==MAX_DEPTH_EM){
        return util.get_utility(curpos);
    }
    //terminal state reached
    if(board->terminal_pos()){
        return util.get_utility(curpos);;
    }else if(curr_agent==board->maxagent){ 
        numnodes++;
        coord nextpos;
        double max_util=INT_MIN;
        coord max_pos=curpos;
        bool max_util_sabotage =false;

        Order iterator(board,curr_agent);
        while((nextpos=iterator.next_pos(curpos,board->minagent))!=iterator.end()){
            //change the board state(including blitzing) 
            myprintf("\n%sThe board position tried for player max at depth %d is %d,%d\n",std::string(depth,'\t').c_str(),depth,nextpos.first,nextpos.second);
            int other_agent;
            coord returnpos; //duumy for the return
            bool sabotage_done = false;
            bool return_sabotage ;//dummy

            //first evaluate all regular moves (i.e. para drop and blitz(you will always be able to do one of the twoi) */)
            std::vector<coord> modified_pos;
            other_agent = board->move_normal(nextpos,curr_agent,modified_pos);
            myprintf("\n%sThe board after default move:\n",std::string(depth,'\t').c_str());
            board->print_board(depth);
            if(modified_pos.size()>0) myprintf("\n%sBlitzing!\n",std::string(depth,'\t').c_str());
            double util_regular = expectiminimax(nextpos,returnpos,depth+1,board->minagent,numnodes,return_sabotage);
            board->undo_move_normal(nextpos,curr_agent,other_agent,modified_pos);
            myprintf("\n%sreceived utility with default %f\n",std::string(depth,'\t').c_str(),util_regular);

            //next try out the sabotage move
            //now try out the positive sabotage move 
            double util_sabotage = INT_MIN;
            modified_pos.clear();
            board->identify_sabotage_pos(nextpos,curr_agent,modified_pos);
            if(modified_pos.size()>0){
                myprintf("\n%sSabotaging!\n",std::string(depth,'\t').c_str());
                // a sabotage move is possible
                other_agent = board->move_sabotage(nextpos,curr_agent,modified_pos,true);
                myprintf("\n%sThe board after sabotage move(+):\n",std::string(depth,'\t').c_str());
                board->print_board(depth);
                double util_sabotage_pos = expectiminimax(nextpos,returnpos,depth+1,board->minagent,numnodes,return_sabotage);
                board->undo_move_sabotage(nextpos,curr_agent,other_agent,modified_pos,true);
                //now try out the negative sabotage move            
                modified_pos.clear();
                other_agent = board->move_sabotage(nextpos,curr_agent,modified_pos,false);
                myprintf("\n%sThe board after sabotage move(-):\n",std::string(depth,'\t').c_str());
                board->print_board(depth);
                double util_sabotage_neg = expectiminimax(nextpos,returnpos,depth+1,board->minagent,numnodes,return_sabotage);
                board->undo_move_sabotage(nextpos,curr_agent,other_agent,modified_pos,false);

                util_sabotage = (1-gamma)*util_sabotage_neg+gamma*util_sabotage_pos;
                myprintf("\n%sreceived utility with sabotage %f\n",std::string(depth,'\t').c_str(),util_sabotage);
            }else myprintf("\n%sNo Sabotage!",std::string(depth,'\t').c_str());
            /* take the max of the two */
            double util = 0;
            if(util_sabotage > util_regular){
                sabotage_done = true;
                util = util_sabotage;
            }else {
                sabotage_done =  false;
                util = util_regular;
            }
            //cacluate total util 
            //myprintf("\n%sTotal utility for max %f\n",std::string(depth,'\t').c_str(),util); 
            if (util>max_util){
                max_util = util;
                max_util_sabotage = sabotage_done;
                max_pos = nextpos;
            }
        }
        best_move = max_pos;
        sabotage_move = max_util_sabotage;
        myprintf("\n%sBest move for max at depth %d: %d,%d with util %f",std::string(depth,'\t').c_str(),depth,best_move.first,best_move.second,max_util);
        return max_util;
    }else if(curr_agent==board->minagent){

        numnodes++;
        //evaluate all possible successor states, without pruning
        coord nextpos;
        coord min_pos = curpos;
        double min_util=INT_MAX;
        double min_util_sabotage = false;

        Order iterator(board,curr_agent);
        while((nextpos=iterator.next_pos(curpos,board->maxagent))!=iterator.end()){

            myprintf("\n%sThe board position tried for player min at depth %d is %d,%d\n",std::string(depth,'\t').c_str(),depth,nextpos.first,nextpos.second);
            int other_agent;
            coord returnpos; //duumy for the return
            bool sabotage_done = false;
            bool return_sabotage; //dummy

            //first evaluate all regular moves (i.e. para drop and blitz)
            std::vector<coord> modified_pos;
            other_agent = board->move_normal(nextpos,curr_agent,modified_pos);
            myprintf("\n%sThe board after default move:\n",std::string(depth,'\t').c_str());
            board->print_board(depth);
            if(modified_pos.size()>0) myprintf("\n%sBlitzing!\n",std::string(depth,'\t').c_str());
            double util_regular = expectiminimax(nextpos,returnpos,depth+1,board->maxagent,numnodes,return_sabotage);
            board->undo_move_normal(nextpos,curr_agent,other_agent,modified_pos);
            myprintf("\n%sreceived utility with default %f\n",std::string(depth,'\t').c_str(),util_regular);
            
            double util_sabotage = INT_MAX;
            modified_pos.clear();
            board->identify_sabotage_pos(nextpos,curr_agent,modified_pos);
            if(modified_pos.size()>0){
                myprintf("\n%sSabotaging!\n",std::string(depth,'\t').c_str());
                //now try out the sabotage move
                //now try out the positive sabotage move 
                other_agent = board->move_sabotage(nextpos,curr_agent,modified_pos,true);
                myprintf("\n%sThe board after sabotage move(+):\n",std::string(depth,'\t').c_str());
                board->print_board(depth);
                double util_sabotage_pos = expectiminimax(nextpos,returnpos,depth+1,board->maxagent,numnodes,return_sabotage);
                board->undo_move_sabotage(nextpos,curr_agent,other_agent,modified_pos,true);
                //now try out the negative sabotage move            
                modified_pos.clear();
                other_agent = board->move_sabotage(nextpos,curr_agent,modified_pos,false);
                myprintf("\n%sThe board after sabotage move(-):\n",std::string(depth,'\t').c_str());
                board->print_board(depth);
                double util_sabotage_neg = expectiminimax(nextpos,returnpos,depth+1,board->maxagent,numnodes,return_sabotage);
                board->undo_move_sabotage(nextpos,curr_agent,other_agent,modified_pos,false);

                util_sabotage = (1-gamma)*util_sabotage_neg+gamma*util_sabotage_pos;
                myprintf("\n%sreceived utility with sabotage %f\n",std::string(depth,'\t').c_str(),util_sabotage);
            }else myprintf("\n%sNo Sabotaging!\n",std::string(depth,'\t').c_str());
            /* take the min of the two */
            double util = 0;
            if(util_sabotage < util_regular){
                sabotage_done = true;
                util = util_sabotage;
            }else {
                sabotage_done = false;
                util = util_regular;
            }
            if (util<min_util){
                min_util = util;
                min_util_sabotage = sabotage_done;
                min_pos = nextpos;
            }
        }
        best_move = min_pos;
        sabotage_move = min_util_sabotage;
        myprintf("\n%sBest move for min at depth %d: %d,%d with util %f",std::string(depth,'\t').c_str(),depth,best_move.first,best_move.second,min_util);
        return min_util;
    }else{
        myprintf("Error state of game");
        exit(1);
    }
}
/* =====================================================================*/
/* function for time calculatio from : http://stackoverflow.com/questions/1861294/how-to-calculate-execution-time-of-a-code-snippet-in-c */
typedef unsigned long long timestamp_t;
/* returns timestamp in microseconds */
timestamp_t get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}
typedef MiniMax<DefaultMoveOrder,DefaultUtility> minimaxagent;
typedef ExpectiMiniMax<DefaultMoveOrder,DefaultUtility> expectiagent;
typedef AlphaBeta<PriorityMoveOrder,DefaultUtility> abagent;
/* function to simulate gameplay , with Agent1 taking blue and going first*/
template<class Agent1, class Agent2>
void play_game(GameBoard& board){
    /* let them both look at the board */
    Agent1 a1(&board);
    Agent2 a2(&board);
    //curr_agent  = BLUE;
    std::vector<coord> move_arr1,move_arr2;
    /* till the game finishes*/
    int total_1=0,total_2=0;
    timestamp_t time1=0, time2=0;
    int nrounds = 0;
    timestamp_t t0,t1;
    while(!board.terminal_pos()){
        nrounds++;
        coord a1_move,a2_move;
        int numnodes1=0,numnodes2=0;

        t0 = get_timestamp();
        double util1 = a1.play(a1_move,BLUE,numnodes1);
        t1 = get_timestamp();
        time1 += (t1-t0)/1000.0L; 
        total_1+=numnodes1;
        printf("\nPlayer BLUE moved to position %d,%d with util: %f and number of nodes expanded: %d\n",a1_move.first,a1_move.second,util1,numnodes1);
        board.move_normal(a1_move,BLUE,move_arr1); 
        board.print_board();

        t0 = get_timestamp();
        double util2 = a2.play(a2_move,GREEN,numnodes2);
        t1 = get_timestamp();
        time2 += (t1-t0)/1000.0L; 
        total_2+=numnodes2;
        printf("\nPlayer GREEN moved to position %d,%d with util: %f and number of nodes expanded :%d\n",a2_move.first,a2_move.second,util2,numnodes2);
        board.move_normal(a2_move,GREEN,move_arr2);
        board.print_board();
    }
    cout<<"\nThe final state of the board is \n";
    board.print_board();
    cout<<"\nThe final scores of the players are \n";
    cout<<"\nBLUE:"<<board.total_util[BLUE]<<" "<<"GREEN:"<<board.total_util[GREEN];
    cout<<"\nThe total number of nodes expanded per player are \n";
    cout<<"\nBLUE:"<<total_1<<" "<<"GREEN:"<<total_2;
    cout<<"\nThe average number of nodes expanded per move are \n";
    cout<<"\nBLUE:"<<total_1/nrounds<<" "<<"GREEN:"<<total_2/nrounds;
    cout<<"\nThe average time taken  in ms per move is \n";
    cout<<"\nBLUE:"<<(double)time1/nrounds<<" "<<"GREEN:"<<(double)time2/nrounds;
     
}
void play_expectiminimax(GameBoard& board){
    double gamma_vals[]  = {0.2,0.4,0.6,0.8,1.0};
    for(int i=2;i<3;++i){
        //running average of total scores after each game        
        int total_1=0,total_2=0;
        int total_sabotage=0;

        for(int j=0;j<1;++j){
            double gamma = gamma_vals[i];
            cout<<"\n\tPlaying iteration "<<j<<"\n";
            timestamp_t time1=0, time2=0;
            int nrounds = 0;
            int numsabotage=0;
            timestamp_t t0,t1;
            board.reset();

            //intialize the agents and the board
            expectiagent a1(&board,gamma),a2(&board,gamma);

            while(!board.terminal_pos()){
                nrounds++;
                coord a1_move,a2_move;
                vector<coord> move_arr1,move_arr2; //dummy
                int numnodes1=0,numnodes2=0;
                bool sabotage_move=false;

                //player 1
                t0 = get_timestamp();
                double util1 = a1.play(a1_move,BLUE,numnodes1,sabotage_move);
                t1 = get_timestamp();
                time1 += (t1-t0)/1000.0L; 
                printf("\n\t\tPlayer BLUE moved to position %d,%d (%d) with util: %f and number of nodes expanded: %d\n",a1_move.first,a1_move.second,sabotage_move,util1,numnodes1);
                if(sabotage_move){
                    //see if sabotage move is successfule
                    bool success = ( (rand()%10) < (int)(10*gamma) )?true:false;
                    printf("\n\t\tSabotage status:%d\n",success);
                    move_arr1.clear();
                    if(success)board.identify_sabotage_pos(a1_move,BLUE,move_arr1);
                    board.move_sabotage(a1_move,BLUE,move_arr1,success); 
                    numsabotage++;
                }else board.move_normal(a1_move,BLUE,move_arr1);
                board.print_board();
                //cout<<"\n\t\tBLUE:"<<board.total_util[BLUE]<<" "<<"GREEN:"<<board.total_util[GREEN];

                //player 2
                t0 = get_timestamp();
                double util2 = a2.play(a2_move,GREEN,numnodes2,sabotage_move);
                t1 = get_timestamp();
                time2 += (t1-t0)/1000.0L; 
                printf("\n\t\tPlayer GREEN moved to position %d,%d (%d) with util: %f and number of nodes expanded :%d\n",a2_move.first,a2_move.second,sabotage_move,util2,numnodes2);
                if(sabotage_move){
                    //see if sabotage move is successful 
                    bool success = ( (rand()%10) < (int)(10*gamma) )?true:false;
                    printf("\n\t\tSabotage status:%d\n",success);
                    move_arr2.clear();
                    if(success)board.identify_sabotage_pos(a2_move,GREEN,move_arr2);
                    board.move_sabotage(a2_move,GREEN,move_arr2,success); 
                    numsabotage++;
                }else board.move_normal(a2_move,GREEN,move_arr2);
                board.print_board();
                //cout<<"\n\t\tBLUE:"<<board.total_util[BLUE]<<" "<<"GREEN:"<<board.total_util[GREEN];
            }

            //per game stats
            cout<<"\n\tThe average time taken  in ms per move is \n";
            cout<<"\n\tBLUE:"<<(double)time1/nrounds<<" "<<"GREEN:"<<(double)time2/nrounds;
            cout<<"\n\tThe final state of the board after iteration"<<j<<" is \n";
            board.print_board();
            cout<<"\n\tThe final scores of the players are \n";
            cout<<"\n\tBLUE:"<<board.total_util[BLUE]<<" "<<"GREEN:"<<board.total_util[GREEN];
            
            //running averages
            total_1 += board.total_util[BLUE];
            total_2 += board.total_util[GREEN];
            total_sabotage += numsabotage;
        }

        cout<<"\nThe average score of the players are \n";
        cout<<"\nBLUE:"<<total_1/10<<" "<<"GREEN:"<<(double)total_2/10;
        cout<<"\nThe average number if sabotages is "<<(double)total_sabotage/10<<"\n";
    }
}
void test_minimax(GameBoard &b){
    minimaxagent algo(&b) ;
    std::vector<coord> move_list;
    coord startpos(0,0);
    int numnodes;
    int root_util = algo.minimax(startpos,move_list,0,b.maxagent,numnodes);
    cout<<"The order of moves is\n";
    for(int i=0;i<move_list.size();++i)
        myprintf("{%d,%d}\n",move_list[i].first,move_list[i].second);
    cout<<"Total utility "<<root_util;
}
void test_alphabeta(GameBoard &b){
    abagent algo(&b) ;
    std::vector<coord> move_list;
    coord startpos(0,0);
    int numnodes;
    int root_util = algo.alphabeta(startpos,move_list,0,b.maxagent,INT_MIN,INT_MAX,numnodes);
    cout<<"The order of moves is\n";
    for(int i=0;i<move_list.size();++i)
        myprintf("{%d,%d}\n",move_list[i].first,move_list[i].second);
    cout<<"Total utility "<<root_util;
}
void test_exxpectiminimax(GameBoard &b){
    expectiagent algo(&b,0.8) ;
    coord startpos(0,0);
    coord best_move;
    int numnodes;
    bool return_sabotage;
    int root_util = algo.expectiminimax(startpos,best_move,0,b.maxagent,numnodes,return_sabotage);
    cout<<"\nThe best move for max is "<<best_move.first<<","<<best_move.second;
    cout<<"\nTotal utility "<<root_util;
}
int main(){

    //n rows , m columns

    setvbuf(stdout, NULL, _IONBF, 0);

    int n,m; 
    n = MAX_Y;  
    m = MAX_X;
    GameBoard b(n,m,GREEN,BLUE);

    //read in the board from input
    for (int i=0;i<n;++i)
        for (int j=0;j<m;++j)
            cin>>b.board[i*m+j][0];
    cout<<"\nThe inital board is \n";
    b.print_board();
    cout<<"\nStarting the game\n";
    //test_minimax(b);
    //test_alphabeta(b);
    //test_exxpectiminimax(b);
    //play_game<minimaxagent,minimaxagent>(b);
    play_expectiminimax(b);
    return 0;
}
