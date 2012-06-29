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

#ifndef TRANSLATOR_HANDLER_H
#define TRANSLATOR_HANDLER_H

#include <QNetworkRequest>
#include <QMap>
#include <QWebPage>

#define SAFETY_INTERVAL 250

class TranslationHandler : public QObject
{
    Q_OBJECT
public:
    TranslationHandler(QObject *parent = 0);
    ~TranslationHandler();

    enum Type {
        Noun,
        Adjective,
        Verb,
        Adverb,
        Preposition,
        Interjection,
        Pronoun,
        Conjunction,
        Particle,
        Any
    };

    class Request {
    public:
        Type m_type;
        QString m_word;
        QWebPage *m_page;

        Request(Type type);
        ~Request();
        QWebFrame *webFrame() { return m_page ? m_page->mainFrame() : 0; };
    };

    void generateMultipleWords(Type type = Any);
    void translateWord(const QString &word, Type type = Any);
    
signals:
    void translated(const QString &word,
                    const QMultiMap<TranslationHandler::Type,QString> &translation);
    void generated(const QMultiMap<TranslationHandler::Type,QString> &words);
    void error();
    
private slots:
    void onMultipleGenerationFinished(bool ok);
    void onTranslationFinished(bool ok);
    void reTranslate();

private:
    Type stringToType(const QString &strType);
    QString typeToString(Type type);

    QHash<QObject*, Request*> m_pendingRequests;
    QNetworkRequest m_request;
    QObject *m_pendingKey;
};

#endif // TRANSLATOR_HANDLER_H
