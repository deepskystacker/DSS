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

namespace
{
	constexpr double arcsec = 1.0 / 3600.0;

	// Reference point and CTYPEs for a plate solved image, but no matrix.
	DSS::WCSKeywords makeKeys()
	{
		DSS::WCSKeywords k;
		k.crpix1 = 512.0;
		k.crpix2 = 384.0;
		k.crval1 = 83.8;
		k.crval2 = -5.4;
		k.ctype1 = "RA---TAN";
		k.ctype2 = "DEC--TAN";
		return k;
	}

	void checkMatrix(const DSS::WCSInfo& w, double cd11, double cd12, double cd21, double cd22)
	{
		CHECK(w.cd11 == Catch::Approx(cd11).margin(1e-15));
		CHECK(w.cd12 == Catch::Approx(cd12).margin(1e-15));
		CHECK(w.cd21 == Catch::Approx(cd21).margin(1e-15));
		CHECK(w.cd22 == Catch::Approx(cd22).margin(1e-15));
	}
}

TEST_CASE("WCS keywords: CD matrix is used directly", "[WCS][keywords]")
{
	auto k = makeKeys();
	k.cd11 = -2e-4; k.cd12 = 1e-5; k.cd21 = 2e-5; k.cd22 = 3e-4;
	// CD takes precedence over anything else that is present.
	k.cdelt1 = 1.0; k.cdelt2 = 1.0; k.pc11 = 5.0; k.crota2 = 45.0;

	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	CHECK(w.ok);
	CHECK(w.crpix1 == 512.0);
	CHECK(w.crpix2 == 384.0);
	CHECK(w.crval1 == 83.8);
	CHECK(w.crval2 == -5.4);
	checkMatrix(w, -2e-4, 1e-5, 2e-5, 3e-4);
	CHECK(w.matrixSource == DSS::WCSInfo::MatrixSource::CD);
}

TEST_CASE("WCS keywords: missing CD elements default to zero", "[WCS][keywords]")
{
	auto k = makeKeys();
	k.cd11 = -arcsec; k.cd22 = arcsec;

	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	checkMatrix(w, -arcsec, 0.0, 0.0, arcsec);
}

TEST_CASE("WCS keywords: CD = CDELT * PC", "[WCS][keywords]")
{
	auto k = makeKeys();
	k.cdelt1 = -2.0 * arcsec; k.cdelt2 = 3.0 * arcsec;
	k.pc11 = 0.8; k.pc12 = -0.6; k.pc21 = 0.6; k.pc22 = 0.8;
	k.crota2 = 45.0;	// Must be ignored when a PC matrix is present.

	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	// Row i is scaled by CDELTi.
	checkMatrix(w, -2.0 * arcsec * 0.8, -2.0 * arcsec * -0.6, 3.0 * arcsec * 0.6, 3.0 * arcsec * 0.8);
	CHECK(w.matrixSource == DSS::WCSInfo::MatrixSource::CdeltPC);
}

TEST_CASE("WCS keywords: missing PC elements default to identity", "[WCS][keywords]")
{
	auto k = makeKeys();
	k.cdelt1 = -arcsec; k.cdelt2 = arcsec;
	k.pc12 = 0.01;

	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	checkMatrix(w, -arcsec, -arcsec * 0.01, 0.0, arcsec);
}

TEST_CASE("WCS keywords: CDELT defaults to 1 with a PC matrix", "[WCS][keywords]")
{
	auto k = makeKeys();
	k.pc11 = -2e-4; k.pc12 = 1e-5; k.pc21 = 2e-5; k.pc22 = 3e-4;

	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	checkMatrix(w, -2e-4, 1e-5, 2e-5, 3e-4);
}

TEST_CASE("WCS keywords: CDELT alone is an unrotated image", "[WCS][keywords]")
{
	auto k = makeKeys();
	k.cdelt1 = -arcsec; k.cdelt2 = arcsec;

	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	checkMatrix(w, -arcsec, 0.0, 0.0, arcsec);
	CHECK(w.rotationAngle() == Catch::Approx(0.0).margin(1e-9));
	CHECK(w.pixelScale() == Catch::Approx(1.0).margin(1e-9));
	CHECK(w.matrixSource == DSS::WCSInfo::MatrixSource::CdeltCrota);
}

TEST_CASE("WCS keywords: CDELT + CROTA2", "[WCS][keywords]")
{
	for (const double rotation : { 0.0, 33.0, -120.0, 180.0 })
	{
		auto k = makeKeys();
		k.cdelt1 = -arcsec; k.cdelt2 = arcsec;
		k.crota2 = rotation;

		DSS::WCSInfo w;
		REQUIRE(w.setFromKeywords(k));
		const auto expected = makeWcs(83.8, -5.4, 512.0, 384.0, rotation);
		checkMatrix(w, expected.cd11, expected.cd12, expected.cd21, expected.cd22);
		CHECK(w.pixelScale() == Catch::Approx(1.0).margin(1e-9));
	}

	// Non-square pixels: column j of the rotation is scaled by CDELTj
	// for the off-diagonal terms.
	auto k = makeKeys();
	k.cdelt1 = -2.0 * arcsec; k.cdelt2 = 3.0 * arcsec;
	k.crota2 = 30.0;
	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	const double c = std::cos(M_PI / 6.0), s = std::sin(M_PI / 6.0);
	checkMatrix(w, -2.0 * arcsec * c, -3.0 * arcsec * s, -2.0 * arcsec * s, 3.0 * arcsec * c);
}

TEST_CASE("WCS keywords: incomplete headers are rejected", "[WCS][keywords]")
{
	DSS::WCSInfo w;

	// No matrix information at all.
	CHECK_FALSE(w.setFromKeywords(makeKeys()));
	CHECK_FALSE(w.ok);
	CHECK(w.matrixSource == DSS::WCSInfo::MatrixSource::None);

	// Only one CDELT and nothing else.
	auto k = makeKeys();
	k.cdelt1 = -arcsec;
	CHECK_FALSE(w.setFromKeywords(k));

	// CROTA2 without CDELT.
	k = makeKeys();
	k.crota2 = 10.0;
	CHECK_FALSE(w.setFromKeywords(k));

	// Missing reference value / pixel.
	k = makeKeys();
	k.cd11 = -arcsec; k.cd22 = arcsec;
	k.crval2.reset();
	CHECK_FALSE(w.setFromKeywords(k));
	k = makeKeys();
	k.cd11 = -arcsec; k.cd22 = arcsec;
	k.crpix1.reset();
	CHECK_FALSE(w.setFromKeywords(k));

	// Singular matrices.
	k = makeKeys();
	k.cd11 = -arcsec;
	CHECK_FALSE(w.setFromKeywords(k));
	k = makeKeys();
	k.cdelt1 = 0.0; k.cdelt2 = arcsec;
	CHECK_FALSE(w.setFromKeywords(k));
}

TEST_CASE("WCS keywords: a failed read clears a previous success", "[WCS][keywords]")
{
	auto k = makeKeys();
	k.cd11 = -arcsec; k.cd22 = arcsec;
	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	CHECK_FALSE(w.setFromKeywords(DSS::WCSKeywords{}));
	CHECK_FALSE(w.ok);
}

TEST_CASE("WCS keywords: CTYPE and CUNIT", "[WCS][keywords]")
{
	auto good = makeKeys();
	good.cd11 = -arcsec; good.cd22 = arcsec;
	DSS::WCSInfo w;

	// Distortion suffix, padding and case are tolerated.
	auto k = good;
	k.ctype1 = "RA---TAN-SIP"; k.ctype2 = "DEC--TAN-SIP";
	CHECK(w.setFromKeywords(k));
	k = good;
	k.ctype1 = " ra---tan "; k.ctype2 = "dec--tan";
	CHECK(w.setFromKeywords(k));
	k = good;
	k.cunit1 = "deg"; k.cunit2 = "DEG ";
	CHECK(w.setFromKeywords(k));

	// Missing CTYPE means a linear axis, not RA/Dec.
	k = good;
	k.ctype1.clear(); k.ctype2.clear();
	CHECK_FALSE(w.setFromKeywords(k));

	// Swapped axes.
	k = good;
	k.ctype1 = "DEC--TAN"; k.ctype2 = "RA---TAN";
	CHECK_FALSE(w.setFromKeywords(k));

	// Other projections and coordinate systems.
	k = good;
	k.ctype1 = "RA---SIN"; k.ctype2 = "DEC--SIN";
	CHECK_FALSE(w.setFromKeywords(k));
	k = good;
	k.ctype1 = "GLON-TAN"; k.ctype2 = "GLAT-TAN";
	CHECK_FALSE(w.setFromKeywords(k));

	// Units other than degrees.
	k = good;
	k.cunit1 = "rad"; k.cunit2 = "rad";
	CHECK_FALSE(w.setFromKeywords(k));
}
