#include "SortedList.h"

// Constructor
template<typename T, typename Pred>
inline SortedList<T, Pred>::SortedList(Pred sorter, ObjectAllocator* Allocator, bool SharedAllocator)
{
	sorter_ = sorter;
	shareAllocator_ = SharedAllocator;
	objAllocator_ = Allocator;

	if (Allocator != nullptr) //???¢¯?? ??¢¥??? ??¡Æ©ª
	{
		freeAllocator_ = false;
	}
	else if (Allocator == nullptr) //???¨ù ??¢¥???¢¬? ¢¬¢¬??¡Æ? ¢¬©£?? ??¢¥?¢¯¢® ¢¥??¨ª ??¢¥??? ??¢¯?
	{
		freeAllocator_ = true;
		OAConfig config(true);
		objAllocator_ = new ObjectAllocator(sizeof(Node), config);
	}
	head_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
	tail_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
	head_->Next = tail_;
	tail_->Prev = head_;
}

// Copy constructor
template<typename T, typename Pred>
SortedList<T, Pred>::SortedList(const SortedList& rhs)
{
	if (rhs.shareAllocator_ == true)
	{
		this->objAllocator_ = rhs.objAllocator_;

		this->freeAllocator_ = rhs.freeAllocator_;
		this->shareAllocator_ = rhs.shareAllocator_;
	}
	else
	{//???¨ù ??¢¥???¢¬? ¢¬¢¬??¡Æ? ¢¬©£?? ??¢¥?¢¯¢® ??¢¯?
		OAConfig config(true);

		this->objAllocator_ = rhs.objAllocator_;
		objAllocator_ = new ObjectAllocator(sizeof(Node), config);

		this->freeAllocator_ = rhs.freeAllocator_;
		this->shareAllocator_ = rhs.shareAllocator_;
	}
	head_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
	tail_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
	head_ = rhs.head_;
	tail_ = rhs.tail_;
	size_ = rhs.size_;
}

// Destructor
template<typename T, typename Pred>
SortedList<T, Pred>::~SortedList()
{

}

// Assignment operator
template<typename T, typename Pred>
SortedList<T, Pred>& SortedList<T, Pred>::operator=(const SortedList& rhs)
{

	if (this == &rhs)
	{
		return *this;
	}


	if (rhs.shareAllocator_ == true)
	{
		this->objAllocator_ = rhs.objAllocator_;
		this->freeAllocator_ = rhs.freeAllocator_;
		this->shareAllocator_ = rhs.shareAllocator_;
	}
	else
	{
		OAConfig config(true);

		this->objAllocator_ = rhs.objAllocator_;
		objAllocator_ = new ObjectAllocator(sizeof(Node), config);

		this->freeAllocator_ = rhs.freeAllocator_;
		this->shareAllocator_ = rhs.shareAllocator_;
	}

	this->head_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
	this->tail_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
	this->size_ = rhs.size_;
	this->head_ = rhs.head_;
	this->tail_ = rhs.tail_;

	return *this;
}

//Subscript operator (only need one)
template<typename T, typename Pred>
const T& SortedList<T, Pred>::operator[](size_t index) const
{
	Node* curr = nullptr;
	curr = head_->Next;
	size_t i = 0;
	while (i != index)
	{
		curr = curr->Next;
		i += 1;
	}

	return curr->Data;
}


// add to end
template<typename T, typename Pred>
void SortedList<T, Pred>::push_back(const T& value)
{
	Node* mem = reinterpret_cast<Node*>(objAllocator_->Allocate());
	Node* node = new (mem) Node(value);

	node->Prev = tail_->Prev;
	tail_->Prev->Next = node;
	node->Next = tail_;
	tail_->Prev = node;

	size_++;
}

// insert at proper (sorted) position
template<typename T, typename Pred>
void SortedList<T, Pred>::insert(const T& value)
{
	this->push_back(value);

	Node* unsorted = head_;
	Node* endofsorted = nullptr;

	for (unsigned int i = 0; i < size_; i++)
	{
		unsorted = unsorted->Next;
		endofsorted = unsorted->Prev;

		for (unsigned int j = i; j > 0; j--)
		{
			if (sorter_(unsorted->Data, endofsorted->Data) == true)
			{
				Node* storage1 = unsorted->Next;

				unsorted->Next->Prev = endofsorted;
				unsorted->Next = endofsorted;
				unsorted->Prev = endofsorted->Prev;

				endofsorted->Prev->Next = unsorted;
				endofsorted->Next = storage1;
				endofsorted->Prev = unsorted;

				endofsorted = endofsorted->Prev;
			}
			endofsorted = endofsorted->Prev;
		}
	}
}

// number of nodes
template<typename T, typename Pred>
unsigned SortedList<T, Pred>::size() const
{
	return size_;
}

// free all nodes
template<typename T, typename Pred>
void SortedList<T, Pred>::clear()
{

}

// Sorts using the selection sort algorithm (N^2)
template<typename T, typename Pred>
template<typename Sorter>
inline void SortedList<T, Pred>::selection_sort(Sorter sorter)
{
	Node* currpos = head_;
	Node* min = nullptr;
	Node* pos1 = nullptr;

	min = currpos->Next;
	currpos = currpos->Next;
	pos1 = currpos->Next;

	for (unsigned int i = 0; i < size_; i++)
	{

		for (unsigned int j = i + 1; j < size_; j++)
		{
			if (sorter(min->Data, pos1->Data) == true)
			{
				min = min;
			}
			else if (sorter(pos1->Data, min->Data) == true) {
				min = pos1;
			}
			pos1 = pos1->Next;
		}
		Node* restoreprev = currpos->Prev;
		Node* restorenext = currpos->Next;
		if (min->Prev == currpos || min->Next == currpos)
		{
			currpos->Prev = min;
			currpos->Next = min->Next;
			currpos->Next->Prev = currpos;

			min->Prev = restoreprev;
			min->Next = currpos;
			min->Prev->Next = min;

			min = currpos;
			pos1 = currpos->Next;
		}
		else {

			currpos->Prev->Next = min;
			currpos->Next->Prev = min;
			currpos->Prev = min->Prev;
			currpos->Next = min->Next;

			min->Prev->Next = currpos;
			min->Next->Prev = currpos;

			min->Prev = restoreprev;
			min->Next = restorenext;

			currpos = min;

			min = currpos->Next;
			currpos = currpos->Next;
			pos1 = currpos->Next;
		}
	}
}

// Sorts using the merge sort algorithm (NlgN)
template<typename T, typename Pred>
template<typename Sorter>
void SortedList<T, Pred>::merge_sort(Sorter sorter)
{
	sorter(1, 2);
}


// If the extra credit has been implemented or not
template<typename T, typename Pred>
bool SortedList<T, Pred>::ImplementedExtraCredit()
{
	return false;
}

// The sizeof a node
template<typename T, typename Pred>
unsigned SortedList<T, Pred>::nodesize()
{
	return sizeof(Node);
}

