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
#include <QBuffer>
#include <QTimer>
#include <QTime>
#include <QDebug>

#include <QGraphicsScene>
#include <QGraphicsView>

#include "translator.h"

#define WORD_COUNT 10

Translator::Translator(QObject *parent)
    : QObject(parent),
    m_state(Empty),
    m_translator(new TranslationHandler(this)),
    m_wordCount(WORD_COUNT),
    m_view(0)
{
    qsrand(QTime::currentTime().msec());
    connect(m_translator, SIGNAL(error()),
            SLOT(onError()));
    connect(m_translator, SIGNAL(generated(QMultiMap<TranslationHandler::Type,QString>)),
            SLOT(onGenerated(QMultiMap<TranslationHandler::Type,QString>)));
    connect(m_translator, SIGNAL(translated(QString,QMultiMap<TranslationHandler::Type,QString>)),
            SLOT(onTranslated(QString,QMultiMap<TranslationHandler::Type,QString>)));
}

Translator::~Translator()
{
}

Translator::State Translator::state()
{
    return m_state;
}

void Translator::onError()
{
    qDebug("Making another try due to error");
    randomWord();
}

void Translator::randomWord()
{
    if (m_state != Generation || m_state != Translation) {
        m_dictionary.clear();
        m_words.clear();
        m_state = Generation;
        m_translator->generateMultipleWords();

        /* Prepare and expose view */
        if (!m_view) {
            m_view = new LinkView(m_wordCount, this);
            connect(m_translator, SIGNAL(captcha(const QWebPage*)),
                    m_view, SLOT(captcha(const QWebPage*)));
            m_view->show();
        }
    }
}

void Translator::onGenerated(const QMultiMap<TranslationHandler::Type,QString> &words)
{
    m_words = words;
    m_state = Translation;
    QTimer::singleShot(0, this, SLOT(iterateTranslation()));
}

void Translator::iterateTranslation()
{
    int cnt = m_dictionary.count();
    if (cnt >= m_wordCount) {
        m_state = Done;
        m_words.clear();

        int *shuffle = new int[cnt];
        memset(shuffle, -1, cnt * sizeof(int));
        QList<QPair<QString,QString> >::const_iterator i;
        int j = 0;
        for (i = m_dictionary.constBegin(); i != m_dictionary.constEnd(); ++i, ++j) {
            int idx = rand() % (cnt - j);
            if (shuffle[j] == -1) {
                if (shuffle[j + idx] == -1)
                    shuffle[j] = j + idx;
                else
                    shuffle[j] = shuffle[j + idx];
                shuffle[j + idx] = j;
            }
            m_view->appendTranslation(i->second, shuffle[j]);
        }
        delete[] shuffle;
    } else {
        QMapIterator<TranslationHandler::Type,QString> i(m_words);
        if (i.hasNext()) {
            i.next();
            qDebug() << "Generated:" << i.value();
            translateWord(i.value(), i.key());
        }
    }
}

void Translator::translateWord(const QString &word, TranslationHandler::Type type)
{
    m_state = Translation;
    m_translator->translateWord(word, type);
}

void Translator::onTranslated(const QString &word,
    const QMultiMap<TranslationHandler::Type,QString> &translation)
{
    if (translation.count() > 0) {
        qDebug() << "Translation:" << translation;
        QList<TranslationHandler::Type> keys = translation.keys();
        QString value = translation.value(keys.at(rand() % keys.count()));
        m_dictionary << QPair<QString,QString>(word, value);

        m_view->appendOriginal(word);
    } else {
        qDebug("No translation found, skipping");
    }

    QMapIterator<TranslationHandler::Type,QString> i(m_words);
    if (i.hasNext()) {
        i.next();
        m_words.remove(i.key(), i.value());
    }

    QTimer::singleShot(SAFETY_INTERVAL, this, SLOT(iterateTranslation()));
}
