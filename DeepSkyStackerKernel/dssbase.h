#pragma once
/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
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

//
// This class is a base class for the following classes: DeepSkyStacker,
// DeepSkyStackerLive and DeepSkyStackerCommandLine
//  
class QString;

class DSSBase
{
public:

	//
	// Enum values that match QMessageBox::Icon values
	//
	enum class Severity
	{
		None = 0,
		Information = 1,
		Warning = 2,
		Critical = 3
	};

	enum class Method
	{
		QMessageBox = 0,
		QErrorMessage = 1
	};

	// severity 2 is QMessageBox::Warning
	virtual void reportError(const QString& message, const QString& type,
		Severity severity = Severity::Warning, Method method = Method::QMessageBox,
		bool terminate = false) = 0;

	inline static DSSBase* instance()
	{
		return theInstance;
	}

	inline static void setInstance(DSSBase* instance)
	{
		theInstance = instance;
	}

private:
	static inline DSSBase* theInstance{ nullptr };
};