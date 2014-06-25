/** @file */

#ifndef __HASHMAP_H
#define __HASHMAP_H

#define _DEBUG
#ifdef _DEBUG
	#include "memwatch.h"
#endif

#include "ElementNotExist.h"
#include "Utility.h"

#define getNode() ((Entry *)malloc(sizeof(Entry)))
#define getType(type) ((type*)malloc(sizeof(type)))
#define getArray(size) ((List*)malloc(sizeof(List) * size))

template<class K, class V, class H>
class HashMap {
private:
	typedef K* Kp;
	typedef V* Vp;
	H getHashCode;
	
public:

	class Iterator;

	class Entry {

		friend Iterator;
		friend HashMap;

	private:
		Kp key;
		Vp value;
		int hashCode;
		Entry *next, *l, *r;

	public:
		Entry(): key(NULL), value(NULL), hashCode(0), next(NULL), l(this), r(this) {
		}

		Entry(const K &key, const V &value):
			key(new (getType(K)) K(key)), value(new (getType(V)) V(value)), hashCode(getHashCode.hashCode(key)), next(NULL), l(NULL), r(NULL) {
		}

		Entry(const K &key, const V &value, const int &hashCode):
			key(new (getType(K)) K(key)), value(new (getType(V)) V(value)), hashCode(hashCode), next(NULL), l(NULL), r(NULL) {
		}

		const K& getKey() const {
			return *key;
		}

		const V& getValue() const {
			return *value;
		}
		
		~Entry() {
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
	typedef Entry *List;

	const static double LOAD_FACTOR = 0.50;
	const static int TABLE_SIZE[];

	int _size;
	int hashModPtr, capacity;
	List header;
	List *pool;

	void ensureCapacity(int cap) {
		if (capacity * LOAD_FACTOR < cap) {
			capacity = TABLE_SIZE[++hashModPtr];
			List* newPool = getArray(capacity);
			for (int i = 0; i < capacity; ++i) newPool[i] = NULL;

			for (List p = header->r; p != header; p = p->r) {
				int h = p->hashCode % capacity;
				if (h < 0) h += capacity;
				p->next = newPool[h];
				newPool[h] = p;
			}
			if (pool) free(pool);
			pool = newPool;
		}
	}

	void cloneTo(int &otherSize, int &otherHashModPtr, int &otherCapacity, List &otherHeader, List* &otherPool) const {
		otherSize = _size;
		otherHashModPtr = hashModPtr;
		otherCapacity = capacity;
		otherHeader = new ( getNode() ) Entry();
		otherPool = getArray(capacity);
		for (int i = 0; i < capacity; ++i) otherPool[i] = NULL;
		for (List p = header->r; p != header; p = p->r) {
			List element = new (getNode()) Entry();
			element->key = new (getType(K)) K(*p->key);
			element->value = new (getType(V)) V(*p->value);
			element->hashCode = p->hashCode;
			List l = element->l = otherHeader->l, r = element->r = otherHeader;
			l->r = r->l = element;
			int h = element->hashCode % capacity;
			element->next = otherPool[h];
			otherPool[h] = element;
		}
	}

	void destroy() { // everything but header is cleared
		_size = 0;
		hashModPtr = 0;
		capacity = 0;
		if (pool) {
			free(pool);
			pool = NULL;
		}

		for (List p = header->r, next; p != header; p = next) {
			next = p->r;
			p->~Entry();
			free(p);
		}
		header->l = header->r = header;
	}

public:
	class Iterator {
	private:
		List header, p;
	public:

		Iterator(): header(NULL), p(NULL) {
		}

		Iterator(const HashMap<K, V, H> &hmap): header(hmap.header), p(header) {
		}

		bool hasNext() {
			return header != NULL && p != NULL && p->r != header;
		}

		const Entry &next() {
			if (!hasNext())
				throw ElementNotExist(toString(__LINE__));
			p = p->r;
			return *p;
		}
	};

	HashMap(): 
		getHashCode(), _size(0), hashModPtr(0), capacity(TABLE_SIZE[0]), 
		header(new (getNode()) Entry()), pool(getArray(capacity)) {
		for (int i = 0; i < capacity; ++i)
			pool[i] = NULL;
	}

	HashMap(const HashMap<K, V, H> &other): getHashCode(other.getHashCode), _size(0), hashModPtr(0), capacity(0), header(NULL), pool(NULL) {
		other.cloneTo(_size, hashModPtr, capacity, header, pool);
	}
	
	~HashMap() {
		destroy();
		header->~Entry();
		free(header);
		header = NULL;
	}
	
	HashMap<K, V, H>& operator = (const HashMap<K, V, H> &other) {
		if (this != &other) {
			destroy();
			getHashCode = other.getHashCode;
			other.cloneTo(_size, hashModPtr, capacity, header, pool);
		}
		return *this;
	}

	Iterator iterator() const {
		return Iterator(*this);
	}

	void clear() {
		destroy();
		capacity = TABLE_SIZE[0];
		pool = getArray(capacity);
		for (int i = 0; i < capacity; ++i) pool[i] = NULL;
	}
	
	bool containsKey(const K &key) const {
		int code = getHashCode.hashCode(key), h = code % capacity;
		if (h < 0) h += capacity;
		for (List p = pool[h]; p; p = p->next)
			if (p->hashCode == code && *p->key == key)
				return true;
		return false;
	}

	bool containsValue(const V &value) const {
		for (List p = header->r; p != header; p = p->r)
			if (*p->value == value)
				return true;
		return false;
	}

	const V& get(const K &key) const {
		int code = getHashCode.hashCode(key), h = code % capacity;
		if (h < 0) h += capacity;
		for (List p = pool[h]; p; p = p->next)
			if (p->hashCode == code && *p->key == key)
				return *p->value;
		throw ElementNotExist(toString(__LINE__));
		return *header->value;
	}
	
	bool isEmpty() const {
		return _size == 0;
	}
	
	void put(const K &key, const V &value) {
		int code = getHashCode.hashCode(key), h = code % capacity;
		if (h < 0) h += capacity;
		for (List p = pool[h]; p; p = p->next)
			if (code == p->hashCode && *p->key == key) {
				if (*p->value != value) {
					p->value->~V();
					free(p->value);
					p->value = new (getType(V)) V(value);
				}
				return;
			}
		++_size;
		List p = new (getNode()) Entry(key, value, code), l = p->l = header->l, r = p->r = header;
		l->r = r->l = p;
		p->next = pool[h];
		pool[h] = p;
		ensureCapacity(_size + 1);
	}
	
	void remove(const K &key) {
		int code = getHashCode.hashCode(key), h = code % capacity;
		if (h < 0) h += capacity;
		for (List p = pool[h], last = NULL; p; last = p, p = p->next)
			if (p->hashCode == code && *p->key == key) {
				if (last == NULL) {
					pool[h] = pool[h]->next;
				}
				else {
					last->next = p->next;
				}
				List l = p->l, r = p->r;
				l->r = r;
				r->l = l;
				p->~Entry();
				free(p);
				--_size;
				return;
			}
		throw ElementNotExist(toString(__LINE__));
	}
	
	int size() const {
		return _size;
	}
};

template <class K, class V, class H>
const int HashMap<K, V, H>::TABLE_SIZE[] = {
		37, 131, 521, 2053,
		8209, 32771, 131101, 524309, 2097169,
		8388617, 33554467, 134217757,
		536870923, 1073741827 };

/*
const int HashMap<K, V, H>::TABLE_SIZE[] = {
		17, 37, 67, 131, 257, 521, 1031, 2053, 4099,
		8209, 16411, 32771, 65537, 131101, 262147, 524309, 1048583, 2097169,
		4194319, 8388617, 16777259, 33554467, 67108879, 134217757,
		268435459, 536870923, 1073741827 };
*/

#undef getNode
#undef getKey
#undef getValue
#undef getArray

#endif

