/** @file */

#ifndef __ARRAYLIST_H
#define __ARRAYLIST_H

#include "IndexOutOfBound.h"
#include "ElementNotExist.h"

template <class T>
class ArrayList {
private:

	typedef T* Tp;

	Tp* base;
	int _size;
	int capacity;

	void cloneTo(Tp* &otherBase, int &otherSize, int &otherCapacity) const {
		otherBase = new Tp[capacity];
		otherSize = _size;
		otherCapacity = capacity;
		for (int i = 0; i < _size; ++i)
			otherBase[i] = new T(*base[i]);
		for (int i = _size; i < capacity; ++i)
			otherBase[i] = NULL;
	}

	void ensureCapacity(int cap) {
		if (cap > capacity) {
			capacity = std::max(capacity << 1, cap);
			Tp* newArray = new Tp[capacity];
			for (int i = 0; i < _size; ++i)
				newArray[i] = base[i];
			for (int i = _size; i < capacity; ++i)
				newArray[i] = NULL;
			if (base) delete[] base;
			base = newArray;
		}
	}

	void trimToSize(int cap) {
		if (cap <= capacity / 4) {
			capacity = cap;
			if (_size > capacity) {
				for (int i = capacity; i < _size; ++i) {
					if (base[i]) {
						delete base[i];
						base[i] = NULL;
					}
				}
				_size = capacity;
			}
			Tp *newArray = new Tp[capacity];
			for (int i = 0; i < _size; ++i) newArray[i] = base[i];
			for (int i = _size; i < capacity; ++i) newArray[i] = NULL; 

			delete[] base;
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
				throw ElementNotExist("");
			lastPos = nextPos;
			++nextPos;
			return from->get(lastPos);
		}

		void remove() {
			if (!(from != NULL && 0 <= lastPos && lastPos < from->_size))
				throw ElementNotExist("");
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
		base[_size++] = new T(e);
		return true;
	}

	void add(int idx, const T& e) {
		if (!(0 <= idx && idx <= _size))
			throw IndexOutOfBound("");
		ensureCapacity(_size + 1);
		for (int i = _size - 1; i >= idx; --i)
			base[i + 1] = base[i];
		base[idx] = new T(e);
		++_size;
	}

	void clear() {
		for (int i = 0; i < _size; ++i) {
			if (base[i]) {
				delete base[i];
				base[i] = NULL;
			}
		}
		if (base) delete[] base;
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
			throw IndexOutOfBound("");
		return *base[idx];
	}

	bool isEmpty() const {
		return _size == 0;
	}

	void removeIndex(int idx) {
		if (!(0 <= idx && idx < _size))
			throw IndexOutOfBound("");
		if (base[idx]) {
			delete base[idx];
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
			throw IndexOutOfBound("");
		if (base[idx]) {
			delete base[idx];
		}
		base[idx] = new T(e);
	}

	int size() const {
		return _size;
	}

	Iterator iterator() const {
		return Iterator( const_cast<ArrayList<T>*> (this) );
	}
};

#endif /* __ARRAYLIST_H */
