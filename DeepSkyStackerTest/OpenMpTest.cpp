#include "stdafx.h"
#include "catch.h"
#include "avx_bitmap_filler.h"
#include "GrayBitmap.h"
#include "BitMapFiller.h"
#include "MedianFilterEngine.h"

TEST_CASE("OpenMP parallelization", "[OpenMP]")
{
	SECTION("Schedule static")
	{
		std::vector<float> data(8 * 1000);
		float x = 0;
		std::for_each(data.begin(), data.end(), [&x](float& v) { v = x++; });
		float* p = data.data();
#pragma omp parallel for schedule(static)
		for (int i = 0; i < 8000; ++i)
			p[i] *= 2.0f;

		std::vector<float> expected(data.size());
		x = 0;
		std::for_each(expected.begin(), expected.end(), [&x](float& v) { v = x++; v *= 2.0f; });

		REQUIRE(omp_get_num_procs() > 1);
		REQUIRE(memcmp(data.data(), expected.data(), data.size() * sizeof(float)) == 0);
	}

	SECTION("Schedule dynamic")
	{
		std::vector<float> data(8 * 1000);
		float x = 0;
		std::for_each(data.begin(), data.end(), [&x](float& v) { v = x++; });
		float* p = data.data();
#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < 8000; ++i)
			p[i] += 2.0f;

		std::vector<float> expected(data.size());
		x = 0;
		std::for_each(expected.begin(), expected.end(), [&x](float& v) { v = x++; v += 2.0f; });

		REQUIRE(omp_get_num_procs() > 1);
		REQUIRE(memcmp(data.data(), expected.data(), data.size() * sizeof(float)) == 0);
	}

	SECTION("Schedule guided")
	{
		std::vector<float> data(8 * 1000);
		float x = 0;
		std::for_each(data.begin(), data.end(), [&x](float& v) { v = x++; });
		float* p = data.data();
#pragma omp parallel for schedule(guided, 100)
		for (int i = 0; i < 8000; ++i)
			p[i] = std::sqrt(p[i]);

		std::vector<float> expected(data.size());
		x = 0;
		std::for_each(expected.begin(), expected.end(), [&x](float& v) { v = x++; v = std::sqrt(v); });

		REQUIRE(omp_get_num_procs() > 1);
		REQUIRE(memcmp(data.data(), expected.data(), data.size() * sizeof(float)) == 0);
	}

	SECTION("Parallel and for loop separated")
	{
		std::vector<float> data(8 * 1000);
		float x = 0;
		const auto func = [&]() {
			std::for_each(data.begin(), data.end(), [&x](float& v) { v = x++; });
			float* p = data.data();
#pragma omp parallel
			{
#pragma omp for schedule(guided, 10)
				for (int i = 0; i < 8000; ++i)
					p[i] = std::sqrt(p[i]);
			}
		};

		REQUIRE_NOTHROW(func());

		std::vector<float> expected(data.size());
		x = 0;
		std::for_each(expected.begin(), expected.end(), [&x](float& v) { v = x++; v = std::sqrt(v); });

		REQUIRE(omp_get_num_procs() > 1);
		REQUIRE(memcmp(data.data(), expected.data(), data.size() * sizeof(float)) == 0);
	}

	SECTION("Firstprivate simple")
	{
		std::vector<float> data(8 * 1000);
		float x = 0;
		const auto func = [&]() {
			std::for_each(data.begin(), data.end(), [&x](float& v) { v = x++; });
			float* p = data.data();

#pragma omp parallel for firstprivate(p) schedule(guided, 10)
				for (int i = 0; i < 8000; ++i)
					p[i] = std::sqrt(p[i]);
		};

		REQUIRE_NOTHROW(func());

		std::vector<float> expected(data.size());
		x = 0;
		std::for_each(expected.begin(), expected.end(), [&x](float& v) { v = x++; v = std::sqrt(v); });

		REQUIRE(omp_get_num_procs() > 1);
		REQUIRE(memcmp(data.data(), expected.data(), data.size() * sizeof(float)) == 0);
	}

	SECTION("Critical")
	{
		std::vector<float> data(8 * 1000);
		float x = 0;
		float sum = 0;
		const auto func = [&]() {
			std::for_each(data.begin(), data.end(), [&x](float& v) { v = x++; });
			float* p = data.data();
#pragma omp parallel
			{
#pragma omp for schedule(guided, 10)
				for (int i = 0; i < 8000; ++i)
					p[i] = p[i] * 0.01f;
#pragma omp for schedule(dynamic)
				for (int i = 0; i < 8000; ++i)
				{
#pragma omp critical(OpenMpUnitTest_1)
					sum += p[i];
				}
			}
		};

		REQUIRE_NOTHROW(func());

		std::vector<float> expected(data.size());
		x = 0;
		std::for_each(expected.begin(), expected.end(), [&x](float& v) { v = x++; v *= 0.01f; });

		REQUIRE(omp_get_num_procs() > 1);
		REQUIRE(memcmp(data.data(), expected.data(), data.size() * sizeof(float)) == 0);
		REQUIRE(std::abs(sum - 7999.0f * 8000.0f * 0.5f * 0.01f) < 1);
	}

	SECTION("Firstprivate with complex object")
	{
		std::vector<int> glob(65536, 0);

		const auto func = [&glob]() {
			std::vector<float> data(8 * 1000);
			float x = 0;
			std::for_each(data.begin(), data.end(), [&x](float& v) { v = x++; x++; });
			float* p = data.data();
			std::vector<int> priv(65536, 0);
#pragma omp parallel firstprivate(priv) if(1)
			{
#pragma omp for schedule(static, 10)
				for (int i = 0; i < 8000; ++i)
				{
					++priv[static_cast<size_t>(p[i])];
				}
#pragma omp critical(OpenMpUnitTest_2)
				for (size_t i = 0; i < priv.size(); ++i)
					glob[i] += priv[i];
			}
		};

		REQUIRE_NOTHROW(func());

		std::vector<int> expected(glob.size(), 0);
		for (size_t i = 0; i < 16000; i += 2)
			expected[i] = 1;

		REQUIRE(omp_get_num_procs() > 1);
		REQUIRE(memcmp(glob.data(), expected.data(), glob.size() * sizeof(int)) == 0);
	}

	SECTION("Firstprivate with BitmapFiller")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		pBitmap->Init(3, 2);
		CopyableSmartPtr<BitmapFillerInterface> filler = std::make_unique<NonAvxBitmapFiller>(pBitmap.get(), nullptr, 1.0, 1.0, 1.0);
		filler->setGrey(true);
#pragma warning (suppress: 4189)
		const bool isThreadSafe = filler->isThreadSafe();
//#pragma omp parallel firstprivate(filler) if(filler->isThreadSafe()) // Visual Studio 2019, V 16.10.4: Access violation here
#pragma omp parallel for firstprivate(filler) if(isThreadSafe)
		for (int line = 0; line < 2; ++line)
		{
			filler->setMaxColors(255);
			filler->setWidth(3);
			filler->setHeight(2);
			std::uint8_t inputData[3] = { 19, 45, 243 };
			REQUIRE(filler->Write(inputData, 1, 3, line) == 3);
		}
	}

	class OmpTest
	{
		std::vector<float> v;
		size_t width;
	public:
		OmpTest(const size_t w, const size_t h) : v(w * h, 0.0f), width{ w } {}
		OmpTest(const OmpTest& rhs) : v{ rhs.v }, width{ rhs.width } {}
		~OmpTest() = default;
		virtual bool isThreadSafe() const { return true; }
		size_t process(const int line, const std::uint8_t* p)
		{
			for (size_t n = line * width; n < (line + 1) * width; ++n)
				v[n] = static_cast<float>(*p++);
			return width;
		}
	};

	SECTION("Firstprivate with class")
	{
		OmpTest tc(3, 2);
#pragma warning (suppress: 4189)
		const bool isThreadSafe = tc.isThreadSafe();
//#pragma omp parallel for firstprivate(tc) if(tc.isThreadSafe()) // Crashes in VS 2019 V 16.10.4
#pragma omp parallel for firstprivate(tc) if(isThreadSafe)
		for (int line = 0; line < 2; ++line)
		{
			using T = std::uint8_t;
			T data[3] = { static_cast<T>(17 + line), static_cast<T>(29 + 2 * line), static_cast<T>(51 + 3 * line) };
			REQUIRE(tc.process(line, data) == 3);
		}
	}
}
