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

		for (unsigned int j = 0; j < stats_.ArraySize; j++)
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

		for (unsigned int j = 0; j < stats_.ArraySize; j++)
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

		if (useitem < stats_.ArraySize) // 현재 사용중인 노드
		{
			for (int i = useitem; i > 0; i--)
			{
				head_->values[i] = head_->values[i-1];
			}
			head_->values[0] = value;
			head_->count++;
			stats_.ItemCount++;
		}
		else //사용중 노드 없음
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

// arrays will be sorted, if calling this
template<typename T, int Size>
void BList<T, Size>::insert(const T& value)
{
	push_back(value);

	BNode* curr = head_;
	int first = 0;

	if (curr != nullptr)
	{
		if (Size != 1)
		{
			for (int i = 0; i < stats_.NodeCount; i++)
			{
				for (int j = 0; j < stats_.ArraySize; j++)
				{

					if (value < curr->values[j])
					{
						if (checkfullnode(curr) == true) //노드가 다 찼을떄
						{
							BNode* newnode = new BNode;
							newnode->next = curr->next;
							curr->next = newnode;
							newnode->prev = curr;

							for (int k = static_cast<int>(Size / 2); k < Size; k++)
							{
								newnode->values[first] = curr->values[k];
								first++;
							} //노드를 만들어서 안의 데이터를 나눠줌

							for (int m = curr->count; m > 0; m--)
							{
								if (value < curr->values[m])
								{
									for (int push = curr->count; push > 0; push--)
									{
										curr->values[push + 1] = curr->values[push];
									}
									curr->values[m] = value;
									curr->count++;
								}
								else {
									break;
								}
							}

							for (int n = 0; n < newnode->count; n++)
							{
								if (value < newnode->values[n])
								{
									if (value < newnode->values[n])
									{
										for (int push = newnode->count; push > 0; push--)
										{
											newnode->values[push + 1] = newnode->values[push];
										}
										newnode->values[n] = value;
										newnode->count++;
									}
								}
								else {
									break;
								}
							}

							stats_.ItemCount++;
							stats_.NodeCount++;
						}
						else if (checkfullnode(curr) == false) //노드 다 안참
						{
							for (int l = j; l > 0; l--)
							{
								curr->values[l + 1] = curr->values[l];
							}
							curr->values[j] = value;
							stats_.ItemCount++;
							curr->count++;
						}
					}
				}
			}
			if (curr->next != tail_)
			{
				curr = curr->next;
			}
		}
		else if (Size == 1)
		{

		}
	}
}

template<typename T, int Size>
void BList<T, Size>::remove(int index)
{

}

template<typename T, int Size>
void BList<T, Size>::remove_by_value(const T& value)
{

}

// returns index, -1 if not found
template<typename T, int Size>
int BList<T, Size>::find(const T& value) const
{
	return 0;
}

// for l-values
template<typename T, int Size>
T& BList<T, Size>::operator[](int index)
{
	BNode* curr = head_;
	int sum = stats_.ItemCount;
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
		if (j == stats_.ItemCount)
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



