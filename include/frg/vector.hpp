#ifndef FRG_VECTOR_HPP
#define FRG_VECTOR_HPP

#include <utility>

#include <frg/macros.hpp>

namespace frg FRG_VISIBILITY {

template<typename T, typename Allocator>
class vector {
public:
	friend void swap(vector &a, vector &b) {
		using std::swap;
		swap(a._allocator, b._allocator);
		swap(a._elements, b._elements);
		swap(a._size, b._size);
		swap(a._capacity, b._capacity);
	}

	vector(Allocator &allocator);

	vector(const vector &other) = delete;

	vector(vector &&other)
	: vector(*other._allocator) {
		swap(*this, other);
	}

	~vector();

	T &push(const T &element);

	T &push(T &&element);

	T &push_back(T &&element) {
		return push(std::move(element));
	}

	T pop();

	template<typename... Args>
	void resize(size_t new_size, Args &&... args);

	T *data() {
		return _elements;
	}

	size_t size() const {
		return _size;
	}

	bool empty() const {
		return size() == 0;
	}

	T *begin() {
		return _elements;
	}

	const T *begin() const {
		return _elements;
	}

	T *end() {
		return _elements + _size;
	}

	const T *end() const {
		return _elements + _size;
	}

	T &front() {
		return _elements[0];
	}
	const T &front() const {
		return _elements[0];
	}

	T &back() {
		return _elements[_size - 1];
	}
	const T &back() const {
		return _elements[_size - 1];
	}

	const T &operator[] (size_t index) const {
		return _elements[index];
	}
	T &operator[] (size_t index) {
		return _elements[index];
	}

private:
	void _ensure_capacity(size_t capacity);

	Allocator *_allocator;
	T *_elements;
	size_t _size;
	size_t _capacity;
};

template<typename T, typename Allocator>
vector<T, Allocator>::vector(Allocator &allocator)
: _allocator{&allocator}, _elements{nullptr}, _size{0}, _capacity{0} { }

template<typename T, typename Allocator>
vector<T, Allocator>::~vector() {
	for(size_t i = 0; i < _size; i++)
		_elements[i].~T();
	_allocator->free(_elements);
}

template<typename T, typename Allocator>
T &vector<T, Allocator>::push(const T &element) {
	_ensure_capacity(_size + 1);
	T *pointer = new (&_elements[_size]) T(element);
	_size++;
	return *pointer;
}

template<typename T, typename Allocator>
T &vector<T, Allocator>::push(T &&element) {
	_ensure_capacity(_size + 1);
	T *pointer = new (&_elements[_size]) T(std::move(element));
	_size++;
	return *pointer;
}

template<typename T, typename Allocator>
template<typename... Args>
void vector<T, Allocator>::resize(size_t new_size, Args &&... args) {
	FRG_ASSERT(_size < new_size);
	_ensure_capacity(new_size);
	for(size_t i = _size; i < new_size; i++)
		new (&_elements[i]) T(std::forward<Args>(args)...);
	_size = new_size;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::_ensure_capacity(size_t capacity) {
	if(capacity <= _capacity)
		return;

	size_t new_capacity = capacity * 2;
	T *new_array = (T *)_allocator->allocate(sizeof(T) * new_capacity);
	for(size_t i = 0; i < _capacity; i++)
		new (&new_array[i]) T(std::move(_elements[i]));

	for(size_t i = 0; i < _size; i++)
		_elements[i].~T();
	_allocator->free(_elements);

	_elements = new_array;
	_capacity = new_capacity;
}

template<typename T, typename Allocator>
T vector<T, Allocator>::pop() {
	_size--;
	T element = std::move(_elements[_size]);
	_elements[_size].~T();
	return element;
}

} // namespace frg

#endif // FRG_VECTOR_HPP