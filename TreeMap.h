/** @file */

#ifndef __TREEMAP_H
#define __TREEMAP_H

#define _DEBUG
#ifdef _DEBUG
	#include "memwatch.h"
#endif

#include <ctime>
#include "ElementNotExist.h"
#include "Utility.h"

#define getNode()		(	(Node *) malloc	(sizeof(Node))	)
#define getType(type)	(	(type *) malloc	(sizeof(type))	)

template <class K, class V>
class TreeMap {
private:
	typedef K* Kp;
	typedef V* Vp;

	unsigned seed;
	unsigned nextUnsigned() {
		return seed = (unsigned)(seed * 48271LL % 2147483647);
	}

public:

	class Entry {
		friend TreeMap;
	private:
		Kp key;
		Vp value;
	public:
		Entry(const Kp &key, const Vp &value): key(key), value(value) {
		}
		Entry(const Entry &other): key(other.key), value(other.value) {
		}
		const K& getKey() const {
			return *key;
		}
		const V& getValue() const {
			return *value;
		}
	};

private:
	class Node {
	public:
		Kp key;
		Vp value;
		unsigned prio;
		Node *ch[2], *pre;
		Node(): key(NULL), value(NULL), prio(2147483647U), pre(this) {
			ch[0] = ch[1] = this;
		}
		Node(const K &key, const V &value, int prio, Node *null):
			key(new (getType(K)) K(key)), value(new (getType(V)) V(value)), prio(prio), pre(null) {
			ch[0] = ch[1] = null;
		}
		~Node() {
			if (key) {
				key->~K();
				free(key);
			}
			if (value) {
				value->~V();
				free(value);
			}
		}
	};

private:
	typedef Node *Tree;

	int _size;
	Tree null, root;

	void deleteTree(Tree t) {
		if (t == null) return;
		deleteTree(t->ch[0]);
		deleteTree(t->ch[1]);
		t->~Node();
		free(t);
	}

	Tree cloneTree(Tree t, Tree newNull) const {
		if (t == null) return newNull;
		Tree ret = new (getNode()) Node(*t->key, *t->value, t->prio, newNull);
		Tree lc = ret->ch[0] = cloneTree(t->ch[0], newNull);
		Tree rc = ret->ch[1] = cloneTree(t->ch[1], newNull);
		if (lc != newNull) lc->pre = ret;
		if (rc != newNull) rc->pre = ret;
		return ret;
	}

	void cloneTo(Tree &newRoot, Tree &newNull) const {
		newNull = new (getNode()) Node();
		newRoot = cloneTree(root, newNull);
		newRoot->pre = newNull;
	}

	void rotate(Tree &x, int d) { // rotate x into ch[d]
		Tree y = x->ch[!d];
		x->ch[!d] = y->ch[d];
		if (x->ch[!d] != null) x->ch[!d]->pre = x;
		y->ch[d] = x;
		y->pre = x->pre;
		x->pre = y;
		x = y;
	}
	
	bool insert(Tree &x, const K &key, const V &value) {
		if (x == null) {
			x = new (getNode()) Node(key, value, nextUnsigned(), null);
			return true;
		}
		if (*x->key == key) {
			x->value->~V();
			free(x->value);
			x->value = new (getNode()) V(value);
			return false;
		}
		int d = *x->key < key;
		bool ret = insert(x->ch[d], key, value);
		x->ch[d]->pre = x;
		if (x->ch[d]->prio < x->prio)
			rotate(x, !d);
		return ret;
	}

	Tree downToLeaf(Tree x) {
		if (x->ch[0] == null) {
			Tree ret = x->ch[1];
			x->~Node();
			free(x);
			return ret;
		}
		if (x->ch[1] == null) {
			Tree ret = x->ch[0];
			x->~Node();
			free(x);
			return ret;
		}
		int d = x->ch[0]->prio < x->ch[1]->prio;
		rotate(x, d);
		x->ch[d] = downToLeaf(x->ch[d]);
		if (x->ch[d] != null)
			x->ch[d]->pre = x;
		return x;
	}
	
	bool erase(Tree &x, const K &key) {
		if (x == null) return false;
		if (*x->key != key) {
			int d = *x->key < key;
			bool ret = erase(x->ch[d], key);
			if (x->ch[d] != null)
				x->ch[d]->pre = x;
			return ret;
		}
		x = downToLeaf(x);
		return true;
	}

	Tree searchForKey(const K &key) const {
		for (Tree t = root; t != null; ) {
			if (*t->key == key)
				return t;
			if (*t->key > key)
				t = t->ch[0];
			else
				t = t->ch[1];
		}
		return null;
	}

public:
	class Iterator {
	private:
		TreeMap<K, V> *from;
		Tree p;

	public:
		Iterator(): from(NULL), p(NULL) {
		}

		Iterator(TreeMap<K, V> *f): from(f) {
			Tree null = from->null;
			for (p = from->root; p->ch[0] != null; p = p->ch[0]);
		}

		bool hasNext() {
			return from != NULL && p != from->null;
		}
		
		const Entry next() {
			Tree null = from->null;
			if (!hasNext())
				throw ElementNotExist(toString(__LINE__));
			Tree ret = p;
			if (p->ch[1] != null) {
				for (p = p->ch[1]; p->ch[0] != null; p = p->ch[0]);
			}
			else {
				Tree last;
				do {
					last = p;
					p = p->pre;
				} while (p != null && p->ch[0] != last);
			}
			return Entry(ret->key, ret->value);
		}
	};
	
	TreeMap(): seed((unsigned int)time(NULL)), _size(0), null(new (getNode()) Node()), root(null) {
	}

	~TreeMap() {
		deleteTree(root);
		if (null) {
			null->~Node();
			free(null);
		}
	}
	
	TreeMap<K, V>& operator = (const TreeMap<K, V> &x) {
		if (this != &x) {
			_size = x._size;
			seed = x.seed;
			
			if (null) {
				null->~Node();
				free(null);
			}

			deleteTree(root);
			x.cloneTo(root, null);

		}
		return *this;
	}

	TreeMap(TreeMap<K, V> &x) {
		_size = x._size;
		seed = x.seed;
		x.cloneTo(root, null);
	}
	
	Iterator iterator() {
		return Iterator(this);
	}

	void clear() {
		_size = 0;
		deleteTree(root);
		root = null;
	}

	bool containsKey(const K &key) const {
		return searchForKey(key) != null;
	}

	bool containsValue(const V &value) {
		for (Iterator itr(iterator()); itr.hasNext(); )
			if (*itr.next().value == value)
				return true;
		return false;
	}

	const V& get(const K &key) const {
		Tree ret = searchForKey(key);
		if (ret == null)
			throw ElementNotExist(toString(__LINE__));
		return *ret->value;
	}

	bool isEmpty() const {
		return _size == 0;
	}

	void put(const K &key, const V &value) {
		if ( insert(root, key, value) )
			++_size;
	}

	void remove(const K &key) {
		if (!erase(root, key))
			throw ElementNotExist(toString(__LINE__));
		root->pre = null;
		--_size;
	}

	int size() const {
		return _size;
	}
};

#undef getNode
#undef getType

#endif /* __TREEMAP_H */
