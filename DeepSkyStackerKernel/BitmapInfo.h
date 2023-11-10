#pragma once
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
// BitmapInfo.h : header file
#include "BitmapExtraInfo.h"
#include "cfa.h"

class CBitmapInfo
{
public:
	fs::path m_strFileName;
	QString m_strFileType;
	QString m_strModel;
	int m_lISOSpeed;
	int m_lGain;
	double m_fExposure;
	double m_fAperture;
	int m_lWidth;
	int m_lHeight;
	int m_lBitsPerChannel;
	int m_lNrChannels;
	bool m_bCanLoad;
	bool m_bFloat;
	CFATYPE m_CFAType;
	bool m_bMaster;
	bool m_bFITS16bit;
	QString m_strDateTime;
	QDateTime m_DateTime;
	QDateTime m_InfoTime;
	CBitmapExtraInfo m_ExtraInfo;
	int m_xBayerOffset;
	int m_yBayerOffset;
	QString m_filterName;

private:
	void CopyFrom(const CBitmapInfo& bi);
	void Init();

public:
	CBitmapInfo();
	CBitmapInfo(const CBitmapInfo& bi);
	CBitmapInfo(const fs::path& fileName);

	virtual ~CBitmapInfo() = default;

	CBitmapInfo& operator = (const CBitmapInfo& bi);
	bool operator<(const CBitmapInfo& other) const;
	bool operator==(const CBitmapInfo& other) const;

	bool CanLoad() const;
	bool IsCFA();
	bool IsMaster();
	void GetDescription(QString& strDescription);
	bool IsInitialized();
};

bool RetrieveEXIFInfo(const fs::path& fileName, CBitmapInfo& BitmapInfo);