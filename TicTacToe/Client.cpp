#include <iostream>
#include <vector>
#include <random>
using namespace std;


class Cell{
    private:
        char value;
    public:
        Cell(){
            value = '-';
        }
        char getValue(){
            return value;
        }
        void setValue(char val){
            value = val;
        }
    ~Cell(){};
};

class Board{
    private:
        int size;
        vector<vector<Cell>> layout;
    public:
        Board(int size){
            this->size = size;
            layout.resize(size, vector<Cell>(size));
        }
        void displayBoard(){
            for(int i=0; i<size; i++){
                for(int j=0; j<size; j++){
                    cout<<layout[i][j].getValue()<<" ";
                }
                cout<<endl;
            }
        }
        bool isBoardFull(){
            for(int i=0; i<size; i++){
                for(int j=0; j<size; j++){
                    if(layout[i][j].getValue() == '-'){
                        return false;
                    }
                }
            }
            return true;
        }
        bool setCell(int row,int col, char val){
            if(row>size||col>size||row<0||col<0){
                cout<<"Invalid Row and Column Selected\n"; 
                return false;
            }
            if(layout[row][col].getValue()=='-'){
                layout[row][col].setValue(val);
                return true;
            }
            if(layout[row][col].getValue()!='-'){
                cout<<"Invalid Row and Column Selected, Cell not empty\n"; 
                return false;
            }
            return true;
        }
        char getCellValue(int row, int col){
            return layout[row][col].getValue();
        }
        void setCellValue(int row, int col, char val){
            layout[row][col].setValue(val);
        }
        int getSize(){
            return this->size;
        }
    ~Board(){};
};

class Player{
    private :
    string name;
    char symbol;

    public :
    void setSymbol(char symbol){
        this->symbol=symbol;
    };
    char getSymbol(){
        return this->symbol;
    }
    string getName(){
        return this->name;
    }
    Player(string name,char symbol){
        this->name=name;
        this->symbol= symbol;
    }
    virtual pair<int,int> playMove(int boardSize)=0;
    ~Player(){};
};

class AiPlayer: public Player{
    public :
    AiPlayer(string name, char symbol):Player(name,symbol){
        cout<<"AI Player created\n";
    }
    protected:
    pair<int,int> playMove(int boardSize){
        static random_device rd;
        static mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, boardSize - 1);
        int row = dist(gen);
        int col = dist(gen);
        return {row, col};
    }
};
class HumanPlayer: public Player{
    public :

    HumanPlayer(string name, char symbol):Player(name,symbol){
        cout<<"Human Player created: "<<name<<"\n";
    }
    protected:

    pair<int,int> playMove(int boardSize){
        int row,col;
        cout<<"Enter Row\n";
        cin>>row;
        cout<<"Enter Col\n";
        cin>>col;
        return {row,col};
    }
};

class WinningStrategy {

    protected: 

    int length;
    WinningStrategy(int k){
        this->length=k;
    }
    public:

    virtual bool checkWinner(Board* b,Player* p,int row,int col) =0;
    ~WinningStrategy(){};
};

class RowColumnDiagonalWinningStrategy: public WinningStrategy{ 
    public:
    RowColumnDiagonalWinningStrategy(int length): WinningStrategy(length){
        cout<<"Row Column Winning strategy created\n";
    }
    bool checkWinner(Board* b,Player* p,int row,int col){
        int len= this->length;
        int boardSize= b->getSize();
        char symbol= p->getSymbol();

        // column check
        int rowCount=0;
        for(int i=row;i<boardSize;i++){
            char cellValue= b->getCellValue(i,col);
            if(cellValue==symbol)
                rowCount++;
            else
                break;
        }
        for(int i=row-1;i>=0;i--){
            char cellValue= b->getCellValue(i,col);
            if(cellValue==symbol)
                rowCount++;
            else
                break;
        }
        if(rowCount>=len){
            return true;
        }

        int colCount=0;
        //row check
        for(int i=col;i<boardSize;i++){
            char cellValue= b->getCellValue(row,i);
            if(cellValue==symbol)
                colCount++;
            else
                break;
        }
        for(int i=col-1;i>=0;i--){
            char cellValue= b->getCellValue(row,i);
            if(cellValue==symbol)
                colCount++;
            else
                break;
        }
        
        if(colCount>=len)
            return true;

        // diagonal (top-left to bottom-right)
        int diag1Count = 0;
        for(int i=0; row + i < boardSize && col + i < boardSize; ++i){
            if(b->getCellValue(row + i, col + i) == symbol) diag1Count++; else break;
        }
        for(int i=1; row - i >= 0 && col - i >= 0; ++i){
            if(b->getCellValue(row - i, col - i) == symbol) diag1Count++; else break;
        }
        if(diag1Count >= len) return true;

        // anti-diagonal (top-right to bottom-left)
        int diag2Count = 0;
        for(int i=0; row + i < boardSize && col - i >= 0; ++i){
            if(b->getCellValue(row + i, col - i) == symbol) diag2Count++; else break;
        }
        for(int i=1; row - i >= 0 && col + i < boardSize; ++i){
            if(b->getCellValue(row - i, col + i) == symbol) diag2Count++; else break;
        }
        if(diag2Count >= len) return true;

        return false;
        
    }

};

class PlayerFactory{
    public :
    Player* createPlayer(string type,string name,char symbol){
        if(type=="AI"){
            return new AiPlayer(name,symbol);
        }
        if(type=="Human"){
            return new HumanPlayer(name,symbol);
        }
        return new HumanPlayer(name,symbol);
    }
};

class Game{
    private: 
    Player* playerA;
    Player* playerB;
    int turn;

    Board* board;

    WinningStrategy* strategy;

    public :
    Game(Player* p1,Player* p2,Board* b,WinningStrategy* strategy){
        this->playerA=p1;
        this->playerB=p2;
        this->strategy=strategy;
        this->turn=0;
        this->board=b;
    }


    void StartGame(){
        while(!board->isBoardFull()){
            pair<int,int> move;
            board->displayBoard();
            if(turn%2==0){
                cout<<playerA->getName()<<"'s turn\n";
                move=playerA->playMove(board->getSize());
                bool isValid=board->setCell(move.first,move.second,playerA->getSymbol());
                while(!isValid){
                    move=playerA->playMove(board->getSize());
                    isValid=board->setCell(move.first,move.second,playerA->getSymbol());
                }

                bool isWinner=strategy->checkWinner(board,playerA,move.first,move.second);

                if(isWinner){
                    cout<<"Player A : "<<playerA->getName()<<" won the game\n";
                    board->displayBoard();
                    return;
                }
            }
            else{
                cout<<playerB->getName()<<"'s turn\n";


                move=playerB->playMove(board->getSize());
                bool isValid=board->setCell(move.first,move.second,playerB->getSymbol());
                while(!isValid){
                    move=playerB->playMove(board->getSize());
                    isValid=board->setCell(move.first,move.second,playerB->getSymbol());
                }

                bool isWinner=strategy->checkWinner(board,playerB,move.first,move.second);

                if(isWinner){
                    cout<<"Player b : "<<playerB->getName()<<" won the game\n";
                    board->displayBoard();
                    return;
                }
            }
            turn++;
        }
        cout<<"The Game is Draw\n";

    }
};

int main(){
    
    cout<<"Welcome to Tic Tac Toe Game\n";
    cout<<"select the board size\n";
    int boardSize;
    cin>>boardSize;

    Board* board= new Board(boardSize);

    string playerName;
    char playerSymbol;
    string playerType;
    PlayerFactory* myFactory= new PlayerFactory();

    cout<<"Enter the Type of player A\n";
    cin>>playerType;

    cout<<"Enter the name of player A\n";
    cin>>playerName;

    cout<<"Enter a symbol for player A\n";
    cin>>playerSymbol;

    Player* playerA= myFactory->createPlayer(playerType,playerName,playerSymbol);

    cout<<"Enter the Type of player B\n";
    cin>>playerType;

    cout<<"Enter the name of player B\n";
    cin>>playerName;

    cout<<"Enter a symbol for player B\n";
    cin>>playerSymbol;


    Player* playerB= myFactory->createPlayer(playerType,playerName,playerSymbol);

    int winningLength;
    cout<<"Enter the winning Length value\n";
    cin>>winningLength;

    WinningStrategy* currentWinningStrategy= new RowColumnDiagonalWinningStrategy(winningLength);

    //create game
    Game* game= new Game(playerA,playerB,board,currentWinningStrategy);

    cout<<"Game starting\n";

    game->StartGame();
    
    cout<<"Game Ended\n";

    return 0;


}