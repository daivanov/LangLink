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
#include <QApplication>
#include <QWebPage>
#include <QDebug>

#include "link-progress-indicator.h"
#include "link-button.h"
#include "link-view.h"

LinkView::LinkView(int capacity, QObject *parent)
    : QObject(parent),
    m_scene(new QGraphicsScene(this)),
    m_view(new QGraphicsView()),
    m_hSeparator(0),
    m_vSeparator(0),
    m_progressIndicator(0),
    m_button(0),
    m_closeButton(0),
    m_movingItem(0),
    m_activeLines(0),
    m_capacity(capacity)
{
    m_scene->setBackgroundBrush(Qt::black);
    m_scene->installEventFilter(this);

    m_view->setWindowState(m_view->windowState() ^ Qt::WindowFullScreen);
    m_view->setScene(m_scene);
}

LinkView::~LinkView()
{
    delete m_view;
}

bool LinkView::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != m_scene) {
        if (event->type() == QEvent::GraphicsSceneResize) {
            QGraphicsWebView *captcha = qobject_cast<QGraphicsWebView*>(obj);
            if (captcha) {
                /* Center captcha in the screen */
                captcha->setPos(m_scene->sceneRect().center() -
                                captcha->boundingRect().bottomRight() / 2);
            }
        }
        return QObject::eventFilter(obj, event);
    }

    switch (event->type()) {
    case QEvent::WindowActivate: {
        QRectF newSceneRect(QPointF(0.0, 0.0), m_view->maximumViewportSize());
        m_scene->setSceneRect(newSceneRect);
        m_width = newSceneRect.width() / (m_capacity + 1);
        m_height = newSceneRect.height() / 10;
        m_transform.reset();
        m_transform.rotate(-qAsin(m_height / m_width) / M_PI * 180);
        if (!m_closeButton) {
            m_closeButton = new LinkButton(m_height);
            QPolygonF shape;
            shape << QPointF(0.0, 0.25) << QPointF(0.25, 0.0)
                  << QPointF(0.5, 0.25)
                  << QPointF(0.75, 0.0) << QPointF(1.0, 0.25)
                  << QPointF(0.75, 0.5)
                  << QPointF(1.0, 0.75) << QPointF(0.75, 1.0)
                  << QPointF(0.5, 0.75)
                  << QPointF(0.25, 1.0) << QPointF(0.0, 0.75)
                  << QPointF(0.25, 0.5);
            m_closeButton->setPolygon(shape);
            m_closeButton->setCenterPos(mapFromPos(m_capacity));
            m_scene->addItem(m_closeButton);
            connect(m_closeButton, SIGNAL(clicked()),
                    qApp, SLOT(quit()));
        }
        if (!m_progressIndicator) {
            m_progressIndicator =
                new LinkProgressIndicator(QString::number(m_capacity), m_width);
            m_progressIndicator->setCenterPos(m_scene->sceneRect().center());
            m_scene->addItem(m_progressIndicator);
            m_progressIndicator->start();
        }
    }
        break;
    case QEvent::GraphicsSceneMousePress:
        if (!m_movingItem) {
            QGraphicsSceneMouseEvent *mouseEvent =
                static_cast<QGraphicsSceneMouseEvent*>(event);
            if (mouseEvent) {
                QGraphicsItem *item =
                   m_scene->itemAt(mouseEvent->scenePos(), m_transform);
                if (item) {
                    if (m_translatedItems.contains(item)) {
                        m_movingItem = item;
                        m_translation = m_movingItem->pos() - mouseEvent->scenePos();
                        m_gapPos = m_originPos = mapToPos(m_movingItem->pos() +
                                                          m_movingItem->boundingRect().center());
                    } else {
                        LinkItem *linkItem = dynamic_cast<LinkItem*>(item);
                        if (linkItem)
                            linkItem->setNextState();
                    }
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
                        m_scene->itemAt(mapFromPos(pos), m_transform);
                    if (item == m_movingItem) {
                        QList<QGraphicsItem*> collidingItems = m_scene->collidingItems(m_movingItem);
                        if (!collidingItems.isEmpty()) {
                            item = dynamic_cast<LinkItem*>(collidingItems.first());
                        } else {
                            qCritical("Can't find colliding item");
                            item = 0;
                        }
                    }
                    LinkItem *linkItem2 = dynamic_cast<LinkItem*>(item);
                    if (linkItem2 && linkItem2->state() != LinkItem::Correct) {
                        linkItem2->setCenterPos(mapFromPos(m_gapPos));
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
                linkItem->setCenterPos(mapFromPos(m_gapPos));
                if (m_gapPos == m_originPos) {
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

QPointF LinkView::mapFromPos(qreal pos, qreal levelShift) const
{
    return QPointF((pos + 0.5) * m_width,
                   (levelShift + m_activeLines + 0.5) * m_height);
}

void LinkView::appendOriginal(const QString &item)
{
    int cnt = m_originalItems.count();
    LinkItem *linkItem = new LinkItem(item);
    m_activeLines = 1;
    linkItem->setTransform(m_transform);
    linkItem->setCenterPos(mapFromPos(cnt, -1));
    m_originalItems.append(linkItem);
    m_scene->addItem(linkItem);
    m_progressIndicator->setCount(QString::number(m_capacity - cnt - 1));
}

void LinkView::appendTranslation(const QString &item, int pos)
{
    LinkItem *linkItem = new LinkItem(item);
    LinkItem::State state = m_savedStates.take(item);
    if (state == LinkItem::Inactive)
        state = LinkItem::Undefined;
    linkItem->setState(state);
    linkItem->setTransform(m_transform);
    linkItem->setCenterPos(mapFromPos(pos));
    m_translatedItems.append(linkItem);
    m_scene->addItem(linkItem);
    if (m_translatedItems.count() == m_capacity) {
        m_progressIndicator->stop();
        if (!m_hSeparator) {
            m_hSeparator =
                new QGraphicsLineItem(QLineF(mapFromPos(-0.5, - m_activeLines + 0.5),
                                             mapFromPos(m_capacity + 0.5, - m_activeLines + 0.5)));
            m_hSeparator->setPen(QPen(Qt::yellow));
            m_scene->addItem(m_hSeparator);
        } else {
            m_hSeparator->setLine(QLineF(mapFromPos(-0.5, - m_activeLines + 0.5),
                                         mapFromPos(m_capacity + 0.5, - m_activeLines + 0.5)));
        }
        if (!m_vSeparator) {
            m_vSeparator =
                new QGraphicsLineItem(QLineF(mapFromPos(m_capacity - 0.5, - m_activeLines - 0.5),
                                             mapFromPos(m_capacity - 0.5, 0.5)));
            m_vSeparator->setPen(QPen(Qt::yellow));
            m_scene->addItem(m_vSeparator);
        } else {
            m_vSeparator->setLine(QLineF(mapFromPos(m_capacity - 0.5, - m_activeLines - 0.5),
                                         mapFromPos(m_capacity - 0.5, 0.5)));
        }
        if (!m_button) {
            m_button = new LinkButton(m_height);
            QPolygonF shape;
            shape << QPointF(0.0, 0.0) << QPointF(0.5, 1.0) << QPointF(1.0, 0.0);
            m_button->setPolygon(shape);
            m_button->setCenterPos(mapFromPos(m_capacity));
            m_scene->addItem(m_button);
            connect(m_button, SIGNAL(clicked()),
                    SLOT(evaluateLine()));
        } else {
            m_button->setCenterPos(mapFromPos(m_capacity));
            m_button->show();
        }
    }
}

void LinkView::setOverallAssessment(int correct)
{
    LinkItem *assessment = new LinkItem(QString::number(correct));
    assessment->setCenterPos(mapFromPos(m_capacity, -1));
    m_scene->addItem(assessment);
    if (correct == m_capacity) {
        //% "Congratulations!"
        LinkItem *greeting = new LinkItem(qtTrId("qtn_langlink_congrats"));
        greeting->setCenterPos(mapFromPos(m_capacity / 2.0));
        m_scene->addItem(greeting);
        m_progressIndicator->setCount(QString::number(m_capacity));
    }
}

void LinkView::show()
{
    m_view->show();
}

void LinkView::clear()
{
    QList<QGraphicsItem*> items = m_scene->items();
    foreach(QGraphicsItem *item, items) {
        LinkItem *linkItem = dynamic_cast<LinkItem*>(item);
        if (linkItem) {
            m_scene->removeItem(linkItem);
            delete linkItem;
        }
    }
    if (m_button)
        m_button->hide();
    if (m_hSeparator)
        m_hSeparator->setLine(0, 0, 0, 0);
    if (m_vSeparator)
        m_vSeparator->setLine(0, 0, 0, 0);
    if (m_progressIndicator)
        m_progressIndicator->start();
    m_activeLines = 0;
    m_originalItems.clear();
    QRectF newSceneRect(QPointF(0.0, 0.0), m_view->maximumViewportSize());
    m_scene->setSceneRect(newSceneRect);
}

void LinkView::evaluateLine()
{
    if (m_translatedItems.isEmpty()) {
        emit solved();
        return;
    }

    QMap<qreal,QPair<int,QString> > translations;
    int origin = 0;
    while (!m_translatedItems.isEmpty()) {
        QGraphicsItem *item = m_translatedItems.takeFirst();
        LinkItem *linkItem = dynamic_cast<LinkItem*>(item);
        if (linkItem) {
            translations.insert(linkItem->pos().x(),
                                QPair<int,QString>(origin, linkItem->text()));
            m_savedStates.insert(linkItem->text(), linkItem->state());
        }
        ++origin;
    }

    /* Prepare data for next iteration */
    m_activeLines++;
    QRectF sceneRect = m_scene->sceneRect();
    if (sceneRect.height() < (m_activeLines + 1) * m_height) {
        sceneRect.setHeight((m_activeLines + 1) * m_height);
        m_scene->setSceneRect(sceneRect);
    }

    emit result(translations.values());

    /* Update scene */
    m_button->setCenterPos(mapFromPos(m_capacity));
    m_vSeparator->setLine(QLineF(mapFromPos(m_capacity - 0.5, - m_activeLines - 0.5),
                                 mapFromPos(m_capacity - 0.5, 0.5)));
}

void LinkView::captcha(const QWebPage *page)
{
    qDebug("Captcha");
    QGraphicsWebView *webView = new QGraphicsWebView();
    webView->setAttribute(Qt::WA_DeleteOnClose);
    webView->setPage(const_cast<QWebPage*>(page));
    webView->installEventFilter(this);
    connect(page, SIGNAL(loadStarted()),
            webView, SLOT(close()));
    m_scene->addItem(webView);
}
