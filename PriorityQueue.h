/** @file */
#ifndef __PRIORITYQUEUE_H
#define __PRIORITYQUEUE_H

#define _DEBUG
#ifdef _DEBIG
	#include "memwatch.h"
#endif

#include <cassert>
#include "Utility.h"
#include "ArrayList.h"
#include "ElementNotExist.h"

template<class V>
class Less {
public:
	bool operator()(const V& a, const V& b) {
		return a < b;
	}
};

#define getNode()			( (Node *) 	malloc	(sizeof(Node))			)
#define getValue()			( (V *) 	malloc	(sizeof(V))				)
#define getTreeArray(size)	( (Tree *)	malloc	(sizeof(Tree) * size)	)

template<class V, class C = Less<V> >
class PriorityQueue {
private:

	typedef V *Vp;

	class Node {
	public:
		Vp key;
		int dist;
		Node *lc, *rc, *fa;
		bool del;

		Node(): key(NULL), dist(-1), lc(this), rc(this), fa(this), del(false) {
		}

		Node(const V &key, const Node *&null): key(new (getValue()) V(key)), dist(0), lc(null), rc(null), fa(null), del(false) {
		}

		~Node() {
			if (key) {
				free(key);
				key = NULL;
			}
		}
	};

	typedef Node *Tree;

	int _size;
	Tree null, root;
	C compare;

	Tree* copyToTreeArray(int &otherSize, const Tree &otherNull) {
		if (root == null) return NULL;

		int head = 0, tail = 0;
		Tree *p = getTreeArray(_size);
		for (p[tail++] = root; head < tail; ) {
			Tree x = p[head++];
			if (x->lc != null)
				p[tail++] = x->lc;
			if (x->rc != null)
				p[tail++] = x->rc;
		}
		otherSize = 0;
		for (int i = 0; i < tail; ++i)
			if (!p[i]->del) {
				p[otherSize++] = new (getNode()) Node(*p[i]->key, otherNull);
			}
		if (otherSize == 0) {
			if (p) free(p);
			return NULL;
		}
		return p;
	}

	Tree* convertToTreeArray(const ArrayList<V> &x, int &otherSize) {
		otherSize = x.size();
		if (otherSize == 0)
			return NULL;
		Tree *p = getTreeArray(otherSize);
		for (int i = 0; i < otherSize; ++i)
			p[i] = new (getNode()) Node(x.get(i), null);
		return p;
	}

	Tree buildFromTreeArray(Tree *p, const int &size) { // [0, size)
		if (p == NULL) return null;

		int head = 0, tail = size; // [head, tail)
		while (tail - head > 1) {
			Tree x = p[head++ % size];
			Tree y = p[head++ % size];
			p[tail++ % size] = merge(x, y, null);
		}
		Tree ret = p[head % size];
		if (p) free(p);
		return ret;
	}
	
	void deleteTree(Tree t) {
		if (t == null) return;
		deleteTree(t->lc);
		deleteTree(t->rc);
		t->~Node();
		free(t);
	}

	Tree merge(Tree a, Tree b, Tree fa) {
		Tree ret = NULL;
		if (a == null) ret = b;
		else if (b == null) ret = a;
		else {
			if (compare(*b->key, *a->key)) // b->key < a->key
				swap(a, b);
			a->rc = merge(a->rc, b, a);
			if (a->lc->dist < a->rc->dist)
				swap(a->lc, a->rc);
			a->dist = a->rc->dist + 1;
			ret = a;
		}
		if (ret != null)
			ret->fa = fa;
		return ret;
	}

	void deleteNode(Tree t) {
		Tree y = t->fa, x = merge(t->lc, t->rc, y);
		assert(x->fa == y);
		if (y == null) {
			root = x;
			return;
		}
		(y->lc == t ? y->lc : y->rc) = x;
		for ( ; y != null; x = y, y = y->fa) {
			if (y->lc->dist < y->rc->dist)
				swap(y->lc, y->rc);
			if (y->rc->dist + 1 == y->dist)
				break;
			y->dist = y->rc->dist + 1;
		}
	}

public:
	class Iterator {
	private:
		PriorityQueue<V, C> *from;
		Tree null, last, toVisit;
		ArrayList<Tree> forgetMeNot;
		
		Tree getNext(Tree p) {
			if (p == null) return from->root;
			if (p->lc != null) return p->lc;
			if (p->rc != null) return p->rc;
			for (Tree last; ; ) {
				last = p;
				p = p->fa;
				if (p == null) return null;
				if (p->lc == last && p->rc != null) {
					p = p->rc;
					break;
				}
			}
			return p;
		}

	public:

		Iterator(): from(NULL), null(NULL), last(NULL), toVisit(NULL) {
		}

		Iterator(PriorityQueue<V, C> *from): from(from), null(from->null), last(null), toVisit(from->root) {
		}

		bool hasNext() {
			return from != NULL && toVisit != null;
		}

		const V &next() {
			if (!hasNext())
				throw ElementNotExist(toString(__LINE__));
			last = toVisit;
			toVisit = getNext(toVisit);
			return *last->key;
		}

		void remove() {
			if (last == null)
				throw ElementNotExist(toString(__LINE__));
			if (last == from->root) {
				from->pop();
				last = null;
				toVisit = from->root;
			}
			else {
				last->del = true;
				forgetMeNot.add(last);
			}
		}
		
		~Iterator() {
			int tmpSize = forgetMeNot.size();
			for (int i = 0; i < tmpSize; ++i) {
				Tree t = forgetMeNot.get(i);
				from->deleteNode(t);
				t->~Node();
				free(t);
			}
		}
	};

	PriorityQueue(): _size(0), null(new (getNode()) Node()), root(null), compare() {
	}

	~PriorityQueue() {
		clear();
		null->~Node();
		free(null);
	}

	PriorityQueue<V, C>& operator=(const PriorityQueue<V, C> &x) {
		if (this != &x) {
			clear();
			Tree *p = x.copyToTreeArray(_size, null);
			root = buildFromTreeArray(p, _size);
		}
		return *this;
	}

	PriorityQueue(const PriorityQueue<V, C> &x): _size(0), null(new (getNode()) Node()), root(null), compare() {
		Tree *p = x.copyToTreeArray(_size, null);
		root = buildFromTreeArray(p, _size);
	}

	PriorityQueue(const ArrayList<V> &x): _size(0), null(new (getNode()) Node()), root(null), compare()  {
		Tree *p = convertToTreeArray(x, _size);
		root = buildFromTreeArray(p, _size);
	}

	Iterator iterator() {
		return Iterator(this);
	}

	void clear() {
		deleteTree(root);
		_size = 0;
		root = null;
	}

	const V &front() const {
		if (root == null)
			throw ElementNotExist(toString(__LINE__));
		return *root->key;
	}
	
	bool empty() const {
		return _size == 0;
	}

	void push(const V &value) {
		Tree newNode = new (getNode()) Node(value);
		root = merge(root, newNode, null);
	}

	void pop() {
		Tree tmp = root;
		root = merge(root->lc, root->rc, null);
		
		tmp->~Node();
		free(tmp);
	}

	int size() const {
		return _size;
	}
};

#undef getTreeArray
#undef getNode

#endif
