#include "stdafx.h"
#include "QEventLogger.h"
#include <QApplication>
#include <QTableView>

QEventLogger::QEventLogger(const QString& logFileBaseName,
    QWidget* mainWidget,
    bool screenshotsEnabled,
    QObject* parent) :
    QObject(parent),
    mainWidget(mainWidget),
    screenshotsEnabled(screenshotsEnabled),
    tableView{ mainWidget->findChild<QTableView*>("tableView") }
{
    // Build log file name.
    QDateTime now = QDateTime::currentDateTime();
    QString fullLogFileName = logFileBaseName + " " + now.toString(Qt::ISODate).replace(":", "-") + ".csv";

    // Open log file.
    this->logFile = new QFile(fullLogFileName);
    if (!this->logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        exit(1);
    this->log = new QTextStream(this->logFile);

    // Write header to log file.
    *log << "; Date and time are: " << now.toString(Qt::ISODate) << '\n';
    *log << "; Resolution: " << mainWidget->size().width() << 'x' << mainWidget->size().height() << '\n';
    *log << "time,input type,event type,target widget class,details\n";
    log->flush();

    // Create the dir in which screenshots will be stored, if requested.
    if (screenshotsEnabled) {
        screenshotDirName = "./screenshots " + now.toString(Qt::ISODate).replace(":", "-");
        qDebug() << QDir().mkdir(screenshotDirName);
    }

    // Start timer.
    this->timer = new QElapsedTimer();
    this->timer->start();

    }

QEventLogger::~QEventLogger()
{
    logFile->close();
    delete log; delete logFile;
    delete timer;
}

bool QEventLogger::eventFilter(QObject* obj, QEvent* event) {
    static QMouseEvent* mouseEvent;
    static QKeyEvent* keyEvent;
    static QHoverEvent* hoverEvent;
    static QFocusEvent* focusEvent;
    static QString eventType, details;
    static int inputType, mouseButton, modifierKey, id;
    static QString inputTypeAsString, className, targetWidget;
    inputType = NONE;

    details = "";
    inputTypeAsString = "";

    switch (event->type()) {
    case QEvent::MouseMove:
        inputType = MOUSE;
        eventType = "MouseMove";
        break;
    case QEvent::MouseTrackingChange:
        inputType = MOUSE;
        eventType = "MouseTrackingChange";
        break;
    case QEvent::MouseButtonPress:
        inputType = MOUSE;
        eventType = "MouseButtonPress";
        break;
    case QEvent::MouseButtonRelease:
        inputType = MOUSE;
        eventType = "MouseButtonRelease";
        break;
    case QEvent::MouseButtonDblClick:
        inputType = MOUSE;
        eventType = "MouseButtonDblClick";
        break;
    case QEvent::KeyPress:
        inputType = KEYBOARD;
        eventType = "KeyPress";
    case QEvent::KeyRelease:
        inputType = KEYBOARD;
        eventType = "KeyRelease";
        break;
    case QEvent::HoverEnter:
        inputType = HOVER;
        eventType = "HoverEnter";
        break;
    case QEvent::GraphicsSceneHoverEnter:
        inputType = HOVER;
        eventType = "GraphicsSceneHoverEnter";
        break;
    case QEvent::HoverLeave:
        inputType = HOVER;
        eventType = "HoverLeave";
        break;
    case QEvent::GraphicsSceneHoverLeave:
        inputType = HOVER;
        eventType = "GraphicsSceneHoverLeave";
        break;
    case QEvent::FocusIn:
        inputType = FOCUS;
        eventType = "FocusIn";
        break;
    case QEvent::FocusOut:
        inputType = FOCUS;
        eventType = "FocusOut";
        break;
    default:
        break;
    }

    if (inputType == MOUSE) {
        mouseEvent = static_cast<QMouseEvent*>(event);

        // Collect the mouse buttons that are pressed.
        mouseButton = mouseEvent->buttons();
        QString buttonsPressed;
        if (mouseButton & Qt::LeftButton)
            buttonsPressed += 'L';
        if (mouseButton & Qt::MiddleButton)
            buttonsPressed += 'M';
        if (mouseButton & Qt::RightButton)
            buttonsPressed += 'R';

        if (event->type() == QEvent::MouseTrackingChange)
            details = "Mouse Tracking Change";
        else
        {
            // Build the details string.
            details += QString::number(mouseEvent->x()) + ';' + QString::number(mouseEvent->y());
            details += ';' + buttonsPressed;
        }

        inputTypeAsString = "Mouse";
    }
    else if (inputType == KEYBOARD) {
        keyEvent = static_cast<QKeyEvent*>(event);

        // Collect the modifier keys that are pressed.
        modifierKey = keyEvent->modifiers();
        QString modifierKeysPressed;
        if (modifierKey & Qt::ShiftModifier)
            modifierKeysPressed += ":shift";
        if (modifierKey & Qt::ControlModifier)
            modifierKeysPressed += ":ctrl";
        if (modifierKey & Qt::AltModifier)
            modifierKeysPressed += ":alt";
        if (modifierKey & Qt::MetaModifier)
            modifierKeysPressed += ":meta";

        // TODO: support special keys, such as ESC and arrow keys, when
        // keyEvent->text() == "".

        // Build the details string.
        details += QKeySequence(keyEvent->key()).toString(QKeySequence::PortableText);
        details += ';' + keyEvent->text();
        details += ';' + modifierKeysPressed;

        inputTypeAsString = "Keyboard";
    }
    else if (inputType == HOVER) {
        hoverEvent = static_cast<QHoverEvent*>(event);

        // qDebug() << hoverEvent << hoverEvent->pos() << obj->metaObject()->className() << obj->inherits("QWidget");
    }
    else if (inputType == FOCUS) {
        focusEvent = static_cast<QFocusEvent*>(event);
        inputTypeAsString = "Focus";
        switch (focusEvent->reason())
        {
        case Qt::MouseFocusReason:
            details += "Mouse action";
            break;
        case Qt::TabFocusReason:
            details += "Tab pressed";
            break;
        case Qt::BacktabFocusReason:
            details += "Backtab pressed";
            break;
        case Qt::ActiveWindowFocusReason:
            details += "Window System";
            break;
        case Qt::PopupFocusReason:
            details += "Popup";
            break;
        case Qt::ShortcutFocusReason:
            details += "Buddy shortcut";
            break;
        case Qt::MenuBarFocusReason:
            details += "Menu bar";
            break;
        case Qt::OtherFocusReason:
            details += "Other";
            break;
        }

        //
        // Add the value of QApplication::focusWidget()
        //
        QObject* object{ nullptr };
        object = QApplication::focusWidget();
        if (nullptr != object)
        {
            className = object->metaObject()->className();
            if (!this->widgetPointerToID.contains(className) || !this->widgetPointerToID[className].contains(object))
            {
                this->widgetPointerToID[className][object] = this->widgetPointerToID[className].size();
            }
            id = this->widgetPointerToID[className][object];
            className += " " + QString::number(id);
            details += ";" + className;
        }
        else details += ";nullptr";

        //
        // Add the value of tableView->focusWidget
        //
        object = this->tableView->focusWidget();
        if (nullptr != object)
        {
            className = object->metaObject()->className();
            if (!this->widgetPointerToID.contains(className) || !this->widgetPointerToID[className].contains(object))
            {
                this->widgetPointerToID[className][object] = this->widgetPointerToID[className].size();
            }
            id = this->widgetPointerToID[className][object];
            className += " " + QString::number(id);
            details += ";" + className;
        }
        else details += ";nullptr";


        // qDebug() << focusEvent << obj->metaObject()->className();
    }

    if (!inputTypeAsString.isEmpty()) {
        className = obj->metaObject()->className();
        if (!this->widgetPointerToID.contains(className) || !this->widgetPointerToID[className].contains(obj)) {
            this->widgetPointerToID[className][obj] = this->widgetPointerToID[className].size();
        }
        id = this->widgetPointerToID[className][obj];
        targetWidget = className + " " + QString::number(id);
        this->appendToLog(inputTypeAsString, eventType, targetWidget, details);
    }

    // Always propagate the event further.
    return false;
}

void QEventLogger::appendToLog(const QString& inputType, const QString& eventType, const QString& targetWidget, const QString& details) {
    static int elapsedTime;

    // Store the amount of time that has elapsed, so there are no inconsistencies between further usages.
    elapsedTime = this->timer->elapsed();

    if (this->screenshotsEnabled && eventType.compare("MouseMove") != 0)
        mainWidget->grab().save(screenshotDirName + "/" + QString::number(elapsedTime) + ".png", "PNG");

    *(this->log) << elapsedTime << ',' << inputType << ',' << eventType << ',' << targetWidget << ',' << details << '\n';
    //qDebug() << elapsedTime << inputType << eventType << targetWidget << details;
    this->log->flush();
}
