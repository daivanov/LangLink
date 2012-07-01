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

#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QTimer>
#include <QDebug>

#include "translation-handler.h"

static QString GenerationUrl("http://unique-names.com/random-word.php");
static QString TranslationUrl("http://m.sanakirja.org");

TranslationHandler::Request::Request(Type type) : m_type(type),
    m_page(new QWebPage())
{
}

TranslationHandler::Request::~Request()
{
    m_page->deleteLater();
}

TranslationHandler::TranslationHandler(QObject *parent)
    : QObject(parent), m_pendingKey(0)
{
    m_request.setHeader(QNetworkRequest::ContentTypeHeader,
                         "application/x-www-form-urlencoded; charset=utf-8");
    m_request.setRawHeader("User-Agent",
                            "Mozilla/5.0 (X11; Linux x86_64; rv:13.0) Gecko/20100101 Firefox/13.0");
}

TranslationHandler::~TranslationHandler()
{
    
    for (QHash<QObject*, TranslationHandler::Request*>::iterator i = m_pendingRequests.begin();
         m_pendingRequests.count() > 0;
         i = m_pendingRequests.erase(i))
        delete i.value();
}

void TranslationHandler::generateMultipleWords(Type type)
{
    m_request.setUrl(GenerationUrl);
    Request *req = new Request(type);
    m_pendingRequests.insert(req->webFrame(), req);
    connect(req->webFrame(), SIGNAL(loadFinished(bool)),
            SLOT(onMultipleGenerationFinished(bool)));
    req->webFrame()->load(m_request);
}

void TranslationHandler::onMultipleGenerationFinished(bool ok)
{
    Request *req = m_pendingRequests.take(sender());
    if (ok && req) {
        QMultiMap<Type,QString> words;
        QWebElement table =
            req->webFrame()->findFirstElement("table");
        QWebElementCollection items = table.findAll("li");
        foreach (QWebElement item, items)
            words.insert(Any, item.toPlainText());
        emit generated(words);
    } else {
        qCritical("Generation loading has failed");
        emit error();
    }
    delete req;
}

void TranslationHandler::translateWord(const QString &word, Type type)
{
    QString url = TranslationUrl + QString("/search.php?q=%1&l=3&l2=17").arg(word);
    m_request.setUrl(url);
    Request *req = new Request(type);
    req->m_word = word;
    m_pendingRequests.insert(req->webFrame(), req);
    connect(req->webFrame(), SIGNAL(loadFinished(bool)),
            SLOT(onTranslationFinished(bool)));
    req->webFrame()->load(m_request);
}

void TranslationHandler::onTranslationFinished(bool ok)
{
    Request *req = m_pendingRequests.take(sender());
    if (ok && req) {
        QMultiMap<Type,QString> translation;
        QWebElement table =
            req->webFrame()->findFirstElement("table[id=\"translations\"]");
        if (table.isNull()) {
            QWebElement suggestions =
                req->webFrame()->findFirstElement("ul[id=\"suggestions\"]");
            QWebElementCollection links = suggestions.findAll("a");
            m_pendingKey = sender();
            m_pendingRequests.insert(m_pendingKey, req);
            if (links.count() > 0) {
                /* Try random suggestion */
                QWebElement link = links.at(rand() % links.count());
                req->m_word = link.toPlainText().trimmed();
                qDebug() << "Changing word to" << req->m_word;
                m_request.setUrl(TranslationUrl + link.attribute("href"));
                QTimer::singleShot(SAFETY_INTERVAL, this, SLOT(reTranslate()));
            } else {
                /* Solve captcha */
                QWebElementCollection divs = req->webFrame()->findAllElements("div");
                foreach (QWebElement div, divs) {
                    if (div.attribute("class") != QLatin1String("content search_results")) {
                        QString js =
                            QString("document.getElementById('%1').style.display = 'none'").arg(div.attribute("id"));
                        req->webFrame()->evaluateJavaScript(js);
                    } else {
                        QWebElement p = div.findFirst("p");
                        // TODO: localize string
                        p.setInnerXml("Oops! Your help is required");
                        QWebElement submit = div.findFirst("input[type=\"submit\"]");
                        // TODO: localize string
                        submit.setAttribute("value", "Submit");
                        div.setAttribute("align", "center");
                    }
                }
                emit captcha(req->m_page);
            }
            return;
        } else {
            /* Proceed with translation */
            QWebElementCollection cells = table.findAll("td");
            Type type = Any;
            foreach (QWebElement cell, cells) {
                if (cell.attribute("colspan") != QString()) {
                    QString strType = cell.toPlainText().trimmed();
                    type = stringToType(strType);
                } else {
                    QWebElement anchor = cell.findFirst("a");
                    if (!anchor.isNull() &&
                        (req->m_type == Any || req->m_type == type)) {
                        QString word = anchor.toPlainText().trimmed();
                        translation.insert(type, word);
                    }
                }
            }
        }
        emit translated(req->m_word, translation);
    } else {
        qCritical("Translation loading has failed");
        emit error();
    }
    delete req;
}

void TranslationHandler::reTranslate()
{
    if (m_pendingKey && m_pendingRequests.contains(m_pendingKey)) {
        Request *req = m_pendingRequests.value(m_pendingKey);
        req->webFrame()->load(m_request);
        m_pendingKey = 0;
    }
}

QString TranslationHandler::typeToString(Type type)
{
    Type supportedType;
    if (type < Pronoun)
        supportedType = type;
    else
        supportedType = (Type)(rand() % (Pronoun));

    QString wordType;
    switch (supportedType) {
    case Noun:
        qDebug("Noun");
        return "pos=n";
    case Adjective:
        qDebug("Adjective");
        return "pos=a";
    case Verb:
        qDebug("Verb");
        if (rand() % 2 == 0)
            return "pos=t";
        else
            return "pos=i";
    case Adverb:
        qDebug("Adverb");
        return "pos=e";
    case Interjection:
        qDebug("Interjection");
        return "pos=z";
    case Preposition:
        qDebug("Preposition");
        return "pos=s";
    default:
        qFatal("Unexpected type %u", supportedType);
        break;
    }

    return "";
}

TranslationHandler::Type TranslationHandler::stringToType(const QString &strType)
{
    if (strType == "Substantiivit")
        return Noun;
    if (strType == "Pronominit")
        return Pronoun;
    if (strType == "Adjektiivit")
        return Adjective;
    if (strType == "Verbit")
        return Verb;
    if (strType == "Adverbit")
        return Adverb;
    if (strType == "Prepositiot")
        return Preposition;
    if (strType == "Konjunktiot")
        return Conjunction;
    if (strType == "Huudahdukset")
        return Interjection;
    if (strType == "Partikkelit")
        return Particle;

    qWarning() << "Unrecognized type" << strType;
    return Any;
}
