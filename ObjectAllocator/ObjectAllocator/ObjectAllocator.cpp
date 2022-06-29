#define _CRT_SECURE_NO_WARNINGS

#include "ObjectAllocator.h"
#include <cstring>


ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
{
	config_ = config;
	OAStats_.ObjectSize_ = ObjectSize;

	allocate_new_page();
}

ObjectAllocator::~ObjectAllocator()
{

	char* checkPage = reinterpret_cast<char*>(PageList_) + sizeof(void*);
	if (config_.HBlockInfo_.type_ == OAConfig::hbExternal)
	{
		for (unsigned j = 0; j < OAStats_.PagesInUse_; j++)
		{
			for (unsigned i = 0; i < config_.ObjectsPerPage_; i++)
			{
				MemBlockInfo* tempMem;
				MemBlockInfo** memBlockLoc = reinterpret_cast<MemBlockInfo**>(reinterpret_cast<char*>(checkPage));
				tempMem = *memBlockLoc;
				if (tempMem != nullptr)
				{
					delete[] tempMem->label;
					delete tempMem;
					*memBlockLoc = nullptr;
				}
				checkPage += (OAStats_.ObjectSize_ + config_.PadBytes_ * 2 + config_.HBlockInfo_.size_);
			}
			PageList_ = PageList_->Next;
		}
	}

	char* page;
	while (PageList_ != NULL)
	{
		page = reinterpret_cast<char*>(PageList_);
		PageList_ = PageList_->Next;
		delete[] page;
	}

}

void* ObjectAllocator::Allocate(const char* label)
{
	if (config_.UseCPPMemManager_)
	{
		OAStats_.FreeObjects_--;
		OAStats_.Allocations_++;
		OAStats_.ObjectsInUse_++;
		if (OAStats_.MostObjects_ <= OAStats_.ObjectsInUse_)
		{
			OAStats_.MostObjects_ = OAStats_.ObjectsInUse_;
		}
		return new char[OAStats_.ObjectSize_];
	}

	if (FreeList_ == nullptr)
	{
		allocate_new_page();
	}

	OAStats_.Allocations_++;

	//set freelist
	GenericObject* Freeptr;
	Freeptr = FreeList_;
	FreeList_ = FreeList_->Next;

	OAStats_.FreeObjects_--;
	OAStats_.MostObjects_++;
	OAStats_.ObjectsInUse_++;

	//setup header block

	if (config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbBasic || config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbExtended)
	{
		char* counter = reinterpret_cast<char*>(Freeptr) - config_.PadBytes_ - sizeof(char) - sizeof(unsigned) - sizeof(unsigned short);
		unsigned i = static_cast<unsigned int>(*counter);
		if (i == UNALLOCATED_PATTERN)
		{
			i = 0;
		}
		memset(reinterpret_cast<char*>(Freeptr) - config_.HBlockInfo_.size_ - config_.PadBytes_, 0x00, config_.HBlockInfo_.size_);
		memset(reinterpret_cast<char*>(Freeptr) - config_.PadBytes_ - sizeof(char), 0x01, sizeof(char));
		char* setMem = reinterpret_cast<char*>(Freeptr) - config_.PadBytes_ - sizeof(char) - sizeof(unsigned);
		*setMem = static_cast<char>(OAStats_.Allocations_);
		if (config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbExtended)
		{
			i++;
			//*counter = i;
		}
	}
	else if (config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbExternal)
	{
		MemBlockInfo* newMemInfo = new MemBlockInfo;
		if (label)
		{
			size_t stringLength = strlen(label);
			char* labelLoc = new char[stringLength + 1];
			strcpy(labelLoc, label);
			newMemInfo->label = labelLoc;
			*(labelLoc + stringLength) = 0;
		}
		newMemInfo->in_use = true;
		newMemInfo->alloc_num = OAStats_.Allocations_;

		char* loc = reinterpret_cast<char*>(Freeptr) - config_.HBlockInfo_.size_ - config_.PadBytes_;
		*reinterpret_cast<MemBlockInfo**>(loc) = newMemInfo;
	}

	if (config_.DebugOn_ == true)
	{
		memset(reinterpret_cast<char*>(Freeptr), ALLOCATED_PATTERN, OAStats_.ObjectSize_);
	}
	return Freeptr;
}

unsigned ObjectAllocator::is_on_free_list(void* objptr) const
{
	char* first = reinterpret_cast<char*>(PageList_) + sizeof(void*);
	GenericObject* objPtr_ = FreeList_;

	if (config_.HBlockInfo_.type_ == config_.hbBasic || config_.HBlockInfo_.type_ == config_.hbExtended)
	{
		first = first - sizeof(char);
		unsigned i = static_cast<unsigned>(*first);
		return i;
	}
	else if (config_.HBlockInfo_.type_ == config_.hbExternal)
	{
		MemBlockInfo* tempMem = *reinterpret_cast<MemBlockInfo**>(first - config_.HBlockInfo_.size_);
		return tempMem->in_use;
	}
	if (config_.DebugOn_ == true)
	{
		while (objPtr_ != nullptr)
		{
			if (objPtr_ == objptr)
			{
				return false;
			}
			else
			{
				objPtr_ = objPtr_->Next;
				return true;
			}
		}
	}
}

void ObjectAllocator::Free(void* Object)
{
	GenericObject* objptr = reinterpret_cast<GenericObject*>(Object);
	GenericObject* temp2 = FreeList_;
	GenericObject* temp3 = FreeList_;
	if (config_.UseCPPMemManager_ == true)
	{
		OAStats_.Deallocations_++;
		OAStats_.ObjectsInUse_--;
		OAStats_.FreeObjects_++;
		delete[] objptr;
		return;
	}

	if (config_.DebugOn_ == true)
	{
		validate_object(Object);


		while (temp3 != nullptr)
		{
			if (temp3 == Object)
			{
				throw OAException(OAException::E_MULTIPLE_FREE, "");
			}
			else
			{
				temp3 = temp2->Next;
				temp2 = temp3;
			}
		}
		memset(reinterpret_cast<char*>(objptr), FREED_PATTERN, OAStats_.ObjectSize_);
	}

	if (config_.HBlockInfo_.type_ == OAConfig::hbBasic || config_.HBlockInfo_.type_ == OAConfig::hbExtended)
	{
		memset(reinterpret_cast<char*>(objptr) - config_.PadBytes_ - config_.HBlockInfo_.size_ + sizeof(unsigned short) + config_.HBlockInfo_.additional_, 0x00, sizeof(unsigned int) + sizeof(char));
	}
	if (config_.HBlockInfo_.type_ == OAConfig::hbExternal)
	{
		MemBlockInfo* tempMem;
		MemBlockInfo** memBlockLoc = reinterpret_cast<MemBlockInfo**>(reinterpret_cast<char*>(objptr) - config_.PadBytes_ - sizeof(unsigned));
		tempMem = *memBlockLoc;
		tempMem->in_use = false;
		delete[] tempMem->label;
		delete tempMem;
		*memBlockLoc = nullptr;
	}
	objptr->Next = FreeList_;
	FreeList_ = objptr;
	memset(reinterpret_cast<char*>(objptr) + sizeof(void*), FREED_PATTERN, OAStats_.ObjectSize_ - sizeof(void*));
	OAStats_.Deallocations_++;
	OAStats_.ObjectsInUse_--;
	OAStats_.FreeObjects_++;
}

unsigned ObjectAllocator::DumpMemoryInUse([[maybe_unused]] DUMPCALLBACK fn) const
{
	void* ptr = reinterpret_cast<char*>(PageList_) + sizeof(void*);
	unsigned retVal = 0;
	for (unsigned i = 0; i < config_.ObjectsPerPage_; i++)
	{
		if (is_on_free_list(ptr) != 1)
		{
			fn(ptr, config_.HBlockInfo_.size_ + config_.PadBytes_ * 2 + OAStats_.ObjectSize_);
			retVal++;
		}
	}
	return retVal;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
	if (!config_.DebugOn_ || !config_.PadBytes_)
	{
		return 0;
	}

	unsigned corruptedBlocks = 0;

	GenericObject* LastPage = PageList_;

	//Each page
	while (LastPage != nullptr)
	{
		char* block = reinterpret_cast<char*>(LastPage);
		block += config_.HBlockInfo_.size_ + config_.LeftAlignSize_ + sizeof(void*) + config_.PadBytes_;

		//for each block in the page
		for (unsigned int i = 0; i < config_.ObjectsPerPage_; i++)
		{
			GenericObject* objectData;
			objectData = reinterpret_cast<GenericObject*>(block + i * (config_.PadBytes_ * 2 + config_.InterAlignSize_ + config_.HBlockInfo_.size_));

			for (size_t i = 0; i < config_.PadBytes_; i++)
			{

				char* leftVal = reinterpret_cast<char*>(objectData) - config_.PadBytes_;
				char* rightVal = reinterpret_cast<char*>(objectData) + OAStats_.ObjectSize_;
				for (unsigned i = 0; i < config_.PadBytes_; i++)
				{
					char pp = static_cast<char>(PAD_PATTERN);
					if (leftVal[i] != pp || rightVal[i] != pp)
					{
						throw OAException(OAException::E_CORRUPTED_BLOCK, "Padding corruption.");
					}
				}
			}
		}
		LastPage = LastPage->Next;
	}
	//OAException(OAException::E_BAD_BOUNDARY, " ");

	return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{

	/*for (unsigned int i = 0; i < OAStats_.PagesInUse_; i++)
	{

	}*/

	return 0;
}

bool ObjectAllocator::ImplementedExtraCredit()
{
	return false;
}

void ObjectAllocator::SetDebugState(bool State)
{
	if (State == true)
	{
		config_.DebugOn_ = true;
	}
	else
	{
		config_.DebugOn_ = false;
	}
}

const void* ObjectAllocator::GetFreeList() const
{
	return FreeList_;
}

const void* ObjectAllocator::GetPageList() const
{
	return PageList_;
}

OAConfig ObjectAllocator::GetConfig() const
{
	return config_;
}

OAStats ObjectAllocator::GetStats() const
{
	return OAStats_;
}

void ObjectAllocator::allocate_new_page() {
	GenericObject* temp;
	GenericObject* temp2;

	//GenericObject* temp4;

	PageSize = sizeof(void*);
	PageSize += (config_.ObjectsPerPage_ * OAStats_.ObjectSize_);
	PageSize += config_.ObjectsPerPage_ * config_.PadBytes_ * 2;
	PageSize += config_.ObjectsPerPage_ * config_.HBlockInfo_.size_;
	PageSize += (config_.ObjectsPerPage_ - 1) * config_.InterAlignSize_;
	PageSize += config_.LeftAlignSize_;

	OAStats_.PageSize_ = PageSize;

	if (OAStats_.PagesInUse_ < config_.MaxPages_)
	{
		NewPage = new unsigned char[OAStats_.PageSize_];

		OAStats_.FreeObjects_ = config_.ObjectsPerPage_;
		OAStats_.PagesInUse_++;
	}
	else
	{
		throw OAException(OAException::E_NO_MEMORY, "");
	}

	memset(NewPage, UNALLOCATED_PATTERN, PageSize);

	//set pagelist_
	GenericObject* prevPage;
	prevPage = PageList_;
	PageList_ = reinterpret_cast<GenericObject*>(NewPage);
	PageList_->Next = prevPage;

	//set first free list

	FreeList_ = reinterpret_cast<GenericObject*>(NewPage + sizeof(void*) + config_.PadBytes_ + config_.HBlockInfo_.size_ + config_.LeftAlignSize_);
	temp = FreeList_;
	FreeList_->Next = nullptr;

	memset(reinterpret_cast<char*>(temp) - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
	memset(reinterpret_cast<char*>(temp) + OAStats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);
	//basic header
	memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - config_.HBlockInfo_.size_, nuse_flag, config_.HBlockInfo_.size_); //FreeList_ À§Ä¡ - header - pad  
	////alignment pattern
	//memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - config_.HBlockInfo_.size_ - config_.LeftAlignSize_, ALIGN_PATTERN, config_.LeftAlignSize_);

	for (unsigned int i = 1; i < config_.ObjectsPerPage_; i++)
	{
		//set freelist
		GenericObject* prevFree = FreeList_;

		temp2 = reinterpret_cast<GenericObject*>(reinterpret_cast<char*>(FreeList_) + config_.PadBytes_ + OAStats_.ObjectSize_ + config_.PadBytes_ + config_.HBlockInfo_.size_ + config_.InterAlignSize_);
		FreeList_ = temp2;
		FreeList_->Next = prevFree;

		//Pad Pattern
		memset(reinterpret_cast<char*>(temp2) - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
		memset(reinterpret_cast<char*>(temp2) + OAStats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);

		//basic header pattern
		memset(reinterpret_cast<char*>(temp2) - config_.PadBytes_ - config_.HBlockInfo_.size_, nuse_flag, config_.HBlockInfo_.size_); //FreeList_ À§Ä¡ - header - pad  


		// if(alignment is exits)
		////alignment pattern
		//memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - config_.HBlockInfo_.size_ - config_.LeftAlignSize_, ALIGN_PATTERN, config_.LeftAlignSize_);


	}
	memset(reinterpret_cast<char*>(NewPage) + PageSize - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);

}

void  ObjectAllocator::validate_object(void* Object)
{
	GenericObject* PrevPage = PageList_;

	bool return_ = false;

	if (!config_.PadBytes_)
	{
		return;
	}

	char* ptrObject = reinterpret_cast<char*>(Object);

	for (unsigned i = 0; i < OAStats_.PagesInUse_; i++)
	{

		char* block = reinterpret_cast<char*>(PrevPage);
		//block += config_.HBlockInfo_.size_ + config_.LeftAlignSize_ + sizeof(void*) + config_.PadBytes_;

		if (block < ptrObject && block + OAStats_.PageSize_ > ptrObject)
		{
			block += OAStats_.ObjectSize_ + (config_.PadBytes_ * 2) + config_.HBlockInfo_.size_;
			for (unsigned int j = 0; j < config_.ObjectsPerPage_; j++)
			{

				if (block == ptrObject)
				{
					return_ = true;
				}

				if (j < config_.ObjectsPerPage_ - 1)
				{
					block += OAStats_.ObjectSize_ + (config_.PadBytes_ * 2) + config_.HBlockInfo_.size_;
				}
			}
		}
		char* leftVal = reinterpret_cast<char*>(Object) - config_.PadBytes_;
		char* rightVal = reinterpret_cast<char*>(Object) + OAStats_.ObjectSize_;
		if (config_.PadBytes_ == 0)
		{
			return;
		}
		for (unsigned i = 0; i < config_.PadBytes_; i++)
		{
			char pp = static_cast<char>(PAD_PATTERN);
			if (leftVal[i] != pp || rightVal[i] != pp)
			{
				throw OAException(OAException::E_CORRUPTED_BLOCK, "Padding corruption.");
			}
		}
		PrevPage = PrevPage->Next;
	}
	if (return_ == false)
	{
		throw OAException(OAException::E_BAD_BOUNDARY, " ");
	}
}

