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
#include "link-view.h"
#include "link-item.h"

class QNetworkConfigurationManager;
class QNetworkSession;

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

    State state() const;

public slots:
    void randomWord();
    void translateWord(const QString &word, TranslationHandler::Type = TranslationHandler::Any);

private slots:
    void onOnlineStateChanged(bool online);
    void onError();
    void onGenerated(const QMultiMap<TranslationHandler::Type,QString> &words);
    void onTranslated(const QString &word,
                      const QMultiMap<TranslationHandler::Type,QString> &translation);
    void iterateTranslation();
    void onResult(const QList<QPair<int,QString> > &translation);

private:
    void requestOnlineState();
    void requestOfflineState();
    void makeGuess();

    State m_state;
    TranslationHandler *m_translator;
    int m_wordCount;
    QMultiMap<TranslationHandler::Type,QString> m_words;
    QList<QPair<QString,QString> > m_dictionary;
    LinkView *m_view;
    int *m_shuffle;
    QNetworkConfigurationManager *m_networkManager;
    QNetworkSession *m_networkSession;
    bool m_online;
};

#endif // TRANSLATOR_H
