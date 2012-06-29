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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>

#include "translation-handler.h"

class Translator : public QObject
{
    Q_OBJECT

public:
    Translator(QObject *parent = 0);
    ~Translator();

    enum State {
        Empty,
        Generation,
        Translation,
        Done
    };

    State state();

public slots:
    void randomWord();
    void translateWord(const QString &word, TranslationHandler::Type = TranslationHandler::Any);

private slots:
    void onError();
    void onGenerated(const QMultiMap<TranslationHandler::Type,QString> &words);
    void onTranslated(const QString &word,
                      const QMultiMap<TranslationHandler::Type,QString> &translation);
    void iterateTranslation();
    
private:
    State m_state;
    TranslationHandler *m_translator;
    QMultiMap<TranslationHandler::Type,QString> m_words;
    QList<QPair<QString,QString> > m_dictionary;
};

#endif // TRANSLATOR_H
