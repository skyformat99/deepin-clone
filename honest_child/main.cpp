/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDebug>

#include <unistd.h>

int main(int argc, char *argv[])
{
    if (!qEnvironmentVariableIsEmpty(DEEPIN_CLONE_UID)) {
        const quint32 &uid = qgetenv(DEEPIN_CLONE_UID).toUInt();

        setuid(uid);
    }

    QApplication app(argc, argv);

    Q_UNUSED(app)

    if (qEnvironmentVariableIsSet(DEEPIN_CLONE_OPEN_DIALOG)) {
        QFileDialog dialog(0, QString(), QDir::homePath());

        dialog.setWindowFlags(dialog.windowFlags() | Qt::WindowStaysOnTopHint);
        dialog.setMimeTypeFilters(QStringList() << "application-x-deepinclone-dim");
        dialog.setNameFilters(QStringList() << QString::fromUtf8(qgetenv(DEEPIN_CLONE_NAME_FILTER)) + "(*.dim)");
        dialog.setDefaultSuffix("dim");
        dialog.setWindowTitle(qgetenv(DEEPIN_CLONE_TITLE));

        if (qgetenv(DEEPIN_CLONE_OPEN_DIALOG) == DEEPIN_CLONE_GET_FILE) {
            dialog.setFileMode(QFileDialog::AnyFile);
            dialog.setAcceptMode(QFileDialog::AcceptSave);
            dialog.selectFile(QString::fromUtf8(qgetenv(DEEPIN_CLONE_FILE_NAME)));
        } else {
            dialog.setFileMode(QFileDialog::ExistingFile);
            dialog.setAcceptMode(QFileDialog::AcceptOpen);
        }

        qputenv(DEEPIN_CLONE_FILE_NAME, "");

        if (dialog.exec() == QFileDialog::Accepted) {
            printf("%s", dialog.selectedFiles().first().toUtf8().constData());
        }
    } else if (qEnvironmentVariableIsSet(DEEPIN_CLONE_OPEN_URL)) {
        QDesktopServices::openUrl(QUrl(QString::fromUtf8(qgetenv(DEEPIN_CLONE_OPEN_URL))));
    }

    return 0;
}
