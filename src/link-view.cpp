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

#include <QtCore/qmath.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWebView>
#include <QWebPage>
#include <QDebug>

#include "link-button.h"
#include "link-item.h"
#include "link-view.h"

LinkView::LinkView(int capacity, QObject *parent)
    : QObject(parent),
    m_scene(new QGraphicsScene(this)),
    m_view(new QGraphicsView()),
    m_hSeparator(0),
    m_vSeparator(0),
    m_button(0),
    m_movingItem(0),
    m_activeLines(0),
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
    case QEvent::GraphicsSceneMousePress:
        if (!m_movingItem) {
            QGraphicsSceneMouseEvent *mouseEvent =
                static_cast<QGraphicsSceneMouseEvent*>(event);
            if (mouseEvent) {
                QGraphicsItem *item =
                    m_scene->itemAt(mouseEvent->scenePos(), QTransform());
                if (item && m_translatedItems.contains(item)) {
                    m_movingItem = item;
                    m_translation = m_movingItem->pos() - mouseEvent->scenePos();
                    m_gapPos = m_originPos = mapToPos(mouseEvent->scenePos());
                }
            }
        }
        break;
    case QEvent::GraphicsSceneMouseMove:
        if (m_movingItem) {
            LinkItem *linkItem = dynamic_cast<LinkItem*>(m_movingItem);
            QGraphicsSceneMouseEvent *mouseEvent =
                static_cast<QGraphicsSceneMouseEvent*>(event);
            if (mouseEvent && linkItem) {
                linkItem->setPos(mouseEvent->scenePos() + m_translation);
                int pos = mapToPos(linkItem->center());
                if (pos != m_gapPos) {
                    QGraphicsItem *item =
                        m_scene->itemAt(mapFromPos(pos), QTransform());
                    if (item == linkItem)
                        qCritical("FIXME: use colliding items");
                    if (item) {
                        QRectF boundary = item->boundingRect();
                        item->setPos(mapFromPos(m_gapPos) - 0.5 * boundary.bottomRight());
                        m_gapPos = pos;
                    }
                }
            }
        }
        break;
    case QEvent::GraphicsSceneMouseRelease:
        if (m_movingItem) {
            LinkItem *linkItem = dynamic_cast<LinkItem*>(m_movingItem);
            if (linkItem) {
                int pos = mapToPos(linkItem->center());
                QRectF boundary = linkItem->boundingRect();
                linkItem->setPos(mapFromPos(pos) - 0.5 * boundary.bottomRight());
                if (pos == m_originPos) {
                    linkItem->setNextState();
                }
            }
            m_movingItem = 0;
        }
        break;
    default:
        break;
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

int LinkView::mapToPos(const QPointF &point) const
{
    int pos = qFloor(point.x() / m_width);
    if (pos >= m_capacity)
        pos = m_capacity - 1;
    else if (pos < 0)
        pos = 0;
    return pos;
}

QPointF LinkView::mapFromPos(int pos) const
{
    return QPointF((pos + 0.5) * m_width, (m_activeLines + 0.5) * m_height);
}

void LinkView::appendOriginal(const QString &item)
{
    int cnt = m_originalItems.count();
    LinkItem *linkItem = new LinkItem(item);
    QRectF boundary = linkItem->boundingRect();
    m_height = 1.5 * boundary.height();
    m_activeLines = 1;
    QPointF center((cnt + 0.5) * m_width, 0.5 * m_height);
    linkItem->setPos(center - 0.5 * boundary.bottomRight());
    m_originalItems.append(linkItem);
    m_scene->addItem(linkItem);
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
    LinkItem *linkItem = new LinkItem(item);
    linkItem->setState(LinkItem::Undefined);
    QRectF boundary = linkItem->boundingRect();
    linkItem->setPos(mapFromPos(pos) - 0.5 * boundary.bottomRight());
    m_translatedItems.append(linkItem);
    m_scene->addItem(linkItem);
    if (!m_button && m_translatedItems.count() == m_capacity) {
        m_button = new LinkButton(0.8 * m_height);
        QRectF boundary = m_button->boundingRect();
        m_button->setPos(mapFromPos(m_capacity) - 0.5 * boundary.bottomRight());
        m_scene->addItem(m_button);
        connect(m_button, SIGNAL(clicked()),
                SLOT(evaluateLine()));
    }
}

void LinkView::setAssessment(int correct)
{
    LinkItem *assessment = new LinkItem(QString::number(correct));
    QRectF boundary = assessment->boundingRect();
    assessment->setPos(mapFromPos(m_capacity) -
                      (QPointF(0, m_height) + 0.5 * boundary.bottomRight()));
    m_scene->addItem(assessment);
}

void LinkView::show()
{
    m_view->show();
}

void LinkView::evaluateLine()
{
    QMap<qreal,QString> translations;
    foreach(QGraphicsItem *item, m_translatedItems) {
        LinkItem *linkItem = dynamic_cast<LinkItem*>(item);
        if (linkItem) {
            translations.insert(linkItem->pos().x(), linkItem->text());
        }
    }
    /* Prepare data for next iteration */
    m_activeLines++;
    int cnt = m_translatedItems.count();
    m_translatedItems.clear();

    emit result(translations.values());

    /* Update scene */
    QRectF boundary = m_button->boundingRect();
    m_button->setPos(mapFromPos(m_capacity) - 0.5 * boundary.bottomRight());
    m_vSeparator->setLine(cnt * m_width, 0, cnt * m_width,
                          (m_activeLines + 1) * m_height);
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
