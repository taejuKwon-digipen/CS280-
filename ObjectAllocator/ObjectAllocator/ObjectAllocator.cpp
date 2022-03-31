#define _CRT_SECURE_NO_WARNINGS

#include "ObjectAllocator.h"
#include <cstring>


ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
{
	/*
	left alignment -> calc the byte offset of the 1st data section
	size of (void*) + left alignment + header size + padding => number multiple alignment
	calc the left alignment to get the exact multiple

	Inter alignment -> inter alignment+header+left padding+ object size + rightpadding

	Allocate a page, maybe mark it
	Build the free list
	*/
	GenericObject* temp;
	GenericObject* temp2;

	GenericObject* temp3;
	config_ = config;

	PageSize = sizeof(void*);
	PageSize += config.ObjectsPerPage_ * (ObjectSize);
	PageSize += config.ObjectsPerPage_ * config.PadBytes_ * 2;


	if (config.HBlockInfo_.type_ == config.hbBasic)
	{
		PageSize += (config.ObjectsPerPage_ * config.HBlockInfo_.size_);
	}
	else if (config.HBlockInfo_.type_ == config.hbExtended)
	{
		PageSize += (config.ObjectsPerPage_ * config.HBlockInfo_.size_);
	}
	else if (config.HBlockInfo_.type_ == config.hbExternal)
	{
		PageSize += (config.ObjectsPerPage_ * config.HBlockInfo_.size_);
	}
	PageSize += (config.ObjectsPerPage_ - 1) * config.InterAlignSize_;

	OAStats_.FreeObjects_ += config.ObjectsPerPage_;
	OAStats_.ObjectSize_ = ObjectSize;
	OAStats_.PageSize_ = PageSize;

	char* NewPage;
	NewPage = new char[PageSize];
	memset(NewPage, UNALLOCATED_PATTERN, PageSize);
	OAStats_.PagesInUse_++;

	PageList_ = reinterpret_cast<GenericObject*>(NewPage);
	temp3 = PageList_;
	temp3->Next = nullptr;


	FreeList_ = reinterpret_cast<GenericObject*>(NewPage + sizeof(void*) + config_.PadBytes_ + config_.HBlockInfo_.size_);

	temp = FreeList_;
	temp->Next = nullptr; //first next

	memset(reinterpret_cast<char*>(temp) - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);

	//basic header
	memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - config.HBlockInfo_.size_, nuse_flag, config_.HBlockInfo_.size_); //FreeList_ À§Ä¡ - header - pad  

	memset(reinterpret_cast<char*>(temp) + ObjectSize, PAD_PATTERN, config_.PadBytes_);

	for (int i = 1; i < config.ObjectsPerPage_; i++)
	{
		temp2 = reinterpret_cast<GenericObject*>(reinterpret_cast<char*>(temp) + config_.PadBytes_ + OAStats_.ObjectSize_ + config_.PadBytes_ + config_.HBlockInfo_.size_);
		temp2->Next = temp;
		FreeList_ = temp2;
		temp = temp2;
		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
		//basic header
		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - config.HBlockInfo_.size_, nuse_flag, config_.HBlockInfo_.size_); //FreeList_ À§Ä¡ - header - pad   
		memset(reinterpret_cast<char*>(temp) + ObjectSize, PAD_PATTERN, config_.PadBytes_);
	}

}

ObjectAllocator::~ObjectAllocator()
{
	/*
	if using external headers
	for each allocated block
	  delete the label
	  delete the header
  walk the page list and delete each one
	bytes array delete
	*/

}

void* ObjectAllocator::Allocate(const char* label)
{
	/*
	if( freelist is empty)
	try to allocate a new page

 get generic object from the free list head ptr
 update the free list head ptr to the next element
 update the ALLOC pattern
 update the header
 give the raw memory location of the object to the user
	*/
	OAStats_.FreeObjects_--;
	OAStats_.MostObjects_++;
	OAStats_.Allocations_++;
	OAStats_.ObjectsInUse_++;


	GenericObject* temp;

	if (FreeList_ == nullptr)
	{
		if (OAStats_.PagesInUse_ < config_.MaxPages_)
		{
			allocate_new_page();
		}
		else
		{
			throw OAException(OAException::E_NO_MEMORY, "");
		}
	}


	temp = FreeList_;
	FreeList_ = temp->Next;

	if (config_.HBlockInfo_.type_ == config_.hbBasic)
	{
		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_, OAStats_.Allocations_, sizeof(char));
		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - sizeof(char), use_flag, sizeof(char));
	}
	else if (config_.HBlockInfo_.type_ == config_.hbExtended)
	{
		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_, 0x00, config_.HBlockInfo_.additional_); //extra bytes
		unsigned short* use_count = reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_ + config_.HBlockInfo_.additional_);
		unsigned short count = (*use_count) + 1;
		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_ + config_.HBlockInfo_.additional_, count, sizeof(char)); //use count

		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_ + config_.HBlockInfo_.additional_ + sizeof(unsigned short), OAStats_.Allocations_, sizeof(char)); //allocate count

		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - sizeof(char), use_flag, sizeof(char));
	}
	else if (config_.HBlockInfo_.type_ == config_.hbExternal)
	{
		MemBlockInfo* block1 = new MemBlockInfo;
		char* new_label = new char[sizeof(label)];
		strcpy(new_label, label);
		block1->label = new_label;


	}

	memset(reinterpret_cast<char*>(temp), ALLOCATED_PATTERN, OAStats_.ObjectSize_);

	return temp;

}

void ObjectAllocator::Free(void* Object)
{
	/*
	 if debug
	validate the block
	check for double free
	set FREE pattern
  put the block back on the free list
  making this memory be the new free list head ptr
	*/
	OAStats_.FreeObjects_++;
	OAStats_.ObjectsInUse_--;
	OAStats_.Deallocations_++;

	GenericObject* temp;
	temp = reinterpret_cast<GenericObject*>(Object);

	GenericObject* temp2 = FreeList_;

	GenericObject* temp3 = FreeList_;

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

	if (config_.HBlockInfo_.type_ == config_.hbBasic)
	{
		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_, nuse_flag, sizeof(char));
		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - sizeof(char), nuse_flag, sizeof(char));
	}
	else if (config_.HBlockInfo_.type_ == config_.hbExtended)
	{
		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_, 0x00, config_.HBlockInfo_.additional_); //extra bytes

		unsigned short* use_count = reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_ + config_.HBlockInfo_.additional_);
		unsigned short count = *use_count;
		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_ + config_.HBlockInfo_.additional_, count, sizeof(char)); //use count

		memset(reinterpret_cast<char*>(temp) - config_.HBlockInfo_.size_ - config_.PadBytes_ + config_.HBlockInfo_.additional_ + sizeof(unsigned short), 0x00, sizeof(char)); //allocate count
		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - sizeof(char), nuse_flag, sizeof(char)); //allocate count
	}
	else if (config_.HBlockInfo_.type_ == config_.hbExternal)
	{
		blockInfo.in_use = false;
	}

	temp->Next = FreeList_;
	FreeList_ = temp;
	memset(reinterpret_cast<char*>(temp) + sizeof(void*), FREED_PATTERN, OAStats_.ObjectSize_ - sizeof(void*));

}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
	/*
	for each Page
	  for each block in the page
		if block is in use (not on the free list)
		  invoke the callback ptr data section
	return how many times you invoked the callback
	*/
	return 0;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
	/*
	 for each Page
	for each Block in the Page
	  if block is not valid (corrupted padding)
		  invoke the callback ptr data section
  return how corrupted blocks there are
	*/
	return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
	/*
	for each Page
  for each block in page
	if block is NOT on the freelist
	  break
  if we didn't break
	move all page blocks off the free list
	remove the page from the page list
	delete the page
	*/
	return 0;
}

bool ObjectAllocator::ImplementedExtraCredit()
{
	return false;
}

void ObjectAllocator::SetDebugState(bool State)
{

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

void ObjectAllocator::allocate_new_page()
{
	GenericObject* temp;
	GenericObject* temp2;

	GenericObject* temp3;

	GenericObject* temp4;

	PageSize = sizeof(void*);
	PageSize += config_.ObjectsPerPage_ * (OAStats_.ObjectSize_);
	PageSize += config_.ObjectsPerPage_ * config_.PadBytes_ * 2;

	if (config_.HBlockInfo_.type_ == config_.hbBasic)
	{
		PageSize += (config_.ObjectsPerPage_ * config_.HBlockInfo_.size_);
	}
	else if (config_.HBlockInfo_.type_ == config_.hbExtended)
	{
		PageSize += (config_.ObjectsPerPage_ * config_.HBlockInfo_.size_);
	}
	else if (config_.HBlockInfo_.type_ == config_.hbExternal)
	{
		PageSize += (config_.ObjectsPerPage_ * config_.HBlockInfo_.size_);
	}
	PageSize += (config_.ObjectsPerPage_ - 1) * config_.InterAlignSize_;

	OAStats_.FreeObjects_ += config_.ObjectsPerPage_;

	char* NewPage;
	NewPage = new char[PageSize];
	memset(NewPage, UNALLOCATED_PATTERN, PageSize);
	OAStats_.PagesInUse_++;

	temp3 = PageList_;
	PageList_ = reinterpret_cast<GenericObject*>(NewPage);
	temp4 = PageList_;
	temp4->Next = temp3;

	FreeList_ = reinterpret_cast<GenericObject*>(NewPage + sizeof(void*) + config_.PadBytes_ + config_.HBlockInfo_.size_);
	temp = FreeList_;
	temp->Next = nullptr;

	memset(reinterpret_cast<char*>(temp) - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
	memset(reinterpret_cast<char*>(temp) + OAStats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);
	//basic header
	memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - config_.HBlockInfo_.size_, nuse_flag, config_.HBlockInfo_.size_); //FreeList_ À§Ä¡ - header - pad  


	for (int i = 1; i < config_.ObjectsPerPage_; i++)
	{
		temp2 = reinterpret_cast<GenericObject*>(reinterpret_cast<char*>(temp) + config_.PadBytes_ + OAStats_.ObjectSize_ + config_.PadBytes_ + config_.HBlockInfo_.size_);
		temp2->Next = temp;
		FreeList_ = temp2;
		temp = temp2;

		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
		memset(reinterpret_cast<char*>(temp) + OAStats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);
		//basic header
		memset(reinterpret_cast<char*>(temp) - config_.PadBytes_ - config_.HBlockInfo_.size_, nuse_flag, config_.HBlockInfo_.size_); //FreeList_ À§Ä¡ - header - pad  

	}
	memset(reinterpret_cast<char*>(NewPage) + PageSize - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
}
