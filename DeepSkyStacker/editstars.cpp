/****************************************************************************
**
** Copyright (C) 2020 David C. Partridge
* **
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
#include "editstars.h"
#include "Delaunay.h"
#include "imageview.h"
#include "DSSCommon.h"
#include "StackingDlg.h"
#include "RegisterEngine.h"
#include "BackgroundCalibration.h"
#include "DeepSkyStacker.h"
#include "zexcept.h"

// Classes etc. private to this file
#if (0)
namespace
{
	inline QRgb SolveColor(double fValue, double fMean, double fStdDev)
	{
		double			red = 0, green = 0, blue = 0;

		fValue -= fMean;
		fValue /= fStdDev;

		if (std::abs(fValue) < 0.5)
			green = 255;
		else if (fValue < 0)
		{
			red = std::max(0.0, 255.0 - std::max(0.0, 3.0 - std::abs(fValue)) * 255.0 / 3.0);
			green = std::max(0.0, std::max(0.0, 3.0 - std::abs(fValue)) * 255.0 / 3.0);
		}
		else
		{
			blue = std::max(0.0, 255.0 - std::max(0.0, 3.0 - std::abs(fValue)) * 255.0 / 3.0);
			green = std::max(0.0, std::max(0.0, 3.0 - std::abs(fValue)) * 255.0 / 3.0);
		};

		return qRgba(red, green, blue, 0.3 * 255);
	};

	inline double StarValue(const CStar& star)
	{
		return (star.m_fLargeMinorAxis + star.m_fSmallMinorAxis) / (star.m_fLargeMajorAxis + star.m_fSmallMajorAxis);
	};

}
#endif


namespace DSS
{
#if (0)
	void QualityGrid::InitGrid(STARVECTOR& vStars)
	{
		double fPowSum = 0.0;
		vertexSet sVertices;

		mean = 0;
		triangles.clear();

		if (!vStars.empty())
		{
			for (auto& star : vStars)
			{
				if (!star.m_bRemoved)
				{
					vertex v(static_cast<float>(star.m_fX), static_cast<float>(star.m_fY));

					star.m_fX = v.GetX();
					star.m_fY = v.GetY();

					sVertices.insert(v);

					const double fValue = StarValue(star);

					mean += fValue;
					fPowSum += fValue * fValue;
				}
			}

			if (!sVertices.empty())
			{
				stdDev = sqrt(fPowSum / sVertices.size() - pow(mean / sVertices.size(), 2));
				mean /= sVertices.size();
			};

			Delaunay delaunay;
			triangleSet sTriangles;

			//
			// ### To Do
			// March 2023: Delaunay::Triangulate() does nothing. sTriangles will be empty.
			//
			delaunay.Triangulate(sVertices, sTriangles);

			triangles.reserve(sTriangles.size());

			for (ctIterator cit = sTriangles.cbegin(); cit != sTriangles.cend(); ++cit)
			{
				DelaunayTriangle tr;
				tr.pt1 = cit->GetVertex(0)->GetPoint();
				tr.pt2 = cit->GetVertex(1)->GetPoint();
				tr.pt3 = cit->GetVertex(2)->GetPoint();

				// Find the value for each point
				const auto solve = [mean = this->mean, stdev = this->stdDev, &vStars](const float x, const float y)->QRgb
				{
					const auto it = lower_bound(vStars.cbegin(), vStars.cend(), CStar(x, y));
					return SolveColor(it != vStars.cend() ? StarValue(*it) : mean, mean, stdev);
				};

				tr.cr1 = solve(tr.pt1.x(), tr.pt1.y());
				tr.cr2 = solve(tr.pt2.x(), tr.pt2.y());
				tr.cr3 = solve(tr.pt3.x(), tr.pt3.y());

				triangles.push_back(tr);
			}
		}
	}
#endif

	EditStars::EditStars(QWidget* parent) :
		QWidget{ parent },
		imageView{ dynamic_cast<ImageView*>(parent) },
		fileName{},
		stars{},
		refStars{},
	//	m_pBitmap{},
		transformation{},
		vVotedPairs{},
		m_ptCursor{},
	//	m_GrayBitmap
		m_Action{ EditStarAction::None },
		m_SelectedStar{},
	//	m_lRemovedIndice{ 0 },
	//	m_bRemoveComet{ false },
		m_bCometMode{ false },
		m_fXComet{ 0 },
		m_fYComet{ 0 },
		m_bComet{ false },
		m_fLightBkgd{ 0 },
		m_bDirty{ false },
		m_fScore{ 0 },
		m_fQuality{ 0 },
		m_lNrStars{ 0 },
		m_fFWHM{ 0 },
	//	m_fBackground{ 0 },
	//	m_QualityGrid;
		forceHere{ false },
	//	displayGrid{ false },
	//	m_tipShowCount{ 0 }
		disabled{ false }
	{
		ZASSERT(nullptr != imageView);
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setAttribute(Qt::WA_NoSystemBackground);
		setAttribute(Qt::WA_TranslucentBackground);
		setAttribute(Qt::WA_WState_ExplicitShowHide);
//		setToolTip(tr("Ctrl+G to toggle display of the Grid"));
	}

	bool EditStars::isDirty() const
	{
		return this->m_bDirty;
	}

	void EditStars::setTransformation(const CBilinearParameters& Tr, const VOTINGPAIRVECTOR& vVP)
	{
		if (g_bShowRefStars)
		{
			transformation = Tr;
			vVotedPairs = vVP;
		}
	}

	void EditStars::clearRefStars()
	{
		refStars.clear();
	}

	void EditStars::setBitmap(std::shared_ptr<CMemoryBitmap> bmp)
	{
//		m_pBitmap = bmp;
//		m_GrayBitmap.Init(RCCHECKSIZE + 1, RCCHECKSIZE + 1);
		m_bDirty = false;
//		m_fBackground = 0;
//		if (static_cast<bool>(m_pBitmap))
//			computeBackgroundValue();
		this->disabled = !static_cast<bool>(bmp); // If bmp is empty (nullptr), then set disabled to true to prevent drawing events.
	}

	//
	// refStars are the detected stars in the reference frame.
	//
	void EditStars::setRefStars(STARVECTOR const& Stars)
	{
		if (g_bShowRefStars)
		{
			refStars = Stars;
			std::ranges::sort(refStars, CompareStarLuminancy);
		}
	}

	void EditStars::leaveEvent(QEvent*)
	{
		m_ptCursor.setX(-1);
		m_ptCursor.setY(-1);
	}

	template <bool ComputeQuality>
	void EditStars::addOrRemoveStar(const CStar& star, const bool remove)
	{
		if (auto it = std::ranges::find(this->stars, star); it != std::end(this->stars))
		{
			it->m_bRemoved = remove;
			if constexpr (ComputeQuality)
				this->computeOverallQuality();
		}
	}

	void EditStars::mousePressEvent(QMouseEvent* e)
	{
		if (Qt::LeftButton == e->button())
		{
			switch (m_Action)
			{
				case EditStarAction::Add:
					if (this->m_bCometMode) // Add comet
					{
						qDebug() << "Processing StarAction add comet";
						if (m_bComet)
							addOrRemoveStar<false>(CStar{ m_fXComet, m_fYComet }, false); // If old comet was a star before.
						m_fXComet = m_SelectedStar.m_fX;
						m_fYComet = m_SelectedStar.m_fY;
						m_bComet = true;
						addOrRemoveStar<false>(m_SelectedStar, true); // It new comet was a star before.
						computeOverallQuality();
					}
					else // Add star
					{
						qDebug() << "Processing StarAction add star";
						addOrRemoveStar<true>(m_SelectedStar, false); // Add star.
						if (m_bComet && m_SelectedStar == CStar{ m_fXComet, m_fYComet }) // If new star was a comet before.
							m_bComet = false;
					}
				break;

				case EditStarAction::Remove:
					if (this->m_bCometMode) // Remove comet
					{
						qDebug() << "Processing StarAction remove comet";
						m_bComet = false;
						addOrRemoveStar<true>(CStar{ m_fXComet, m_fYComet }, false); // If the comet was a star before.
					}
					else // Remove star
					{
						qDebug() << "Processing StarAction remove star";
						addOrRemoveStar<true>(m_SelectedStar, true);
					}
				break;

//				case EditStarAction::AddStar:
//					qDebug() << "Processing StarAction::AddStar";
//					stars.emplace_back(m_SelectedStar);
//					std::sort(stars.begin(), stars.end());
//					if (m_bRemoveComet)
//						m_bComet = false;
//					computeOverallQuality();
////					m_QualityGrid.InitGrid(stars);
//					break;
//
//				case EditStarAction::RemoveStar:
//					qDebug() << "Processing StarAction::RemoveStar";
//					stars[m_lRemovedIndice].m_bRemoved = true;
//					stars.erase(stars.begin() + m_lRemovedIndice);
//					computeOverallQuality();
////					m_QualityGrid.InitGrid(stars);
//					break;
//
//				case EditStarAction::SetComet:
//					qDebug() << "Processing StarAction::SetComet";
//					if (m_lRemovedIndice >= 0)
//					{
//						stars[m_lRemovedIndice].m_bRemoved = true;
//						stars.erase(stars.begin() + m_lRemovedIndice);
//						computeOverallQuality();
////						m_QualityGrid.InitGrid(stars);
//					};
//					m_fXComet = m_SelectedStar.m_fX;
//					m_fYComet = m_SelectedStar.m_fY;
//					m_bComet = true;
//					break;
//
//				case EditStarAction::ResetComet:
//					qDebug() << "Processing StarAction::ResetComet";
//					m_bComet = false;
//					break;
			}
			if (m_Action != EditStarAction::None)
			{
				m_bDirty = true;
			}
			StackingDlg& stackingDlg{ DeepSkyStacker::instance()->getStackingDlg() };
			stackingDlg.pictureChanged();
		}
	}

	void EditStars::mouseReleaseEvent(QMouseEvent* e)
	{
		Inherited::mouseReleaseEvent(e);
	}

	void EditStars::rectButtonPressed()
	{
		//
		// No longer interested in signals from the imageView object
		//
		imageView->disconnect(this, nullptr);
		imageView->clearOverlay();
		hide();
	}

	void EditStars::starsOrCometButtonPressed(const bool cometMode)
	{
		constexpr auto connectionType = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);

		connect(imageView, &ImageView::Image_leaveEvent, this, &EditStars::leaveEvent, connectionType);
		connect(imageView, &ImageView::Image_mousePressEvent, this, &EditStars::mousePressEvent, connectionType);
		connect(imageView, &ImageView::Image_mouseMoveEvent, this, &EditStars::mouseMoveEvent, connectionType);
		connect(imageView, &ImageView::Image_mouseReleaseEvent, this, &EditStars::mouseReleaseEvent, connectionType);
		connect(imageView, &ImageView::Image_resizeEvent, this, &EditStars::resizeMe, connectionType);
		m_bCometMode = cometMode;
		show();
		raise();
	}

	void EditStars::starsButtonPressed()
	{
		starsOrCometButtonPressed(false);
	}

	void EditStars::cometButtonPressed()
	{
		starsOrCometButtonPressed(true);
	}

	void EditStars::saveButtonPressed()
	{
		saveRegisterSettings();
	}

	void EditStars::resizeMe(QResizeEvent* e)
	{
		resize(e->size());
	}

	/*!
	\reimp
	*/
	void EditStars::showEvent(QShowEvent* e)
	{
		resize(imageView->size());
		raise();
		Inherited::showEvent(e);
	}

	void EditStars::paintEvent(QPaintEvent*)
	{
		draw();
	}

	void EditStars::mouseMoveEvent(QMouseEvent* e)
	{
		if (e->modifiers() & Qt::ShiftModifier)
			forceHere = true;
		else
			forceHere = false;

//		if (static_cast<bool>(m_pBitmap))
		{
			//
			// Get the mouse location and convert to image coordinates
			//
			if (auto pt = imageView->screenToImage(QPointF{ e->position() }); pt.x() >= 0 && pt.x() < imageView->imageWidth() && pt.y() >= 0 && pt.y() < imageView->imageHeight())
			{
				// The point is in the image
				m_ptCursor = std::move(pt);
			}
			else
			{
				m_ptCursor.setX(-1);
				m_ptCursor.setY(-1);
			}
		}

		update();

		Inherited::mouseMoveEvent(e);
	}


	void EditStars::setLightFrame(QString name)
	{
		CLightFrameInfo bmpInfo;

//		m_QualityGrid.clear();
		bmpInfo.SetBitmap(name.toStdWString().c_str());
		if (bmpInfo.m_bInfoOk)
		{
			// Get the stars back
			stars = bmpInfo.GetStars();
			std::ranges::sort(stars);
			m_bComet = bmpInfo.m_bComet;
			m_fXComet = bmpInfo.m_fXComet;
			m_fYComet = bmpInfo.m_fYComet;
			m_fLightBkgd = bmpInfo.m_SkyBackground.m_fLight;
		}
		else
			stars.clear();

		fileName = name;
		m_bDirty = false;
		computeOverallQuality();
	}

	void EditStars::computeOverallQuality()
	{
		m_fScore = 0.0;
		m_lNrStars = 0;
		m_fFWHM = 0;
		constexpr double RadiusFactor = 2.35 / 1.5;
		for (const CStar& star : stars)
		{
			if (!star.m_bRemoved)
			{
//				m_fScore += star.m_fQuality;
				m_lNrStars++;
				m_fFWHM += star.m_fMeanRadius * RadiusFactor;
			}
		}
		if (m_lNrStars > 0)
			m_fFWHM /= m_lNrStars;
		std::tie(this->m_fScore, this->m_fQuality) = CRegisteredFrame::ComputeOverallQuality(stars);
	}

//	void EditStars::computeBackgroundValue()
//	{
//		double fResult = 0.0;
//
//		if (static_cast<bool>(m_pBitmap))
//		{
//			CBackgroundCalibration	BackgroundCalibration;
//
//			BackgroundCalibration.m_BackgroundCalibrationMode = BCM_PERCHANNEL;
//			BackgroundCalibration.m_BackgroundInterpolation = BCI_LINEAR;
//			BackgroundCalibration.SetMultiplier(1.0);
//			BackgroundCalibration.ComputeBackgroundCalibration(m_pBitmap.get(), true, nullptr);
//			fResult = BackgroundCalibration.m_fTgtRedBk / 256.0 / 256.0;
//
//			m_fBackground = fResult;
//		}
//	}

//	void EditStars::initGrayBitmap(const QRect& rc)
//	{
//		m_GrayBitmap.SetMultiplier(1.0);
//
//		ZASSERTSTATE(static_cast<bool>(m_pBitmap));
//
//		//
//		// Not worth using OpenMP here - rectangle is quite small
//		//
//		for (int j = rc.top(); j <= rc.bottom(); j++)
// 		{
//			for (int i = rc.left(); i <= rc.right(); i++)
//			{
//				double fGray;
//				m_pBitmap->GetPixel(i, j, fGray);
//				m_GrayBitmap.SetPixel(i - rc.left(), j - rc.top(), fGray / 256.0);
//			}
//		}
//	}

	//
	// pt = mouse cursor
	// rcCheck = rectangle around mouse cursor to detect stars there. Is an output parameter.
	//
//	void EditStars::detectStars(const QPointF& pt, QRect& rcCheck, STARVECTOR& vStars)
//	{
//		// Create a 3*STARMAXSIZE + 1 square rectangle centered on the point
//		vStars.clear();
//		rcCheck.setLeft(pt.x() - RCCHECKSIZE / 2);
//		rcCheck.setRight(pt.x() + RCCHECKSIZE / 2);
//		rcCheck.setTop(pt.y() - RCCHECKSIZE / 2);
//		rcCheck.setBottom(pt.y() + RCCHECKSIZE / 2);
//
//		if (rcCheck.left() < 0)
//		{
//			// Move the rectangle to the right
//			rcCheck.setLeft(0);
//			rcCheck.setRight(RCCHECKSIZE);
//		}
//		else if (rcCheck.right() >= imageView->imageWidth())
//		{
//			rcCheck.setRight(imageView->imageWidth() - 1);
//			rcCheck.setLeft(rcCheck.right() - RCCHECKSIZE);
//		};
//
//		if (rcCheck.top() < 0)
//		{
//			rcCheck.setTop(0);
//			rcCheck.setBottom(RCCHECKSIZE);
//		}
//		else if (rcCheck.bottom() >= imageView->imageHeight())
//		{
//			rcCheck.setBottom(imageView->imageHeight() - 1);
//			rcCheck.setTop(rcCheck.bottom() - RCCHECKSIZE);
//		};
//
//		initGrayBitmap(rcCheck); // rcCheck: l=MouseX-126, r=MouseX+126, t=MouseY-126, b=MouseY+126
//
//		CRegisteredFrame regFrame;
//		DSSRect	rcReg{ STARMAXSIZE, STARMAXSIZE, 
//			rcCheck.width() - (STARMAXSIZE + 1), rcCheck.height() - (STARMAXSIZE + 1) }; // left=50, top=50, right=202, bottom=202
//		STARSET starsInRect;
//
//		// Find stars in rectangle around mouse cursor with a border of 50 pixels.
//		regFrame.m_fBackground = m_fBackground;
//		regFrame.RegisterSubRect(&m_GrayBitmap, rcReg, starsInRect);
//
//		vStars.assign(starsInRect.cbegin(), starsInRect.cend());
//	}

	template <bool WithCross>
	void EditStars::drawCircleAroundStar(const CStar& star, QPainter& painter, const QColor& color, const double radius, const double penWidth) const
	{
		qreal xPos1 = star.m_fX - radius;
		qreal yPos1 = star.m_fY - radius;
		qreal xPos2 = star.m_fX + radius;
		qreal yPos2 = star.m_fY + radius;
		imageView->imageToScreen(xPos1, yPos1);
		imageView->imageToScreen(xPos2, yPos2);

		QPen pen{ color, penWidth };
		painter.setPen(pen);
		painter.drawEllipse(QRectF{ QPointF{ xPos1, yPos1 }, QPointF{ xPos2, yPos2 }});

		// Large stars get an additional cross inside the circle
		if constexpr (WithCross)
		{
			if (radius >= 5.0)
			{
				xPos1 = star.m_fX;
				yPos1 = star.m_fY;
				imageView->imageToScreen(xPos1, yPos1);
				painter.drawLine(QPointF(xPos1 - 5, yPos1), QPointF(xPos1 + 6, yPos1));
				painter.drawLine(QPointF(xPos1, yPos1 - 5), QPointF(xPos1, yPos1 + 6));
			}
		}
	}

	void EditStars::showAction(const CStar& star, QPainter& painter, const bool add) const
	{
		double fRectSize = 20;
		double fDiameter = 10;
		QString strTip;
		QBrush brushAction;
		QPen penAction;

		auto zoom = imageView->zoom();
		if (zoom > 1)
		{
			fRectSize *= zoom;
			fDiameter *= zoom;
		}

		if (add)
		{
			if (m_bCometMode)
				strTip = tr("Click to set the comet here", "IDS_TIP_SETCOMET");
			else
				strTip = tr("Click to add this star", "IDS_TIP_ADDSTAR");
			brushAction.setColor(qRgba(0, 255, 0, 255 * 0.5));
			penAction.setColor(qRgba(0, 255, 0, 255 * 0.7));
			penAction.setWidthF(3.0);
		}
		else
		{
			if (m_bCometMode)
				strTip = tr("Click to remove the comet", "IDS_TIP_REMOVECOMET");
			else
				strTip = tr("Click to remove this star", "IDS_TIP_REMOVESTAR");
			brushAction.setColor(qRgba(255, 255, 0, 255 * 0.5));
			penAction.setColor(qRgba(255, 255, 0, 255 * 0.7));
			penAction.setWidthF(3.0);
		}
		painter.save();
		painter.setBrush(brushAction);
		painter.setPen(penAction);

		double fX = star.m_fX;
		double fY = star.m_fY;
		imageView->imageToScreen(fX, fY);

		constexpr double ninetyDegrees = 90.0 * 16.0;
		constexpr double oneeightyDegrees = 180.0 * 16.0;

		painter.drawArc(QRectF((fX - fRectSize), (fY - fRectSize), fDiameter, fDiameter), -oneeightyDegrees, -ninetyDegrees);
		painter.drawArc(QRectF((fX + fRectSize - fDiameter), (fY - fRectSize), fDiameter, fDiameter), 0.0, ninetyDegrees);
		painter.drawArc(QRectF((fX + fRectSize - fDiameter), (fY + fRectSize - fDiameter), fDiameter, fDiameter), 0.0, -ninetyDegrees);
		painter.drawArc(QRectF((fX - fRectSize), (fY + fRectSize - fDiameter), fDiameter, fDiameter), -ninetyDegrees, -ninetyDegrees);

		QFont font("Helvetica", 10, QFont::Normal);
		painter.setFont(font);
		QFontMetrics fontMetrics(font);
		QSizeF size{ fontMetrics.size(0, strTip) };
		QRectF rect(QPointF(fX, fY - fRectSize), size);

		painter.drawText(rect, strTip, Qt::AlignVCenter | Qt::AlignRight);
		painter.restore();
	}

	void EditStars::showInfoBox(QPainter& painter, QPen& pen, const QRect& rcClient) const
	{
		QRectF rcText;

		const QPoint globalMouseLocation{ QCursor::pos() };
		const QPointF mouseLocation{ mapFromGlobal(globalMouseLocation) };

		QFont font("Helvetica", 9, QFont::Normal);
		painter.setFont(font);
		QFontMetrics fontMetrics(font);

		QString	strText{ tr("#Stars: %1\nScore: %2\nQuality: %3\nFWHM: %4", "IDS_LIGHTFRAMEINFO")
			.arg(m_lNrStars)
			.arg(m_fScore, 0, 'f', 2)
			.arg(m_fQuality, 0, 'f', 2)
			.arg(m_fFWHM, 0, 'f', 2) };

		if (m_bComet)
		{
			QString strComet{ tr("\nComet:%1", "IDS_LIGHTFRAMEINFOCOMET").arg(tr("Yes", "IDS_YES")) };
			strText += strComet;
		}
		constexpr auto txtMargin = 6;
		constexpr auto borderMargin = 2;

		QSizeF size{ fontMetrics.size(0, strText) };
		size.rheight() += txtMargin * 2;
		size.rwidth() += txtMargin * 2;

		if ((mouseLocation.x() >= rcClient.right() - 150) && (mouseLocation.y() <= 150))
		{
			// Draw the rectangle at the left bottom
			rcText.setLeft(2);
			rcText.setTop(rcClient.bottom() - (size.height() + borderMargin));
		}
		else
		{
			rcText.setLeft(rcClient.right() - (size.width() + borderMargin));
			rcText.setTop(2);
		}
		rcText.setRight(rcText.left() + size.width());
		rcText.setBottom(rcText.top() + size.height());

		QBrush brush{ Qt::transparent };
		brush.setColor(qRgba(255, 255, 255, 200));

		painter.fillRect(rcText, brush);

		pen.setColor(qRgba(0, 0, 0, 200));
		pen.setWidthF(1.0);
		painter.setPen(pen);

		painter.drawRect(rcText.adjusted(borderMargin, borderMargin, -borderMargin, -borderMargin));

		brush.setColor(qRgb(0, 0, 0)); painter.setBrush(brush);

		QRectF rect(QPointF(rcText.left() + txtMargin, rcText.top() + txtMargin), size);
		painter.drawText(rect, strText, Qt::AlignLeft | Qt::AlignTop);
	}

	void EditStars::draw()
	{
		if (this->disabled)
			return;
		//
		// Note this creates the star/comet overlay pixmap the same size as the
		// imageView control, *not* the size of the image.  This means all the
		// scaling has to done here.
		//
		QRect rcClient{ rect() };
		// size_t	width = rcClient.width(), height = rcClient.height();

		//
		// Fill the pixmap with transparency
		//
		QPainter painter(this);
		painter.setClipRect(imageView->displayRect);

		painter.setRenderHint(QPainter::Antialiasing);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);

//		if (m_QualityGrid.empty() && !stars.empty())
//			m_QualityGrid.InitGrid(stars);

		//if (displayGrid && !m_QualityGrid.empty())
		//  	drawQualityGrid(painter, rcClient);
		QPen pen(Qt::red, 1.0);
		painter.setPen(pen);

		//
		// If showRefStars: Red circles around the according stars in the reference frame (= refStars).
		//
		if (g_bShowRefStars && !m_bCometMode)
		{
			for (int i = 0; const auto& refStar : refStars)
			{
				if (isRefStarVoted(i))
				{
					drawCircleAroundStar<true>(refStar, painter, Qt::red, refStar.m_fMeanRadius, 1.0);
				}
				++i;
			}
		}

		// Green circles around the registered stars
		for (int i = 0; const auto& star : stars)
		{
			if (isTgtStarVoted(i) && !star.m_bRemoved)
			{
				drawCircleAroundStar<true>(star, painter, star.m_bAdded ? qRgba(0, 0, 255, 255) : qRgba(0, 190, 0, 255), star.m_fMeanRadius, 1.0);

				// If showRefStars: Line from ref-star to target-star.
				// (target-star: star in the current frame, ref-star: according star in the reference frame).
				if (g_bShowRefStars && !refStars.empty())
				{
					QPointF			ptOrg;
					QPointF			ptDst;

					ptOrg.rx() = star.m_fX;
					ptOrg.ry() = star.m_fY;

					ptDst = ptOrg;

					ptDst = transformation.transform(ptOrg);

					ptOrg.rx() += 0.5; ptOrg.ry() += 0.5;
					ptDst.rx() += 0.5; ptDst.ry() += 0.5;

					imageView->imageToScreen(ptOrg.rx(), ptOrg.ry());
					imageView->imageToScreen(ptDst.rx(), ptDst.ry());

					painter.drawLine(ptOrg, ptDst);
				}

				// If showRefStars: Line in direction of large majos axis.
				if (g_bShowRefStars && star.m_fLargeMajorAxis > 0)
				{
					constexpr double DegRadFactor = 3.14159265358979323846 / 180.0;
					QPointF	ptOrg{ star.m_fX + 0.5, star.m_fY + 0.5 };
					QPointF ptDst{ ptOrg };

					ptDst.rx() += star.m_fLargeMajorAxis * cos(star.m_fMajorAxisAngle * DegRadFactor);
					ptDst.ry() += star.m_fLargeMajorAxis * sin(star.m_fMajorAxisAngle * DegRadFactor);

					imageView->imageToScreen(ptOrg.rx(), ptOrg.ry());
					imageView->imageToScreen(ptDst.rx(), ptDst.ry());

					painter.drawLine(ptOrg, ptDst);
				}
			}
			++i;
		}

		//
		// Purple circle around comet.
		//
		if (m_bComet)
		{
			const double radius = imageView->zoom() < 1.0 ? 30.0 / imageView->zoom() : 30.0;
			drawCircleAroundStar<false>(CStar{ m_fXComet, m_fYComet }, painter, qRgba(255, 0, 255, 210), radius, 2.0);
		}

		//
		// Boxes around detected/potential stars.
		// Options to add/remove them or the comet.
		//
		const auto squaredDist = [](const CStar& star, const auto& cursor) -> double
		{
			const double dx = star.m_fX - cursor.x();
			const double dy = star.m_fY - cursor.y();
			return dx * dx + dy * dy;
		};
		const auto findNearestStar = [this, &squaredDist](std::ranges::view auto view) -> const CStar*
		{
			double minDistSqr = std::numeric_limits<double>::max();
			const CStar* p = nullptr;
			for (const CStar& star : view)
			{
				const double dSqr = squaredDist(star, m_ptCursor);
				if (dSqr < minDistSqr)
				{
					minDistSqr = dSqr;
					p = std::addressof(star);
				}
			}
			return p;
		};

		m_Action = EditStarAction::None;

		if (m_ptCursor.x() >= 0 && m_ptCursor.y() >= 0)
		{
			constexpr double MaxDistSqr = 50.0 * 50.0;
			// Find nearest star from those closer than 50.
			const CStar* pNearestStar = findNearestStar(stars | std::ranges::views::filter([this, &squaredDist](const CStar& star) {
				return squaredDist(star, m_ptCursor) <= MaxDistSqr;
			}));

			const auto action = [this, &painter, &rcClient](auto&& star, const bool add) -> void
			{
				m_SelectedStar = std::forward<decltype(star)>(star);
				painter.setClipRect(rcClient);
				showAction(m_SelectedStar, painter, add);
				m_Action = add ? EditStarAction::Add : EditStarAction::Remove;
			};

			if (m_bCometMode)
			{
				if (pNearestStar != nullptr)
				{
					if (m_bComet && pNearestStar->IsInRadius(m_fXComet, m_fYComet))
					{
						action(CStar{ m_fXComet, m_fYComet }, false);
					}
					else
					{
						if (forceHere && !pNearestStar->IsInRadius(m_ptCursor.x(), m_ptCursor.y()))
						{
							action(CStar{ m_ptCursor.x(), m_ptCursor.y() }, true);
						}
						else
						{
							action(*pNearestStar, true);
						}
					}
				}
				else if (forceHere)
				{
					action(CStar{ m_ptCursor.x(), m_ptCursor.y() }, true);
				}
			}
			else // Edit stars mode
			{
				if (pNearestStar != nullptr)
				{
					action(*pNearestStar, pNearestStar->m_bRemoved);
				}
			}

			// Stars that can be added will be white
			for (const auto& star : stars)
			{
				if (star.m_bRemoved)
				{
					drawCircleAroundStar<true>(star, painter, qRgba(255, 255, 255, 180), star.m_fMeanRadius, 1.0);
				}
			}

			showInfoBox(painter, pen, rcClient);
		}
		painter.end();
		return;

#if (0)
		if (m_ptCursor.x() >= 0 && m_ptCursor.y() >= 0)
		{
			QRect					rcCheck;
			STARVECTOR				vStars;
			double					fNearestNewStarDistance;
			bool					bInNewStar;

			double					fNearestOldStarDistance;
			bool					bInOldStar;

			bool					bAdd = true;
			bool					bShowAction = false;
			CStar					star;

			// Find stars in rectangle around mouse cursor.
			// Output: rcCheck = checked rectangle, vStars = detected stars.
			detectStars(m_ptCursor, rcCheck, vStars);

			auto lNearestNewStar = FindNearestStar(m_ptCursor.x() - rcCheck.left(), m_ptCursor.y() - rcCheck.top(), vStars, bInNewStar, fNearestNewStarDistance);

			fNearestOldStarDistance = 50;
			auto lNearestOldStar = FindNearestStarWithinDistance(m_ptCursor.x(), m_ptCursor.y(), stars, bInOldStar, fNearestOldStarDistance);

			m_lRemovedIndice = -1;
			m_bRemoveComet = false;
			if (m_bCometMode)
			{
				if (lNearestNewStar >= 0 || lNearestOldStar >= 0)
				{
					bool		bRemoveComet = false;
					if (m_bComet)
					{
						// Check for comet removal
						if ((lNearestNewStar >= 0) && vStars[lNearestNewStar].IsInRadius(m_fXComet - rcCheck.left(), m_fYComet - rcCheck.top()))
							bRemoveComet = true;
						else if ((lNearestOldStar >= 0) && stars[lNearestOldStar].IsInRadius(m_fXComet, m_fYComet))
							bRemoveComet = true;

						if (bRemoveComet)
						{
							star.m_fX = m_fXComet;
							star.m_fY = m_fYComet;
							bShowAction = true;
							bAdd = false;
						};
					};
					if (!bRemoveComet)
					{
						// Check to add the comet
						if ((lNearestOldStar >= 0) && (lNearestNewStar < 0))
						{
							bAdd = true;
							m_lRemovedIndice = lNearestOldStar;
							star = stars[lNearestOldStar];
							bShowAction = true;
						}
						else if ((lNearestOldStar < 0) && (lNearestNewStar >= 0))
						{
							bAdd = true;
							star = vStars[lNearestNewStar];
							star.m_fX += rcCheck.left();
							star.m_fY += rcCheck.top();
							bShowAction = true;
						}
						else // Both new and old star nearest
						{
							if (fNearestNewStarDistance < fNearestOldStarDistance)
							{
								star = vStars[lNearestNewStar];
								star.m_fX += rcCheck.left();
								star.m_fY += rcCheck.top();
							}
							else
							{
								m_lRemovedIndice = lNearestOldStar;
								star = stars[lNearestOldStar];
							};
							bAdd = true;
							bShowAction = true;
						};
						//
						// forceHere is set by the MouseMove event handler detecting the shift key is pressed...
						//
						if (forceHere)	
						{
							bAdd = true;
							star.m_fX = m_ptCursor.x();
							star.m_fY = m_ptCursor.y();
							bShowAction = true;
						}
					}
				}
				else if (forceHere)
				{
					bAdd = true;
					star.m_fX = m_ptCursor.x();
					star.m_fY = m_ptCursor.y();
					bShowAction = true;
				};
				if (bAdd && m_lRemovedIndice < 0)
				{
					// Check that the comet is not in a existing star
					fNearestOldStarDistance = 50;
					lNearestOldStar = FindNearestStarWithinDistance(star.m_fX, star.m_fY, stars, bInOldStar, fNearestOldStarDistance);
					if (bInOldStar)
						m_lRemovedIndice = lNearestOldStar;
				};
			}
			else
			{
				if (lNearestNewStar >= 0 || lNearestOldStar >= 0)
				{
					if (bInOldStar || lNearestNewStar < 0)
					{
						bAdd = false;
						bShowAction = true;
					}
					else if ((lNearestOldStar < 0) && (lNearestNewStar >= 0))
					{
						bAdd = true;
						bShowAction = true;
					}
					else // Both new and old star nearest
					{
						// If the new star is in an old star - remove
						QPoint		pt(vStars[lNearestNewStar].m_fX + rcCheck.left(), vStars[lNearestNewStar].m_fY + rcCheck.top());
						if (stars[lNearestOldStar].IsInRadius(pt))
							bAdd = false;
						else if (fNearestNewStarDistance >= fNearestOldStarDistance * 1.10)
							bAdd = false;
						else
							bAdd = true;
						bShowAction = true;
					};
				};
				if (bShowAction)
				{
					if (bAdd)
					{
						star = vStars[lNearestNewStar];
						star.m_fX += rcCheck.left();
						star.m_fY += rcCheck.top();

						if (m_bComet && star.IsInRadius(m_fXComet, m_fYComet))
							m_bRemoveComet = true;
					}
					else
						star = stars[lNearestOldStar];
				};
			};


			// Draw all the potentially registrable stars
			for (int i = 0; i < vStars.size(); i++)
			{
				QRect		rc;
				CStar& theStar = vStars[i];

				double		fX, fY;

				fX = theStar.m_fX - theStar.m_fMeanRadius + 0.5 + rcCheck.left();
				fY = theStar.m_fY - theStar.m_fMeanRadius + 0.5 + rcCheck.top();
				imageView->imageToScreen(fX, fY);
				rc.setLeft(fX);
				rc.setTop(fY);

				fX = theStar.m_fX + theStar.m_fMeanRadius + 0.5 + rcCheck.left();
				fY = theStar.m_fY + theStar.m_fMeanRadius + 0.5 + rcCheck.top();
				imageView->imageToScreen(fX, fY);
				rc.setRight(fX);
				rc.setBottom(fY);

				pen.setColor(qRgba(255, 255, 255, 255 * 0.7));

				if (bAdd && bShowAction && lNearestNewStar == i)
					pen.setColor(qRgba(255, 255, 255, 255 * 0.9));
				painter.setPen(pen);

				painter.drawEllipse(rc);

				if (rc.width() > 10 && rc.height() > 10)
				{
					fX = theStar.m_fX + 0.5 + rcCheck.left();
					fY = theStar.m_fY + 0.5 + rcCheck.top();
					imageView->imageToScreen(fX, fY);
					painter.drawLine(QPointF(fX - 5, fY), QPointF(fX + 6, fY));
					painter.drawLine(QPointF(fX, fY - 5), QPointF(fX, fY + 6));
				};
			};
			painter.setClipRect(rcClient);

			if (bShowAction)
			{
				double		fRectSize = 20;
				double		fDiameter = 10;
				double		fX, fY;
				QString		strTip;
				QBrush		brushAction;
				QPen		penAction;

				auto zoom = imageView->zoom();
				if (zoom > 1)
				{
					fRectSize *= zoom;
					fDiameter *= zoom;
				}

				if (bAdd)
				{
					if (m_bCometMode)
						strTip = tr("Click to set the comet here", "IDS_TIP_SETCOMET");
					else
						strTip = tr("Click to add this star", "IDS_TIP_ADDSTAR");
					brushAction.setColor(qRgba(0, 255, 0, 255 * 0.5));
					penAction.setColor(qRgba(0, 255, 0, 255 * 0.7)); pen.setWidthF(3.0);
				}
				else
				{
					if (m_bCometMode)
						strTip = tr("Click to remove the comet", "IDS_TIP_REMOVECOMET");
					else
						strTip = tr("Click to remove this star", "IDS_TIP_REMOVESTAR");
					brushAction.setColor(qRgba(255, 255, 0, 255 * 0.5));
					penAction.setColor(qRgba(255, 255, 0, 255 * 0.7)); pen.setWidthF(3.0);
				};
				painter.save();
				painter.setBrush(brushAction);
				painter.setPen(penAction);

				fX = star.m_fX;		fY = star.m_fY;
				imageView->imageToScreen(fX, fY);

				constexpr double ninetyDegrees { 90.0 * 16.0 };
				constexpr double oneeightyDegrees{ 180.0 * 16.0 };

				painter.drawArc(QRectF((fX - fRectSize), (fY - fRectSize), fDiameter, fDiameter), -oneeightyDegrees, -ninetyDegrees);
				painter.drawArc(QRectF((fX + fRectSize - fDiameter), (fY - fRectSize), fDiameter, fDiameter), 0.0, ninetyDegrees);
				painter.drawArc(QRectF((fX + fRectSize - fDiameter), (fY + fRectSize - fDiameter), fDiameter, fDiameter), 0.0, -ninetyDegrees);
				painter.drawArc(QRectF((fX - fRectSize), (fY + fRectSize - fDiameter), fDiameter, fDiameter), -ninetyDegrees, -ninetyDegrees);

				QFont font("Helvetica", 10, QFont::Normal);
				painter.setFont(font);
				QFontMetrics fontMetrics(font);
				QSizeF size{ fontMetrics.size(0, strTip) };
				QRectF rect (QPointF(fX, fY - fRectSize), size);

				painter.drawText(rect,strTip, Qt::AlignVCenter | Qt::AlignRight);

				m_SelectedStar = star;
				if (m_bCometMode)
					m_Action = bAdd ? EditStarAction::SetComet : EditStarAction::ResetComet;
				else
				{
					m_lRemovedIndice = lNearestOldStar;
					m_Action = bAdd ? EditStarAction::AddStar : EditStarAction::RemoveStar;
				};
				painter.restore();
			};

			{
				QRectF rcText;

				const QPoint globalMouseLocation{ QCursor::pos() };
				const QPointF mouseLocation{ mapFromGlobal(globalMouseLocation) };

				QFont font("Helvetica", 9, QFont::Normal);
				painter.setFont(font);
				QFontMetrics fontMetrics(font);

				QString	strText{ tr("#Stars: %1\nScore: %2\nFWHM: %3", "IDS_LIGHTFRAMEINFO")
					.arg(m_lNrStars)
					.arg(m_fScore, 0, 'f', 2)
					.arg(m_fFWHM, 0, 'f', 2) };

				if (m_bComet)
				{
					QString			strComet{ tr("\nComet:%1", "IDS_LIGHTFRAMEINFOCOMET").arg(tr("Yes", "IDS_YES")) };

					strText += strComet;
				}
				constexpr auto txtMargin = 6;
				constexpr auto borderMargin = 2;

				QSizeF size{ fontMetrics.size(0, strText) };
				size.rheight() += txtMargin * 2;
				size.rwidth() += txtMargin * 2;

				if ((mouseLocation.x() >= rcClient.right() - 150) &&
					(mouseLocation.y() <= 150))
				{
					// Draw the rectangle at the left bottom
					rcText.setLeft(2);
					rcText.setTop(rcClient.bottom() - (size.height() + borderMargin));
				}
				else
				{
					rcText.setLeft(rcClient.right() - (size.width() + borderMargin));
					rcText.setTop(2);
				}
				rcText.setRight(rcText.left() + size.width());
				rcText.setBottom(rcText.top() + size.height());

				QBrush brush{ Qt::transparent };
				brush.setColor(qRgba(255, 255, 255, 200));

				painter.fillRect(rcText, brush);

				pen.setColor(qRgba(0, 0, 0, 200)); pen.setWidthF(1.0);
				painter.setPen(pen);

				painter.drawRect(rcText.adjusted(borderMargin, borderMargin, -borderMargin, -borderMargin));

				brush.setColor(qRgb(0, 0, 0)); painter.setBrush(brush);

				QRectF rect(QPointF(rcText.left() + txtMargin, rcText.top() + txtMargin), size);
				painter.drawText(rect, strText, Qt::AlignLeft | Qt:: AlignTop);
			}
		}
		painter.end();

		//imageView->setOverlayPixmap(pixmap);
		//imageView->update();
#endif
	}

//	void	EditStars::drawQualityGrid(QPainter& painter, const QRect& rcClient)
//	{
//		// Find the first top/left point in the image
//		bool					bDraw = true;
//		decltype(QPointF::xp)	x1, x2, imageWidth { imageView->imageWidth() };
//		decltype(QPointF::yp)	y1, y2, imageHeight{ imageView->imageHeight() };
//
//		QPointF		pt(rcClient.left(), rcClient.top());
//
//		pt = imageView->screenToImage(pt);	// Convert to image co-ordinate
//		if (pt.x() < 0)
//			x1 = 0;
//		else if (pt.x() >= imageWidth)
//			bDraw = false;
//		else
//			x1 = pt.x();
//
//		if (pt.y() < 0)
//			y1 = 0;
//		else if (pt.y() >= imageHeight)
//			bDraw = false;
//		else
//			y1 = pt.y();
//
//		pt.setX(rcClient.right());	pt.setY(rcClient.bottom());
//		pt = imageView->screenToImage(pt);
//
//		if (pt.x() >= imageWidth)
//			x2 = imageWidth - 1;
//		else if (pt.x() < 0)
//			bDraw = false;
//		else
//			x2 = pt.x();
//
//		if (pt.y() >= imageHeight)
//			y2 = imageHeight - 1;
//		else if (pt.y() < 0)
//			bDraw = false;
//		else
//			y2 = pt.y();
//
//		if (bDraw)
//		{
//			for (size_t i = 0; i < m_QualityGrid.triangles.size(); i++)
//			{
//				DelaunayTriangle& tr = m_QualityGrid.triangles[i];
//				QRectF					rc1(x1, y1, x2, y2),
//					rc2,
//					rc3;
//
//				rc2.setLeft(std::min(tr.pt1.x(), std::min(tr.pt2.x(), tr.pt3.x())));
//				rc2.setTop(std::min(tr.pt1.y(), std::min(tr.pt2.y(), tr.pt3.y())));
//				rc2.setRight(std::max(tr.pt1.x(), std::max(tr.pt2.x(), tr.pt3.x())));
//				rc2.setBottom(std::max(tr.pt1.y(), std::max(tr.pt2.y(), tr.pt3.y())));
//
//				if (rc1.intersects(rc2))
//				{
//					rc3 = rc1.intersected(rc2);
//					// Draw the triangle
//					QVector<QPointF>	points;
//
//					points.append(imageView->imageToScreen(tr.pt1));
//					points.append(imageView->imageToScreen(tr.pt2));
//					points.append(imageView->imageToScreen(tr.pt3));
//
//					QPolygonF polygon{ points };
//
//					QRgb colors[3]{ Qt::black, Qt::black, Qt::black };
//					INT				nColors = 3;
//
//					colors[0] = tr.cr1;
//					colors[1] = tr.cr2;
//					colors[2] = tr.cr3;
//
//					PathGradientBrush	brush(&path);
//					brush.SetCenterColor(Color(
//						((double)tr.cr1.GetAlpha() + (double)tr.cr2.GetAlpha() + (double)tr.cr3.GetAlpha()) / 3.0,
//						((double)tr.cr1.GetRed() + (double)tr.cr2.GetRed() + (double)tr.cr3.GetRed()) / 3.0,
//						((double)tr.cr1.GetGreen() + (double)tr.cr2.GetGreen() + (double)tr.cr3.GetGreen()) / 3.0,
//						((double)tr.cr1.GetBlue() + (double)tr.cr2.GetBlue() + (double)tr.cr3.GetBlue()) / 3.0));
//					brush.SetSurroundColors(colors, &nColors);
//
//					painter->FillPolygon(&brush, pt, 3);
//				};
//			};
//		};
//	};

	/* ------------------------------------------------------------------- */

	bool EditStars::event(QEvent* event)
	{
		static unsigned int tipShowCount = 0;

		if (QEvent::ToolTip == event->type())
		{
			//
			// If the mouse is over the image, but not over the toolbar,
			// get the tooltip text and if there is any, display it
			//
			const QPoint globalMouseLocation = QCursor::pos();
			const QPointF mouseLocation = mapFromGlobal(globalMouseLocation);
			if (imageView->displayRect.contains(mouseLocation) && !imageView->m_pToolBar->underMouse() && (tipShowCount % 25 == 0))
			{
				if (const QString tip = toolTip(); !tip.isEmpty())
				{
					QToolTip::showText(globalMouseLocation, tip, this);
				}
				++tipShowCount; // Overflow is no harm, just starts again at 0.
			}
			return true;
		}
		// Make sure the rest of events are handled
		return Inherited::event(event);
	}

	void EditStars::keyPressEvent(QKeyEvent* e)
	{
		bool handled = false;
		switch (e->key())
		{
		case Qt::Key_G:
			// Handle Ctrl+G to toggle grid display
			if (e->modifiers() & Qt::ControlModifier)
			{
//				displayGrid = !displayGrid;
				update();
				
				handled = true;
			}
			break;
		default:
			break;
		}

		if (!handled)
			Inherited::keyPressEvent(e);
	}

	void EditStars::saveRegisterSettings()
	{
		if (!fileName.isEmpty())
		{
			CRegisteredFrame regFrame;
			regFrame.m_bComet = m_bComet;
			regFrame.m_fXComet = m_fXComet;
			regFrame.m_fYComet = m_fYComet;
			regFrame.m_SkyBackground.m_fLight = m_fLightBkgd;

			STARVECTOR vStars;
			std::ranges::copy_if(stars, std::back_inserter(vStars), [](const auto& star) { return !star.m_bRemoved; });

			regFrame.SetStars(vStars);
			stars = std::move(vStars);

			fs::path file{ fileName.toStdU16String() };
			file.replace_extension(".info.txt");

			regFrame.SaveRegisteringInfo(file.generic_wstring().c_str());

			m_bDirty = false;
		}
	}

}
