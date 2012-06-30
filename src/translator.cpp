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
#include <QWebView>
#include <QWebFrame>
#include <QBuffer>
#include <QTimer>
#include <QTime>
#include <QDebug>

#include "translator.h"

Translator::Translator(QObject *parent)
    : QObject(parent),
    m_state(Empty),
    m_translator(new TranslationHandler(this))
{
    qsrand(QTime::currentTime().msec());
    connect(m_translator, SIGNAL(error()),
            SLOT(onError()));
    connect(m_translator, SIGNAL(generated(QMultiMap<TranslationHandler::Type,QString>)),
            SLOT(onGenerated(QMultiMap<TranslationHandler::Type,QString>)));
    connect(m_translator, SIGNAL(translated(QString,QMultiMap<TranslationHandler::Type,QString>)),
            SLOT(onTranslated(QString,QMultiMap<TranslationHandler::Type,QString>)));
    connect(m_translator, SIGNAL(captcha(const QWebPage*)),
            SLOT(onCaptcha(const QWebPage*)));
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
    QMapIterator<TranslationHandler::Type,QString> i(m_words);
    if (i.hasNext()) {
        i.next();
        qDebug() << "Generated:" << i.value();
        translateWord(i.value(), i.key());
    } else {
        qDebug() << m_dictionary;
        m_state = Done;
        qApp->quit();
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

void Translator::onCaptcha(const QWebPage *page)
{
    QWebView *view = new QWebView();
    view->setPage(const_cast<QWebPage*>(page));
    view->show();
    connect(page, SIGNAL(loadStarted()),
            view, SLOT(hide()));
    connect(page, SIGNAL(loadStarted()),
            view, SLOT(deleteLater()));
}