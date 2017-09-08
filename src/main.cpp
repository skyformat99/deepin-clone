#include <QDebug>
#include <QStorageInfo>

#ifdef ENABLE_GUI
#include <DApplication>
#include <DTitlebar>
#include <DThemeManager>

#include "mainwindow.h"

DWIDGET_USE_NAMESPACE
#else
#include <QCoreApplication>
#endif

#include "helper.h"
#include "dglobal.h"
#include "clonejob.h"
#include "commandlineparser.h"

bool Global::isOverride = true;
#ifdef ENABLE_GUI
bool Global::isTUIMode = false;
#else
bool Global::isTUIMode = true;
#endif

int Global::bufferSize = 1024 * 1024;
int Global::compressionLevel = 2;

inline static bool isTUIMode(int argc, char *argv[])
{
#ifndef ENABLE_GUI
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    return true;
#endif

    if (qEnvironmentVariableIsEmpty("DISPLAY"))
        return true;

    for (int i = 1; i < argc; ++i)
        if (argv[i] == QByteArray("--tui"))
            return true;

    return false;
}

int main(int argc, char *argv[])
{
    QCoreApplication *a;

    if (isTUIMode(argc, argv)) {
        Global::isTUIMode = true;

        a = new QCoreApplication(argc, argv);
    }
#ifdef ENABLE_GUI
    else {
        DApplication::loadDXcbPlugin();
        DApplication *app = new DApplication(argc, argv);

        app->setApplicationDisplayName(QObject::tr("Deepin Clone"));
        app->setTheme("light");
        a = app;
    }
#endif

    a->setApplicationName("deepin-clone");
    a->setApplicationVersion("0.0.1");
    a->setOrganizationName("deepin");

    CommandLineParser parser;

    parser.process(*a);

    if (Global::isTUIMode) {
        if (!parser.target().isEmpty()) {
            CloneJob job;

            QObject::connect(&job, &QThread::finished, a, &QCoreApplication::quit);

            job.start(parser.source(), parser.target());

            return a->exec();
        }
    }
#ifdef ENABLE_GUI
    else {
        Global::isOverride = true;

        MainWindow *window = new MainWindow;

        window->setFixedSize(860, 660);
        window->setStyleSheet(DThemeManager::instance()->getQssForWidget("main", window));
        window->setWindowIcon(QIcon::fromTheme("deepin-clone"));
        window->titlebar()->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint
                                           | Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
        window->titlebar()->setIcon(window->windowIcon().pixmap(24));
        window->titlebar()->setTitle(QString());
        window->show();

        qApp->setProductIcon(window->windowIcon().pixmap(128));

        if (!parser.source().isEmpty() && !parser.target().isEmpty()) {
            window->startWithFile(parser.source(), parser.target());
        }

        QObject::connect(a, &QCoreApplication::aboutToQuit, window, &MainWindow::deleteLater);
    }
#endif

    return Global::isTUIMode ? qApp->exec() : a->exec();
}
