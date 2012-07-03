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

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsWebView>
#include <QWebPage>
#include <QDebug>

#include "link-view.h"

#define WIDTH 100
#define HEIGHT 25

LinkView::LinkView(QObject *parent)
    : QObject(parent),
    m_scene(new QGraphicsScene(this)),
    m_view(new QGraphicsView()),
    m_hSeparator(0),
    m_vSeparator(0)
{
    m_scene->setBackgroundBrush(Qt::black);

    m_view->setWindowState(m_view->windowState() ^ Qt::WindowMaximized);
    m_view->setScene(m_scene);
}

LinkView::~LinkView()
{
    delete m_view;
}

void LinkView::appendOriginal(const QString &item)
{
    int cnt = m_originalItems.count();
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(item);
    textItem->setBrush(Qt::yellow);
    textItem->setPos((cnt + 0.5) * WIDTH - textItem->boundingRect().width()/2,
                     0.5 * HEIGHT - textItem->boundingRect().height()/2);
    m_originalItems.append(textItem);
    m_scene->addItem(textItem);
    if (!m_hSeparator) {
        m_hSeparator =
            new QGraphicsLineItem(0, HEIGHT, (cnt + 2) * WIDTH, HEIGHT);
        m_hSeparator->setPen(QPen(Qt::yellow));
        m_scene->addItem(m_hSeparator);
    } else {
        m_hSeparator->setLine(0, HEIGHT, (cnt + 2) * WIDTH, HEIGHT);
    }
    if (!m_vSeparator) {
        m_vSeparator =
            new QGraphicsLineItem((cnt + 1) * WIDTH, 0, (cnt + 1) * WIDTH, 2 * HEIGHT);
        m_vSeparator->setPen(QPen(Qt::yellow));
        m_scene->addItem(m_vSeparator);
    } else {
        m_vSeparator->setLine((cnt + 1) * WIDTH, 0, (cnt + 1) * WIDTH, 2 * HEIGHT);
    }
}

void LinkView::appendTranslation(const QString &item, int pos)
{
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(item);
    textItem->setBrush(Qt::yellow);
    textItem->setPos((pos + 0.5) * WIDTH - textItem->boundingRect().width()/2,
                     1.5 * HEIGHT - textItem->boundingRect().height()/2);
    m_translatedItems.append(textItem);
    m_scene->addItem(textItem);
}

void LinkView::show()
{
    m_view->show();
}

void LinkView::captcha(const QWebPage *page)
{
    qDebug("Captcha");
    QGraphicsWebView *webView = new QGraphicsWebView();
    webView->setAttribute(Qt::WA_DeleteOnClose);
    webView->setPage(const_cast<QWebPage*>(page));
    connect(page, SIGNAL(loadStarted()),
            webView, SLOT(close()));
    m_scene->addItem(webView);
}
