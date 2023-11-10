#include "stdafx.h"
#include "catch.h"
#include "BitmapIterator.h"
#include "ColorBitmap.h"


TEST_CASE("Gray Pixel Iterator", "[GrayPixelIterator]")
{
	SECTION("Reading with const gray iterator pixel by pixel")
	{
		typedef float T;
		constexpr int rows = 7;
		constexpr int cols = 5;
		auto bitmap = std::make_shared<CGrayBitmapT<T>>();
		bitmap->Init(cols, rows);
		std::iota(bitmap->m_vPixels.begin(), bitmap->m_vPixels.end(), 17);

		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> it{ bitmap };

		std::vector<T> result(rows * cols);
		const double m = bitmap->GetMultiplier();
		for (int y = 0, ndx = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++ndx, ++it)
			{
				result[ndx] = it.GetPixel() * m;
			}
		}

		REQUIRE(memcmp(bitmap->m_vPixels.data(), result.data(), rows * cols * sizeof(T)) == 0);
	}

	SECTION("Reading with iterator being type agnostic")
	{
		typedef std::uint16_t T;
		constexpr int rows = 11;
		constexpr int cols = 6;
		std::shared_ptr<CMemoryBitmap> bitmap = std::make_shared<CGrayBitmapT<T>>();
		bitmap->Init(cols, rows);
		auto* p = dynamic_cast<CGrayBitmapT<T>*>(bitmap.get());
		REQUIRE(p != nullptr);
		std::iota(p->m_vPixels.begin(), p->m_vPixels.end(), 42);

		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> it{ bitmap };

		std::vector<T> result(rows * cols);
		const double m = p->GetMultiplier();
		bool everythingOK = true;
		for (int y = 0, ndx = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++ndx, ++it)
			{
				double r, g, b;
				it.GetPixel(r, g, b);
				result[ndx] = r * m;
				if (r != g || r != b || g != b)
					everythingOK = false;
			}
		}

		REQUIRE(memcmp(p->m_vPixels.data(), result.data(), rows * cols * sizeof(T)) == 0);
		REQUIRE(everythingOK == true);
	}

	SECTION("Writing with non-const gray iterator pixel by pixel")
	{
		typedef std::uint16_t T;
		constexpr int rows = 6;
		constexpr int cols = 4;
		auto bitmap = std::make_shared<CGrayBitmapT<T>>();
		bitmap->Init(cols, rows);
		BitmapIterator<std::shared_ptr<CMemoryBitmap>> it{ bitmap };

		for (int y = 0, val = 4; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++val)
			{
				it.SetPixel(static_cast<double>(val));
				++it;
			}
		}

		std::vector<T> expected(rows * cols);
		T val = 4 * 256;
		std::for_each(expected.begin(), expected.end(), [&val](auto& v) { v = val; val += 256; });

		REQUIRE(memcmp(bitmap->m_vPixels.data(), expected.data(), sizeof(T) * rows * cols) == 0);
	}

	SECTION("Writing to a gray bitmap with a color iterator does nothing")
	{
		typedef std::uint16_t T;
		constexpr int rows = 6;
		constexpr int cols = 4;
		auto bitmap = std::make_shared<CGrayBitmapT<T>>();
		bitmap->Init(cols, rows);
		std::fill(bitmap->m_vPixels.begin(), bitmap->m_vPixels.end(), 13);
		BitmapIterator<std::shared_ptr<CMemoryBitmap>> it{ bitmap };

		for (int y = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++it)
			{
				it.SetPixel(9.0, 10.0, 13.5);
			}
		}

		std::vector<T> expected(rows * cols, 13);
		REQUIRE(memcmp(bitmap->m_vPixels.data(), expected.data(), sizeof(T) * rows * cols) == 0);
	}

	SECTION("Shift picture by x/y offset")
	{
		typedef std::uint16_t T;
		constexpr int rows = 6;
		constexpr int cols = 4;
		auto bitmap = std::make_shared<CGrayBitmapT<T>>();
		bitmap->Init(cols, rows);
		const double m = bitmap->GetMultiplier();
		std::fill(bitmap->m_vPixels.begin(), bitmap->m_vPixels.end(), 1);
		BitmapIterator<std::shared_ptr<CMemoryBitmap>> it{ bitmap };

		for (int y = 0; y < rows - 1; ++y)
		{
			it.Reset(cols / 2, y + 1);
			for (int x = cols/2; x < cols; ++x, ++it)
			{
				it.SetPixel(9);
			}
		}

		std::vector<T> expected(rows * cols, 1);
		for (size_t y = 1; y < rows; ++y)
			for (size_t x = cols / 2; x < cols; ++x)
				expected[y * cols + x] = static_cast<T>(9 * m);

		REQUIRE(memcmp(bitmap->m_vPixels.data(), expected.data(), sizeof(T) * rows * cols) == 0);
	}

	SECTION("Reading with stride 2")
	{
		typedef float T;
		constexpr int rows = 9;
		constexpr int cols = 8;
		auto bitmap = std::make_shared<CGrayBitmapT<T>>();
		bitmap->Init(cols, rows);
		const double m = bitmap->GetMultiplier();
		std::iota(bitmap->m_vPixels.begin(), bitmap->m_vPixels.end(), static_cast<T>(0));
		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> it{ bitmap };

		std::vector<double> result(rows * cols, 0);
		std::vector<double> expected(rows * cols, 0);
		for (int y = 0, ndx = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; x += 2, it += 2, ++ndx)
			{
				result[ndx] = it.GetPixel();
			}
		}

		for (size_t y = 0, v = 0, ndx = 0; y < rows; ++y)
			for (size_t x = 0; x < cols; x += 2, ++ndx, v += 2)
				expected[ndx] = static_cast<T>(static_cast<double>(v) / m);

		REQUIRE(memcmp(result.data(), expected.data(), sizeof(T) * rows * cols) == 0);
	}
}

TEST_CASE("Color Pixel Iterator", "[ColorPixelIterator]")
{
	SECTION("Reading with const color iterator pixel by pixel type agnostic")
	{
		typedef std::uint16_t T;
		constexpr int rows = 8;
		constexpr int cols = 7;
		std::shared_ptr<CMemoryBitmap> bitmap = std::make_shared<CColorBitmapT<T>>();
		bitmap->Init(cols, rows);
		auto* p = dynamic_cast<CColorBitmapT<T>*>(bitmap.get());
		REQUIRE(p != nullptr);
		std::iota(p->m_Red.m_vPixels.begin(), p->m_Red.m_vPixels.end(), 7);
		std::iota(p->m_Green.m_vPixels.begin(), p->m_Green.m_vPixels.end(), 8);
		std::iota(p->m_Blue.m_vPixels.begin(), p->m_Blue.m_vPixels.end(), 9);
		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> it{ bitmap };

		std::vector<T> rr(rows * cols), gr(rows * cols), br(rows * cols);
		const double m = p->GetMultiplier();
		for (int y = 0, ndx = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++ndx, ++it)
			{
				double r, g, b;
				it.GetPixel(r, g, b);
				rr[ndx] = r * m;
				gr[ndx] = g * m;
				br[ndx] = b * m;
			}
		}

		REQUIRE(memcmp(p->m_Red.m_vPixels.data(), rr.data(), rows * cols * sizeof(T)) == 0);
		REQUIRE(memcmp(p->m_Green.m_vPixels.data(), gr.data(), rows * cols * sizeof(T)) == 0);
		REQUIRE(memcmp(p->m_Blue.m_vPixels.data(), br.data(), rows * cols * sizeof(T)) == 0);
	}

	SECTION("Reading gray values from color bitmap returns L of HSL conversion")
	{
		typedef float T;
		constexpr int rows = 8;
		constexpr int cols = 5;
		std::shared_ptr<CMemoryBitmap> bitmap = std::make_shared<CColorBitmapT<T>>();
		bitmap->Init(cols, rows);
		auto* p = dynamic_cast<CColorBitmapT<T>*>(bitmap.get());
		REQUIRE(p != nullptr);
		std::fill(p->m_Red.m_vPixels.begin(), p->m_Red.m_vPixels.end(), 7);
		std::fill(p->m_Green.m_vPixels.begin(), p->m_Green.m_vPixels.end(), 8);
		std::fill(p->m_Blue.m_vPixels.begin(), p->m_Blue.m_vPixels.end(), 9);
		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> it{ bitmap };

		std::vector<double> vGray(rows * cols);
		const double m = p->GetMultiplier();
		for (int y = 0, ndx = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++ndx, ++it)
			{
				vGray[ndx] = it.GetPixel();
			}
		}

		constexpr double lVal = (7.0 + 9.0) / 510.0 * 255.0;
		std::vector<double> expected(rows * cols, lVal / m);
		REQUIRE(memcmp(expected.data(), vGray.data(), rows * cols * sizeof(T)) == 0);
	}

	SECTION("Writing with non-const color iterator pixel by pixel")
	{
		typedef std::uint32_t T;
		constexpr int rows = 6;
		constexpr int cols = 4;
		auto bitmap = std::make_shared<CColorBitmapT<T>>();
		bitmap->Init(cols, rows);
		BitmapIterator<std::shared_ptr<CMemoryBitmap>> it{ bitmap };

		for (int y = 0, r = 1, g = 11, b = 21; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++r, ++g, ++b)
			{
				it.SetPixel(static_cast<double>(r), static_cast<double>(g), static_cast<double>(b));
				++it;
			}
		}

		std::vector<T> rexp(rows * cols), gexp(rows * cols), bexp(rows * cols);
		constexpr T mult = 256 * 65536;
		T val = 1 * mult;
		std::for_each(rexp.begin(), rexp.end(), [&val, mult](auto& v) { v = val; val += mult; });
		val = 11 * mult;
		std::for_each(gexp.begin(), gexp.end(), [&val, mult](auto& v) { v = val; val += mult; });
		val = 21 * mult;
		std::for_each(bexp.begin(), bexp.end(), [&val, mult](auto& v) { v = val; val += mult; });

		REQUIRE(memcmp(dynamic_cast<CColorBitmapT<T>*>(bitmap.get())->m_Red.m_vPixels.data(), rexp.data(), sizeof(T) * rows * cols) == 0);
		REQUIRE(memcmp(dynamic_cast<CColorBitmapT<T>*>(bitmap.get())->m_Green.m_vPixels.data(), gexp.data(), sizeof(T) * rows * cols) == 0);
		REQUIRE(memcmp(dynamic_cast<CColorBitmapT<T>*>(bitmap.get())->m_Blue.m_vPixels.data(), bexp.data(), sizeof(T) * rows * cols) == 0);
	}

	SECTION("Performance of reading large bitmap with color iterator")
	{
		typedef std::uint16_t T;
		constexpr int rows = 6000;
		constexpr int cols = 3500;
		std::shared_ptr<CMemoryBitmap> bitmap = std::make_shared<CColorBitmapT<T>>();
		bitmap->Init(cols, rows);
		auto* p = dynamic_cast<CColorBitmapT<T>*>(bitmap.get());
		REQUIRE(p != nullptr);
		std::iota(p->m_Red.m_vPixels.begin(), p->m_Red.m_vPixels.end(), 0);
		std::iota(p->m_Green.m_vPixels.begin(), p->m_Green.m_vPixels.end(), 1);
		std::iota(p->m_Blue.m_vPixels.begin(), p->m_Blue.m_vPixels.end(), 2);
		const double m = p->GetMultiplier();

		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> it{ bitmap };

		std::vector<T> result(rows * cols, 0);
		const double t1 = omp_get_wtime();
		for (int y = 0, ndx = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x, ++ndx, ++it)
			{
				double r, g, b;
				it.GetPixel(r, g, b);
				result[ndx] = static_cast<T>(r);
			}
		}
		const double delta_t = omp_get_wtime() - t1;
		printf("Color bitmap iterator const reads %6.1f MPix/s (3 colors each)\n", (static_cast<double>(rows) * cols) / delta_t / 1e6);

		REQUIRE(result[0] == 0);
		REQUIRE(result[1000] == static_cast<T>(1000.0 / m));
	}
}
