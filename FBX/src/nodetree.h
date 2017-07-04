#pragma once

#include <stdlib.h>


template<class T>
class NodeTree
{
public:

	NodeTree::NodeTree()
	{
		mRoot = NULL;
		mEnd = NULL;
		count = 0;
	};

	virtual NodeTree::~NodeTree()
	{
		for (T *type = mRoot; type != NULL; )
		{
			T *temp = type;
			type = type->mNext;
			delete temp;
		}
		count = 0;
	};


	virtual T* GetRoot()
	{
		return mRoot;
	}

	virtual void AddChild(
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
		item->mId = count;
		++count;
	}

	virtual T* GetNextChildFirst(
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
	
	int Size()
	{
		return count + 1;
	}
private:
	int count;

};

template<class T>
class NodeTreeItem
{
	friend class NodeTree<T>;
public:
	NodeTreeItem::NodeTreeItem()
	{
		mNext = NULL;
		mPrevious = NULL;
		mParent = NULL;
		mFirstChild = NULL;
		mEndChild = NULL;
	};

	virtual NodeTreeItem::~NodeTreeItem()
	{
		for (T *item = mFirstChild; item != NULL; )
		{
			T *temp = item;
			item = item->mNext;
			delete temp;
		}
	};

	int GetId()
	{
		return mId;
	}

	T *mNext;
	T *mPrevious;
	T *mParent;
	T *mFirstChild;
	T *mEndChild;
private:
	int mId;
};
