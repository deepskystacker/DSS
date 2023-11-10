/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include "stdafx.h"
#include "tracecontrol.h"
#include "ZExcBase.h"

namespace DSS
{
	TraceControl::TraceControl(std::string fName) :
		fileName { fName },
		dirName { QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) },
		erase { true }
	{
		namespace chr = std::chrono;
		namespace fs = std::filesystem;

		using namespace std::literals;

		fs::path path{ fileName }; path = path.stem();	// only want the name

		//
		// Set the start of the filename of the trace file based on whether
		// we are being used in DSS, DSSCL or DSSLive
		//
		QString start{  };
		if ("DeepSkyStacker" == path) start = "DSSTrace";
		else if ("DeepSkyStackerCL" == path) start = "DSSCLTrace";
		else if ("DeepSkyStackerLive" == path) start = "DSSLiveTrace";
		else ZASSERT(false);
		
		(void) _putenv(traceTo.toStdString().c_str()); // set Z_TRACETO=FILE

		std::time_t time = std::time({});
		char timeString[std::size("yyyy-mm-ddThh-mm-ssZ")];
		std::strftime(std::data(timeString), std::size(timeString), "%FT%H-%M-%SZ", std::gmtime(&time));
		std::cout << timeString << '\n';
	
		file = dirName.toStdU16String();
		QString name = QString{ "%1_%2.log" }.
			arg(start).
			arg(timeString);

		file /= "DeepSkyStacker";
		create_directories(file);
		file /= name.toStdU16String();

		QString traceFile = QString{ "Z_TRACEFILE=%1" }.arg(file.generic_u16string().c_str());
		(void) _putenv(traceFile.toStdString().c_str());
	}

	TraceControl::~TraceControl()
	{
		if (erase) fs::remove(file);
	}
}
