/** @file */
#ifndef __PRIORITYQUEUE_H
#define __PRIORITYQUEUE_H

#define _DEBUG
#ifdef _DEBUG
	#include "memwatch.h"
#endif

#include <cassert>
#include "Utility.h"
#include "ArrayList.h"
#include "ElementNotExist.h"

template <class V>
class Less {
public:
	bool operator() (const V& a, const V& b) {
		return a < b;
	}
};

#define getArray(size) ((Vp*)malloc(sizeof(Vp) * size))
#define getNode() ((Vp)malloc(sizeof(V)))

template<class V, class C = Less<V> >
class PriorityQueue {
private:
	typedef V *Vp;

	C compare;

	Vp *queue;
	int capacity;
	int _size;

	void cloneTo(Vp *&otherQueue, int &otherCapacity, int &otherSize) const {
		otherQueue = getArray(capacity);
		otherCapacity = capacity;
		otherSize = _size;
		for (int i = 0; i < _size; ++i) otherQueue[i] = new (getNode()) V(*queue[i]);
		for (int i = _size; i < capacity; ++i) otherQueue[i] = NULL;
	}

	void grow(int minCapacity) {
		if (minCapacity > capacity) {
			capacity <<= 1;
			if (minCapacity > capacity) capacity = minCapacity;

			Vp *newQueue = getArray(capacity);
			for (int i = 0; i < _size; ++i)
				newQueue[i] = queue[i];
			for (int i = _size; i < capacity; ++i) newQueue[i] = NULL;
			if (queue) free(queue);
			queue = newQueue;
		}
	}

	void siftUp(int k, Vp x) {
		while (k > 0) {
			int parent = (k - 1) >> 1;
			Vp e = queue[parent];
			if (!compare(*x, *e)) break; // x >= e
            queue[k] = e;
            k = parent;
        }
        queue[k] = x;
	}

	void siftDown(int k, Vp x) {
		int half = _size >> 1;
		while (k < half) {
			int child = (k << 1) + 1;
			Vp c = queue[child];
			int right = child + 1;
			if (right < _size && compare(*queue[right], *c)) // queue[right] < queue[left]
				c = queue[child = right];
			if (!compare(*c, *x)) // x <= c
				break;
			queue[k] = c;
			k = child;
		}
		queue[k] = x;
	}

	int removeAt(int i, int limit) { // return where queue[_size - 1] has gone
		assert(0 <= i && i < _size);
		assert(i < limit);
		int s = --_size;
		if (s == i) {
			queue[s]->~V();
			free(queue[s]);
			queue[s] = NULL;
			return -1;
		}
		Vp x = queue[s];
		queue[s] = NULL;
		queue[i]->~V();
		free(queue[i]);
		queue[i] = NULL;
		
		int moveInside = i, k = i;
		{ // siftDown
			for (int child; (child = (k << 1) + 1) < _size; k = child) {
				Vp c = queue[child];
				if (child + 1 < _size && compare(*queue[child + 1], *c)) // queue[right] < queue[left]
					c = queue[++child];
				if (!compare(*c, *x)) // x <= c
					break;
				queue[k] = c;
				if (k < limit && child < limit) {
					moveInside = child;
				}
				else if (k < limit && child >= limit) {
					moveInside = k;
				}
			}
			queue[k] = x;
			if (k != i)
				return moveInside;
		}
		assert(k == i && moveInside == i && queue[i] == x);
		for (int parent; k > 0; k = parent) {
			Vp e = queue[parent = (k - 1) >> 1];
			if (!compare(*x, *e)) break; // x >= e
            queue[k] = e;
        }
        queue[k] = x;
		return k;
	}
	
public:

	class Iterator {
	public:
		PriorityQueue<V, C> *pq;
		int lastPos, nextPos, extraPos;

		Iterator(PriorityQueue<V, C> *pq): pq(pq), lastPos(-1), nextPos(0), extraPos(-1) {
		}

		Iterator(): pq(NULL), lastPos(-1), nextPos(-1), extraPos(-1) {
		}

		bool hasNext() {
			return pq != NULL && ((0 <= nextPos && nextPos < pq->_size) || extraPos != -1);
		}

		const V &next() {
			if (!hasNext())
				throw ElementNotExist(toString(__LINE__));
			Vp ret = NULL;
			if (extraPos != -1) {
				ret = pq->queue[extraPos];
				lastPos = extraPos;
				extraPos = -1;
			}
			else {
				ret = pq->queue[nextPos];
				lastPos = nextPos;
				++nextPos;
			}
			return *ret;
		}

		void remove() {
			if (!pq || lastPos == -1)
				throw ElementNotExist(toString(__LINE__));
			int pos = pq->removeAt(lastPos, nextPos);
			if (pos < nextPos) extraPos = pos;
			lastPos = -1;
		}
	};

	PriorityQueue(): queue(NULL), capacity(0), _size(0) {
	}

	~PriorityQueue() {
		clear();
	}
	
	PriorityQueue<V, C> &operator = (const PriorityQueue<V, C> &x) {
		if (this != &x) {
			clear();
			x.cloneTo(queue, capacity, _size);
		}
		return *this;
	}

	PriorityQueue(const PriorityQueue<V, C> &x): queue(NULL), capacity(0), _size(0) {
		x.cloneTo(queue, capacity, _size);
	}

	PriorityQueue(const ArrayList<V> &x): queue(NULL), capacity(0), _size(x.size()) {
		if (_size > 0) {
			for (capacity = 11; capacity < _size; capacity <<= 1);
			queue = getArray(capacity);
			for (int i = 0; i < _size; ++i) queue[i] = new (getNode()) V(x.get(i));
			for (int i = (_size >> 1) - 1; i >= 0; --i)
				siftDown(i, queue[i]);
		}
	}

	Iterator iterator() {
		return Iterator(this);
	}

	void clear() {
		for (int i = 0; i < _size; ++i) {
			queue[i]->~V();
			free(queue[i]);
			queue[i] = NULL;
		}
		if (queue) free(queue);
		queue = NULL;
		capacity = 0;
		_size = 0;
	}

	const V &front() const {
		if (_size == 0)
			throw ElementNotExist(toString(__LINE__));
		return *queue[0];
	}
	
	bool empty() const {
		return _size == 0;
	}

	void push(const V &value) {
		if (_size >= capacity) grow(_size + 1);
		++_size;
		if (_size == 1) queue[0] = new (getNode()) V(value);
		else siftUp(_size - 1, new (getNode()) V(value));
	}

	void pop() {
		if (_size == 0)
			throw ElementNotExist(toString(__LINE__));
		int s = --_size;
		Vp toRemove = queue[0];
		Vp x = queue[s];
		queue[s] = NULL;
		if (s != 0)
			siftDown(0, x);
		toRemove->~V();
		free(toRemove);
	}

	int size() const {
		return _size;
	}
};

#undef getArray
#undef getNode

#endif
