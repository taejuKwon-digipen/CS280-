#include "BList.h"

template<typename T, int Size>
inline BList<T, Size>::BList()
{

}

template<typename T, int Size>
BList<T, Size>::BList(const BList& rhs)
{
}

template<typename T, int Size>
BList<T, Size>::~BList()
{
}

template<typename T, int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList& rhs)
{
	// TODO: 여기에 return 문을 삽입합니다.
}

template<typename T, int Size>
void BList<T, Size>::push_back(const T& value)
{
}

template<typename T, int Size>
void BList<T, Size>::push_front(const T& value)
{
}

template<typename T, int Size>
void BList<T, Size>::insert(const T& value)
{
}

template<typename T, int Size>
void BList<T, Size>::remove(int index)
{
}

template<typename T, int Size>
void BList<T, Size>::remove_by_value(const T& value)
{
}

template<typename T, int Size>
int BList<T, Size>::find(const T& value) const
{
	return 0;
}

template<typename T, int Size>
T& BList<T, Size>::operator[](int index)
{
	
}

template<typename T, int Size>
const T& BList<T, Size>::operator[](int index) const
{
	
}

template<typename T, int Size>
size_t BList<T, Size>::size() const
{
	return size_t();
}

template<typename T, int Size>
void BList<T, Size>::clear()
{
}

template<typename T, int Size>
size_t BList<T, Size>::nodesize()
{
	return sizeof(BNode);
}

template<typename T, int Size>
const BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
	return head_;
}

template<typename T, int Size>
BListStats BList<T, Size>::GetStats() const
{
	return BListStats();
}
