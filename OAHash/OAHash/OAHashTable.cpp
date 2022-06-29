#include "OAHashTable.h"
#include <cmath>

template<typename T>
inline OAHashTable<T>::OAHashTable(const OAHTConfig& Config): config(Config)
{
	
	stat.TableSize_ = Config.InitialTableSize_;
	table = new OAHTSlot[stat.TableSize_];
	nowitem = 0;
	InitTable();
}

template<typename T>
OAHashTable<T>::~OAHashTable()
{
	delete[] table;
}

template<typename T>
void OAHashTable<T>::insert(const char* Key, const T& Data)//1
{
	int i = 0;
	//OHAShlot 들의 배열에각각 key, Data 넣기
	//차있지 않으면 그자리에 데이터를 넣고, 차있으면 다음 인덱스로 이동
	//stat.probe를 더해주면 된다?

	if ((nowitem / stat.TableSize_) > config.MaxLoadFactor_)
	{
		GrowTable();q

	if (table[config.PrimaryHashFunc_(Key, stat.TableSize_)].State == OAHTSlot::DELETED || table[config.PrimaryHashFunc_(Key, stat.TableSize_)].State == OAHTSlot::UNOCCUPIED)
	{
		table[config.PrimaryHashFunc_(Key, stat.TableSize_)].State = OAHTSlot::OCCUPIED;
		strncpy(table[config.PrimaryHashFunc_(Key, stat.TableSize_)].Key, Key, MAX_KEYLEN);
		table[config.PrimaryHashFunc_(Key, stat.TableSize_)].Data = Data;
	}
	else {
		while (table[config.PrimaryHashFunc_(Key, stat.TableSize_) + i].State == OAHTSlot::OCCUPIED)
		{
			i++;
		}
		strncpy(table[config.PrimaryHashFunc_(Key, stat.TableSize_)].Key, Key, MAX_KEYLEN);
		table[config.PrimaryHashFunc_(Key, stat.TableSize_) + i].Data = Data;
		table[config.PrimaryHashFunc_(Key, stat.TableSize_) + i].State = OAHTSlot::OCCUPIED;
	}
	nowitem++;
}

template<typename T>
void OAHashTable<T>::remove(const char* Key)
{
	if (table[config.PrimaryHashFunc_(Key, stat.TableSize_)].State == OAHTSlot::UNOCCUPIED )
	{
		throw OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "not exist");
	}
	else {
		table[config.PrimaryHashFunc_(Key, stat.TableSize_)].State = OAHTSlot::DELETED;
		table[config.PrimaryHashFunc_(Key, stat.TableSize_)].Data = NULL;
		//strncpy
	}
	nowitem--;
}

template<typename T>
const T& OAHashTable<T>::find(const char* Key) const
{
	for (int i = 0; i < stat.TableSize_; i++)
	{
		if (table[i].Key == Key)
		{
			/*return *table[i].Data;*/
		}
		else {
			throw OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "not exist");
		}
	}
}

template<typename T>
void OAHashTable<T>::clear()
{

}

template<typename T>
OAHTStats OAHashTable<T>::GetStats() const
{
	return stat;
}

template<typename T>
typename const OAHashTable<T>::OAHTSlot* OAHashTable<T>::GetTable() const
{
	return table;
}

template<typename T>
void OAHashTable<T>::InitTable()
{
	for (int i = 0; i < stat.TableSize_; i++)
	{
		table[i].State = OAHTSlot::UNOCCUPIED;
	}
}

template<typename T>
void OAHashTable<T>::GrowTable()
{
	double factor = std::ceil(stat.TableSize_ * config.GrowthFactor_);
	unsigned new_size = GetClosestPrime(static_cast<unsigned>(factor));
	stat.TableSize_ = new_size;

	OAHTSlot* table2 = new OAHTSlot[new_size];

	for (int i = 0; i < stat.TableSize_; i++)
	{
		if (table2[i].State != OAHTSlot::UNOCCUPIED)
		{
			strncpy(table2[i].Key, table[i].Key, MAX_KEYLEN);
			table2[i].State = table[i].State;
			table2[i].Data = table[i].Data;
		}
	}
	delete[] table;

	table = new OAHTSlot[new_size];
	InitTable();
	for (int i = 0; i < stat.TableSize_; i++)
	{
		if (table2[i].State != OAHTSlot::UNOCCUPIED)
		{
			strncpy(table[i].Key, table2[i].Key, MAX_KEYLEN);
			table[i].State = table2[i].State;
			table[i].Data = table2[i].Data;
		}
	}

	delete[] table2;
}


