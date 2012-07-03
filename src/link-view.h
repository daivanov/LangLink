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

#ifndef LINK_VIEW_H
#define LINK_VIEW_H

#include <QObject>
class QGraphicsScene;
class QGraphicsView;
class QGraphicsSimpleTextItem;
class QGraphicsLineItem;
class QWebPage;

class LinkView : public QObject
{
    Q_OBJECT

public:
    LinkView(QObject *parent = 0);
    ~LinkView();
    void appendOriginal(const QString &item);
    void appendTranslation(const QString &item, int pos);

public slots:
    void show();
    void captcha(const QWebPage *page);

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QList<QGraphicsSimpleTextItem*> m_originalItems;
    QList<QGraphicsSimpleTextItem*> m_translatedItems;
    QGraphicsLineItem *m_hSeparator;
    QGraphicsLineItem *m_vSeparator;
};

#endif // LINK_VIEW_H
