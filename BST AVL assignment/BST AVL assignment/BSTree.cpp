#include "BSTree.h"
template<typename T>
BSTree<T>::BSTree(ObjectAllocator* oa, bool ShareOA)
{
	OAConfig config(true);
	shareAllocator_ = ShareOA;
	//count_ = node->count;

	if (allocator != nullptr)
	{
		allocator = oa;
		freeAllocator_ = false;
	}
	else if (allocator == nullptr)
	{
		freeAllocator_ = true;
		OAConfig config(true);
		allocator = new ObjectAllocator(sizeof(BinTreeNode), config);
	}

}

template<typename T>
BSTree<T>::BSTree(const BSTree& rhs)
{
	if (rhs.shareAllocator_)
	{
		allocator = rhs.allocator; // Use rhs' allocator
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

	this->allocator = rhs.allocator;
	allocator = new ObjectAllocator(sizeof(BinTreeNode), config);

	this->freeAllocator_ = rhs.freeAllocator_;
	this->shareAllocator_ = rhs.shareAllocator_;

	this->node = rhs.node;

	return *this;
}

template<typename T>
void BSTree<T>::insert(const T& value)
{
	InsertItem(node, value);
	//node->count++;
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

}

template<typename T>
void BSTree<T>::remove(const T& value)
{
	DeleteItem(node, value);
	//node->count--;
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
	ItemExists(node, value, compares);
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
	{
		return tree_height(node->left) + 1;
	}
	else
	{
		return tree_height(node->right) + 1;
	}
}

template<typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
	return node;
}

template<typename T>
bool BSTree<T>::ImplementedIndexing()
{
	return false;
}

template<typename T>
typename BSTree<T>::BinTree& BSTree<T>::get_root()
{
	return node;
}

template<typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T& value) const
{
	BinTree tree = nullptr;
	BinTree node;

	tree = reinterpret_cast<BinTree>(allocator->Allocate());
	node = new (tree) BinTreeNode(value);
	node->data = value;
	node->left = 0;
	node->right = 0;
	return node;
}

template<typename T>
void BSTree<T>::free_node(BinTree node)
{
	delete node;
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

template<typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree& predecessor) const
{
	predecessor = tree->left;
	while (predecessor->right != 0)
	{
		predecessor = predecessor->right;
	}
}

template<typename T>
bool BSTree<T>::ItemExists(BinTree tree, const T& value, unsigned& compares) const
{
	if (tree == 0)
	{
		compares++;
		return false;
	}
	else if (value == tree->data)
	{
		compares++;
		return true;
	}
	else if (value < tree->data)
	{
		compares++;
		return ItemExists(tree->left, value, compares);
	}
	else
	{
		compares++;
		return ItemExists(tree->right, value, compares);
	}
}

template<typename T>
void BSTree<T>::InsertItem(BinTree tree, const T& value)
{
	if (tree == 0)
		tree = make_node(value);
	else if (value < tree->data)
		InsertItem(tree->left, value);
	else if (value > tree->data)
		InsertItem(tree->right, value);
}

template<typename T>
void BSTree<T>::DeleteItem(BinTree tree, const T& value)
{
	if (tree == 0)
		return;
	else if (value < tree->data)
		DeleteItem(tree->left, value);
	else if (value > tree->data)
		DeleteItem(tree->right, value);
	else // (Data == tree->data)
	{
		if (tree->left == 0)
		{
			BinTree temp = tree;
			tree = tree->right;
			free_node(temp);
		}
		else if (tree->right == 0)
		{
			BinTree temp = tree;
			tree = tree->left;
			free_node(temp);
		}
		else
		{
			BinTree pred = 0;
			find_predecessor(tree, pred);
			tree->data = pred->data;
			DeleteItem(tree->left, tree->data);
		}
	}
}
