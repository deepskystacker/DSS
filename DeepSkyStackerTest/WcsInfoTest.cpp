#include "pch.h"
#include "catch_amalgamated.hpp"
#include "wcsinfo.h"

namespace
{
	// North-up, east-left image, scaleArcsec arcsec/pixel, rotated by rotDeg.
	DSS::WCSInfo makeWcs(double ra, double dec, double crpix1, double crpix2, double rotDeg = 0.0, double scaleArcsec = 1.0)
	{
		const double scale = scaleArcsec / 3600.0;
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

namespace
{
	constexpr int frameWidth = 3000;
	constexpr int frameHeight = 2000;
	constexpr double degrees = M_PI / 180.0;

	// The same image with its rows in the opposite order (which is what a
	// plate solver sees for a file stored top-down): the Y axis is mirrored.
	DSS::WCSInfo mirrored(DSS::WCSInfo w)
	{
		w.cd12 = -w.cd12;
		w.cd22 = -w.cd22;
		return w;
	}

	// Largest difference in pixels, over a grid covering the frame, between
	// the transformation and the exact mapping through the sky.
	double worstError(const CBilinearParameters& transformation, const DSS::WCSInfo& frame, const DSS::WCSInfo& ref)
	{
		double worst = 0.0;
		for (int x = 0; x <= frameWidth; x += frameWidth / 6)
			for (int y = 0; y <= frameHeight; y += frameHeight / 4)
			{
				const auto exact = frame.mapTo(ref, x, y);
				REQUIRE(exact.has_value());
				const QPointF pt = transformation.transform(QPointF(x, y));
				worst = std::max(worst, std::abs(pt.x() - std::get<0>(*exact)));
				worst = std::max(worst, std::abs(pt.y() - std::get<1>(*exact)));
			}
		return worst;
	}
}

TEST_CASE("WCS toBitmapCoordinates", "[WCS][transformation]")
{
	const auto fits = makeWcs(150.0, 40.0, 1500.5, 1000.25, 12.0);
	const auto [ra, dec] = fits.pixelToWorld(321.0, 654.0);	// FITS (1-based) pixel

	SECTION("top-down: pixel centres become 0-based")
	{
		auto w = fits;
		w.toBitmapCoordinates(frameHeight, false);
		CHECK(w.crpix1 == Catch::Approx(1499.5).margin(1e-12));
		CHECK(w.crpix2 == Catch::Approx(999.25).margin(1e-12));
		const auto [ra2, dec2] = w.pixelToWorld(320.0, 653.0);
		CHECK(ra2 == Catch::Approx(ra).margin(1e-11));
		CHECK(dec2 == Catch::Approx(dec).margin(1e-11));
	}

	SECTION("bottom-up: the rows are reversed as well")
	{
		auto w = fits;
		w.toBitmapCoordinates(frameHeight, true);
		// FITS row 654 (1-based) is loaded into bitmap row height - 654.
		const auto [ra2, dec2] = w.pixelToWorld(320.0, frameHeight - 654.0);
		CHECK(ra2 == Catch::Approx(ra).margin(1e-11));
		CHECK(dec2 == Catch::Approx(dec).margin(1e-11));
		// Reversing the rows changes the parity of the image.
		CHECK((w.cd11 * w.cd22 - w.cd12 * w.cd21) * (fits.cd11 * fits.cd22 - fits.cd12 * fits.cd21) < 0.0);
	}
}

TEST_CASE("WCS transformation between identical frames is the identity", "[WCS][transformation]")
{
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 5.0);
	const auto t = ref.transformationTo(ref, frameWidth, frameHeight);
	REQUIRE(t.has_value());
	CHECK(t->Type == TT_BILINEAR);
	CHECK(t->a0 == Catch::Approx(0.0).margin(1e-9));
	CHECK(t->a1 == Catch::Approx(1.0).margin(1e-9));
	CHECK(t->a2 == Catch::Approx(0.0).margin(1e-9));
	CHECK(t->a3 == Catch::Approx(0.0).margin(1e-9));
	CHECK(t->b0 == Catch::Approx(0.0).margin(1e-9));
	CHECK(t->b1 == Catch::Approx(0.0).margin(1e-9));
	CHECK(t->b2 == Catch::Approx(1.0).margin(1e-9));
	CHECK(t->b3 == Catch::Approx(0.0).margin(1e-9));
	CHECK(t->Angle(frameWidth) == Catch::Approx(0.0).margin(1e-9));
}

TEST_CASE("WCS transformation recovers a known pixel shift", "[WCS][transformation]")
{
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 20.0);
	// Frame pointed at the sky position 10,20 pixels from the reference crpix.
	const auto [ra, dec] = ref.pixelToWorld(1510.0, 1020.0);
	const auto frame = makeWcs(ra, dec, 1500.0, 1000.0, 20.0);
	const auto t = frame.transformationTo(ref, frameWidth, frameHeight);
	REQUIRE(t.has_value());

	// The shift is exact at the frame's reference pixel.
	const QPointF centre = t->transform(QPointF(1500.0, 1000.0));
	CHECK(centre.x() == Catch::Approx(1510.0).margin(1e-3));
	CHECK(centre.y() == Catch::Approx(1020.0).margin(1e-3));

	// Away from it the meridians converge, so the field is very slightly
	// rotated between the two tangent points (about 0.003 degrees here),
	// which moves pixel (0, 0) by a fraction of a pixel more.
	double xOffset = 0.0, yOffset = 0.0;
	t->Offsets(xOffset, yOffset);
	CHECK(xOffset == Catch::Approx(10.0).margin(0.2));
	CHECK(yOffset == Catch::Approx(20.0).margin(0.2));
	CHECK(t->Angle(frameWidth) == Catch::Approx(0.0).margin(1e-3));
	CHECK(worstError(*t, frame, ref) < 0.01);

	// A different crpix for the same pointing is a shift too.
	const auto frame2 = makeWcs(150.0, 30.0, 1450.0, 960.0, 20.0);
	const auto t2 = frame2.transformationTo(ref, frameWidth, frameHeight);
	REQUIRE(t2.has_value());
	t2->Offsets(xOffset, yOffset);
	CHECK(xOffset == Catch::Approx(50.0).margin(1e-6));
	CHECK(yOffset == Catch::Approx(40.0).margin(1e-6));
}

TEST_CASE("WCS transformation rotation has the right sign for either parity", "[WCS][transformation]")
{
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 10.0);
	const auto frame = makeWcs(150.0, 30.0, 1500.0, 1000.0, 25.0);

	SECTION("normal parity")
	{
		const auto t = frame.transformationTo(ref, frameWidth, frameHeight);
		REQUIRE(t.has_value());
		CHECK(t->Angle(frameWidth) == Catch::Approx(-15.0 * degrees).margin(1e-9));
		// The rotation is about the common reference pixel ...
		const QPointF centre = t->transform(QPointF(1500.0, 1000.0));
		CHECK(centre.x() == Catch::Approx(1500.0).margin(1e-6));
		CHECK(centre.y() == Catch::Approx(1000.0).margin(1e-6));
		// ... so the offset of pixel (0, 0) is far from zero.
		double xOffset = 0.0, yOffset = 0.0;
		t->Offsets(xOffset, yOffset);
		CHECK(std::hypot(xOffset, yOffset) > 100.0);
		CHECK(worstError(*t, frame, ref) < 1e-6);
	}

	SECTION("mirrored frames rotate the other way")
	{
		const auto t = mirrored(frame).transformationTo(mirrored(ref), frameWidth, frameHeight);
		REQUIRE(t.has_value());
		CHECK(t->Angle(frameWidth) == Catch::Approx(15.0 * degrees).margin(1e-9));
		CHECK(worstError(*t, mirrored(frame), mirrored(ref)) < 1e-6);
	}

	SECTION("meridian flip")
	{
		const auto flipped = makeWcs(150.0, 30.0, 1500.0, 1000.0, 190.0);
		const auto t = flipped.transformationTo(ref, frameWidth, frameHeight);
		REQUIRE(t.has_value());
		CHECK(std::abs(t->Angle(frameWidth)) == Catch::Approx(180.0 * degrees).margin(1e-9));
		const QPointF corner = t->transform(QPointF(0.0, 0.0));
		CHECK(corner.x() == Catch::Approx(3000.0).margin(1e-6));
		CHECK(corner.y() == Catch::Approx(2000.0).margin(1e-6));
	}
}

TEST_CASE("WCS transformation is refused for frames of opposite parity", "[WCS][transformation]")
{
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 10.0);
	const auto frame = makeWcs(150.0, 30.0, 1500.0, 1000.0, 25.0);
	CHECK_FALSE(mirrored(frame).transformationTo(ref, frameWidth, frameHeight).has_value());
	CHECK_FALSE(frame.transformationTo(mirrored(ref), frameWidth, frameHeight).has_value());

	// Nor is there one for frames on opposite sides of the sky, or for
	// nonsensical sizes.
	CHECK_FALSE(makeWcs(330.0, -30.0, 1500.0, 1000.0).transformationTo(ref, frameWidth, frameHeight).has_value());
	CHECK_FALSE(frame.transformationTo(ref, 0, frameHeight).has_value());
	CHECK_FALSE(frame.transformationTo(ref, frameWidth, frameHeight, 0).has_value());
}

TEST_CASE("WCS transformation follows the exact mapping across the frame", "[WCS][transformation]")
{
	// Dithered and slightly rotated frames, including close to the pole
	// where the field rotates quickly with RA.
	for (const double dec0 : { -60.0, 0.0, 45.0, 85.0 })
	{
		const auto ref = makeWcs(210.0, dec0, 1500.0, 1000.0, 37.0);
		const auto [ra, dec] = ref.pixelToWorld(1560.0, 960.0);
		const auto frame = makeWcs(ra, dec, 1480.0, 1015.0, 39.5);
		const auto t = frame.transformationTo(ref, frameWidth, frameHeight);
		REQUIRE(t.has_value());
		CHECK(worstError(*t, frame, ref) < 0.01);
	}
}

TEST_CASE("WCS transformation with super-pixel binning", "[WCS][transformation]")
{
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 10.0);
	const auto [ra, dec] = ref.pixelToWorld(1530.0, 990.0);
	const auto frame = makeWcs(ra, dec, 1500.0, 1000.0, 13.0);

	// Both rendered at half size: rendered pixel r is centred on WCS
	// pixel 2r + 0.5.
	const auto t = frame.transformationTo(ref, frameWidth / 2, frameHeight / 2, 2, 2);
	REQUIRE(t.has_value());
	for (const auto [x, y] : { std::pair{ 0.0, 0.0 }, std::pair{ 750.0, 500.0 }, std::pair{ 1400.0, 30.0 }, std::pair{ 12.0, 990.0 } })
	{
		const auto exact = frame.mapTo(ref, 2.0 * x + 0.5, 2.0 * y + 0.5);
		REQUIRE(exact.has_value());
		const QPointF pt = t->transform(QPointF(x, y));
		CHECK(pt.x() == Catch::Approx((std::get<0>(*exact) - 0.5) / 2.0).margin(0.01));
		CHECK(pt.y() == Catch::Approx((std::get<1>(*exact) - 0.5) / 2.0).margin(0.01));
	}
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

	// The projection code must be exactly TAN, optionally followed by a
	// distortion suffix.
	k = good;
	k.ctype1 = "RA---TANX"; k.ctype2 = "DEC--TANX";
	CHECK_FALSE(w.setFromKeywords(k));
	k = good;
	k.ctype1 = "RA---TANGENT";
	CHECK_FALSE(w.setFromKeywords(k));
	k = good;
	k.ctype1 = "RA---TAN-TPD"; k.ctype2 = "DEC--TAN-TPD";
	CHECK(w.setFromKeywords(k));
	k = good;
	k.ctype1 = "RA---TPV"; k.ctype2 = "DEC--TPV";
	CHECK_FALSE(w.setFromKeywords(k));

	// Long forms of degrees.
	k = good;
	k.cunit1 = "degree"; k.cunit2 = "Degrees";
	CHECK(w.setFromKeywords(k));

	// One bad axis is enough to reject the header.
	k = good;
	k.ctype2.clear();
	CHECK_FALSE(w.setFromKeywords(k));
	k = good;
	k.ctype1 = "RA---SIN";
	CHECK_FALSE(w.setFromKeywords(k));
	k = good;
	k.cunit2 = "arcsec";
	CHECK_FALSE(w.setFromKeywords(k));
}

TEST_CASE("WCS keywords: more incomplete headers", "[WCS][keywords]")
{
	DSS::WCSInfo w;

	// Only CDELT2.
	auto k = makeKeys();
	k.cdelt2 = arcsec;
	CHECK_FALSE(w.setFromKeywords(k));

	// The other reference pixel / value keywords.
	k = makeKeys();
	k.cd11 = -arcsec; k.cd22 = arcsec;
	k.crpix2.reset();
	CHECK_FALSE(w.setFromKeywords(k));
	k = makeKeys();
	k.cd11 = -arcsec; k.cd22 = arcsec;
	k.crval1.reset();
	CHECK_FALSE(w.setFromKeywords(k));
}

TEST_CASE("WCS keywords: singularity test is relative to the scale", "[WCS][keywords]")
{
	DSS::WCSInfo w;

	// A tiny (0.0036 mas/pixel) but perfectly valid matrix: an absolute
	// determinant threshold would wrongly reject it.
	auto k = makeKeys();
	k.cd11 = -1e-9; k.cd22 = 1e-9;
	CHECK(w.setFromKeywords(k));

	// Rows that are multiples of each other.
	k = makeKeys();
	k.cd11 = 1e-4; k.cd12 = 2e-4; k.cd21 = 2e-4; k.cd22 = 4e-4;
	CHECK_FALSE(w.setFromKeywords(k));

	// Singular via PC and via CDELT + CROTA2.
	k = makeKeys();
	k.cdelt1 = -arcsec; k.cdelt2 = arcsec;
	k.pc11 = 1.0; k.pc12 = 1.0; k.pc21 = 1.0; k.pc22 = 1.0;
	CHECK_FALSE(w.setFromKeywords(k));
	k = makeKeys();
	k.cdelt1 = -arcsec; k.cdelt2 = 0.0; k.crota2 = 30.0;
	CHECK_FALSE(w.setFromKeywords(k));
}

TEST_CASE("WCS keywords: non-finite values are rejected", "[WCS][keywords]")
{
	constexpr double nan = std::numeric_limits<double>::quiet_NaN();
	constexpr double inf = std::numeric_limits<double>::infinity();
	DSS::WCSInfo w;

	for (const double bad : { nan, inf, -inf })
	{
		auto k = makeKeys();
		k.cd11 = bad; k.cd22 = arcsec;
		CHECK_FALSE(w.setFromKeywords(k));

		k = makeKeys();
		k.cdelt1 = bad; k.cdelt2 = arcsec;
		CHECK_FALSE(w.setFromKeywords(k));

		k = makeKeys();
		k.cdelt1 = -arcsec; k.cdelt2 = arcsec; k.crota2 = bad;
		CHECK_FALSE(w.setFromKeywords(k));

		// The reference pixel and value.
		for (auto member : { &DSS::WCSKeywords::crpix1, &DSS::WCSKeywords::crpix2,
			&DSS::WCSKeywords::crval1, &DSS::WCSKeywords::crval2 })
		{
			k = makeKeys();
			k.cd11 = -arcsec; k.cd22 = arcsec;
			k.*member = bad;
			CHECK_FALSE(w.setFromKeywords(k));
		}
	}
}

TEST_CASE("WCS pixel scale of non-square and sheared pixels", "[WCS]")
{
	auto k = makeKeys();
	k.cdelt1 = -2.0 * arcsec; k.cdelt2 = 3.0 * arcsec; k.crota2 = 25.0;
	DSS::WCSInfo w;
	REQUIRE(w.setFromKeywords(k));
	// Geometric mean of the two axis scales.
	CHECK(w.pixelScale() == Catch::Approx(std::sqrt(6.0)).margin(1e-9));

	// Shear does not change the area of a pixel.
	k = makeKeys();
	k.cd11 = -arcsec; k.cd12 = 0.3 * arcsec; k.cd22 = arcsec;
	REQUIRE(w.setFromKeywords(k));
	CHECK(w.pixelScale() == Catch::Approx(1.0).margin(1e-9));
}

TEST_CASE("WCS at and across the celestial poles", "[WCS]")
{
	SECTION("tangent point exactly at a pole round trips")
	{
		for (const double dec0 : { 90.0, -90.0 })
		{
			const auto w = makeWcs(45.0, dec0, 1000.0, 800.0, 15.0);
			for (const auto [x, y] : { std::pair{ 1000.0, 800.0 }, std::pair{ 0.0, 0.0 }, std::pair{ 2000.0, 1600.0 }, std::pair{ 1700.0, 20.0 } })
			{
				const auto [ra, dec] = w.pixelToWorld(x, y);
				CHECK(ra >= 0.0);
				CHECK(ra < 360.0);
				CHECK(std::abs(dec) <= 90.0);
				const auto back = w.worldToPixel(ra, dec);
				REQUIRE(back.has_value());
				CHECK(std::get<0>(*back) == Catch::Approx(x).margin(1e-6));
				CHECK(std::get<1>(*back) == Catch::Approx(y).margin(1e-6));
			}
		}
	}

	SECTION("a pixel beyond the pole is on the opposite meridian")
	{
		// The tangent point is 36" from the north pole; 72 pixels north
		// of it is 36" beyond the pole, on the other side.
		const auto w = makeWcs(30.0, 89.99, 500.0, 500.0);
		const auto [ra, dec] = w.pixelToWorld(500.0, 572.0);
		CHECK(ra == Catch::Approx(210.0).margin(1e-6));
		CHECK(dec == Catch::Approx(89.99).margin(1e-6));

		const auto back = w.worldToPixel(ra, dec);
		REQUIRE(back.has_value());
		CHECK(std::get<0>(*back) == Catch::Approx(500.0).margin(1e-6));
		CHECK(std::get<1>(*back) == Catch::Approx(572.0).margin(1e-6));
	}
}

TEST_CASE("WCS RA normalisation", "[WCS]")
{
	// A position a minute fraction of a pixel west of RA 0 is less than
	// half an ulp below 360, so adding 360 rounds to exactly 360, which
	// must be reported as 0.
	const auto w = makeWcs(0.0, 0.0, 100.0, 100.0);
	const auto [ra, dec] = w.pixelToWorld(100.0 + 1e-12, 100.0);
	CHECK(ra >= 0.0);
	CHECK(ra < 360.0);
	CHECK(ra == 0.0);

	// CRVAL1 outside [0, 360) is normalised too.
	CHECK(std::get<0>(makeWcs(370.0, 10.0, 0.0, 0.0).pixelToWorld(0.0, 0.0)) == Catch::Approx(10.0).margin(1e-9));
	CHECK(std::get<0>(makeWcs(-10.0, 10.0, 0.0, 0.0).pixelToWorld(0.0, 0.0)) == Catch::Approx(350.0).margin(1e-9));

	// worldToPixel accepts RA in any equivalent range.
	const auto w2 = makeWcs(0.01, 20.0, 500.0, 500.0);
	const auto expected = w2.worldToPixel(359.99, 20.0);
	REQUIRE(expected.has_value());
	for (const double ra2 : { -0.01, 719.99 })
	{
		const auto p = w2.worldToPixel(ra2, 20.0);
		REQUIRE(p.has_value());
		CHECK(std::get<0>(*p) == Catch::Approx(std::get<0>(*expected)).margin(1e-6));
		CHECK(std::get<1>(*p) == Catch::Approx(std::get<1>(*expected)).margin(1e-6));
	}
}

TEST_CASE("WCS worldToPixel hemisphere boundary", "[WCS]")
{
	const auto w = makeWcs(10.0, 0.0, 0.0, 0.0);
	CHECK(w.worldToPixel(99.0, 0.0).has_value());		// 89 degrees away
	CHECK_FALSE(w.worldToPixel(100.5, 0.0).has_value());	// 90.5 degrees away

	const auto north = makeWcs(10.0, 60.0, 0.0, 0.0);
	CHECK(north.worldToPixel(190.0, 61.0).has_value());		// 59 degrees away, over the pole
	CHECK_FALSE(north.worldToPixel(190.0, 10.0).has_value());	// 110 degrees away
}

TEST_CASE("WCS top-down and bottom-up files of the same field agree", "[WCS][transformation]")
{
	// The solution in bitmap (0-based, top row first) coordinates.
	const auto bitmap = makeWcs(150.0, 30.0, 1500.0, 1000.0, 10.0);

	// What a plate solver writes for each file, describing pixels in
	// file order with 1-based centres.
	auto topDown = bitmap;
	topDown.crpix1 += 1.0;
	topDown.crpix2 += 1.0;

	auto bottomUp = mirrored(bitmap);
	bottomUp.crpix1 += 1.0;
	bottomUp.crpix2 = frameHeight - bitmap.crpix2;

	topDown.toBitmapCoordinates(frameHeight, false);
	bottomUp.toBitmapCoordinates(frameHeight, true);

	for (const auto* w : { &topDown, &bottomUp })
	{
		CHECK(w->crpix1 == Catch::Approx(bitmap.crpix1).margin(1e-12));
		CHECK(w->crpix2 == Catch::Approx(bitmap.crpix2).margin(1e-12));
		checkMatrix(*w, bitmap.cd11, bitmap.cd12, bitmap.cd21, bitmap.cd22);
	}

	const auto t = bottomUp.transformationTo(topDown, frameWidth, frameHeight);
	REQUIRE(t.has_value());
	for (const auto [x, y] : { std::pair{ 0.0, 0.0 }, std::pair{ 3000.0, 2000.0 }, std::pair{ 123.0, 1789.0 } })
	{
		const QPointF pt = t->transform(QPointF(x, y));
		CHECK(pt.x() == Catch::Approx(x).margin(1e-6));
		CHECK(pt.y() == Catch::Approx(y).margin(1e-6));
	}
}

TEST_CASE("WCS transformation with different binning in the two frames", "[WCS][transformation]")
{
	// Same pointing, so the transformation is exactly the change of
	// rendered pixel grid: rendered pixel r of a binned frame is centred
	// on WCS pixel 2r + 0.5.
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 10.0);

	SECTION("super-pixel frame, normal reference")
	{
		const auto t = ref.transformationTo(ref, frameWidth / 2, frameHeight / 2, 2, 1);
		REQUIRE(t.has_value());
		for (const auto [x, y] : { std::pair{ 0.0, 0.0 }, std::pair{ 1500.0, 1000.0 }, std::pair{ 321.0, 777.0 } })
		{
			const QPointF pt = t->transform(QPointF(x, y));
			CHECK(pt.x() == Catch::Approx(2.0 * x + 0.5).margin(1e-6));
			CHECK(pt.y() == Catch::Approx(2.0 * y + 0.5).margin(1e-6));
		}
	}

	SECTION("normal frame, super-pixel reference")
	{
		const auto t = ref.transformationTo(ref, frameWidth, frameHeight, 1, 2);
		REQUIRE(t.has_value());
		for (const auto [x, y] : { std::pair{ 0.0, 0.0 }, std::pair{ 3000.0, 2000.0 }, std::pair{ 321.0, 777.0 } })
		{
			const QPointF pt = t->transform(QPointF(x, y));
			CHECK(pt.x() == Catch::Approx((x - 0.5) / 2.0).margin(1e-6));
			CHECK(pt.y() == Catch::Approx((y - 0.5) / 2.0).margin(1e-6));
		}
	}
}

TEST_CASE("WCS transformation between different pixel scales", "[WCS][transformation]")
{
	// E.g. frames from two different telescopes: 2"/pixel onto 1"/pixel,
	// sharing a tangent point, so the mapping is exactly linear.
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 10.0, 1.0);
	const auto frame = makeWcs(150.0, 30.0, 700.0, 500.0, 10.0, 2.0);
	const auto t = frame.transformationTo(ref, frameWidth, frameHeight);
	REQUIRE(t.has_value());
	for (const auto [x, y] : { std::pair{ 700.0, 500.0 }, std::pair{ 0.0, 0.0 }, std::pair{ 800.0, 450.0 } })
	{
		const QPointF pt = t->transform(QPointF(x, y));
		CHECK(pt.x() == Catch::Approx(1500.0 + 2.0 * (x - 700.0)).margin(1e-6));
		CHECK(pt.y() == Catch::Approx(1000.0 + 2.0 * (y - 500.0)).margin(1e-6));
	}
	CHECK(worstError(*t, frame, ref) < 1e-6);
}

TEST_CASE("WCS transformation across RA 0/360", "[WCS][transformation]")
{
	const auto ref = makeWcs(0.003, 30.0, 1500.0, 1000.0, 10.0);
	// 40 pixels west of the reference pixel is west of RA 0.
	const auto [ra, dec] = ref.pixelToWorld(1540.0, 1015.0);
	REQUIRE(ra > 359.0);
	const auto frame = makeWcs(ra, dec, 1500.0, 1000.0, 10.0);

	const auto t = frame.transformationTo(ref, frameWidth, frameHeight);
	REQUIRE(t.has_value());
	const QPointF centre = t->transform(QPointF(1500.0, 1000.0));
	CHECK(centre.x() == Catch::Approx(1540.0).margin(1e-3));
	CHECK(centre.y() == Catch::Approx(1015.0).margin(1e-3));
	CHECK(worstError(*t, frame, ref) < 0.01);

	// And the other way round.
	const auto back = ref.transformationTo(frame, frameWidth, frameHeight);
	REQUIRE(back.has_value());
	CHECK(worstError(*back, ref, frame) < 0.01);
}

TEST_CASE("WCS transformation for a field containing the pole", "[WCS][transformation]")
{
	// The frames are 0.83 x 0.56 degrees, so both contain the pole.
	for (const double dec0 : { 89.9, -90.0 })
	{
		const auto ref = makeWcs(210.0, dec0, 1500.0, 1000.0, 37.0);
		const auto [ra, dec] = ref.pixelToWorld(1560.0, 960.0);
		const auto frame = makeWcs(ra, dec, 1480.0, 1015.0, 39.5);
		const auto t = frame.transformationTo(ref, frameWidth, frameHeight);
		REQUIRE(t.has_value());
		CHECK(worstError(*t, frame, ref) < 0.01);
	}
}

TEST_CASE("WCS transformation rejects nonsensical sizes and binning", "[WCS][transformation]")
{
	const auto ref = makeWcs(150.0, 30.0, 1500.0, 1000.0, 10.0);
	CHECK_FALSE(ref.transformationTo(ref, frameWidth, 0).has_value());
	CHECK_FALSE(ref.transformationTo(ref, -frameWidth, frameHeight).has_value());
	CHECK_FALSE(ref.transformationTo(ref, frameWidth, -frameHeight).has_value());
	CHECK_FALSE(ref.transformationTo(ref, frameWidth, frameHeight, -1).has_value());
	CHECK_FALSE(ref.transformationTo(ref, frameWidth, frameHeight, 1, 0).has_value());
	CHECK_FALSE(ref.transformationTo(ref, frameWidth, frameHeight, 1, -2).has_value());
}
