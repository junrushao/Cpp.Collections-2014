/** @file */
#ifndef __DEQUE_H
#define __DEQUE_H

#include <cassert>

#define _DEBUG
#ifdef _DEBUG
	#include "memwatch.h"
#endif

#include "ElementNotExist.h"
#include "IndexOutOfBound.h"
#include "Utility.h"

#define getNode() ((Tp)malloc(sizeof(T)))
#define getArray(size) ((Tp*) malloc(sizeof(T) * size))

template <class T>
class Deque {
private:
	typedef T *Tp;

	int head, tail; // [head, tail)
	int capacity;
	Tp *queue;

	void doubleCapacity() {
		assert(tail - head == capacity);
		int newCapacity = (capacity == 0) ? (16) : (capacity << 1);
		Tp *newQueue = getArray(newCapacity);
		for (int i = 0; i < newCapacity; ++i)
			newQueue[i] = NULL;
		for (int i = head, pos = 0; i < tail; ++i)
			newQueue[pos++] = queue[i & (capacity - 1)];
		if (queue != NULL)
			free(queue);
		tail -= head;
		head = 0;
		capacity = newCapacity;
		queue = newQueue;
	}

	void cloneTo(int &otherHead, int &otherTail, int &otherCapacity, Tp *&otherQueue) const {
		otherHead = 0;
		otherTail = 0;
		otherCapacity = capacity;
		otherQueue = getArray(otherCapacity);
		for (int i = 0; i < otherCapacity; ++i)
			otherQueue[i] = NULL;
		for (int i = head; i < tail; ++i)
			otherQueue[otherTail++] = queue[i & (otherCapacity - 1)];
	}

	void removeIdx(int index) {
		int pos = head + index;
		if (!(head <= pos && pos < tail))
			throw IndexOutOfBound(toString(__LINE__));
		int p = pos & (capacity - 1);
		queue[p]->~T();
		free(queue[p]);
		queue[p] = NULL;
		for (int i = pos + 1; i < tail; ++i)
			queue[(i - 1) & (capacity - 1)] = queue[i & (capacity - 1)];
		--tail;
	}

public:

	class Iterator {
	private:
		const bool dir;
		Deque<T> *from;
		int lastPos;
	public:
		
		Iterator(const bool &dir, Deque<T> *from): dir(dir), from(from) {
			if (dir == false)
				lastPos = -1;
			else
				lastPos = from->size();
		}

		bool hasNext() {
			int next = lastPos + (dir ? -1 : 1);
			return 0 <= next && next < from->size();
		}

		const T &next() {
			int next = lastPos + (dir ? -1 : 1);
			if (!( 0 <= next && next < from->size() ))
				throw ElementNotExist(toString(__LINE__));
			return from->get(next);
		}
		
		void remove() {
			if (!(0 <= lastPos && lastPos < from->size()))
				throw ElementNotExist(toString(__LINE__));
			from->removeIdx(lastPos);
			lastPos -= (dir ? -1 : 1);
		}
	};

	Deque(): head(0), tail(0), capacity(0), queue(NULL) {
	}

	~Deque() {
		clear();
	}

	Deque& operator = (const Deque& x) {
		if (this != &x) {
			clear();
			x.cloneTo(head, tail, capacity, queue);
		}
		return *this;
	}

	Deque(const Deque& x): head(0), tail(0), capacity(0), queue(NULL) {
		x.cloneTo(head, tail, capacity, queue);
	}

	void addFirst(const T& e) {
		if (tail - head == capacity)
			doubleCapacity();
		queue[--head & (capacity - 1)] = new (getNode()) T(e);
	}

	void addLast(const T& e) {
		if (tail - head == capacity)
			doubleCapacity();
		queue[tail++ & (capacity - 1)] = new (getNode()) T(e);
	}

	bool contains(const T& e) const {
		for (int i = head; i < tail; ++i)
			if (*queue[i & (capacity - 1)] == e)
				return true;
		return false;
	}

	void clear() {
		for (int i = head; i < tail; ++i) {
			queue[i]->~T();
			free(queue[i]);
		}
		if (queue)
			free(queue);
		head = tail = 0;
		capacity = 0;
		queue = NULL;
	}

	bool isEmpty() const {
		return head == tail;
	}

	const T& getFirst() {
		if (head == tail)
			throw ElementNotExist(toString(__LINE__));
		return *queue[head & (capacity - 1)];
	}

	const T& getLast() {
		if (head == tail)
			throw ElementNotExist(toString(__LINE__));
		return *queue[(tail - 1) & (capacity - 1)];
	}

	void removeFirst() {
		if (head == tail)
			throw ElementNotExist(toString(__LINE__));
		++head;
	}

	void removeLast() {
		if (head == tail)
			throw ElementNotExist(toString(__LINE__));
		--tail;
	}

	const T& get(int index) {
		int pos = head + index;
		if (!(head <= pos && pos < tail))
			throw IndexOutOfBound(toString(__LINE__));
		pos &= capacity - 1;
		return *queue[pos];
	}

	void set(int index, const T& e) {
		int pos = head + index;
		if (!(head <= pos && pos < tail))
			throw IndexOutOfBound(toString(__LINE__));
		pos &= capacity - 1;
		queue[pos]->~T();
		free(queue[pos]);
		queue[pos] = new (getNode) T(e);
	}

	int size() const {
		return tail - head;
	}

	Iterator iterator() {
		return Iterator(false, this);
	}

	Iterator descendingIterator() {
		return Iterator(true, this);
	}
};

#undef getNode
#undef getArray

#endif
