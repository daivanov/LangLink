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

LinkView::LinkView(int capacity, QObject *parent)
    : QObject(parent),
    m_scene(new QGraphicsScene(this)),
    m_view(new QGraphicsView()),
    m_hSeparator(0),
    m_vSeparator(0),
    m_capacity(capacity)
{
    m_scene->setBackgroundBrush(Qt::black);
    m_scene->installEventFilter(this);

    m_view->setWindowState(m_view->windowState() ^ Qt::WindowMaximized);
    m_view->setScene(m_scene);
}

LinkView::~LinkView()
{
    delete m_view;
}

bool LinkView::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::WindowActivate: {
        QRectF newSceneRect(QPointF(0.0, 0.0), m_view->maximumViewportSize());
        m_scene->setSceneRect(newSceneRect);
        m_width = newSceneRect.width() / (m_capacity + 1);
    }
        break;
    default:
        break;
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

void LinkView::appendOriginal(const QString &item)
{
    int cnt = m_originalItems.count();
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(item);
    textItem->setBrush(Qt::yellow);
    QRectF boundary = textItem->boundingRect();
    m_height = 1.5 * boundary.height();
    textItem->setPos((cnt + 0.5) * m_width - boundary.width()/2,
                     0.5 * m_height - boundary.height()/2);
    m_originalItems.append(textItem);
    m_scene->addItem(textItem);
    if (!m_hSeparator) {
        m_hSeparator =
            new QGraphicsLineItem(0, m_height, (cnt + 2) * m_width, m_height);
        m_hSeparator->setPen(QPen(Qt::yellow));
        m_scene->addItem(m_hSeparator);
    } else {
        m_hSeparator->setLine(0, m_height, (cnt + 2) * m_width, m_height);
    }
    if (!m_vSeparator) {
        m_vSeparator =
            new QGraphicsLineItem((cnt + 1) * m_width, 0, (cnt + 1) * m_width, 2 * m_height);
        m_vSeparator->setPen(QPen(Qt::yellow));
        m_scene->addItem(m_vSeparator);
    } else {
        m_vSeparator->setLine((cnt + 1) * m_width, 0, (cnt + 1) * m_width, 2 * m_height);
    }
}

void LinkView::appendTranslation(const QString &item, int pos)
{
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(item);
    textItem->setBrush(Qt::yellow);
    QRectF boundary = textItem->boundingRect();
    m_height = 1.5 * boundary.height();
    textItem->setPos((pos + 0.5) * m_width - boundary.width()/2,
                     1.5 * m_height - boundary.height()/2);
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
