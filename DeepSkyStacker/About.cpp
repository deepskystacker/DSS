#include <QDir>
#include <QSettings>
#include <QString>
#include <QDebug>
#include "About.h"
#include "ui/ui_About.h"
#include "DSSVersion.h"
#include <fitsio.h>
#include <tiffio.h>
#include <libraw/libraw_version.h>

#define xstr(s) str(s)
#define str(s) #s

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    QString strHTML("<body link=#0000ff></body><img valign=top align=right src=\"qrc:///logo.png\">");
    QString strText;
    QString copyright(DSSVER_COPYRIGHT);

    QStringList copyrightList = copyright.split(", ");
    for (auto& i: copyrightList) {
        i = i.replace(" ", "&nbsp;");
    }
    copyright = copyrightList.join(", ");

    strText = QString(QCoreApplication::translate("About", "DeepSkyStacker version %1")).arg(VERSION_DEEPSKYSTACKER);
    strText += "<br>";
    strText += copyright;
    strText = strText.replace("\n", "<br>");
    strHTML += "<b>" + strText + "</b><br>";
    strText = QString("<a href=\"%1\">%1</a><br><br>").arg("http://deepskystacker.free.fr/");
    strHTML += strText;

    strText = QString(QCoreApplication::translate("About",
                "RAW file decoding by LibRaw (version %1)\nCopyright © 1997-2019 LibRaw LLC")).arg(LIBRAW_VERSION_STR);
    strText = strText.replace("\n", "<br>");
    strHTML += strText + "<br>";
    strText = QString("<a href=\"%1\">%1</a><br><br>").arg("http://libraw.org/");
    strHTML += strText;

    copyright = TIFFGetVersion();
    copyright = copyright.remove(0, copyright.indexOf("Version ") + 8);
    copyright = copyright.left(copyright.indexOf("Copyright")-1);
    strText = QString(QCoreApplication::translate("About",
                "TIFF file encoding/decoding by LibTIFF (version %1)\nCopyright © 1988-1996 Sam Leffler\nCopyright © 1991-1996 Silicon Graphics, Inc.")).arg(copyright);
    strText = strText.replace("\n", "<br>");
    strHTML += strText + "<br>";
    strText = QString("<a href=\"%1\">%1</a><br><br>").arg("http://www.remotesensing.org/libtiff/");
    strHTML += strText;

    strText = QString(QCoreApplication::translate("About",
                "FITS decoding by CFitsIO (version %1)\nCopyright NASA")).arg(xstr(CFITSIO_VERSION));
    strText = strText.replace("\n", "<br>");
    strHTML += strText + "<br>";
    strText = QString("<a href=\"%1\">%1</a><br>").arg("http://heasarc.gsfc.nasa.gov/docs/software/fitsio/fitsio.html");
    strHTML += strText;

    ui->setupUi(this);
    ui->comboBox->addItem(tr("Default"), "");
    QDir dir(":/i18n/", "DSS.*.qm");
    for(auto it: dir.entryList())
    {
		QString lang = it.section(".", 1, 1);
        QString langName = QLocale(lang).nativeLanguageName();
        langName[0] = langName[0].toUpper();
        ui->comboBox->addItem(langName, lang);
    }
    setLanguage(QSettings().value("Language", "").toString());
    ui->cbCheckUpdate->setChecked(QSettings().value("InternetCheck", false).toBool());
    ui->html->setText(strHTML);

    strHTML  = "<img valign=center src=\"qrc:///flags/spanish.png\">&nbsp;&nbsp;";
    strHTML += tr("Spanish version: Ricardo Contreras and the team from") + " ";
    strHTML += QString("<a href=\"http://espacioprofundo.com.ar/\">%1</a><br/>").arg(tr("Espacio Profundo"));

    strHTML += "<img valign=center src=\"qrc:///flags/czech.png\">&nbsp;&nbsp;";
    strHTML += tr("Czech version:") + " ";
    strHTML += QString("<a href=\"mailto:lukavsky.premysl@tiscali.cz\">%1</a><br/>").arg(tr("Lukavský Premysl"));

    strHTML += "<img valign=center src=\"qrc:///flags/italian.png\">&nbsp;&nbsp;";
    strHTML += tr("Italian version:") + " ";
    strHTML += QString("<a href=\"http://www.astrofilipc.it\">%1</a><br/>").arg(tr("Fabio Papa"));

    strHTML += "<img valign=center src=\"qrc:///flags/catalan.png\">&nbsp;&nbsp;";
    strHTML += tr("Catalan version:") + " ";
    strHTML += QString("<a href=\"mailto:astroemporda@gmail.com\">%1</a><br/>").arg(tr("Francesc Pruneda"));

    strHTML += "<img valign=center src=\"qrc:///flags/german.png\">&nbsp;&nbsp;";
    strHTML += tr("German version:") + " ";
    strHTML += QString("<a href=\"mailto:kasakow@arcor.de\">%1</a><br/>").arg(tr("Wilhelm-Michael Kasakow"));

    strHTML += "<img valign=center src=\"qrc:///flags/dutch.png\">&nbsp;&nbsp;";
    strHTML += tr("Dutch version:") + " ";
    strHTML += QString("<a href=\"http://www.geocities.com/me_gielis/\">%1</a> %2 ").arg(tr("Eelko Gielis"), tr("and"));
    strHTML += QString("%1<br/>").arg(tr("Inge van de Stadt"));

    strHTML += "<img valign=center src=\"qrc:///flags/taiwan.png\">&nbsp;&nbsp;";
    strHTML += tr("Traditional Chinese version:") + " ";
    strHTML += QString("<a href=\"http://www.aoc.nrao.edu/~whwang/\">%1</a><br/>").arg(tr("Wei-Hao Wang"));

    strHTML += "<img valign=center src=\"qrc:///flags/portugal.png\">&nbsp;<img valign=center src=\"qrc:///flags/brazil.png\">&nbsp;&nbsp;";
    strHTML += tr("Portuguese version:") + " ";
    strHTML += QString("<a href=\"http://www.astrofotografia.biz/\">%1</a><br/>").arg(tr("Manuel Fernandes"));

    strHTML += "<img valign=center src=\"qrc:///flags/romanian.png\">&nbsp;&nbsp;";
    strHTML += tr("Romanian version:") + " ";
    strHTML += QString("<a href=\"http://www.astronomy.ro/\">%1</a><br/>").arg(tr("Emil Kolbert and Valentin Stoica"));

    strHTML += "<img valign=center src=\"qrc:///flags/russian.png\">&nbsp;&nbsp;";
    strHTML += tr("Russian version:") + " ";
    strHTML += QString("<a href=\"http://oleg.milantiev.com/\">%1</a><br/>").arg(tr("Oleg Milantiev"));

    strHTML += "<img valign=center src=\"qrc:///flags/turkish.png\">&nbsp;&nbsp;";
    strHTML += tr("Turkish version:") + " ";
    strHTML += QString("<a href=\"http://www.physics.metu.edu.tr/\">%1</a><br/>").arg(tr("Rasid Tugral"));

    ui->credits->setText(strHTML);
}

About::~About()
{
    delete ui;
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
        emit languageChanged();
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

void About::storeSettings()
{
    QSettings settings;
    settings.setValue("Language", m_Language);
    settings.setValue("InternetCheck", m_InternetCheck);
}