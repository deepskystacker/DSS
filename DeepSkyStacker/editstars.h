#pragma once
#include "Stars.h"
#include "MatchingStars.h"
#include "GrayBitmap.h"
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
class QMouseEvent;
class CMemoryBitmap;
class CBilinearParameters;
extern bool g_bShowRefStars;

namespace DSS
{
	class ImageView;

//	enum class EditStarAction
//	{
//		None = 0,
//		AddStar,
//		RemoveStar,
//		SetComet,
//		ResetComet
//	};

	class DelaunayTriangle
	{
	public:
		QPointF			pt1, pt2, pt3;
		QRgb			cr1, cr2, cr3;

	public:
		DelaunayTriangle()
		{
		};
		~DelaunayTriangle()
		{
		};

		DelaunayTriangle(const DelaunayTriangle& rhs) = default;
		DelaunayTriangle(DelaunayTriangle&& rhs) = default;

		DelaunayTriangle& operator = (const DelaunayTriangle& rhs) = default;
		DelaunayTriangle& operator = (DelaunayTriangle&& rhs) = default;

		QPointF inCentre()
		{
			const qreal a{ hypotf(pt1.x() - pt2.x(), pt1.y() - pt2.y()) };
			const qreal b{ hypotf(pt2.x() - pt3.x(), pt2.y() - pt3.y()) };
			const qreal c{ hypotf(pt3.x() - pt1.x(), pt3.y() - pt1.y()) };

			const qreal x{ (a * pt1.x() + b * pt2.x() + c * pt3.x()) /
				(a + b + c) };
			const qreal y{ (a * pt1.y() + b * pt2.y() + c * pt3.y()) /
				(a + b + c) };

			return QPointF(x, y);
		}

	};

	typedef std::vector<DelaunayTriangle>	DelaunayTriangleVector;

//	class QualityGrid
//	{
//	public:
//		DelaunayTriangleVector	triangles;
//		double					mean;
//		double					stdDev;
//
//	private:
//	public:
//		QualityGrid() :
//			mean{ 0.0 },
//			stdDev{ 0.0 }
//		{
//		};
//
//		void	InitGrid(STARVECTOR& vStars);
//
//		void	clear()
//		{
//			triangles.clear();
//		};
//
//		bool	empty()
//		{
//			return triangles.empty();
//		};
//	};

    class EditStars final : public QWidget
    {
        Q_OBJECT

            typedef QWidget
            Inherited;

    public:
		explicit EditStars(QWidget* parent);

        // No assignment of or copy construction of this
        EditStars(const EditStars&) = delete;
		EditStars(EditStars&&) = delete;
        EditStars& operator=(const EditStars&) = delete;

		~EditStars() = default;

		bool isDirty() const;

		void setTransformation(const CBilinearParameters& Tr, const VOTINGPAIRVECTOR& vVP);
		void setBitmap(std::shared_ptr<CMemoryBitmap> bmp);
		void setRefStars(STARVECTOR const& Stars);
		void clearRefStars();
		void saveRegisterSettings();

	private:
		void starsOrCometButtonPressed(const bool cometMode);

    public slots:
		void leaveEvent(QEvent* e);
        void mousePressEvent(QMouseEvent* e);
        void mouseMoveEvent(QMouseEvent* e);
        void mouseReleaseEvent(QMouseEvent* e);
		void resizeMe(QResizeEvent* e);

        void rectButtonPressed();
		void starsButtonPressed();
		void cometButtonPressed();
        void saveButtonPressed();

        void setLightFrame(QString name);
		inline void setCometMode(bool mode)
		{
			m_bCometMode = mode;
		}

	protected:
		bool event(QEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void paintEvent(QPaintEvent*) override;
		void showEvent(QShowEvent* e) override;

    private:
		enum class EditStarAction {
			None = 0,
			Add,
			Remove
		};
        ImageView* imageView;
        QString fileName;
        STARVECTOR	stars;
        STARVECTOR	refStars;
//      std::shared_ptr<CMemoryBitmap> m_pBitmap;
        CBilinearParameters transformation;
        VOTINGPAIRVECTOR vVotedPairs;
		QPointF m_ptCursor;
//		CGrayBitmap					m_GrayBitmap; // CGrayBitmapT<double>
		EditStarAction m_Action;
		CStar m_SelectedStar;
//		int							m_lRemovedIndice;
//		bool						m_bRemoveComet;
		bool m_bCometMode;
		double m_fXComet;
		double m_fYComet;
		bool m_bComet;
		double m_fLightBkgd;
		bool m_bDirty;
		double m_fScore;
		double m_fQuality{ 0.0 };
		int m_lNrStars;
		double m_fFWHM;
//		double						m_fBackground;
//		QualityGrid					m_QualityGrid;
		bool forceHere;
//		bool displayGrid;
//		uint m_tipShowCount;
		bool disabled;

		template <bool Refstar>
		bool isStarVoted(const int starIndex)
		{
			if (!g_bShowRefStars || this->vVotedPairs.empty())
				return true;

			if constexpr (Refstar)
				return std::ranges::find_if(vVotedPairs, [starIndex](const CVotingPair& votedPair) { return votedPair.m_RefStar == starIndex; }) != vVotedPairs.end();
			else
				return std::ranges::find_if(vVotedPairs, [starIndex](const CVotingPair& votedPair) { return votedPair.m_TgtStar == starIndex; }) != vVotedPairs.end();
		}

		bool isRefStarVoted(const int lStar)
		{
			return this->isStarVoted<true>(lStar);
		}

		bool isTgtStarVoted(const int lStar)
		{
			return this->isStarVoted<false>(lStar);
		}


//		void initGrayBitmap(const QRect& rc);
//		void detectStars(const QPointF& pt, QRect& rc, STARVECTOR& vStars);

		void computeOverallQuality();
//		void computeBackgroundValue();

		template <bool ComputeQuality>
		void addOrRemoveStar(const CStar& star, const bool remove);
		template <bool WithCross>
		void drawCircleAroundStar(const CStar& star, QPainter& painter, const QColor& color, const double radius, const double penWidth) const;
		void showAction(const CStar& star, QPainter& painter, const bool add) const;
		void showInfoBox(QPainter& painter, QPen& pen, const QRect& rcClient) const;
		void draw();

//		void drawQualityGrid(QPainter& painter, const QRect& rcClient);

		/*!
        \fn void EditStars::setGeometry(const QRect &rect)

        Sets the geometry of to \a rect, specified in the coordinate system
        of its parent widget.

        \sa QWidget::geometry
	    */
		inline void setGeometry(const QRect& geom)
		{
			Inherited::setGeometry(geom);
		}

    };
}
