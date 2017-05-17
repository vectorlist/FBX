#pragma once

#include <cstdlib>

template<class T>
class NodeTree
{
public:
	NodeTree();
	~NodeTree();

	//get root node (level 0)
	virtual T* getRoot();

	//get child
	virtual void addChild(T* parent, T* item);
	virtual T* getNextChildFirst(T* item);

	bool isRoot = false;
private:

	T* mRoot;
	T* mEnd;
};

template<class T>
inline NodeTree<T>::NodeTree()
	: mRoot(NULL), mEnd(NULL)
{

}

template<class T>
inline NodeTree<T>::~NodeTree()
{
	//completed
	for (T* item = mRoot; item != NULL;)
	{
		//get current for delete
		T* tempItem = item;
		//get next item
		item = item->mNext;
		delete tempItem;
	}
}

template<class T>
inline T* NodeTree<T>::getRoot()
{
	return mRoot;
}

template<class T>
inline void NodeTree<T>::addChild(T *parent, T *item)
{
	//parent is not empty
	if (parent)
	{
		//add to the end and move the end to the new node
		if (parent->mFirstChild)
		{
			parent->mEndChild->mNext = item;
			item->mPrevious = parent->mEndChild;
			item->mNext = NULL;
			parent->mEndChild = parent->mEndChild->mNext;
		}
		else
		{
			parent->mEndChild = item;
			parent->mFirstChild = item;
		}
	}
	//if none parent (add root level 0)
	else
	{
		//if has root node
		if (mRoot)
		{
			mEnd->mNext = item;
			item->mNext = NULL;
			item->mPrevious = mEnd;
			mEnd = mEnd->mNext;
		}
		else
		{
			//item node will root and end node
			mRoot = item;
			mEnd = item;
		}
	}
}

//get next of fisrt child
template <class T>
inline T* NodeTree<T>::getNextChildFirst(T* item)
{
	if (item->mFirstChild) {
		return item->mFirstChild;
	}
	if (item->mNext)
	{
		return item->mNext;
	}
	//serch parent
	for (T* parent = item->mParent; parent != NULL; )
	{
		if (parent->mNext)
		{
			return parent->mNext;
		}
		parent = parent->mParent;
	}

	//no founded
	return NULL;
}




//ITEM NODE TREE
template <class T>
class NodeTreeItem
{
public:
	NodeTreeItem();
	~NodeTreeItem();


	//frend class for using this item member and functions
	friend class NodeTree<NodeTreeItem>;

	T* mNext;
	T* mPrevious;
	T* mParent;
	T* mFirstChild;
	T* mEndChild;

private:

};

template<class T>
inline NodeTreeItem<T>::NodeTreeItem()
	: 
	mNext(NULL),
	mPrevious(NULL),
	mParent(NULL),
	mFirstChild(NULL),
	mEndChild(NULL)
{
}

template<class T>
inline NodeTreeItem<T>::~NodeTreeItem()
{
	//TODO : delete all item
	for (T* node = mFirstChild; node != NULL;)
	{
		T* tempNode = node;
		node = node->mNext;
		delete tempNode;
	}
}
