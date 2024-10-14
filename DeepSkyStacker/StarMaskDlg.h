#pragma once
/****************************************************************************
**
** Copyright (C) 2024 David C. Partridge
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
#include <QDialog>
#include "BaseDialog.h"
#include "ui_StarMaskDlg.h"

namespace DSS {
	class StarMaskDlg final : public BaseDialog, public Ui::StarMaskDlg
	{
		Q_OBJECT

		typedef BaseDialog
			Inherited;

	public:
		StarMaskDlg(QWidget* parent, const fs::path& file);
		~StarMaskDlg() {}

		StarMaskDlg(const StarMaskDlg& rhs) = delete;

		StarMaskDlg& operator = (const StarMaskDlg& rhs) = delete;

		inline const fs::path& outputFile() { return outputFile_; }
		inline bool outputIsFits() { return isFits;  }

	private slots:
		void setStarShapePreview(int index);
		void thresholdChanged(int value);
		//void detectHotChanged(int state);
		void minSizeChanged(int value);
		void maxSizeChanged(int value);
		void percentChanged(int value);
		void pixelsChanged(int value);

		void onOK();
		void onCancel();


	private:
		void connectSignalsToSlots();

		fs::path imageFile;
		fs::path outputFile_;
		bool isFits;

	};
} // namespace DSS

#if (0)
// CStarMaskDlg dialog

class CStarMaskDlg : public CDialog
{
	DECLARE_DYNAMIC(CStarMaskDlg)

public:
	CStarMaskDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStarMaskDlg();

	void	SetBaseFileName(LPCTSTR szOutputFile)
	{
		TCHAR				szDrive[1+_MAX_DRIVE];
		TCHAR				szDir[1+_MAX_DIR];

		_tsplitpath(szOutputFile, szDrive, szDir, nullptr, nullptr);

		m_strOutputFile = szDrive;
		m_strOutputFile += szDir;
		m_strOutputFile += "StarMask";
	};

	void	GetOutputFileName(CString & strOutputFile, bool & bFits)
	{
		strOutputFile = m_strOutputFile;
		bFits		  = m_bOutputFITS;
	};

// Dialog Data
	enum { IDD = IDD_STARMASK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnStarShapeChange( );

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
private :
	void	UpdateTexts();
	void	UpdateStarShapePreview();
	bool	AskOutputFile();

private :
	CString					m_strOutputFile;
	bool					m_bOutputFITS;
	CString					m_StarThresholdMask;
	CString					m_MinSizeMask;
	CString					m_MaxSizeMask;
	CString					m_PercentMask;
	CString					m_PixelsMask;

	CComboBox				m_StarShape;
	CStatic					m_StarShapePreview;
	CStatic					m_StarThresholdText;
	CSliderCtrl				m_StarThreshold;
	CButton					m_HotPixels;
	CStatic					m_MinSizeText;
	CSliderCtrl				m_MinSize;
	CStatic					m_MaxSizeText;
	CSliderCtrl				m_MaxSize;
	CStatic					m_PercentText;
	CSliderCtrl				m_Percent;
	CStatic					m_PixelsText;
	CSliderCtrl				m_Pixels;
};
#endif