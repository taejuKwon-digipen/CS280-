#include "BSTree.h"

template<typename T>
BSTree<T>::BSTree(ObjectAllocator* oa, bool ShareOA)
{
	allocator = oa;
	OAConfig config(true);
	shareAllocator_ = ShareOA;

	if (allocator != nullptr) 
	{
		freeAllocator_ = false;
	}
	else if (allocator == nullptr)
	{
		freeAllocator_ = true;
		OAConfig config(true);
		allocator = new ObjectAllocator(sizeof(BinTreeNode), config);
	}
	
	node->data = 0;
	node->left = new ObjectAllocator(sizeof(BinTreeNode), config);;
	node->right = new ObjectAllocator(sizeof(BinTreeNode), config);;
	return node;
}

template<typename T>
BSTree<T>::BSTree(const BSTree& rhs)
{
	if (rhs.shareAllocator_)
	{
		allocator = rhs.objAllocator_; // Use rhs' allocator
		freeAllocator_ = false;            // We don't own it (won't free it)
		shareAllocator_ = true;            // If a copy of 'this object' is made, share the allocator
	}
	else // No sharing, create our own personal allocator
	{
		OAConfig config(true); // Set UseCPPMemManager to true, default the others
		allocator = new ObjectAllocator(sizeof(BinTreeNode), config);

		freeAllocator_ = true;   // We own the allocator, we will have to free it
		shareAllocator_ = false; // Do not share this allocator with any other list
	}

	node = reinterpret_cast<BinTreeNode*>(allocator->Allocate());
	node->left = reinterpret_cast<BinTreeNode*>(allocator->Allocate());
	node->right = reinterpret_cast<BinTreeNode*>(allocator->Allocate());
	node = rhs.node;
	node->left = rhs.node->left;
	node->right = rhs.node->right;
}

template<typename T>
BSTree<T>::~BSTree()
{
}

template<typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	OAConfig config(true);

	this->allocator = rhs.objAllocator_;
	allocator = new ObjectAllocator(sizeof(BinTreeNode), config);

	this->freeAllocator_ = rhs.freeAllocator_;
	this->shareAllocator_ = rhs.shareAllocator_;

	this->data = rhs.data;
	this->left = rhs.left;
	this->right = rhs.right;

	return *this;
}

template<typename T>
void BSTree<T>::insert(const T& value)
{
	if (node == nullptr)
	{
		node = reinterpret_cast<BinTreeNode*>(allocator->Allocate());
	}
	else if (value < node->data)
	{
		node->left->data = value;
	}
	else if (value > node->data)
	{
		node->right->data = value;
	}

	node->count++;
}

template<typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const
{
	/*BinTreeNode* curr = nullptr;

	curr = node;

	size_t i = 1;

	if (index % 2 == 1)
	{
		while (i != index)
		{
			if (curr->left != nullptr)
			{
				curr = curr->left;
			}
			i += 2;
		}
	}
	else {
		i++;
		while (i != index)
		{
			if (curr->right != nullptr)
			{
				curr = curr->right;
				i += 2;
			}
		}
	}

	return curr;*/
	return nullptr;

};

template<typename T>
void BSTree<T>::remove(const T& value)
{
	if (node == nullptr)
		return;
	else if (value < node->data)
		node->left = nullptr;
	else if (value > node->data)
		node->right = nullptr;
	else // (Data == tree->data)
	{
		if (node->left == nullptr)
		{
			BinTreeNode* temp = node;
			node = node->right;
		}
		else if (node->right == nullptr)
		{
			BinTreeNode* temp = node;
			node = node->left;
		}
		/*else
		{
			BinTreeNode* pred = nullptr;
			find(node, pred);
			node->data = pred->data;
			remove(node->left, node->data);
		}*/
	}
	node->count--;
}

template<typename T>
void BSTree<T>::clear()
{
	/*int i = 0;
	while (i < node->count)
	{
		node[i] = nullptr;
	}*/
}

template<typename T>
bool BSTree<T>::find(const T& value, unsigned& compares) const
{
	if (node == nullptr)
		return false;
	else if (value == node->data)
		return true;
	else if (value < node->data)
		return find(node->left->data, value);
	else
		return find(node->right->data, value);
}

template<typename T>
bool BSTree<T>::empty() const
{
	if (node == nullptr)
	{
		return true;
	}
	return false; 
}

template<typename T>
unsigned int BSTree<T>::size() const
{
	return sizeof(BinTreeNode*);
}

template<typename T>
int BSTree<T>::height() const
{
	if (node == nullptr)
	{
		return -1;
	}
	if (tree_height(node->left) > tree_height(node->right))
		return tree_height(node->left) + 1;
	else
		return tree_height(node->right) + 1;
}

template<typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
	return node;
}

template<typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
	if (tree == 0)
	{
		return -1;
	}

	int lh = tree_height(tree->left);
	int rh = tree_height(tree->right);

	if (lh > rh)
	{
		return lh + 1;
	}
	else {
		return rh + 1;
	}
}
