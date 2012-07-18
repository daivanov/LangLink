/*
 * Copyright 2012 Daniil Ivanov <daniil.ivanov@gmail.com>
 *
 * This file is part of LangLink.
 * 
 * LangLink is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * LangLink is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with LangLink. If not, see http://www.gnu.org/licenses/.
 */

#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include "translator.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    QString localizationFile = QString("langlink_%1").arg(QLocale::system().name());
    QString localizationDirectory = QCoreApplication::applicationDirPath() +
                                    "/../translation/";
    QTranslator localization;
    if (!localization.load(localizationFile,  localizationDirectory))
        qCritical("Failed to load localization file");
    a.installTranslator(&localization);

    Translator translator;
    translator.randomWord();

    return a.exec();
}
