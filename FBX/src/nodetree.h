#pragma once

#include <stdlib.h>

template<class T> 
class NodeTree
{
public:

	NodeTree::NodeTree()
		:
		mRoot(NULL),
		mEnd(NULL)
	{};

	virtual NodeTree::~NodeTree()
	{
		for (T *type = mRoot; type != NULL; )
		{
			T *temp = type;
			type = type->mNext;
			delete temp;
		}
	};


	virtual T* getRoot()
	{
		return mRoot;
	}

	virtual void addChild(
		T *parent,
		T *item
	)
	{
		if (parent)	
		{
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

			item->mParent = parent;
		}
		else 
		{
			if (mRoot)
			{
				mEnd->mNext = item;
				item->mNext = NULL;
				item->mPrevious = mEnd;
				mEnd = mEnd->mNext;

			}
			else
			{
				mRoot = item;
				mEnd = item;
			}

		}
	}

	virtual T* getNextChildFirst(
		T *item
	)
	{
		if (item->mFirstChild)
		{
			return item->mFirstChild;
		}

		if (item->mNext)
		{
			return item->mNext;
		}

		for (T *parent = item->mParent; parent != NULL; )
		{
			if (parent->mNext)
			{
				return parent->mNext;
			}

			parent = parent->mParent;
		}

		return NULL;
	}

	T *mRoot;
	T *mEnd;


};

template<class T>
class NodeTreeItem
{
	friend class NodeTree<T>;
public:
	NodeTreeItem::NodeTreeItem()
		:
	mNext(NULL),
	mPrevious(NULL),
	mParent(NULL),
	mFirstChild(NULL),
	mEndChild(NULL)
	{};

	virtual NodeTreeItem::~NodeTreeItem()
	{
		for (T *item = next(); item != NULL; )
		{
			T *temp = item;
			item = item->mNext;
			delete temp;
		}
	};

	virtual T* next()
	{
		return mNext;
	}

	virtual T* parent()
	{
		return mParent;
	}

	virtual T* child()
	{
		return mFirstChild;
	}

	T *mNext;
	T *mPrevious;		
	T *mParent;
	T *mFirstChild;		
	T *mEndChild;		
							
};
