#include <QtGui/QtGui>

#include "server.h"

LLSubClient::LLSubClient(QObject* parent)
    : QObject(parent)
{
    qDebug() << "run1";
    mGraphicsView = new QGraphicsView;
    qDebug() << "run2";
    mGraphicsScene = new QGraphicsScene(mGraphicsView);
    qDebug() << "run3";
    mView = new QWebView;
    qDebug() << "run4" << mView;
    mPage = new LLWebPage;
    qDebug() << "run5" << mView << mPage;
    mView->setPage(mPage);
    mGraphicsScene->addWidget(mView);
    mGraphicsView->setScene(mGraphicsScene);
    mGraphicsScene->setStickyFocus(true);
    mGraphicsView->viewport()->setParent(0);

    mView->setPage(mPage);
    connect(mPage, SIGNAL(noFollowSchemeUrl(const QUrl &)),
            this, SIGNAL(noFollowSchemeUrl(const QUrl &)));
    connect(mPage, SIGNAL(targetUrlClicked(const QUrl &, const QString&)),
            this, SIGNAL(targetUrlClicked(const QUrl &, const QString&)));

    connect(mPage, SIGNAL(loadProgress(int)),
            this, SIGNAL(loadProgress(int)));
    connect(mPage, SIGNAL(statusBarMessage(const QString &)),
            this, SIGNAL(statusBarMessage(const QString &)));
    connect(mPage->mainFrame(), SIGNAL(urlChanged(const QUrl&)),
            this, SIGNAL(urlChanged(const QUrl&)));
    connect(mPage, SIGNAL(loadFinished(bool)),
            this, SIGNAL(loadFinished(bool)));

    connect(mGraphicsScene, SIGNAL(changed(const QList<QRectF> &)),
            this, SLOT(repaintRequestedSlot(const QList<QRectF> &)));
    qDebug() << "run2";
}

LLSubClient::~LLSubClient()
{
    mGraphicsView->viewport()->setParent(mGraphicsView);
    delete mView;
    delete mGraphicsView;
}

void LLSubClient::resize(int width, int height)
{
    qDebug() << __FUNCTION__;
    mGraphicsView->resize(width, height);
    mView->resize(width, height);
}

void LLSubClient::setUrl(const QUrl &url)
{
    qDebug() << __FUNCTION__ << url;
    mPage->mainFrame()->load(url);
}

void LLSubClient::mouseEvent(QEvent::Type type, Qt::MouseButton button, int x, int y)
{
    qDebug() << __FUNCTION__ << type << button << x << y;
    QMouseEvent event(type, QPoint(x, y), QPoint(x, y), button, 0, 0);
    //qApp->notify(&mView, &event);
    qApp->notify(mGraphicsView->viewport(), &event);
    qDebug() << __FUNCTION__ << type << button << x << y << "done";
}

void LLSubClient::keyEvent(QEvent::Type type, int key, QChar text)
{
    qDebug() << __FUNCTION__;
    QKeyEvent event(type, key, Qt::NoModifier, text);
    qApp->notify(mView, &event);
}

void LLSubClient::triggerAction(QWebPage::WebAction action)
{
    qDebug() << __FUNCTION__;
    mPage->triggerAction(action);
}

void LLSubClient::repaintRequestedSlot(const QList<QRectF> &)
{
    qDebug() << __FUNCTION__;
    /*
    for (int i = 0; i < regions.count(); ++i) {
        LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(),
                regions[i].x(), regions[i].y(), regions[i].width(), regions[i].height());

        window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onPageChanged, event);
    }*/

    qDebug() << "generateImage" << mView->size();
    QImage mImage = QImage(mView->size(), QImage::Format_RGB32);
    QPainter painter(&mImage);

    int x = 0;
    int y = 0;
    int width = mView->width();
    int height = mView->height();
#if 1   // Paint from the graphics view
    QRectF r(x, y, width, height);
    QRect g(0, 0, mView->width(), mView->height());
    mGraphicsView->render(&painter, r, g);
#else   // Paint straight from the web page
    QRegion clip(x, y, width, height);
    mPage->mainFrame()->render(&painter, clip);
#endif
    painter.end();
    emit update(mImage);
}

LLClient::LLClient(QObject* parent)
    : QThread(parent)
    , subClient(0)
{
}

LLClient::~LLClient()
{
    delete subClient;
}


void LLClient::run() {
    subClient = new LLSubClient;
    exec();
}


#include "llmozlib2.h"
#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowserwindow_p.h"
#include <QClipboard>

LLServer::LLServer(QObject* parent)
    : QObject(parent)
{
    qApp->inputContext();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->mimeData();
    
    client = new LLClient;
    client->start();
    while (!client->subClient) {}
    for (int i = 1000009; i != 0; --i){}
    qRegisterMetaType<QEvent::Type>("QEvent::Type");
    qRegisterMetaType<Qt::MouseButton>("Qt::MouseButton");
    connect(this, SIGNAL(resize(int, int)),
            client->subClient, SLOT(resize(int, int)));
    connect(this, SIGNAL(setUrl(const QUrl &)),
            client->subClient, SLOT(setUrl(const QUrl &)));
    connect(client->subClient, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
    connect(client->subClient, SIGNAL(urlChanged(const QUrl&)),
            this, SLOT(urlChanged(const QUrl&)));
    connect(client->subClient, SIGNAL(update(const QImage&)),
            this, SLOT(update(const QImage&)));
    connect(this, SIGNAL(mouseEvent(QEvent::Type, Qt::MouseButton, int, int)),
            client->subClient, SLOT(mouseEvent(QEvent::Type, Qt::MouseButton, int, int)));
    connect(this, SIGNAL(keyEvent(QEvent::Type, int, QChar)),
            client->subClient, SLOT(keyEvent(QEvent::Type, int, QChar)));
    connect(client->subClient, SIGNAL(statusBarMessage(const QString &)),
            this, SLOT(statusBarMessage(const QString &)));
    connect(client->subClient, SIGNAL(noFollowSchemeUrl(const QUrl &)),
            this, SLOT(noFollowSchemeUrl(const QUrl &)));
    connect(client->subClient, SIGNAL(targetUrlClicked(const QUrl &, const QString&)),
            this, SLOT(targetUrlClicked(const QUrl &, const QString&)));
}

LLServer::~LLServer()
{
    client->quit();
    delete client;
}

void LLServer::loadProgress(int progress)
{
    window->d->mPercentComplete = progress;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(), progress);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onUpdateProgress, event);
}

void LLServer::loadFinished(bool ok)
{
    Q_UNUSED(ok);
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(),
            window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onNavigateComplete, event);
}

void LLServer::urlChanged(const QUrl& url)
{
    currentUrl = url;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onLocationChange, event);
}

void LLServer::update(const QImage& image)
{
    currentImage = image;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(),
            0, 0, image.width(), image.height());
//            regions[i].x(), regions[i].y(), regions[i].width(), regions[i].height());

    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onPageChanged, event);
}

void LLServer::statusBarMessage(const QString& message)
{
    window->d->mStatusText = message.toStdString();
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(), window->d->mStatusText);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onStatusTextChange, event);
}

void LLServer::noFollowSchemeUrl(const QUrl &url)
{
    std::string rawUri = QString(url.toEncoded()).toStdString();
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), rawUri, rawUri);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onClickLinkNoFollow, event);
}

void LLServer::targetUrlClicked(const QUrl &url, const QString &target)
{
    window->d->mClickHref = QString::fromUtf8(url.toEncoded()).toStdString();
    window->d->mClickTarget = target.toStdString();
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(),
                                       window->getCurrentUri(),
                                       window->d->mClickHref,
                                       window->d->mClickTarget);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onClickLinkHref, event);
}

