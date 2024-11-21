
#ifndef LPCcomplex_h
#define LPCcomplex_h

#include <math.h>
#include <iostream>
#include <functional>

namespace LPC {
	template<typename T>
	struct complex;
	template<typename T, size_t width, size_t height>
	struct array2D;
}

template<typename T>
struct LPC::complex {
	T _Re, _Im;
	static const bool is_integral = std::is_integral_v<T>;
	complex() : _Re(0), _Im(0){};
	template<typename U>
	constexpr complex(complex<U> v) : _Re((T)v._Re), _Im((T)v._Im) {}
	template<typename U, typename V = T>
	constexpr complex(U v, V v1 = 0) : _Re((T)v), _Im((T)v1) {}
	constexpr T& real() { return _Re; }
	constexpr T& imag() { return _Im; }
	constexpr T real() const { return _Re; }
	constexpr T imag() const { return _Im; }
	template<typename U>
	constexpr void real(U v) { _Re = v; }
	template<typename U>
	constexpr void imag(U v) { _Im = v; }
	constexpr complex<T> conjugate() { return complex<T>(_Re, -_Im); }
	constexpr complex<T> operator-() const {
		complex<T> ret(-_Re, -_Im);
		return ret;
	}
	template<typename U>
	constexpr complex<T>& operator+=(complex<U> x) {
		_Re += x._Re;
		_Im += x._Im;
		return *this;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr complex<T>& operator+=(U x) {
		_Re += x;
		return *this;
	}
	template<typename U>
	constexpr auto operator+(complex<U> x) const {
		complex<decltype(_Re + x._Re)> ret = *this;
		ret += x;
		return ret;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr auto operator+(U x) const {
		complex<decltype(_Re + x)> ret = *this;
		ret += x;
		return ret;
	}
	template<typename U>
	constexpr complex<T>& operator-=(complex<U> x) {
		_Re -= x._Re;
		_Im -= x._Im;
		return *this;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr complex<T>& operator-=(U x) {
		_Re -= x;
		return *this;
	}
	template<typename U>
	constexpr auto operator-(complex<U> x) const {
		complex<decltype(_Re - x._Re)> ret = *this;
		ret -= x;
		return ret;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr auto operator-(U x) const {
		complex<decltype(_Re - x)> ret = *this;
		ret -= x;
		return ret;
	}
	template<typename U>
	constexpr complex<T>& operator*=(complex<U> x) {
		T r = _Re * x._Re - _Im * x._Im;
		_Im = _Im * x._Re + _Re * x._Im;
		_Re = r;
		return *this;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr complex<T>& operator*=(U x) {
		_Re *= x;
		_Im *= x;
		return *this;
	}
	template<typename U>
	constexpr auto operator*(complex<U> x) const {
		complex<decltype(_Re * x._Re)> ret = *this;
		ret *= x;
		return ret;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr auto operator*(U x) const {
		complex<decltype(_Re * x)> ret = *this;
		ret *= x;
		return ret;
	}
	template<typename U>
	constexpr complex<T>& operator/=(complex<U> x) {
		T r = _Re * x._Re + _Im * x._Im;
		_Im = _Im * x._Re - _Re * x._Im;
		_Re = r;
		if constexpr (std::is_floating_point_v<T> || std::is_floating_point_v<U>) {
			auto s = (T)1 / (x._Re * x._Re + x._Im * x._Im);
			_Re *= s;
			_Im *= s;
		}
		else {
			U s = x._Re * x._Re + x._Im * x._Im;
			_Re /= s;
			_Im /= s;
		}
		return *this;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr complex<T>& operator/=(U x) {
		if constexpr (std::is_floating_point_v<T> || std::is_floating_point_v<U>) {
			auto s = (T)1 / x;
			_Re *= s;
			_Im *= s;
		}
		else {
			_Re /= x;
			_Im /= x;
		}
		return *this;
	}
	template<typename U>
	constexpr auto operator/(const complex<U>& x) {
		complex<decltype(_Re / x._Re)> ret(*this);
		ret /= x;
		return ret;
	}
	template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
	constexpr auto operator/(U x) {
		complex<decltype(_Re / x)> ret(*this);
		ret /= x;
		return ret;
	}
	template<typename U>
	constexpr complex<T>& operator<<=(U v) {
		_Re <<= v;
		_Im <<= v;
		return *this;
	}
	template<typename U>
	constexpr complex<T>& operator<<(U v) {
		complex<T> ret = *this;
		ret <<= v;
		return ret;
	}
	template<typename U>
	constexpr complex<T>& operator>>=(U v) {
		_Re >>= v;
		_Im >>= v;
		return *this;
	}
	template<typename U>
	constexpr complex<T>& operator>>(U v) {
		complex<T> ret = *this;
		ret >>= v;
		return ret;
	}
	template<typename U>
	constexpr bool operator==(const complex<U>& v) const {
		return v._Re == _Re && v._Im == _Im;
	}
	template<typename U>
	constexpr bool operator!=(const complex<U>& v) const {
		return v._Re != _Re || v._Im != _Im;
	}
	constexpr auto abs() const noexcept {
		return sqrt(_Re * _Re + _Im * _Im);
	}
	constexpr auto abssquare() const noexcept {
		return _Re * _Re + _Im * _Im;
	}
	static const complex i;
};

template<typename T>
const LPC::complex<T> LPC::complex<T>::i = LPC::complex<T>(0, 1);

template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>> inline auto operator+(U v1, LPC::complex<T> v2) { return v2 + v1; }
template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>> inline auto operator-(U v1, LPC::complex<T> v2) { return LPC::complex<U>(v1) - v2; }
template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>> inline auto operator*(U v1, LPC::complex<T> v2) { return v2 * v1; }
template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>> inline auto operator/(U v1, LPC::complex<T> v2) { return LPC::complex<U>(v1) / v2; }

template<typename T>
std::ostream& operator<<(std::ostream& os, LPC::complex<T> x) {
	os << '(' << x._Re;
	if (x._Im > 0) os << '+';
	os << x._Im << "i)";
	return os;
}

template<typename T>
std::istream& operator<<(std::istream& is, LPC::complex<T> x) {
	is >> x._Re >> x._Im;
	return is;
}

template<typename T>
struct std::less<LPC::complex<T>> {
	bool operator()(const LPC::complex<T>& v1, const LPC::complex<T>& v2) const {
		if (v1.imag() == v2.imag()) return v1.real() < v2.real();
		else return v1.imag() < v2.imag();
	}
};

template<typename T, size_t width, size_t height>
struct LPC::array2D {
	T data[height][width];
	T* operator[](size_t index) { return data[index]; }
	template<typename U>
	T& operator[](complex<U> index) { return data[index._Im][index._Re]; }
};

#endif
