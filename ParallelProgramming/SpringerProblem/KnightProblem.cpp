#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <chrono>

// =====================================================================================
// http://peterloos.de/index.php/m-multithreading/m-multithreading-tpl/58-a-mt-knights-problem
// =====================================================================================

// TODO: Die Methoden sollte man alle klein schreiben !!!

// TODO: Das mit dem GameBoard und dem int sollte man templatizieren .....

// TODO: Die Lösungen als Event // mit Lambdas ausgeben ...

// typedefs

// TODO: size_t  um die Warning weg zu bekommen ...

class Coordinate;
using GameBoard = std::vector<std::vector<int>>;
using Solution = std::vector<Coordinate>;
using ListSolutions = std::list<Solution>;

// =====================================================================================

// TODO 1: Der Name ist blöd -- ChessField -- siehe auch Marc Gregoire

// TODO 2: m_row und m_col mit getter und setter schützen !!!

class Coordinate {

friend std::ostream& operator<< (std::ostream&, const Coordinate&); 

public:
    Coordinate();
    Coordinate(int, int);
    Coordinate(const Coordinate&) = default;

    // move semantics
    Coordinate(Coordinate&&) = default;
    Coordinate& operator= (Coordinate&&) = default;

public:
    // getter/setter
    inline int getRow() const { return m_row; }
    inline int getCol() const { return m_col; }

private:
    int m_row;
    int m_col;
};

Coordinate::Coordinate() : Coordinate(0, 0) {};
Coordinate::Coordinate(int row, int col) : m_row(row), m_col(col) {}

std::ostream& operator<< (std::ostream& os, const Coordinate& coordinate) {
    os << "(" << coordinate.m_row << "," << coordinate.m_col << ")";
    return os;
}

// =====================================================================================

// TODO 2: Den Solver als Schablone machen: int , short und long sind mögliche Kandidaten für die Suche ....

// TODO: Da werden viele Koordinaten by Value übergeben ... geht das auch per Referenz

class KnightProblemSolver {

public:
    KnightProblemSolver();
    KnightProblemSolver(int rows, int cols);

public:
    // getter/setter
    int getHeight() const { return m_height; }
    int getWidth() const { return m_width; }
    int setHeight(int rows) { m_height = rows; }
    int setWidth(int cols) { m_width = cols; }

    ListSolutions getSolutions();

    // public interface
    void findMovesSequential();

private:
    // internal helper methods
    void verifyCoordinate(size_t col, size_t row) const;
    int& at(size_t row, size_t col);
    const int& at(size_t row, size_t col) const;

    void findMovesSequential(const Coordinate& coord);
    void setKnightMoveAt(const Coordinate& coord);
    void unsetKnightMoveAt(const Coordinate& coord);
    bool inRange(int row, int col);
    bool canMoveTo(int row, int col);
    bool isSolution();
    std::vector<Coordinate> NextKnightMoves(const Coordinate& coord);

private:
    size_t         m_height;      // height of board
    size_t         m_width;       // width of board
    GameBoard      m_board;       // chess board
    ListSolutions  m_solutions;   // list of found solutions
    Solution       m_current;     // solution being in construction
    int            m_moveNumber;  // number of last knight's move
};

KnightProblemSolver::KnightProblemSolver() : KnightProblemSolver(8, 8) {}

KnightProblemSolver::KnightProblemSolver(int height, int width) {

    m_height = height;
    m_width = width;
    m_moveNumber = 0;
}

ListSolutions KnightProblemSolver::getSolutions() {
    // return a copy 
    return ListSolutions (m_solutions);
}

void KnightProblemSolver::verifyCoordinate(size_t col, size_t row) const {
    if (row >= m_height || col >= m_width) {
        throw std::range_error("");
    }
}

int& KnightProblemSolver::at(size_t row, size_t col) {
    verifyCoordinate(col, row);
    return m_board[col][row];
}

const int& KnightProblemSolver::at(size_t row, size_t col) const {
     verifyCoordinate(col, row);
    return m_board[col][row];
}

void KnightProblemSolver::findMovesSequential() {
    // reset data structures

    // TODO: DAs geht doch nur zum ersten Mal ... danach muss halt irgendwie überall 0 rein ?!?!?!?

    m_board.resize(m_width);
    for (std::vector<int>& column : m_board) {
        column.resize(m_height);
    }

    m_current.clear();

    // stopwatch
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // start at lower left corner            
    Coordinate start = Coordinate(m_height - 1, 0);
    findMovesSequential(start);

    // stopwatch
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "[Value] Elapsed time in milliseconds = " << duration << "[microsecs]" << std::endl;

    m_moveNumber = 0;
}

// private helper - algorithm to solve the Knight's Tour problem
void KnightProblemSolver::findMovesSequential(const Coordinate& coord)
{
    setKnightMoveAt(coord);
    m_current.push_back(coord);

    if (isSolution())
    {
        // need a copy of the current solution
        // List<Coordinate> copy = new List<Coordinate>(current);
        Solution copy(m_current);

        // add found solution to the list of all solutions
        m_solutions.push_back(copy);
    }
    else
    {
        // determine list of possible next moves
        std::vector<Coordinate> nextMoves = NextKnightMoves(coord);

        // do next moves sequential
        for (Coordinate coord : nextMoves) {
            findMovesSequential(coord);
        }
    }

    unsetKnightMoveAt(coord);
    m_current.pop_back();
}

// occupy square on the chess board
void KnightProblemSolver::setKnightMoveAt(const Coordinate& coord)
{
    m_moveNumber++;
    at(coord.getRow(), coord.getCol()) = m_moveNumber;
}

// release square on the chess board
 void KnightProblemSolver::unsetKnightMoveAt(const Coordinate& coord)
 {
     m_moveNumber--;
     at(coord.getRow(), coord.getCol()) = 0;
}

// checks, whether coordinate does exist on the chess board
 bool KnightProblemSolver::inRange(int row, int col)
 {
    return (row >= 0) && (row < m_height) && (col >= 0) && (col < m_width);
}

// checks, whether coordinate is valid and is still not taken
 bool KnightProblemSolver::canMoveTo(int row, int col)
 {
     return inRange(row, col) && (at(row, col) <= 0);
}

// verify, whether current list of moves is a solution
bool KnightProblemSolver::isSolution()
{
    return m_moveNumber >= m_height * m_width;
}

std::vector<Coordinate> KnightProblemSolver::NextKnightMoves(const Coordinate& coord)
{
    std::vector<Coordinate> result;

    if (canMoveTo(coord.getRow() + 2, coord.getCol() + 1))
    {
        result.push_back(Coordinate(coord.getRow() + 2, coord.getCol() + 1));
    }
    if (canMoveTo(coord.getRow() + 1, coord.getCol() + 2))
    {
        result.push_back(Coordinate(coord.getRow() + 1, coord.getCol() + 2));
    }
    if (canMoveTo(coord.getRow() - 2, coord.getCol() + 1))
    {
        result.push_back(Coordinate(coord.getRow() - 2, coord.getCol() + 1));
    }
    if (canMoveTo(coord.getRow() - 1, coord.getCol() + 2))
    {
        result.push_back(Coordinate(coord.getRow() - 1, coord.getCol() + 2));
    }
    if (canMoveTo(coord.getRow() + 2, coord.getCol() - 1))
    {
        result.push_back(Coordinate(coord.getRow() + 2, coord.getCol() - 1));
    }
    if (canMoveTo(coord.getRow() + 1, coord.getCol() - 2))
    {
        result.push_back(Coordinate(coord.getRow() + 1, coord.getCol() - 2));
    }
    if (canMoveTo(coord.getRow() - 2, coord.getCol() - 1))
    {
        result.push_back(Coordinate(coord.getRow() - 2, coord.getCol() - 1));
    }
    if (canMoveTo(coord.getRow() - 1, coord.getCol() - 2))
    {
        result.push_back(Coordinate(coord.getRow() - 1, coord.getCol() - 2));
    }

    return result;
}

// =====================================================================================

void test_01_springer_problem() {

    Coordinate coord(1, 2);
    std::cout << coord << std::endl;
}

void test_09_springer_problem() {

    KnightProblemSolver solver(2, 4);
    solver.findMovesSequential();
    ListSolutions solutions = solver.getSolutions();

    //int counter = 0;
    //for (Solution s : solutions) {
    //    std::cout << counter << ":" << std::endl;

    //    for (Coordinate coord : s) {
    //        std::cout << coord << " ";
    //    }
    //}

    std::cout << "Found: " << solutions.size() <<  std::endl;
}

// =====================================================================================

int main_knight_problem ()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // test_01_springer_problem();
    test_09_springer_problem();

    return 0;
}

// =====================================================================================

