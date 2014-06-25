/** @file */

#ifndef __ARRAYLIST_H
#define __ARRAYLIST_H

#define _DEBUG
#ifdef _DEBUG
	#include "memwatch.h"
#endif

#include "IndexOutOfBound.h"
#include "ElementNotExist.h"
#include "Utility.h"

#define getNode()		(	(T*)	malloc	(sizeof(T)			)	) 
#define getArray(size)	(	(Tp*)	malloc	(sizeof(Tp) * size	)	)

template <class T>
class ArrayList {
private:

	typedef T* Tp;

	Tp* base;
	int _size;
	int capacity;

	void cloneTo(Tp* &otherBase, int &otherSize, int &otherCapacity) const {
		otherBase = getArray(capacity);
		otherSize = _size;
		otherCapacity = capacity;
		for (int i = 0; i < _size; ++i)
			otherBase[i] = new (getNode()) T(*base[i]);
		for (int i = _size; i < capacity; ++i)
			otherBase[i] = NULL;
	}

	void ensureCapacity(int cap) {
		if (cap > capacity) {
			capacity = std::max(capacity << 1, cap);
			Tp* newArray = getArray(capacity);
			for (int i = 0; i < _size; ++i)
				newArray[i] = base[i];
			for (int i = _size; i < capacity; ++i)
				newArray[i] = NULL;
			if (base) free(base);
			base = newArray;
		}
	}

	void trimToSize(int cap) {
		if (cap <= capacity / 4) {
			capacity = cap;
			if (_size > capacity) {
				for (int i = capacity; i < _size; ++i) {
					if (base[i]) {
						base[i]->~T();
						free(base[i]);
					}
				}
				_size = capacity;
			}
			Tp *newArray = getArray(capacity);
			for (int i = 0; i < _size; ++i) newArray[i] = base[i];
			for (int i = _size; i < capacity; ++i) newArray[i] = NULL; 

			if (base) free(base);
			base = newArray;
		}
	}

public:

	class Iterator {

	private:
		ArrayList<T> *from;
		int lastPos, nextPos;

	public:

		Iterator(): from(NULL), lastPos(-1), nextPos(0) {
		}

		Iterator(ArrayList<T> *from): from(from), lastPos(-1), nextPos(0) {
		}

		bool hasNext() const {
			return from != NULL && nextPos < from->_size;
		}

		const T& next() {
			if (!hasNext())
				throw ElementNotExist(toString(__LINE__));
			lastPos = nextPos;
			++nextPos;
			return from->get(lastPos);
		}

		void remove() {
			if (!(from != NULL && 0 <= lastPos && lastPos < from->_size))
				throw ElementNotExist(toString(__LINE__));
			from->removeIndex(lastPos);
			lastPos = -1;
			--nextPos;
		}
    };

	ArrayList(): base(NULL), _size(0), capacity(0) {
	}

	~ArrayList() {
		clear();
	}

	ArrayList<T>& operator = (const ArrayList<T>& rhs) {
		if (this != &rhs) {
			clear();
			rhs.cloneTo(base, _size, capacity);
		}
		return *this;
	}

	ArrayList(const ArrayList<T>& x): base(NULL), _size(0), capacity(0) {
		x.cloneTo(base, _size, capacity);
	}

	bool add(const T& e) {
		ensureCapacity(_size + 1);
		base[_size++] = new (getNode()) T(e);
		return true;
	}

	void add(int idx, const T& e) {
		if (!(0 <= idx && idx <= _size))
			throw IndexOutOfBound(toString(__LINE__));
		ensureCapacity(_size + 1);
		for (int i = _size - 1; i >= idx; --i)
			base[i + 1] = base[i];
		base[idx] = new (getNode()) T(e);
		++_size;
	}

	void clear() {
		for (int i = 0; i < _size; ++i) {
			if (base[i]) {
				base[i]->~T();
				free(base[i]);
			}
		}
		if (base) free(base);
		base = NULL;
		_size = 0;
		capacity = 0;
	}

	bool contains(const T& e) const {
		for (int i = 0; i < _size; ++i)
			if (*base[i] == e)
				return true;
		return false;
	}

	const T& get(int idx) const {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound(toString(__LINE__));
		return *base[idx];
	}

	bool isEmpty() const {
		return _size == 0;
	}

	void removeIndex(int idx) {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound(toString(__LINE__));
		if (base[idx]) {
			base[idx]->~T();
			free(base[idx]);
		}
		for (int i = idx + 1; i < _size; ++i)
			base[i - 1] = base[i];
		base[--_size] = NULL;
		trimToSize(_size);
	}

	bool remove(const T &e) {
		for (int i = 0; i < _size; ++i)
			if (*base[i] == e) {
				removeIndex(i);
				return true;
			}
		return false;
	}

	void set(int idx, const T& e) {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound(toString(__LINE__));
		if (base[idx]) {
			base[idx]->~T();
			free(base[idx]);
		}
		base[idx] = new (getNode()) T(e);
	}

	int size() const {
		return _size;
	}

	Iterator iterator() const {
		return Iterator( const_cast<ArrayList<T>*> (this) );
	}
};

#undef getNode
#undef getArray

#endif /* __ARRAYLIST_H */
