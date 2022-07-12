//---------------------------------------------------------------------------
#ifndef SUDOKUH
#define SUDOKUH
//---------------------------------------------------------------------------
#include <cstddef> /* size_t */
#include <vector>

//! The Sudoku class
class Sudoku
{
public:
    //! Used by the callback function 
    enum MessageType
    {
        MSG_STARTING,      //!< the board is setup, ready to go
        MSG_FINISHED_OK,   //!< finished and found a solution
        MSG_FINISHED_FAIL, //!< finished but no solution found
        MSG_ABORT_CHECK,   //!< checking to see if algorithm should continue
        MSG_PLACING,       //!< placing a symbol on the board
        MSG_REMOVING       //!< removing a symbol (back-tracking)
    };

    //! 1-9 for 9x9, A-P for 16x16, A-Y for 25x25
    enum SymbolType { SYM_NUMBER, SYM_LETTER };

    //! Represents an empty cell (the driver will use a . instead)
    const static char EMPTY_CHAR = ' ';

    //! Implemented in the client and called during the search for a solution
    typedef bool (*SUDOKU_CALLBACK)
        (const Sudoku& sudoku, // the gameboard object itself
            const char* board,    // one-dimensional array of symbols
            MessageType message,  // type of message
            size_t move,          // the move number
            int basesize,         // 3, 4, 5, etc. (for 9x9, 16x16, 25x25, etc.)
            int index,            // index of current cell
            char value,           // symbol (value) in current cell
            int* dup_indexes      // index of each duplicate sorted from lowest
                                  // index to highest. -1 means no conflict.
                                  // max of 3 duplicates.
            );

    //! Statistics as the algorithm works
    struct SudokuStats
    {
        int basesize;      //!< 3, 4, 5, etc.
        int placed;        //!< the number of valid values the algorithm has placed 
        size_t moves;      //!< total number of values that have been tried
        size_t backtracks; //!< total number of times the algorithm backtracked

        //!< Default constructor
        SudokuStats() : basesize(0), placed(0), moves(0), backtracks(0) {}
    };

    // Constructor
    Sudoku(int basesize, SymbolType stype = SYM_NUMBER, SUDOKU_CALLBACK callback = 0);

    // Destructor
    ~Sudoku();

    // The client (driver) passed the board in the values parameter
    void SetupBoard(const char* values, int size);

    // Once the board is setup, this will start the search for the solution
    void Solve();

    // For debugging with the driver
    const char* GetBoard() const;
    SudokuStats GetStats() const;

private:

    SudokuStats stats_;
    int value_;
    int basicsize;
    int index = 1;
    char* theBoard_;           //!< The 1-D representation of the board
    int basesize_;             //!< The size of the side of a block (e.g. 3, 4, 5)
    SUDOKU_CALLBACK callback_; //!< The callback to communicate with the client
    SymbolType sym_type_;      //!< Numbers (1-9) or letters (A-P, A-Y)
    size_t moves_;             //!< The number of total attempts
    size_t backtracks_;        //!< The number of times the algorithm backtracked
    unsigned boardsize;

    //  /*! Peers are all values in the row, column, and neighborhood of a given cell.
    //      Makes it easier to determine if there is a duplicate
    //   */
    struct Peer
    {
        char value; //!< The value in the cell
        int index;  //!< The (0-based) index of the cell
    };
    //// index is a 1-D index in the 2-D board
    unsigned dubindex_;
    bool isvalid(unsigned x, unsigned y, char val);
    bool place_value(int index);           // Function that does the actual recursion
};

#endif  // SUDOKUH