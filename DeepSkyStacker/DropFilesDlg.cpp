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
// DropFilesDlg.cpp : implementation file
//
#include "stdafx.h"
#include <QDropEvent>
#include <QMimeData>

//#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "DropFilesDlg.h"
#include "ui/ui_DropFilesDlg.h"

//#include "BitmapExt.h"

DropFilesDlg::DropFilesDlg(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DropFilesDlg),
	initialised(false),
	type{ PICTURETYPE_UNKNOWN }
{
	ui->setupUi(this);

}

DropFilesDlg::~DropFilesDlg()
{
	delete ui;
}

bool DropFilesDlg::isMasterFile(const fs::path& path)
{
	bool result = false;
	CBitmapInfo			BitmapInfo;

	if (GetPictureInfo(path.generic_wstring().c_str(), BitmapInfo))
		result = BitmapInfo.IsMaster();

	return result;
}

void DropFilesDlg::onInitDialog()
{
	QSettings settings;
	QString string;
	size_t fileCount{ 0 };
	bool checked = false;
	std::vector<fs::path> masters;

	//
	// Restore Window position etc..
	//
	QByteArray ba = settings.value("Dialogs/DropFilesDlg/geometry").toByteArray();
	if (!ba.isEmpty())
	{
		restoreGeometry(ba);
	}
	else
	{
		//
		// Get main Window rectangle
		//
		const QRect r{ DeepSkyStacker::instance()->rect() };
		QSize size = this->size();

		int top = ((r.top() + (r.height() / 2) - (size.height() / 2)));
		int left = ((r.left() + (r.width() / 2) - (size.width() / 2)));
		move(left, top);
	}

	QList<QUrl> urls = dropEvent->mimeData()->urls();
	for (int i = 0; i != urls.size(); ++i)
	{
		QString name{ urls[i].toLocalFile() };
		fs::path path{ name.toStdU16String() };
		switch (status(path).type())
		{
		case fs::file_type::regular:
			if (isMasterFile(path))
				masters.emplace_back(path);
			else
				files.emplace_back(path);
			break;
		case fs::file_type::directory:
			for (const auto& e : fs::directory_iterator{ path })
			{
				auto& entry{ e.path() };
				if (is_regular_file(entry))
				{
					if (isMasterFile(path))
						masters.emplace_back(path);
					else
						files.emplace_back(path);
				}
			}
			break;
		}
	}

	if (files.empty())
		files = std::move(masters);

	fileCount = files.size();

	ui->dropFiles->setTitle(ui->dropFiles->title().arg(fileCount));
	ui->lightFrames->setChecked(true);
}


void DropFilesDlg::accept()
{
	QSettings settings;

	settings.setValue("Dialogs/DropFilesDlg/geometry", saveGeometry());

	if (ui->lightFrames->isChecked())
		type = PICTURETYPE_LIGHTFRAME;
	else if (ui->darkFrames->isChecked())
		type = PICTURETYPE_DARKFRAME;
	else if (ui->darkFlatFrames->isChecked())
		type = PICTURETYPE_DARKFLATFRAME;
	else if (ui->flatFrames->isChecked())
		type = PICTURETYPE_FLATFRAME;
	else
		type = PICTURETYPE_OFFSETFRAME;

	Inherited::accept();
}

void DropFilesDlg::showEvent(QShowEvent* event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitDialog();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}

#if (0)
/* ------------------------------------------------------------------- */
// CDropFilesDlg dialog

IMPLEMENT_DYNAMIC(CDropFilesDlg, CDialog)

CDropFilesDlg::CDropFilesDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CDropFilesDlg::IDD, pParent)
{
	m_hDropInfo = nullptr;
	m_DropType  = PICTURETYPE_UNKNOWN;
}

/* ------------------------------------------------------------------- */

CDropFilesDlg::~CDropFilesDlg()
{
}

/* ------------------------------------------------------------------- */

void CDropFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DROPFILESTEXT, m_Text);
	DDX_Control(pDX, IDC_LIGHTFRAMES, m_LightFrames);
	DDX_Control(pDX, IDC_DARKFRAMES, m_DarkFrames);
	DDX_Control(pDX, IDC_FLATFRAMES, m_FlatFrames);
	DDX_Control(pDX, IDC_BIASFRAMES, m_BiasFrames);
	DDX_Control(pDX, IDC_DARKFLATFRAMES, m_DarkFlatFrames);
}

/* ------------------------------------------------------------------- */

static void	GetFilesInFolder(LPCTSTR szFolder, std::vector<CString>	& vFiles)
{
	CString					strFolder = szFolder;
	WIN32_FIND_DATA			FindData;
	CString					strFileMask;
	HANDLE					hFindFiles;

	strFileMask = strFolder;
	strFileMask += "\\*.*";

	hFindFiles = FindFirstFile(strFileMask, &FindData);
	if (hFindFiles != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString			strFile;

			strFile = strFolder;
			strFile += "\\";
			strFile += FindData.cFileName;

			const auto dwAttributes = GetFileAttributes(strFile);
			if (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY))
				vFiles.push_back(strFile);
		}
		while (FindNextFile(hFindFiles, &FindData));

		FindClose(hFindFiles);
	};
};

/* ------------------------------------------------------------------- */

bool CDropFilesDlg::IsMasterFile(LPCTSTR szFile)
{
	bool				bResult = false;
	CBitmapInfo			BitmapInfo;

	if (GetPictureInfo(szFile, BitmapInfo))
		bResult = BitmapInfo.IsMaster();

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CDropFilesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString					strText;
	std::vector<CString>	vMasters;
	unsigned int lNrFiles = 0;

	m_Text.GetWindowText(strText);

	if (m_hDropInfo)
	{
		lNrFiles = DragQueryFile(m_hDropInfo, 0xFFFFFFFF, nullptr, 0);
		for (unsigned int i = 0; i < lNrFiles; i++)
		{
			TCHAR			szFile[1+_MAX_PATH];
			CString			strFile;

			DragQueryFile(m_hDropInfo, i, szFile, sizeof(szFile)/sizeof(TCHAR));

			// If it's a folder, get all the files in the folder
			const auto dwAttributes = GetFileAttributes(szFile);

			if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::vector<CString>	vFiles;

				GetFilesInFolder(szFile, vFiles);
				for (size_t j = 0; j < vFiles.size(); j++)
				{
					if (IsMasterFile(vFiles[j]))
						vMasters.push_back(vFiles[j]);
					else
						m_vFiles.push_back(vFiles[j]);
				};
			}
			else
			{
				strFile = szFile;
				if (IsMasterFile(strFile))
					vMasters.push_back(strFile);
				else
					m_vFiles.push_back(strFile);
			};
		};

		DragFinish(m_hDropInfo);
		if (!m_vFiles.size())
			m_vFiles = std::move(vMasters);
		lNrFiles = static_cast<unsigned int>(m_vFiles.size());
	};

	strText.Format(strText, lNrFiles);
	m_Text.SetWindowText(strText);

	m_LightFrames.SetCheck(true);

	return true;
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnOK()
{
	if (m_LightFrames.GetCheck())
		m_DropType = PICTURETYPE_LIGHTFRAME;
	else if (m_DarkFrames.GetCheck())
		m_DropType = PICTURETYPE_DARKFRAME;
	else if (m_DarkFlatFrames.GetCheck())
		m_DropType = PICTURETYPE_DARKFLATFRAME;
	else if (m_FlatFrames.GetCheck())
		m_DropType = PICTURETYPE_FLATFRAME;
	else
		m_DropType = PICTURETYPE_OFFSETFRAME;

	CDialog::OnOK();
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnLightFrames()
{
	m_DarkFrames.SetCheck(false);
	m_FlatFrames.SetCheck(false);
	m_BiasFrames.SetCheck(false);
	m_DarkFlatFrames.SetCheck(false);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnDarkFrames()
{
	m_LightFrames.SetCheck(false);
	m_FlatFrames.SetCheck(false);
	m_BiasFrames.SetCheck(false);
	m_DarkFlatFrames.SetCheck(false);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnFlatFrames()
{
	m_LightFrames.SetCheck(false);
	m_DarkFrames.SetCheck(false);
	m_BiasFrames.SetCheck(false);
	m_DarkFlatFrames.SetCheck(false);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnDarkFlatFrames()
{
	m_LightFrames.SetCheck(false);
	m_DarkFrames.SetCheck(false);
	m_BiasFrames.SetCheck(false);
	m_FlatFrames.SetCheck(false);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnBiasFrames()
{
	m_LightFrames.SetCheck(false);
	m_DarkFrames.SetCheck(false);
	m_FlatFrames.SetCheck(false);
	m_DarkFlatFrames.SetCheck(false);
};

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CDropFilesDlg, CDialog)
	ON_BN_CLICKED(IDC_LIGHTFRAMES, OnLightFrames)
	ON_BN_CLICKED(IDC_DARKFRAMES, OnDarkFrames)
	ON_BN_CLICKED(IDC_FLATFRAMES, OnFlatFrames)
	ON_BN_CLICKED(IDC_BIASFRAMES, OnBiasFrames)
	ON_BN_CLICKED(IDC_DARKFLATFRAMES, OnDarkFlatFrames)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

// CDropFilesDlg message handlers
#endif