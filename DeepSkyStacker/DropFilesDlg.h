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
#include <vector>

#include <QDialog>
#include "DSSCommon.h"

namespace Ui {
	class DropFilesDlg;
}

class DropFilesDlg : public QDialog
{
	Q_OBJECT

		typedef QDialog
		Inherited;
public:
	explicit DropFilesDlg(QWidget* parent = nullptr);
	~DropFilesDlg();

	DropFilesDlg(const DropFilesDlg& rhs) = delete;
	DropFilesDlg& operator = (const DropFilesDlg& rhs) = delete;

	inline void	setDropInfo(QDropEvent* e)
	{
		dropEvent = e;
	};

	inline PICTURETYPE	dropType()
	{
		return type;
	};

	inline const std::vector<fs::path>& getFiles()
	{
		return files;
	};

protected:
	void showEvent(QShowEvent* event) override;

private slots:
	void accept() override;

private:
	Ui::DropFilesDlg* ui;

	bool initialised;
	QDropEvent* dropEvent;
	PICTURETYPE type;
	std::vector<fs::path> files;

	void onInitDialog();

	bool isMasterFile(const fs::path& path);
};
#if (0)
// CDropFilesDlg dialog

class CDropFilesDlg : public CDialog
{
	DECLARE_DYNAMIC(CDropFilesDlg)

public:
	CDropFilesDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDropFilesDlg();

	void	SetDropInfo(HDROP hDropInfo)
	{
		m_hDropInfo = hDropInfo;
	};

	PICTURETYPE	GetDropType()
	{
		return m_DropType;
	};

	void	GetDroppedFiles(std::vector<CString> & vFiles)
	{
		vFiles = m_vFiles;
	};

// Dialog Data
	enum { IDD = IDD_DROPFILES };

private :
	HDROP					m_hDropInfo;
	PICTURETYPE				m_DropType;
	CStatic					m_Text;
	CButton					m_LightFrames;
	CButton					m_DarkFrames;
	CButton					m_FlatFrames;
	CButton					m_BiasFrames;
	CButton					m_DarkFlatFrames;
	std::vector<CString>	m_vFiles;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	bool	IsMasterFile(LPCTSTR szFile);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg	void OnLightFrames();
	afx_msg	void OnDarkFrames();
	afx_msg	void OnDarkFlatFrames();
	afx_msg	void OnFlatFrames();
	afx_msg	void OnBiasFrames();

	DECLARE_MESSAGE_MAP()
};
#endif