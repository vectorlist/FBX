#pragma once

#include <cstdlib>

template<class TYPE> 
class LinkedTree
{
public:

	LinkedTree::LinkedTree()
	{
		m_root = NULL; // First node at the root level
		m_end = NULL; // Last node at the root level
	};

	virtual LinkedTree::~LinkedTree()
	{
		for (TYPE *type = m_root; type != NULL; )
		{
			TYPE *temp = type;
			type = type->m_next;
			delete temp;
		}
	};


	virtual TYPE* GetRoot()
	{
		return m_root;
	}

	// Attach a child to this parent at this level in the hierarchy
	virtual void AddAsChild(
		TYPE *parent,
		TYPE *item
	)
	{
		if (parent) // Hierarchy list is not empty - add to the end and move the end to the new node
		{
			if (parent->m_firstChild)
			{
				parent->m_endChild->m_next = item;
				item->m_previous = parent->m_endChild;
				item->m_next = NULL;
				parent->m_endChild = parent->m_endChild->m_next;
			}
			else
			{
				parent->m_endChild = item;
				parent->m_firstChild = item;
			}

			item->m_parent = parent;
		}
		else // Add to the root level
		{
			if (m_root)
			{
				m_end->m_next = item;
				item->m_next = NULL;
				item->m_previous = m_end;
				m_end = m_end->m_next;

			}
			else
			{
				m_root = item;
				m_end = item;
			}

		}
	}

	virtual TYPE* GetNextChildFirst(
		TYPE *item
	)
	{
		if (item->m_firstChild)
		{
			return item->m_firstChild;
		}

		if (item->m_next)
		{
			return item->m_next;
		}

		for (TYPE *parent = item->m_parent; parent != NULL; )
		{
			if (parent->m_next)
			{
				return parent->m_next;
			}

			parent = parent->m_parent;
		}

		return NULL;
	}

	TYPE *m_root;
	TYPE *m_end;


};

template<class ITEM> 
class LinkedTreeItem
{
	friend class LinkedTree<ITEM>;
public:

	LinkedTreeItem::LinkedTreeItem()
	{
		m_next = NULL;
		m_previous = NULL;
		m_parent = NULL;
		m_firstChild = NULL;
		m_endChild = NULL;
	};

	virtual LinkedTreeItem::~LinkedTreeItem()
	{
		for (ITEM *item = m_firstChild; item != NULL; )
		{
			ITEM *temp = item;
			item = item->m_next;
			delete temp;
		}
	};

	ITEM *m_next;			// Next sibling item at this hierarchy level
	ITEM *m_previous;		// Previous sibling item at this hierarchy level
	ITEM *m_parent;			// Parent of these siblings
	ITEM *m_firstChild;		// First child of this item at this hierarchy level
	ITEM *m_endChild;		// End child of this item at this hierarchy level
};