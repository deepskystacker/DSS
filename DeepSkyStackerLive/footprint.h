#pragma once

#include <QWidget>

namespace DSS
{
	class ImageView;
	using Inherited = QWidget;
	class Footprint  : public QWidget
	{
		Q_OBJECT

	private:
		ImageView* imageView;
		QPointF pt1_;
		QPointF pt2_;
		QPointF pt3_;
		QPointF pt4_;

	protected:
		void paintEvent(QPaintEvent*) override;
		void changeEvent(QEvent*) override;
		void showEvent(QShowEvent*) override;

	protected slots:
		void resizeMe(QResizeEvent* e);

	public:
		Footprint(QWidget *parent);
		~Footprint();

		inline void setFootprint(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4)
		{
			pt1_ = pt1;
			pt2_ = pt2;
			pt3_ = pt3;
			pt4_ = pt4;
		};

		inline void clearFootprint()
		{
			QPointF null;
			pt1_ = pt2_ = pt3_ = pt4_ = null;
		};
	};


} // namespace DSS
