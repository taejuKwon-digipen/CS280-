
template<typename T>
AVLTree<T>::AVLTree(ObjectAllocator* oa, bool ShareOA) : BSTree<T>(oa, ShareOA){}

template<typename T>
void AVLTree<T>::insert(const T& value)
{
	std::stack<BinTreeN*> node;
	//(tree, value, node)

	if (RootNode == 0)
	{
		RootNode = BSTree<T>::make_node(value);

		while (!node.empty())
		{
			BinTreeN& y = *node.top();
			node.pop();

			unsigned ylheight = BSTree<T>::tree_height(y->left);
			unsigned yrheight = BSTree<T>::tree_height(y->right);

			if (ylheight + 1 < yrheight)
			{
				BinTreeN& u = y->right;
				unsigned ulheight = BSTree<T>::tree_height(u->left);
				unsigned urheifht = BSTree<T>::tree_height(u->right);

				if (ulheight > yrheight)
				{
					//rotateRight(u)
					//rotateLeft(y);
				}
				else {
					//rotateLeft(y)
				}
			}
			else if (ylheight > yrheight + 1)
			{
				BinTreeN& u = y->left;
				unsigned ulheight = BSTree<T>::tree_height(u->left);
				unsigned urheight = BSTree<T>::tree_height(u->right);

				if (ulheight > urheight)
				{
					//rotateRight(y)
				}
				else
				{
					//rotateLeft(u)
					//rotateRight(y)
				}
			}
		}
	}
}

template<typename T>
void AVLTree<T>::remove(const T& value)
{
	
}
