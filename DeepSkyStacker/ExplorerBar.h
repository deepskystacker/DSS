#pragma once
#include "mrupath.h"
#include "DSSCommon.h"

namespace DSS
{
	namespace Ui {
		class ExplorerBar;
	}

	class ExplorerBar : public QDockWidget
	{
		Q_OBJECT

			typedef QWidget
			Inherited;

	public:
		explicit ExplorerBar(QWidget* parent = nullptr);
		~ExplorerBar();

#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
		inline void setDSSClosing() { dssClosing = true; }
#endif

	public slots:
		void panelChanged(ActivePanel panel);

	signals:
		void addImages(PICTURETYPE type);

		//
		// dssfilelist operations
		//
		void loadList(const QPoint&);
		void clearList();
		void saveList();

		//
		// Check marks
		//
		void checkAbove();
		void checkAll();
		void unCheckAll();

		//
		// Registration
		//
		void registerCheckedImages();
		void computeOffsets();
		void stackCheckedImages();
		void batchStack();

	protected:
		void mousePressEvent(QMouseEvent*) override;
		void showEvent(QShowEvent* event) override;
#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
		void closeEvent(QCloseEvent* event) override;
#endif

	private slots:
		void onOpenLights();
		void onOpenDarks();
		void onOpenFlats();
		void onOpenDarkFlats();
		void onOpenBias();

		void onOpenFilelist();
		void onSaveFilelist();
		void onClearList();

		void onCheckAll();
		void onCheckAbove();
		void onUncheckAll();

		void onRegisterChecked();
		void onComputeOffsets();
		void onStackChecked();
		void onBatchStacking();

		void onOpenPicture();
		void onCopyPicture();
		void onDoStarMask();
		void onSavePicture();

		void onSettings();
		void onDDPSettings();
		void onLoadSettings();
		void onSaveSettings();
		void onRecommendedSettings();

		void onAbout();

		void keepTraceChanged(int);
		void onEnableSoundsStateChanged(int);
		void onShowClippingStateChanged(int);

#if QT_VERSION >= 0x060500
		void onColorSchemeChanged(Qt::ColorScheme colorScheme);
#endif


	private:
		Ui::ExplorerBar* ui;
		bool initialised;
		MRUPath	mruPath;
		QString windowColourName;
		QString activeGroupColourName;

#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
		bool dssClosing;
#endif

		void	LoadSettingFile();
		void	SaveSettingFile();

		void	changeEvent(QEvent*);
		void	makeLinks();

		void onInitDialog();
	};
}