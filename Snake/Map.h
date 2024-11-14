#pragma once

class Map {
private:
	size_t width = 0, height = 0;
	int* buf = nullptr;
public:
	Map() = default;
	inline Map(size_t w, size_t h) { resize(w, h); }
	void resize(size_t w, size_t h);
	inline int& operator()(size_t x, size_t y) { return buf[x + y * width]; }
	inline int* operator[](size_t y) { return buf + y * width; }
	inline size_t getWidth() const { return width; }
	inline size_t getHeight() const { return height; }
	inline size_t getSize() const { return width * height; }
	struct iterator {
		int* data;
		inline iterator(int* p) : data(p) {}
		inline int& operator*() { return *data; }
		inline const int& operator*() const { return *data; }
		inline iterator& operator++() { ++data; return *this; }
		inline bool operator!=(const iterator& x) const { return x.data != data; }
		inline bool operator==(const iterator& x) const { return x.data == data; }
		inline iterator& operator=(const iterator& x) { data = x.data; return *this; }
	};
	inline iterator begin() { return iterator(buf); }
	inline iterator end() { return iterator(buf + width * height); }
	inline int& first() { return *buf; }
};