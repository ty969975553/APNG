#ifndef UTILITIES_HXX
#define UTILITIES_HXX

#include <stdint.h>
#include <stdlib.h>
#include <limits>
#include "stream.hxx"

struct bitmapRegion_t final
{
private:
	const uint32_t _width, _height;
	bitmapRegion_t() = delete;
	bitmapRegion_t &operator =(const bitmapRegion_t &) = delete;
	bitmapRegion_t &operator =(bitmapRegion_t &&region) = delete;

public:
	constexpr bitmapRegion_t(const uint32_t width, const uint32_t height) noexcept : _width(width), _height(height) { }
	bitmapRegion_t(const bitmapRegion_t &region) noexcept : _width(region._width), _height(region._height) { }
	bitmapRegion_t(bitmapRegion_t &&region) noexcept : _width(region._width), _height(region._height) { }

	uint32_t width() const noexcept { return _width; }
	uint32_t height() const noexcept { return _height; }
};

uint32_t swap32(const uint32_t i) noexcept
{
	return ((i >> 24) & 0xFF) | ((i >> 8) & 0xFF00) | ((i & 0xFF00) << 8) | ((i & 0xFF) << 24);
}
void swap(uint32_t &i) noexcept { i = swap32(i); }

uint16_t swap16(const uint16_t i) noexcept
{
	return ((i >> 8) & 0xFF) | ((i & 0xFF) << 8);
}
void swap(uint16_t &i) noexcept { i = swap16(i); }

template<typename T> bool contains(const std::vector<T> &list,
	bool condition(const T &)) noexcept
{
	for (const T &item : list)
	{
		if (condition(item))
			return true;
	}
	return false;
}

template<typename T> std::vector<const T *> extract(const typename std::vector<T>::const_iterator &begin,
	const typename std::vector<T>::const_iterator &end, bool condition(const T &)) noexcept
{
	std::vector<const T *> result;
	for (auto iter = begin; iter != end; ++iter)
	{
		const T &item = *iter;
		if (condition(item))
			result.emplace_back(&item);
	}
	return result;
}

template<typename T> std::vector<const T *> extract(const std::vector<T> &list,
	bool condition(const T &)) noexcept
	{ return extract(list.begin(), list.end(), condition); }

template<typename T> std::vector<typename std::vector<T>::const_iterator> extractIters(const std::vector<T> &list,
	bool condition(const T &)) noexcept
{
	using listIter_t = typename std::vector<T>::const_iterator;
	std::vector<listIter_t> result;
	for (listIter_t iter = list.begin(); iter != list.end(); ++iter)
	{
		const T &item = *iter;
		if (condition(item))
			result.emplace_back(iter);
	}
	return result;
}

template<typename T> const T *extractFirst(const std::vector<T> &list,
	bool condition(const T &)) noexcept
{
	for (const T &item : list)
	{
		if (condition(item))
			return &item;
	}
	return nullptr;
}

template<typename T> bool isBefore(const T *a, const T *b) noexcept
	{ return a < b; }

template<typename T> bool isAfter(const T *a, const T *b) noexcept
	{ return a > b; }

template<typename T> struct pngRGB_t
{
public:
	T r; T g; T b;

	using type = T;
	constexpr pngRGB_t() noexcept : r(0), g(0), b(0) { }
	constexpr pngRGB_t(const T _r, const T _g, const T _b) noexcept : r(_r), g(_g), b(_b) { }
	bool operator ==(const pngRGB_t<T> &pixel) const noexcept { return r == pixel.r && g == pixel.g && b == pixel.b; }
	pngRGB_t<T> operator +(const pngRGB_t<T> &pixel) const noexcept
		{ return {T(r + pixel.r), T(g + pixel.g), T(b + pixel.b)}; }
	pngRGB_t<T> operator -(const pngRGB_t<T> &pixel) const noexcept
		{ return {T(r - pixel.r), T(g - pixel.g), T(b - pixel.b)}; }
	pngRGB_t<T> operator >>(const uint8_t amount) const noexcept
		{ return {T(r >> amount), T(g >> amount), T(b >> amount)}; }
	pngRGB_t<T> operator &(const T mask) const noexcept
		{ return {T(r & mask), T(g & mask), T(b & mask)}; }
	pngRGB_t<T> operator &(const pngRGB_t<T> &mask) const noexcept
		{ return {T(r & mask.r), T(g & mask.g), T(b & mask.b)}; }
	pngRGB_t<uint8_t> &operator +=(const pngRGB_t<uint8_t> &pixel) noexcept
		{ return r += pixel.r, g += pixel.g, b += pixel.b, *this; }

	pngRGB_t<uint16_t> &operator +=(const pngRGB_t<uint16_t> &pixel) noexcept
	{
		r = (uint8_t((r >> 8) + (pixel.r >> 8)) << 8) | uint8_t(r + pixel.r);
		g = (uint8_t((g >> 8) + (pixel.g >> 8)) << 8) | uint8_t(g + pixel.g);
		b = (uint8_t((b >> 8) + (pixel.b >> 8)) << 8) | uint8_t(b + pixel.b);
		return *this;
	}
};

template<typename T> struct pngRGBA_t final : public pngRGB_t<T>
{
public:
	T a;

	using type = T;
	using pngRGBn_t = pngRGB_t<T>;
	constexpr pngRGBA_t() noexcept : pngRGB_t<T>(), a(0) { }
	constexpr pngRGBA_t(const pngRGBn_t rgb, const T _a) : pngRGBn_t(rgb), a(_a) { }
	bool operator ==(const pngRGBA_t<T> &pixel) const noexcept { return pngRGBn_t(*this) == pngRGBn_t(pixel) && a == pixel.a; }
	pngRGBA_t<T> operator +(const pngRGBA_t<T> &pixel) const noexcept
		{ return {pngRGBn_t(*this) + pngRGBn_t(pixel), T(a + pixel.a)}; }
	pngRGBA_t<T> operator -(const pngRGBA_t<T> &pixel) const noexcept
		{ return {pngRGBn_t(*this) - pngRGBn_t(pixel), T(a - pixel.a)}; }
	pngRGBA_t<T> operator >>(const uint8_t amount) const noexcept
		{ return {pngRGBn_t(*this) >> amount, T(a >> amount)}; }
	pngRGBA_t<T> operator &(const T mask) const noexcept
		{ return {pngRGBn_t(*this) & mask, T(a & mask)}; }
	pngRGBA_t<T> operator &(const pngRGBA_t<T> &mask) const noexcept
		{ return {pngRGBn_t(*this) & pngRGBn_t(mask), T(a & mask.a)}; }
	pngRGBA_t<uint8_t> &operator +=(const pngRGBA_t<uint8_t> &pixel) noexcept
		{ return *static_cast<pngRGBn_t *>(this) += pngRGBn_t(pixel), a += pixel.a, *this; }

	pngRGBA_t<uint16_t> &operator +=(const pngRGBA_t<uint16_t> &pixel) noexcept
	{
		*static_cast<pngRGBn_t *>(this) += pngRGBn_t(pixel);
		a = (uint8_t((a >> 8) + (pixel.a >> 8)) << 8) | uint8_t(a + pixel.a);
		return *this;
	}
};

template<typename T> struct pngGrey_t
{
public:
	T v;

	using type = T;
	constexpr pngGrey_t() noexcept : v(0) { }
	constexpr pngGrey_t(const T _v) noexcept : v(_v) { }
	bool operator ==(const pngGrey_t &pixel) const noexcept { return v == pixel.v; }
	pngGrey_t<T> operator +(const pngGrey_t<T> &pixel) const noexcept
		{ return {T(v + pixel.v)}; }
	pngGrey_t<T> operator -(const pngGrey_t<T> &pixel) const noexcept
		{ return {T(v - pixel.v)}; }
	pngGrey_t<T> operator >>(const uint8_t amount) const noexcept
		{ return {T(v >> amount)}; }
	pngGrey_t<T> operator &(const T mask) const noexcept
		{ return {T(v & mask)}; }
	pngGrey_t<T> operator &(const pngGrey_t<T> &mask) const noexcept
		{ return {T(v & mask.v)}; }
	pngGrey_t<uint8_t> &operator +=(const pngGrey_t<uint8_t> &pixel) noexcept
		{ return v += pixel.v, *this; }

	pngGrey_t<uint16_t> &operator +=(const pngGrey_t<uint16_t> &pixel) noexcept
	{
		v = (uint8_t((v >> 8) + (pixel.v >> 8)) << 8) | uint8_t(v + pixel.v);
		return *this;
	}
};

template<typename T> struct pngGreyA_t final : public pngGrey_t<T>
{
public:
	T a;

	using type = T;
	using pngGreyN_t = pngGrey_t<T>;
	constexpr pngGreyA_t() noexcept : pngGrey_t<T>(), a(0) { }
	constexpr pngGreyA_t(const pngGreyN_t grey, const T _a) noexcept : pngGreyN_t(grey), a(_a) { }
	bool operator ==(const pngGreyA_t<T> &pixel) const noexcept { return pngGreyN_t(*this) == pngGreyN_t(pixel) && a == pixel.a; }
	pngGreyA_t<T> operator +(const pngGreyA_t<T> &pixel) const noexcept
		{ return {pngGreyN_t(*this) + pngGreyN_t(pixel), T(a + pixel.a)}; }
	pngGreyA_t<T> operator -(const pngGreyA_t<T> &pixel) const noexcept
		{ return {pngGreyN_t(*this) - pngGreyN_t(pixel), T(a - pixel.a)}; }
	pngGreyA_t<T> operator >>(const uint8_t amount) const noexcept
		{ return {pngGreyN_t(*this) >> amount, T(a >> amount)}; }
	pngGreyA_t<T> operator &(const T mask) const noexcept
		{ return {pngGreyN_t(*this) & mask, T(a & mask)}; }
	pngGreyA_t<T> operator &(const pngGreyA_t<T> &mask) const noexcept
		{ return {pngGreyN_t(*this) & pngGreyN_t(mask), T(a & mask.a)}; }
	pngGreyA_t<uint8_t> &operator +=(const pngGreyA_t<uint8_t> &pixel) noexcept
		{ return *static_cast<pngGreyN_t *>(this) += pngGreyN_t(pixel), a += pixel.a, *this; }

	pngGreyA_t<uint16_t> &operator +=(const pngGreyA_t<uint16_t> &pixel) noexcept
	{
		*static_cast<pngGreyN_t *>(this) += pngGreyN_t(pixel);
		a = (uint8_t((a >> 8) + (pixel.a >> 8)) << 8) | uint8_t(a + pixel.a);
		return *this;
	}
};

using pngRGB8_t = pngRGB_t<uint8_t>;
using pngRGB16_t = pngRGB_t<uint16_t>;
using pngRGBA8_t = pngRGBA_t<uint8_t>;
using pngRGBA16_t = pngRGBA_t<uint16_t>;
using pngGrey8_t = pngGrey_t<uint8_t>;
using pngGrey16_t = pngGrey_t<uint16_t>;
using pngGreyA8_t = pngGreyA_t<uint8_t>;
using pngGreyA16_t = pngGreyA_t<uint16_t>;

template<typename T> bool readRGB(stream_t &stream, T &pixel) noexcept
	{ return stream.read(pixel.r) && stream.read(pixel.g) && stream.read(pixel.b); }
template<typename T> bool readRGBA(stream_t &stream, T &pixel) noexcept
	{ return readRGB(stream, pixel) && stream.read(pixel.a); }
template<typename T> bool readGrey(stream_t &stream, T &pixel) noexcept
	{ return stream.read(pixel.v); }
template<typename T> bool readGreyA(stream_t &stream, T &pixel) noexcept
	{ return readGrey(stream, pixel) && stream.read(pixel.a); }

template<typename T> T safeIndex(const T *const data, int32_t index, int32_t offset) noexcept
	{ return index >= 0 && offset >= 0 ? data[index + offset] : T(); }
template<typename T> auto filterFunc_t() -> T (*)(const T *const, const bitmapRegion_t &, const uint32_t, const uint32_t) noexcept;
template<typename T> using filter_t = decltype(filterFunc_t<T>());
enum class filterTypes_t : uint8_t { none, sub, up, average, paeth };

template<typename T> T filterSub(const T *const data, const bitmapRegion_t &frame, const uint32_t x, const uint32_t y) noexcept
	{ return safeIndex(data, x - 1, y * frame.width()); }
template<typename T> T filterUp(const T *const data, const bitmapRegion_t &frame, const uint32_t x, const uint32_t y) noexcept
	{ return safeIndex(data, x, (y - 1) * frame.width()); }

template<typename T> T filterAverage(const T *const data, const bitmapRegion_t &frame, const uint32_t x, const uint32_t y) noexcept
{
	const T left = safeIndex(data, x - 1, y * frame.width());
	const T up = safeIndex(data, x, (y - 1) * frame.width());
	return (up >> 1) + (left >> 1) + ((up & left) & 1);
}

uint8_t filterPaeth(const uint8_t a, const uint8_t b, const uint8_t c) noexcept
{
	const int16_t pred = a + b - c;
	const uint16_t absA = abs(pred - a);
	const uint16_t absB = abs(pred - b);
	const uint16_t absC = abs(pred - c);

	if (absA <= absB && absA <= absC)
		return a;
	else if (absB <= absC)
		return b;
	return c;
}

uint16_t filterPaeth(const uint16_t a, const uint16_t b, const uint16_t c) noexcept
{
	const uint16_t upper = filterPaeth(uint8_t(a >> 8), uint8_t(b >> 8), uint8_t(c >> 8));
	const uint16_t lower = filterPaeth(uint8_t(a), uint8_t(b), uint8_t(c));
	return (upper << 8) | lower;
}

template<typename T> pngRGB_t<T> filterPaeth(pngRGB_t<T> a, pngRGB_t<T> b, pngRGB_t<T> c) noexcept
	{ return {filterPaeth(a.r, b.r, c.r), filterPaeth(a.g, b.g, c.g), filterPaeth(a.b, b.b, c.b)}; }
template<typename T> pngGrey_t<T> filterPaeth(pngGrey_t<T> a, pngGrey_t<T> b, pngGrey_t<T> c) noexcept
	{ return {filterPaeth(a.v, b.v, c.v)}; }

template<typename T> pngRGBA_t<T> filterPaeth(pngRGBA_t<T> a, pngRGBA_t<T> b, pngRGBA_t<T> c) noexcept
{
	using pngRGBn_t = typename pngRGBA_t<T>::pngRGBn_t;
	return {filterPaeth(pngRGBn_t(a), pngRGBn_t(b), pngRGBn_t(c)), filterPaeth(a.a, b.a, c.a)};
}

template<typename T> pngGreyA_t<T> filterPaeth(pngGreyA_t<T> a, pngGreyA_t<T> b, pngGreyA_t<T> c) noexcept
{
	using pngGreyN_t = typename pngGreyA_t<T>::pngGreyN_t;
	return {filterPaeth(pngGreyN_t(a), pngGreyN_t(b), pngGreyN_t(c)), filterPaeth(a.a, b.a, c.a)};
}

template<typename T> T filterPaeth(const T *const data, const bitmapRegion_t &frame, const uint32_t x, const uint32_t y) noexcept
{
	const T left = safeIndex(data, x - 1, y * frame.width());
	const T up = safeIndex(data, x, (y - 1) * frame.width());
	const T upLeft = safeIndex(data, x - 1, (y - 1) * frame.width());
	return filterPaeth(left, up, upLeft);
}

template<typename T> filter_t<T> selectFilter(const filterTypes_t filter) noexcept
{
	switch (filter)
	{
		case filterTypes_t::sub:
			return filterSub<T>;
		case filterTypes_t::up:
			return filterUp<T>;
		case filterTypes_t::average:
			return filterAverage<T>;
		case filterTypes_t::paeth:
			return filterPaeth<T>;
		case filterTypes_t::none:
		default:
			return nullptr;
	}
}

template<typename T, bool copyFunc(stream_t &, T &)> bool copyFrame(stream_t &stream, void *const dataPtr, const bitmapRegion_t frame) noexcept
{
	T *const data = static_cast<T *const>(dataPtr);
	const uint32_t width = frame.width();
	const uint32_t height = frame.height();
	for (uint32_t y = 0; y < height; ++y)
	{
		filterTypes_t filter;
		if (!stream.read(filter))
			return false;
		// This treats unknown or invalid filter types as filterTypes_t::none.
		filter_t<T> filterFunc = selectFilter<T>(filter);

		for (uint32_t x = 0; x < width; ++x)
		{
			if (!copyFunc(stream, data[x + (y * width)]))
				return false;
			if (filterFunc)
				data[x + (y * width)] += filterFunc(data, frame, x, y);
		}
	}
	return true;
}

template<typename T> T compNop(const T a, const T) noexcept { return a; }
template<typename T> T compSource(const T, const T b) noexcept { return b; }
template<typename T> T compOver(const T a, const T b, const T alpha) noexcept
{
	constexpr const T max = std::numeric_limits<T>::max();
	constexpr const uint8_t bits = std::numeric_limits<T>::digits;
	return T(((max - alpha + 1) * a) >> bits) + T(((alpha + 1) * b) >> bits);
}

template<blendOp_t::_blendOp_t op, typename T> T compOp(const T a, const T b, const T alpha) noexcept
{
	if (op == blendOp_t::source)
		return compSource(a, b);
	else
		return compOver(a, b, alpha);
}

template<blendOp_t::_blendOp_t op, typename T> T compOpA(const T a, const T b) noexcept
{
	if (op == blendOp_t::source)
		return compSource(a, b);
	else
		return compNop(a, b);
}

template<typename T, blendOp_t::_blendOp_t op, typename U = T> U compRGB(const T pixelA, const T pixelB, const typename T::type alpha) noexcept
	{ return {compOp<op>(pixelA.r, pixelB.r, alpha), compOp<op>(pixelA.g, pixelB.g, alpha), compOp<op>(pixelA.b, pixelB.b, alpha)}; }
template<typename T, blendOp_t::_blendOp_t op> T compRGBA(const T pixelA, const T pixelB, const typename T::type) noexcept
	{ return {compRGB<T, op, typename T::pngRGBn_t>(pixelA, pixelB, pixelB.a), compOpA<op>(pixelA.a, pixelB.a)}; }
template<typename T, blendOp_t::_blendOp_t op, typename U = T> U compGrey(const T pixelA, const T pixelB, const typename T::type alpha) noexcept
	{ return {compOp<op>(pixelA.v, pixelB.v, alpha)}; }
template<typename T, blendOp_t::_blendOp_t op> T compGreyA(const T pixelA, const T pixelB, const typename T::type) noexcept
	{ return {compGrey<T, op, typename T::pngGreyN_t>(pixelA, pixelB, pixelB.a), compOpA<op>(pixelA.a, pixelB.a)}; }

template<typename T> pngRGB_t<T> pixelFromTransRGB(const uint16_t *const transValue) noexcept
	{ return pngRGB_t<T>(transValue[0], transValue[1], transValue[2]); }
template<typename T> pngGrey_t<T> pixelFromTransGrey(const uint16_t transValue) noexcept
	{ return pngGrey_t<T>(transValue); }

template<> pngRGB8_t bitmap_t::transparent<pngRGB8_t>() const noexcept { return pixelFromTransRGB<uint8_t>(transValue); }
template<> pngRGB16_t bitmap_t::transparent<pngRGB16_t>() const noexcept { return pixelFromTransRGB<uint16_t>(transValue); }
template<> pngGrey8_t bitmap_t::transparent<pngGrey8_t>() const noexcept { return pixelFromTransGrey<uint8_t>(transValue[0]); }
template<> pngGrey16_t bitmap_t::transparent<pngGrey16_t>() const noexcept { return pixelFromTransGrey<uint16_t>(transValue[0]); }

template<typename T> void compFrame(T compFunc(const T, const T, const typename T::type), const bitmap_t &source, bitmap_t &destination,
	const uint32_t xOffset, const uint32_t yOffset) noexcept
{
	if ((source.width() + xOffset) > destination.width() || (source.height() + yOffset) > destination.height())
		return;
	const T *const srcData = static_cast<const T *const>(static_cast<const void *const>(source.data()));
	T *const dstData = static_cast<T *const>(static_cast<void *const>(destination.data()));
	const uint32_t width = source.width();
	const uint32_t height = source.height();
	const T trans = source.transparent<T>();
	const typename T::type max = std::numeric_limits<typename T::type>::max();

	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			const uint32_t offsetSrc = x + (y * width);
			const uint32_t offsetDst = (x + xOffset) + ((y + yOffset) * destination.width());
			if (source.hasTransparency())
				dstData[offsetDst] = compFunc(dstData[offsetDst], srcData[offsetSrc], trans == srcData[offsetSrc] ? 0 : max);
			else
				dstData[offsetDst] = compFunc(dstData[offsetDst], srcData[offsetSrc], max);
		}
	}
}

#endif /*UTILITIES_HXX*/
