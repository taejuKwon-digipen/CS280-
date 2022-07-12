#include "Sudoku.h"
#include <vector>
#include <algorithm>

Sudoku::Sudoku(int basesize, SymbolType stype, [[maybe_unused]]SUDOKU_CALLBACK callback)
{
	basesize_ = basesize;
	sym_type_ = stype;
	basicsize = basesize * basesize;
	boardsize = basicsize * basicsize;
	theBoard_ = new char[boardsize];

	stats_.moves = moves_;//!< The number of total attempts
	stats_.backtracks = backtracks_; 
	
	for (int i = 0; i < boardsize; i++)
	{
		theBoard_[i] = EMPTY_CHAR;
	}
}

Sudoku::~Sudoku()
{
	
}

void Sudoku::SetupBoard(const char* values, int size)
{
	for (unsigned i = 0; i < size; i++)
	{
		if (values[i] != '.')
		{
			theBoard_[i] = values[i];
		}
	}

	/*
	init_board with size
		delete current board buffer
		allocate board buffer of size+1
		fill board buffer with EMPTY_CHAR
		null terminate board buffer
	*/
}

void Sudoku::Solve()
{
	callback_(*this, theBoard_, MessageType::MSG_STARTING, moves_, stats_.basesize, index, -1, 0);

	while (is_empty(index) == false)
	{
		index++;
	}

	bool success = place_value(index);

	if (success == true)
	{
		callback_(*this, theBoard_, MessageType::MSG_FINISHED_OK, moves_, stats_.basesize, index, -1, 0);
	}
	else {
		callback_(*this, theBoard_, MessageType::MSG_FINISHED_FAIL, moves_, stats_.basesize, index, -1, 0);
	}
}

bool Sudoku::isvalid(unsigned x, unsigned y, char val)
{
	return false;
}

bool Sudoku::place_value(int index)
{	
	//basicsize ฐก วัมู

	unsigned x = 0;
	unsigned y = 0;

	unsigned index_ = index;

	while (index_ <= basicsize)
	{
		y++;
	}
	x = index_;

	if (y == basicsize)
	{
		return true;
	}

	if (theBoard_[index] != ' ')
	{
		if (x == basicsize - 1)
		{
			if (place_value(((y+1)*basicsize)) == true) //(0,y+1)
			{
				return true;
			}
		}
		else {
			if (place_value((y * basicsize + x)) == true)
			{
				return true;
			}
		}
		return false;
	}

	char val;
	if (sym_type_ == SymbolType::SYM_NUMBER)
	{
		val = '1';
	}
	else {
		val = 'A';
	}

	for (size_t i = 0; i < basicsize; i++)
	{
		if (callback_(*this, theBoard_, MessageType::MSG_ABORT_CHECK, moves_, stats_.basesize, index, val, dubindex_)) {
			return false;
		}

		theBoard_[index] = val;
		stats_.moves++;
		stats_.placed++;
		callback_(*this, theBoard_, MessageType::MSG_PLACING, moves_, stats_.basesize, index, val, dubindex_);

		if (x == basicsize - 1)
		{
			if (place_value(((y + 1) * basicsize)) == true) //(0,y+1)
			{
				return true;
			}
		}
		else {
			if (place_value((y * basicsize + x)) == true)
			{
				return true;
			}
		}

		theBoard_[index] = ' ';

		stats_.backtracks++;
		callback_(*this, theBoard_, MessageType::MSG_REMOVING, moves_, stats_.basesize, index, val, dubindex_);
	}
	return false;
}

bool Sudoku::isvalid(unsigned x, unsigned y, char val) {

}
