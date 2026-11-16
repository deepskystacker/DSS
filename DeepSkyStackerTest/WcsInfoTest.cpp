#include "pch.h"
#include "catch_amalgamated.hpp"
#include "wcsinfo.h"

namespace
{
	// North-up, east-left image, 1 arcsec/pixel, rotated by rotDeg.
	DSS::WCSInfo makeWcs(double ra, double dec, double crpix1, double crpix2, double rotDeg = 0.0)
	{
		constexpr double scale = 1.0 / 3600.0;
		const double r = rotDeg * M_PI / 180.0;
		DSS::WCSInfo w;
		w.crval1 = ra;
		w.crval2 = dec;
		w.crpix1 = crpix1;
		w.crpix2 = crpix2;
		w.cd11 = -scale * std::cos(r);
		// AIPS convention with cdelt1 = -scale, cdelt2 = +scale.
		w.cd12 = -scale * std::sin(r);
		w.cd21 = -scale * std::sin(r);
		w.cd22 = scale * std::cos(r);
		w.ok = true;
		return w;
	}
}

TEST_CASE("WCS reference pixel maps to reference value", "[WCS]")
{
	const auto w = makeWcs(83.8, 60.0, 512.0, 384.0, 30.0);
	const auto [ra, dec] = w.pixelToWorld(512.0, 384.0);
	CHECK(ra == Catch::Approx(83.8).margin(1e-12));
	CHECK(dec == Catch::Approx(60.0).margin(1e-12));
}

TEST_CASE("WCS RA offsets are scaled by 1/cos(dec)", "[WCS]")
{
	// 0.1 degree east-west on the tangent plane (360 pixels at 1"/px).
	const auto w = makeWcs(100.0, 60.0, 0.0, 0.0);
	const auto [ra, dec] = w.pixelToWorld(-360.0, 0.0);	// east = left = -x
	CHECK(ra - 100.0 == Catch::Approx(0.2).margin(0.001));
	CHECK(dec == Catch::Approx(60.0).margin(0.001));
}

TEST_CASE("WCS pixel to world and back round trips", "[WCS]")
{
	for (const double dec0 : { -75.0, 0.0, 45.0, 80.0 })
	{
		const auto w = makeWcs(210.0, dec0, 1000.0, 800.0, 37.0);
		for (const auto [x, y] : { std::pair{ 0.0, 0.0 }, std::pair{ 2000.0, 1600.0 }, std::pair{ 1000.0, 1.0 }, std::pair{ 123.4, 1500.6 } })
		{
			const auto [ra, dec] = w.pixelToWorld(x, y);
			const auto back = w.worldToPixel(ra, dec);
			REQUIRE(back.has_value());
			CHECK(std::get<0>(*back) == Catch::Approx(x).margin(1e-6));
			CHECK(std::get<1>(*back) == Catch::Approx(y).margin(1e-6));
		}
	}
}

TEST_CASE("WCS RA wraps at 0/360", "[WCS]")
{
	const auto w = makeWcs(0.001, 0.0, 0.0, 0.0);
	const auto [ra, dec] = w.pixelToWorld(100.0, 0.0);	// west of the reference
	CHECK(ra >= 0.0);
	CHECK(ra < 360.0);
	CHECK(ra == Catch::Approx(360.0 + 0.001 - 100.0 / 3600.0).margin(1e-6));

	const auto back = w.worldToPixel(ra, dec);
	REQUIRE(back.has_value());
	CHECK(std::get<0>(*back) == Catch::Approx(100.0).margin(1e-6));
}

TEST_CASE("WCS worldToPixel fails for singular matrix and far hemisphere", "[WCS]")
{
	auto w = makeWcs(10.0, 0.0, 0.0, 0.0);
	CHECK(w.worldToPixel(190.0, 0.0) == std::nullopt);	// opposite side of sky

	w.cd11 = w.cd12 = w.cd21 = w.cd22 = 0.0;
	CHECK(w.worldToPixel(10.0, 0.0) == std::nullopt);

	w.cd11 = 1e-4; w.cd12 = 2e-4; w.cd21 = 2e-4; w.cd22 = 4e-4;	// det == 0
	CHECK(w.worldToPixel(10.0, 0.0) == std::nullopt);
}

TEST_CASE("WCS pixel scale and rotation angle", "[WCS]")
{
	CHECK(makeWcs(10.0, 20.0, 0.0, 0.0).pixelScale() == Catch::Approx(1.0).margin(1e-9));
	CHECK(makeWcs(10.0, 20.0, 0.0, 0.0, 33.0).pixelScale() == Catch::Approx(1.0).margin(1e-9));

	// Unrotated, north-up image must report 0 (not 180) degrees.
	CHECK(makeWcs(10.0, 20.0, 0.0, 0.0).rotationAngle() == Catch::Approx(0.0).margin(1e-9));
	CHECK(makeWcs(10.0, 20.0, 0.0, 0.0, 33.0).rotationAngle() == Catch::Approx(33.0).margin(1e-9));
	CHECK(makeWcs(10.0, 20.0, 0.0, 0.0, -120.0).rotationAngle() == Catch::Approx(-120.0).margin(1e-9));
}

TEST_CASE("WCS rotationFrom is reference minus this, wrapped", "[WCS]")
{
	const auto ref = makeWcs(10.0, 20.0, 0.0, 0.0, 10.0);
	CHECK(makeWcs(10.0, 20.0, 0.0, 0.0, 25.0).rotationFrom(ref) == Catch::Approx(-15.0).margin(1e-9));
	// 170 -> -170 is a 20 degree step, not 340.
	CHECK(makeWcs(10.0, 20.0, 0.0, 0.0, 170.0).rotationFrom(makeWcs(10.0, 20.0, 0.0, 0.0, -170.0)) == Catch::Approx(20.0).margin(1e-9));
}

TEST_CASE("WCS offsetXY with identical pointing is zero", "[WCS]")
{
	const auto ref = makeWcs(150.0, 30.0, 100.0, 100.0, 5.0);
	const auto offset = ref.offsetXY(ref);
	REQUIRE(offset.has_value());
	CHECK(std::get<0>(*offset) == Catch::Approx(0.0).margin(1e-6));
	CHECK(std::get<1>(*offset) == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("WCS offsetXY accounts for differing crpix", "[WCS]")
{
	// Same sky pointing, but this frame's reference pixel is elsewhere:
	// its crpix pixel lands on the reference's crpix pixel.
	const auto ref = makeWcs(150.0, 30.0, 100.0, 100.0);
	const auto frame = makeWcs(150.0, 30.0, 50.0, 60.0);
	const auto offset = frame.offsetXY(ref);
	REQUIRE(offset.has_value());
	CHECK(std::get<0>(*offset) == Catch::Approx(50.0).margin(1e-6));
	CHECK(std::get<1>(*offset) == Catch::Approx(40.0).margin(1e-6));
}

TEST_CASE("WCS offsetXY recovers a known pixel shift", "[WCS]")
{
	const auto ref = makeWcs(150.0, 70.0, 100.0, 100.0, 20.0);
	// Frame pointed at the sky position that is 10,20 pixels from the reference crpix.
	const auto [ra, dec] = ref.pixelToWorld(110.0, 120.0);
	const auto frame = makeWcs(ra, dec, 100.0, 100.0, 20.0);
	const auto offset = frame.offsetXY(ref);
	REQUIRE(offset.has_value());
	CHECK(std::get<0>(*offset) == Catch::Approx(10.0).margin(1e-6));
	CHECK(std::get<1>(*offset) == Catch::Approx(20.0).margin(1e-6));
}
