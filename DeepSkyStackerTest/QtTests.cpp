#include "pch.h"
#include "catch_amalgamated.hpp"
#include <qfileinfo.h>

TEST_CASE("Qt")
{
	SECTION("Basename")
	{
		const QFileInfo fileInfo("\\p1\\p2\\file.name.haha.martin.txt");
		const auto bn = fileInfo.baseName();
		const auto cbn = fileInfo.completeBaseName();
		const auto p = fileInfo.path();

		REQUIRE(QString::compare(bn, "file") == 0);
		REQUIRE(QString::compare(cbn, "file.name.haha.martin") == 0);
		REQUIRE(QString::compare(p, "/p1/p2") == 0);
	}
}
