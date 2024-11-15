#include "stdafx.h"
#include "About.h"
#include "ui/ui_About.h"
#include "DSSVersion.h"
#include "libraw/libraw_version.h"
#include "fitsio.h"
#include "tiffio.h"
#include "DeepSkyStacker.h"

extern bool	g_bShowRefStars;
extern bool LoadTranslations();

#define xstr(s) str(s)
#define str(s) #s

namespace DSS
{
    About::About(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::About),
        initialised(false)
    {
        QSettings settings;

        QString strHTML("<body link=#0000ff></body><img valign=top align=right src=\":/logo.png\">");
        QString strText;
        QString copyright{ QString::fromLatin1(DSSVER_COPYRIGHT) };

        QStringList copyrightList = copyright.split(", ");
        for (auto& i : copyrightList) {
            i = i.replace(" ", "&nbsp;");
        }
        copyright = copyrightList.join(", ");

        strText = tr("DeepSkyStacker version %1",
            "IDS_ABOUT_DSS").arg(VERSION_DEEPSKYSTACKER);
        strText += "<br>";
        strText += copyright;
        strText = strText.replace("\n", "<br>");
        strHTML += "<b>" + strText + "</b><br>";
        strText = QString("<a href=\"%1\">%1</a>&nbsp;").arg("http://deepskystacker.free.fr/");
        strText += tr("(Now somewhat out of date)") += "<br><br>";
        strHTML += strText;

        strText = "Qt Application Framework 6.8.0\nCopyright © 2024 The Qt Company Ltd.";
        strText += "<br>";
        strText = strText.replace("\n", "<br>");
        strHTML += strText;
        strText = QString("<a href=\"%1\">%1</a><br><br>").arg("https://www.qt.io/");
        strHTML += strText;

        strText = tr("RAW file decoding by LibRaw (version %1)\nCopyright © 1997-2024 LibRaw LLC",
            "IDS_ABOUT_DCRAW").arg(LIBRAW_VERSION_STR);
        strText = strText.replace("\n", "<br>");
        strHTML += strText + "<br>";
        strText = QString("<a href=\"%1\">%1</a><br><br>").arg("http://libraw.org/");
        strHTML += strText;

        copyright = TIFFGetVersion();
        copyright = copyright.remove(0, copyright.indexOf("Version ") + 8);
        copyright = copyright.left(copyright.indexOf("Copyright") - 1);
        strText = tr("TIFF file encoding/decoding by LibTIFF (version %1)\nCopyright © 1988-1997 Sam Leffler\nCopyright © 1991-1996 Silicon Graphics, Inc.",
            "IDS_ABOUT_TIFF").arg(copyright);
        strText = strText.replace("\n", "<br>");
        strHTML += strText + "<br>";
        strText = QString("<a href=\"%1\">%1</a><br><br>").arg("http://www.simplesystems.org/libtiff/");
        strHTML += strText;

        strText = tr("FITS decoding by CFitsIO (version %1)\nCopyright NASA",
            "IDS_ABOUT_FITS").arg(xstr(CFITSIO_VERSION));
        strText = strText.replace("\n", "<br>");
        strHTML += strText + "<br>";
        strText = QString("<a href=\"%1\">%1</a><br><br>").arg("http://heasarc.gsfc.nasa.gov/docs/software/fitsio/fitsio.html");
        strHTML += strText;

        strText = tr("Exif, IPTC, XMP and ICC image metadata by Exiv2 - Version 0.28.0\nCopyright 2004-2023 Exiv2 authors");
        strText = strText.replace("\n", "<br>");
        strHTML += strText + "<br>";
        strText = QString("<a href=\"%1\">%1</a><br><br>").arg("https://github.com/Exiv2/exiv2");
        strHTML += strText;

        strText = tr("SMTP support by SMTP Client for Qt (C++) - Version 0.27.6\nCopyright Tőkés Attila");
        strText = strText.replace("\n", "<br>");
        strHTML += strText + "<br>";
        strText = QString("<a href=\"%1\">%1</a><br>").arg("https://github.com/bluetiger9/SmtpClient-for-Qt");
        strHTML += strText;

        ui->setupUi(this);
        connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &About::storeSettings);
        connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &About::selectLanguage);
        connect(ui->cbCheckUpdate, &QCheckBox::toggled, this, &About::setCheck);
        connect(ui->aboutQt, &QPushButton::clicked, this, &About::aboutQt);

        ui->comboBox->addItem(tr("Default"), "");
        QDir dir(":/i18n/", "DeepSkyStacker_*.qm");
        for (auto it : dir.entryList())
        {
            QString lang{ it.section('_', 1) };
            lang = lang.section('.', 0, 0);
            QLocale locale{ lang };
            QString variant{ lang.section('_', 1) };
            QString langName = locale.nativeLanguageName();
            if ("en" == lang) langName = "English";
            if (!variant.isEmpty())
            {
                langName += " " + locale.nativeTerritoryName();     // was .nativeCountryName()
            }
            ui->comboBox->addItem(langName, lang);
        }
        setLanguage(settings.value("Language", "").toString());
        ui->cbCheckUpdate->setChecked(settings.value("InternetCheck", false).toBool());
        ui->html->setText(strHTML);

        strHTML = "<img valign=center src=\":/flags/spanish.png\">&nbsp;&nbsp;";
        strHTML += tr("Spanish version: Ricardo Contreras and the team from", "IDS_ABOUT_LANG_SPANISH") + " ";
        strHTML += QString("<a href=\"http://espacioprofundo.com.ar/\">%1</a><br>").arg("Espacio Profundo");

        strHTML += "<img valign=center src=\":/flags/czech.png\">&nbsp;&nbsp;";
        strHTML += tr("Czech version:", "IDS_ABOUT_LANG_CZECH") + " ";
        strHTML += QString("<a href=\"mailto:lukavsky.premysl@tiscali.cz\">%1</a><br>").arg("Lukavský Premysl");

        strHTML += "<img valign=center src=\":/flags/italian.png\">&nbsp;&nbsp;";
        strHTML += tr("Italian version:", "IDS_ABOUT_LANG_ITALIAN") + " ";
        strHTML += QString("<a href=\"http://www.astrofilipc.it\">%1</a><br>").arg("Fabio Papa");

        strHTML += "<img valign=center src=\":/flags/catalan.png\">&nbsp;&nbsp;";
        strHTML += tr("Catalan version:", "IDS_ABOUT_LANG_CATALAN") + " ";
        strHTML += QString("<a href=\"mailto:astroemporda@gmail.com\">%1</a><br>").arg("Francesc Pruneda");

        strHTML += "<img valign=center src=\":/flags/german.png\">&nbsp;&nbsp;";
        strHTML += tr("German version:", "IDS_ABOUT_LANG_GERMAN") + " ";
        strHTML += QString("<a href=\"mailto:kasakow@arcor.de\">%1</a> %2<br>").arg("Wilhelm-Michael Kasakow")
            .arg(tr("and") + " Martin Toeltsch");

        strHTML += "<img valign=center src=\":/flags/dutch.png\">&nbsp;&nbsp;";
        strHTML += tr("Dutch version:", "IDS_ABOUT_LANG_DUTCH") + " ";
        strHTML += QString("<a href=\"http://www.geocities.com/me_gielis/\">%1</a><br>").arg("Eelko Gielis " + tr("and") + " Inge van de Stadt");

        strHTML += "<img valign=center src=\":/flags/prc.png\">&nbsp;&nbsp;";
        strHTML += tr("Simplified Chinese version: 张博 (Zhang Bo)<br>");
 
        strHTML += "<img valign=center src=\":/flags/taiwan.png\">&nbsp;&nbsp;";
        strHTML += tr("Traditional Chinese version:", "IDS_ABOUT_LANG_CHINESET") + " ";
        strHTML += QString("<a href=\"http://www.aoc.nrao.edu/~whwang/\">%1</a><br>").arg("王為豪 (Wei-Hao Wang)");

        strHTML += "<img valign=center src=\":/flags/portugal.png\">&nbsp;<img valign=center src=\":/flags/brazil.png\">&nbsp;&nbsp;";
        strHTML += tr("Portuguese version:", "IDS_ABOUT_LANG_PORTUGUESE") + " ";
        strHTML += QString("<a href=\"http://www.elfirmamento.com/\">%1</a> %2<br>").arg("Manuel Fernandes")
            .arg(tr("and") + " Jose Mozart Fusco");

        strHTML += "<img valign=center src=\":/flags/romanian.png\">&nbsp;&nbsp;";
        strHTML += tr("Romanian version:", "IDS_ABOUT_LANG_ROMANIAN") + " ";
        strHTML += QString("<a href=\"http://www.astronomy.ro/\">%1</a> %2<br>").arg("Emil Kolbert " + tr("and") + " Valentin Stoica")
            .arg(tr("and") + " Iustin Amihaesei");

        strHTML += "<img valign=center src=\":/flags/russian.png\">&nbsp;&nbsp;";
        strHTML += tr("Russian version:", "IDS_ABOUT_LANG_RUSSIAN") + " ";
        strHTML += QString("<a href=\"http://oleg.milantiev.com/\">%1</a><br>").arg("Олег Милантьев (Oleg Milantiev)");

        strHTML += "<img valign=center src=\":/flags/turkish.png\">&nbsp;&nbsp;";
        strHTML += tr("Turkish version:", "IDS_ABOUT_LANG_TURKISH") + " ";
        strHTML += QString("<a href=\"http://www.physics.metu.edu.tr/\">%1</a><br>").arg("Raþid Tuðral (Rasid Tugral)");

        ui->credits->setText(strHTML);


    }

    About::~About()
    {
        delete ui;
    }

    void About::showEvent(QShowEvent* event)
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

    void About::onInitDialog()
    {
        QSettings settings;

        //
        // Restore Window position etc..
        //
        QByteArray ba = settings.value("Dialogs/About/geometry").toByteArray();
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

            int top = (r.top() + (r.height() / 2) - (size.height() / 2));
            int left = (r.left() + (r.width() / 2) - (size.width() / 2));
            move(left, top);
        }
    }

    void About::setInternetCheck(bool check)
    {
        if (m_InternetCheck != check)
        {
            m_InternetCheck = check;
            ui->cbCheckUpdate->setChecked(check);
            emit internetCheckChanged();
        }
    }

    void About::setLanguage(QString lang)
    {
        if (m_Language != lang)
        {
            m_Language = lang;
            ui->comboBox->setCurrentIndex(ui->comboBox->findData(m_Language));
            emit languageChanged(lang);
        }
    }

    void About::selectLanguage(int idx)
    {
        setLanguage(ui->comboBox->itemData(idx).toString());
    }

    void About::setCheck(bool check)
    {
        setInternetCheck(check);
    }

    void About::aboutQt()
    {
        qApp->aboutQt();
    }

    void About::storeSettings()
    {
        QSettings settings;

        settings.setValue("Dialogs/About/geometry", saveGeometry());
        settings.setValue("Language", m_Language);
        settings.setValue("InternetCheck", m_InternetCheck);

        LoadTranslations();
    }
}