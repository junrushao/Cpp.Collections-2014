/** @file */

#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H

#define _DEBUG
#ifdef _DEBUG
	#include "memwatch.h"
#endif

#include "IndexOutOfBound.h"
#include "ElementNotExist.h"
#include "Utility.h"

#define getNode() ((Node *)malloc(sizeof(Node)))

template<class T>
class LinkedList {
private:
	struct Node {
		T data;
		Node *prev, *next;

		Node() :
				data(), prev(this), next(this) {
		}

		Node(const T &data, Node* prev, Node *next) :
				data(data), prev(prev), next(next) {
		}
	};
	typedef Node *List;

	List header;
	int _size;

	void cloneTo(List &otherHeader, int &otherSize) const {
		List first = new (getNode()) Node(), last = first;
		for (List p = header->next; p != header; p = p->next)
			last = last->next = new (getNode()) Node(p->data, last, NULL);
		first->prev = last;
		last->next = first;
		otherHeader = first;
		otherSize = _size;
	}

public:
	class Iterator {
	public:

		LinkedList<T> *list;
		List p;
		
		Iterator(LinkedList<T> *list, List &p):
			list(list), p(p) {
		}
		
		bool hasNext() {
			return p->next != list->header;
		}
		
		const T& next() {
			if (p->next == list->header)
				throw ElementNotExist(toString(__LINE__));
			p = p->next;
			return p->data;
		}
		
		void remove() {
			if (p == list->header)
				throw ElementNotExist(toString(__LINE__));
			--list->_size;
			List left = p->prev, right = p->next;
			left->next = right;
			right->prev = left;
			p->~Node();
			free(p);
			p = left;
		}
	};

	LinkedList() :
			header(new (getNode()) Node()), _size(0) {
	}

	LinkedList(const LinkedList<T> &c): header(NULL), _size(0) {
		c.cloneTo(header, _size);
	}

	LinkedList<T>& operator =(const LinkedList<T> &c) {
		if (&c != this) {
			clear();
			header->~Node();
			free(header);
			cloneTo(header, _size);
		}
		return *this;
	}

	~LinkedList() {
		clear();
		header->~Node();
		free(header);
	}

	bool add(const T& e) {
		List left = header->prev, right = header, mid = new (getNode()) Node(e, left, right);
		left->next = right->prev = mid;
		++_size;
		return true;
	}

	void addFirst(const T& e) {
		List left = header, right = header->next, mid = new (getNode()) Node(e, left, right);
		left->next = right->prev = mid;
		++_size;
	}

	void addLast(const T &e) {
		List left = header->prev, right = header, mid = new (getNode()) Node(e, left, right);
		left->next = right->prev = mid;
		++_size;
	}

	void add(int idx, const T& element) {
		if (!(0 <= idx && idx <= _size))
			throw IndexOutOfBound(toString(__LINE__));
		List left = header;
		for (int i = 1; i <= idx; ++i)
			left = left->next;
		List right = left->next, mid = new (getNode()) Node(element, left, right);
		left->next = right->prev = mid;
		++_size;
	}

	void clear() {
		for (List p = header->next; p != header;) {
			List next = p->next;
			p->~Node();
			free(p);
			p = next;
		}
		header->prev = header->next = header;
	}

	bool contains(const T &e) const {
		for (List p = header->next; p != header; p = p->next)
			if (p->data == e)
				return true;
		return false;
	}

	const T& get(int idx) const {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound(toString(__LINE__));
		List p = header;
		for (int i = 1; i <= idx + 1; ++i)
			p = p->next;
		return p->data;
	}

	const T& getFirst() const {
		if (_size == 0)
			throw ElementNotExist(toString(__LINE__));
		return header->next->data;
	}

	const T& getLast() const {
		if (_size == 0)
			throw ElementNotExist(toString(__LINE__));
		return header->prev->data;
	}

	bool isEmpty() const {
		return _size == 0;
	}

	void removeIndex(int idx) {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound(toString(__LINE__));
		--_size;
		List p = header;
		for (int i = 1; i <= idx + 1; ++i)
			p = p->next;
		List left = p->prev, right = p->next;
		left->next = right;
		right->prev = left;
		p->~Node();
		free(p);
	}

	bool remove(const T &e) {
		for (List p = header->next; p != header; p = p->next)
			if (p->data == e) {
				--_size;
				List left = p->prev, right = p->next;
				left->next = right;
				right->prev = left;

				p->~Node();
				free(p);

				return true;
			}
		return false;
	}

	void removeFirst() {
		if (_size == 0)
			throw ElementNotExist(toString(__LINE__));
		List p = header->next, left = p->prev, right = p->next;
		left->next = right;
		right->prev = left;
		p->~Node();
		free(p);
	}

	void removeLast() {
		if (_size == 0)
			throw ElementNotExist(toString(__LINE__));
		List p = header->prev, left = p->prev, right = p->next;
		left->next = right;
		right->prev = left;
		p->~Node();
		free(p);
	}
	
	void set(int idx, const T &element) {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound(toString(__LINE__));
		List p = header;
		for (int i = 1; i <= idx + 1; ++i)
			p = p->next;
		p->data = element;
	}
	
	Iterator iterator() {
		return Iterator(this, header);
	}
	
	int size() const {
		return _size;
	}
};

#undef getNode
#endif /* __LINKEDLIST_H */
