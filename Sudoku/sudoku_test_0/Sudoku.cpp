#include "Sudoku.h"
#include <vector>
#include <algorithm>

Sudoku::Sudoku(int basesize, SymbolType stype, [[maybe_unused]]SUDOKU_CALLBACK callback)
{
	basesize_ = basesize;
	basicsize = basesize_ * basesize_;
	theBoard_ = new char[basicsize * basicsize];

	sym_type_ = stype;

	stats_.moves = moves_;
	stats_.backtracks = backtracks_;

	callback = 0;
}

Sudoku::~Sudoku()
{
	delete[] theBoard_;
}

void Sudoku::SetupBoard(const char* values, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (values[i] == ' ') {
			theBoard_[i] = ' ';
			//vecvalues.push_back(0);
		}
		else {
			theBoard_[i] = values[i];
			//vecvalues.push_back(values[i]);
		}
	}
	
}

void Sudoku::Solve()
{
	callback_(*this, theBoard_, MessageType::MSG_STARTING, moves_, stats_.basesize, index,-1, 0);

	while (is_empty(index) == false)
	{
		index++;
	}
	
	bool success = place_value(index);

	if (success == true)
	{
		callback_(*this, theBoard_, MessageType::MSG_FINISHED_OK, moves_, stats_.basesize, index,-1, 0);
	}
	else {
		callback_(*this, theBoard_, MessageType::MSG_FINISHED_FAIL, moves_, stats_.basesize, index,-1, 0);
	}
}

bool Sudoku::place_value(int index)
{
	// 같은 열, 행, 블록에 같은 숫자 = false;
	//모든 조건 만족시 true
	//row col cell 을 함수별로 만들어서 아니면 다시 함수실행 하는 방식으로 

	char val;

	std::vector<char> rowval;
	std::vector<char> colval;
	std::vector<char> cellval;

	if (sym_type_ == SymbolType::SYM_NUMBER)
	{
		val = '1';
	}
	else {
		val = 'A';
	}

	int row = 1;
	int col = 1;
	int num = index;
	
	//row
	if (index < basesize_*basesize_)
	{
		row = 0;
		col = 0;
	}
	else {

		while (num > basesize_*basesize_)
		{
			num -= basesize_*basesize_;
			row += 1; //(num, row)
			col += 1;
		}
	}
	int x = row * basesize_*basesize_ + 1;

	for (int i = 0; i < basesize_*basesize_; i++)
	{
		if (theBoard_[x + i] == val)
		{
			val++;
		}
		else {
			rowval.push_back(val);
			val++;
		}
	}
	//col
	int y = col * basesize_*basesize_ + 1;

	for (int i = 0; i < 9; i++)
	{
		if (theBoard_[y * basesize_*basesize_] == val)
		{
			val++;
		}
		else {
			colval.push_back(val);
			val++;
		}
	}

	int cell = index;
	int i = 1;
	while (i * i != basicsize)
	{
		i++;
	}

	int remain = index % i;
	cell -= remain;
	int count = basesize_*basesize_ / i;

	for (int j = 1; j <= count; j++) {
		for (int k = 1; k <= count; k++)
		{
			if (val == theBoard_[cell + k])
			{
				val++;
			}
			else {
				cellval.push_back(val);
				val++;
			}
		}
		cell *= basesize_*basesize_;
	}

	long unsigned int a = 0;
	long unsigned int b = 0;
	std::vector<long unsigned int> firstvals;

	while (a < rowval.size())
	{
		if (find(colval.begin(), colval.end(), rowval[a]) != colval.end())
		{
			firstvals.push_back(a);
		}
		a++;
	}

	char result = 0;

	while (b < cellval.size())
	{
		if ( find(firstvals.begin(), firstvals.end(), cellval[b]) !=  firstvals.end())
		{
			result = cellval[b];
			callback_(*this, theBoard_, MessageType::MSG_PLACING, moves_, stats_.basesize, index, result,0);
			return true;
		}
		b++;
	}
	return true;
}

bool Sudoku::is_empty(int index)
{
	if (theBoard_[index] == ' ')
	{
		return true;
	}
	return false;
}

int Sudoku::next_empty_cell(int from)
{
	int count = basicsize - from;

	/*for (int i = 0; i < count; i++)
	{
		if (theBoard_[i] == ' ')
		{
			return i;
		}
	}*/
	while(theBoard_[count] != ' ')
	{
	    count++;
	    moves_++;
	}
	return count;
}

const char* Sudoku::GetBoard() const
{
	return theBoard_;
}

Sudoku::SudokuStats Sudoku::GetStats() const
{
	return stats_;
}

