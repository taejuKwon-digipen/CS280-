#include "BList.h"

// default constructor
template<typename T, int Size>
inline BList<T, Size>::BList()
{
	head_ = nullptr;
	tail_ = nullptr;
	stats_.NodeSize = nodesize();
	stats_.ArraySize = Size;
}

// copy constructor
template<typename T, int Size>
BList<T, Size>::BList(const BList& rhs) : stats_(rhs.stats_)
{
	const BNode* hnode = rhs.GetHead();
	BNode* curr = nullptr;
	BNode* currprev = nullptr;

	for (int i = 0; i < stats_.NodeCount; i++)
	{
		curr = new BNode;

		for (int j = 0; j < stats_.ArraySize; j++)
		{
			curr->values[j] = hnode->values[j];
			curr->count = hnode->count;
		}

		if (currprev != nullptr)
		{
			currprev->next = curr;
			curr->prev = currprev;
		}
		else {
			head_ = curr;
		}
		tail_ = curr;
		hnode = hnode->next;
	}
}

// destructor
template<typename T, int Size>
BList<T, Size>::~BList()
{

}

// assign operator
template<typename T, int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList& rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	const BNode* hnode = rhs.GetHead();
	BNode* curr = nullptr;
	BNode* currprev = nullptr;

	for (int i = 0; i < stats_.NodeCount; i++)
	{
		curr = new BNode;

		for (int j = 0; j < stats_.ArraySize; j++)
		{
			curr->values[j] = hnode->values[j];
			curr->count = hnode->count;
		}

		if (currprev != nullptr)
		{
			currprev->next = curr;
			curr->prev = currprev;
			tail_ = curr;
		}
		else {
			head_ = curr;
		}

		hnode = hnode->next;
	}

	return *this;
}

// arrays will be unsorted, if calling either of these
template<typename T, int Size>
void BList<T, Size>::push_back(const T& value)
{
	if (tail_ != nullptr && tail_->count < Size)
	{
		unsigned int count_ = tail_->count;
		tail_->values[count_] = value;
		tail_->count++;
		stats_.ItemCount++;
	}
	else {
		BNode* curr = new BNode;
		curr->values[0] = value;
		stats_.ItemCount++;

		if (tail_ == nullptr)
		{
			tail_ = curr;
			head_ = tail_;
		}
		else {
			curr->prev = tail_;
			tail_->next = curr;
			tail_ = curr;
		}
		tail_->count++;
		stats_.NodeCount++;
	}

}

template<typename T, int Size>
void BList<T, Size>::push_front(const T& value)
{
	BNode* currhead = head_;

	if (head_ == nullptr)
	{
		BNode* curr = new BNode;
		curr->values[0] = value;
		head_ = curr;
		head_->count = 1;
		stats_.ItemCount++;
		stats_.NodeCount++;
	}
	else {
		int useitem = head_->count;

		if (useitem < stats_.ArraySize) // ÇöÀç »ç¿ëÁßÀÎ ³ëµå
		{
			for (int i = useitem; i > 0; i--)
			{
				head_->values[i] = head_->values[i - 1];
			}
			head_->values[0] = value;
			head_->count++;
			stats_.ItemCount++;
		}
		else //»ç¿ëÁß ³ëµå ¾øÀ½
		{
			BNode* curr = new BNode;

			curr->values[0] = value;
			currhead->prev = curr;
			head_ = curr;
			curr->next = currhead;
			head_->count = 1;
			stats_.ItemCount++;
			stats_.NodeCount++;
		}
	}
}

template<typename T, int Size>
void BList<T, Size>::split_node(BNode* curr)
{
	//pushback ¾ÈÇÑ¹öÀü
	BNode* newnode = new BNode;
	newnode->prev = curr;

	if (Size != 1)
	{
		int setsize = Size / 2;
		//³ëµå¸¦ ¸¸µé¾î¼­ ¾ÈÀÇ µ¥ÀÌÅÍ¸¦ ³ª´²ÁÜ
		//µ¥ÀÌÅÍ¸¦ ¿Ã¹Ù¸¥ À§Ä¡¿¡ ³ÖÀº°Ç ¾Æ´Ô -> ±×³É split¸¸ ÇØÁÜ
		int value_ = 0;
		for (int i = setsize; i < Size; i++)
		{
			newnode->values[value_] = curr->values[i];
			value_++;

			stats_.ItemCount++;
			stats_.NodeCount++;
			newnode->count++;
		}

		if (curr == tail_)
		{
			tail_ = newnode;
		}
	}
}


// arrays will be sorted, if calling this
template<typename T, int Size>
void BList<T, Size>::insert(const T& value)
{
	push_back(value);

	BNode* curr = head_;
	// unsigned int first = 0;

	if (curr != nullptr)
	{
		if (Size == 1)
		{
			//sort Ãß°¡
			if (stats_.ItemCount == 2)
			{
				if (value < curr->values[0])
				{
					BNode* storage = curr->next;

					tail_->prev = storage;
					tail_ = curr;
					head_ = storage;
					head_->next = curr;
					stats_.ItemCount++;
					tail_->next = nullptr;
				}
			}
			else if (stats_.ItemCount >= 3)
			{
				BNode* unsorted = head_;
				BNode* endofsorted = nullptr;

				for (int i = 0; i < stats_.NodeCount; i++)
				{
					unsorted = unsorted->next;
					endofsorted = unsorted->prev;

					for (int j = i; j > 0; j--)
					{
						if (unsorted->values[0] < endofsorted->values[0])
						{
							if (unsorted == tail_)
							{
								BNode* storage = endofsorted->prev;
								tail_ = endofsorted;
								tail_->prev = unsorted;
								unsorted->next = endofsorted;
								unsorted->prev = storage;
								storage->next = unsorted;
								tail_->next = nullptr;

							}
							else {
								BNode* storage1 = unsorted->next;

								unsorted->next->prev = endofsorted;
								unsorted->next = endofsorted;
								unsorted->prev = endofsorted->prev;

								endofsorted->prev->next = unsorted;
								endofsorted->next = storage1;
								endofsorted->prev = unsorted;
							}
							endofsorted = endofsorted->prev;
						}
					}
					if (endofsorted != head_)
					{
						endofsorted = endofsorted->prev;
					}
				}
			}
		}
		else {

			if (checkfullnode(curr) == true)
			{
				split_node(curr);
				//sortÃß°¡
				for (int i = 0; i < stats_.NodeCount; i++)
				{
					for (int j = 0; j < curr->count; j++)
					{
						if (value < curr->values[j])
						{
							for (int k = curr->count - j; k > 1; k--)
							{
								curr->values[k + 1] = curr->values[k];
								curr->values[k] = value;
								curr->count++;
								stats_.ItemCount++;
							}
						}
					}
					curr = curr->next;
				}
			}
			else {
				for (int i = 0; i < stats_.NodeCount; i++)
				{
					for (int j = 0; j < curr->count; j++)
					{
						if (value < curr->values[j])
						{
							for (int k = curr->count - j; k > 1; k--)
							{
								curr->values[k + 1] = curr->values[k];
								curr->values[k] = value;
								curr->count++;
								stats_.ItemCount++;
							}
						}
					}
					curr = curr->next;
				}
			}
		}
	}
}

template<typename T, int Size>
void BList<T, Size>::remove(int index)
{
	BNode* curr = head_;
	int sum = 0;

	while (sum <= index)
	{
		sum += curr->count;
		curr = curr->next;
	}

	if (curr == head_)
	{
		curr = curr;
	}
	else if (curr)
	{
		curr = curr->prev;
	}
	else {
		curr = tail_;
	}

	///nsigned int m = index % Size;

	for (unsigned int i = 0; i < Size; i++)
	{
		for (int j = i; j < curr->count; j++)
		{
			curr->values[j] = curr->values[j + 1];
		}
	}
	--curr->count;
	--stats_.ItemCount;

	if (curr->count == 0) //delete node
	{
		for (int j = 0; j < Size; ++j)
		{
			if (curr->prev)
			{
				curr->prev->next = curr->next;
			}
			else {
				head_ = curr->next;
			}

			if (curr->next)
			{
				curr->next->prev = curr->prev;
			}
			else {
				tail_ = curr->prev;
			}

			curr->~BNode();
			free(curr);
		}
	}

}

template<typename T, int Size>
void BList<T, Size>::remove_by_value(const T& value)
{
	auto deletion_index = 0;
	auto curr = head_;
	auto is_broken = false;

	while (curr) {
		for (auto i = 0; i < curr->count; ++i)
			if (curr->values[i] == value) {
				deletion_index = i;
				is_broken = true;
				break;
			}
		if (is_broken)
			break;

		curr = curr->next;
	}

	if (curr != nullptr) {
		// delete element
		for (auto j = deletion_index; j < curr->count; ++j)
			curr->values[j] = curr->values[j + 1];
		--curr->count;
		--stats_.ItemCount;

		if (curr->count == 0) {
			for (int j = 0; j < Size; ++j)
			{
				if (curr->prev)
				{
					curr->prev->next = curr->next;
				}
				else {
					head_ = curr->next;
				}

				if (curr->next)
				{
					curr->next->prev = curr->prev;
				}
				else {
					tail_ = curr->prev;
				}

				curr->~BNode();
				free(curr);
			}
			--stats_.NodeCount;
		}
	}
}

// returns index, -1 if not found
template<typename T, int Size>
int BList<T, Size>::find(const T& value) const
{
	BNode* curr = head_;
	int sum = 0;

	while (curr)
	{
		for (int i = 0; i < curr->count; i++)
		{
			if (curr->values[i] == value)
			{
				return sum + i;
			}
		}
		sum += curr->count;
		curr = curr->next;
	}
	return -1;
}

// for l-values
template<typename T, int Size>
T& BList<T, Size>::operator[](int index)
{
	BNode* curr = head_;
	//unsigned int sum = stats_.ItemCount;
	int j = 0;
	int i = 0;

	for (i = index; i > 0; i--)
	{
		j++;
		if (j == stats_.ItemCount)
		{
			curr = curr->next;
		}
	}

	return curr->values[i];
}

// for r-values
template<typename T, int Size>
const T& BList<T, Size>::operator[](int index) const
{
	BNode* curr = head_;
	int sum = stats_.ItemCount;
	int j = 0;
	int i = 0;

	for (i = index; i > 0; i--)
	{
		j++;
		if (j == sum)
		{
			curr = curr->next;
		}
	}

	return curr->values[i];
}

// total number of items (not nodes)
template<typename T, int Size>
size_t BList<T, Size>::size() const
{
	return stats_.ItemCount;
}

// delete all nodes
template<typename T, int Size>
void BList<T, Size>::clear()
{

}

// so the allocator knows the size
template<typename T, int Size>
size_t BList<T, Size>::nodesize()
{
	return sizeof(BNode);
}

template<typename T, int Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
	return head_;
}

template<typename T, int Size>
BListStats BList<T, Size>::GetStats() const
{
	return stats_;
}

template<typename T, int Size>
bool BList<T, Size>::checkfullnode(BNode* node)
{
	if (node->count == Size)
	{
		return true;
	}
	else {
		return false;
	}
}



