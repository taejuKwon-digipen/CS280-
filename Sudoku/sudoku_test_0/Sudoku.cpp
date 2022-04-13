#include "Sudoku.h"

Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback)
{
	basesize_ = basesize;
	basicsize = basesize_ * basesize_;
	theBoard_ = new char[basicsize];

	sym_type_ = stype;
}

Sudoku::~Sudoku()
{
	delete[] theBoard_;
}

void Sudoku::SetupBoard(const char* values, int size)
{
	for (int i = 0; i < size; i++)
	{

	}
}

void Sudoku::Solve()
{
}

const char* Sudoku::GetBoard() const
{
	return theBoard_;
}

Sudoku::SudokuStats Sudoku::GetStats() const
{
	return stats_;
}
