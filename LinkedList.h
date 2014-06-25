/** @file */

#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H

#include "IndexOutOfBound.h"
#include "ElementNotExist.h"

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
		List first = new Node(), last = first;
		for (List p = header->next; p != header; p = p->next)
			last = last->next = new Node(p->data, last, NULL);
		first->prev = last;
		last->next = first;
		otherHeader = first;
		otherSize = _size;
	}

public:
	class Iterator {
	public:

		LinkedList<T> *list;
		List lastPos, nextPos;

		Iterator(): list(NULL), lastPos(NULL), nextPos(NULL) {
		}

		Iterator(LinkedList<T> *list):
			list(list), lastPos(list->header), nextPos(lastPos->next) {
		}

		bool hasNext() const {
			return list != NULL && nextPos != list->header;
		}
		
		const T& next() {
			if (!hasNext())
				throw ElementNotExist("");
			lastPos = nextPos;
			nextPos = nextPos->next;
			return lastPos->data;
		}
		
		void remove() {
			if (lastPos == list->header)
				throw ElementNotExist("");
			--list->_size;
			List left = lastPos->prev, right = lastPos->next;
			left->next = right;
			right->prev = left;
			delete lastPos;
			lastPos = list->header;
		}
	};

	LinkedList() :
			header(new Node()), _size(0) {
	}

	LinkedList(const LinkedList<T> &c): header(NULL), _size(0) {
		c.cloneTo(header, _size);
	}

	LinkedList<T>& operator =(const LinkedList<T> &c) {
		if (&c != this) {
			clear();
			delete header;
			c.cloneTo(header, _size);
		}
		return *this;
	}

	~LinkedList() {
		clear();
		delete header;
	}

	bool add(const T& e) {
		List left = header->prev, right = header, mid = new Node(e, left, right);
		left->next = right->prev = mid;
		++_size;
		return true;
	}

	void addFirst(const T& e) {
		List left = header, right = header->next, mid = new Node(e, left, right);
		left->next = right->prev = mid;
		++_size;
	}

	void addLast(const T &e) {
		List left = header->prev, right = header, mid = new Node(e, left, right);
		left->next = right->prev = mid;
		++_size;
	}

	void add(int idx, const T& element) {
		if (!(0 <= idx && idx <= _size))
			throw IndexOutOfBound("");
		List left = header;
		for (int i = 1; i <= idx; ++i)
			left = left->next;
		List right = left->next, mid = new Node(element, left, right);
		left->next = right->prev = mid;
		++_size;
	}

	void clear() {
		for (List p = header->next; p != header;) {
			List next = p->next;
			delete p;
			p = next;
		}
		header->prev = header->next = header;
		_size = 0;
	}

	bool contains(const T &e) const {
		for (List p = header->next; p != header; p = p->next)
			if (p->data == e)
				return true;
		return false;
	}

	const T& get(int idx) const {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound("");
		List p = header;
		for (int i = 1; i <= idx + 1; ++i)
			p = p->next;
		return p->data;
	}

	const T& getFirst() const {
		if (_size == 0)
			throw ElementNotExist("");
		return header->next->data;
	}

	const T& getLast() const {
		if (_size == 0)
			throw ElementNotExist("");
		return header->prev->data;
	}

	bool isEmpty() const {
		return _size == 0;
	}

	void removeIndex(int idx) {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound("");
		--_size;
		List p = header;
		for (int i = 1; i <= idx + 1; ++i)
			p = p->next;
		List left = p->prev, right = p->next;
		left->next = right;
		right->prev = left;
		delete p;
	}

	bool remove(const T &e) {
		for (List p = header->next; p != header; p = p->next)
			if (p->data == e) {
				--_size;
				List left = p->prev, right = p->next;
				left->next = right;
				right->prev = left;

				delete p;

				return true;
			}
		return false;
	}

	void removeFirst() {
		if (_size == 0)
			throw ElementNotExist("");
		--_size;
		List p = header->next, left = p->prev, right = p->next;
		left->next = right;
		right->prev = left;
		delete p;
	}

	void removeLast() {
		if (_size == 0)
			throw ElementNotExist("");
		--_size;
		List p = header->prev, left = p->prev, right = p->next;
		left->next = right;
		right->prev = left;
		delete p;
	}
	
	void set(int idx, const T &element) {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound("");
		List p = header;
		for (int i = 1; i <= idx + 1; ++i)
			p = p->next;
		p->data = element;
	}
	
	int size() const {
		return _size;
	}
	
	Iterator iterator() const {
		return Iterator(const_cast<LinkedList<T>*>(this));
	}
};

#endif /* __LINKEDLIST_H */
