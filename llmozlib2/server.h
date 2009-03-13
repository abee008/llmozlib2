/*
   Copyright (C) 2008 Torch Mobile Inc. http://www.torchmobile.com/
*/

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Torch Mobile Inc. (http://www.torchmobile.com/) code
 *
 * The Initial Developer of the Original Code is:
 *   Benjamin Meyer (benjamin.meyer@torchmobile.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef LLSERVER_H
#define LLSERVER_H

#include <QtWebKit/QtWebKit>

#include "llwebpage.h"

class QGraphicsScene;
class QGraphicsView;

class LLSubClient : public QObject
{
    Q_OBJECT

signals:
    void loadProgress(int);
    void loadFinished(bool ok);
    void urlChanged(const QUrl& url);
    void update(const QImage& image);
    void statusBarMessage(const QString& message);
    void noFollowSchemeUrl(const QUrl &url);
    void targetUrlClicked(const QUrl &url, const QString &target);

public:
    LLSubClient(QObject* parent = 0);
    ~LLSubClient();

public slots:
    void resize(int width, int height);
    void setUrl(const QUrl &url);
    void mouseEvent(QEvent::Type type, Qt::MouseButton button, int x, int y);
    void keyEvent(QEvent::Type type, int key, QChar text);
    void triggerAction(QWebPage::WebAction action);

private slots:
    void repaintRequestedSlot(const QList<QRectF> &);

private:
    QWebView *mView;
    QGraphicsScene *mGraphicsScene;
    QGraphicsView *mGraphicsView;
    LLWebPage *mPage;

};

class LLClient : public QThread
{
    Q_OBJECT

public:
    LLClient(QObject* parent = 0);
    ~LLClient();
    LLSubClient *subClient;

protected:
    void run();

};

class LLEmbeddedBrowserWindow;
class LLServer : public QObject
{
    Q_OBJECT

signals:
    void resize(int width, int height);
    void setUrl(const QUrl &url);
    void mouseEvent(QEvent::Type type, Qt::MouseButton button, int x, int y);
    void keyEvent(QEvent::Type type, int key, QChar text);
    void triggerAction(QWebPage::WebAction action);

public:
    LLServer(QObject* parent = 0);
    ~LLServer();
    LLEmbeddedBrowserWindow *window;

    QUrl currentUrl;
    QImage currentImage;
    LLClient *client;

private slots:
    void loadProgress(int);
    void loadFinished(bool ok);
    void urlChanged(const QUrl& url);
    void update(const QImage& image);
    void statusBarMessage(const QString& message);
    void noFollowSchemeUrl(const QUrl &url);
    void targetUrlClicked(const QUrl &url, const QString &target);

friend class LLEmbeddedBrowserWindow;
};

#endif
