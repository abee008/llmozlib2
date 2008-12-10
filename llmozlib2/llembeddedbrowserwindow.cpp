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
 * The Original Code is Linden Lab Inc. (http://lindenlab.com) code.
 *
 * The Initial Developer of the Original Code is:
 *   Callum Prentice (callum@ubrowser.com)
 *
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Callum Prentice (callum@ubrowser.com)
 *  Benjamin Meyer (benjamin.meyer@torchmobile.com)
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

#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowser_p.h"
#include "llembeddedbrowserwindow_p.h"

#include "llembeddedbrowser.h"
#include "llwebpage.h"

#include <QtGui/QtGui>
#include <QtWebKit/QtWebKit>
#include <QtOpenGL/QtOpenGL>

Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qtiff)
Q_IMPORT_PLUGIN(qico)

QString LLEmbeddedBrowserWindowPrivate::userAgent() const
{
    if (mParent)
    {
        return mParent->d->mUserAgentString;
    }
    return QString();
}

LLEmbeddedBrowserWindow::LLEmbeddedBrowserWindow()
{
    d = new LLEmbeddedBrowserWindowPrivate();
    d->mPage->window = this;
}

LLEmbeddedBrowserWindow::~LLEmbeddedBrowserWindow()
{
    delete d;
}

void LLEmbeddedBrowserWindow::setParent(LLEmbeddedBrowser* parent)
{
    d->mParent = parent;
    d->mPage->setNetworkAccessManager(parent->d->mNetworkAccessManager);
}

// change the background color that gets used between pages (usually white)
void LLEmbeddedBrowserWindow::setBackgroundColor(const quint8 red, const quint8 green, const quint8 blue)
{
    QColor background_color(red, green, blue);
    QPalette p = d->mPage->palette();
    p.setColor(QPalette::Normal, QPalette::Window, background_color);
    d->mPage->setPalette(p);
}

// change the caret color (we have different backgrounds to edit fields - black caret on black background == bad)
void LLEmbeddedBrowserWindow::setCaretColor(const quint8 red, const quint8 green, const quint8 blue)
{
    Q_UNUSED(red);
    Q_UNUSED(green);
    Q_UNUSED(blue);
    // QtWebKit paints the caret so we don't have to
}

//
void LLEmbeddedBrowserWindow::setEnabled(bool enabled)
{
    // what exactly should this do?
    qDebug() << __FUNCTION__ << "Not implemented";
    d->mEnabled = enabled;
}

// allow consumers of this class to observe events - add themselves as an observer
bool LLEmbeddedBrowserWindow::addObserver(LLEmbeddedBrowserWindowObserver* observer)
{
    return d->mEventEmitter.addObserver(observer);
}

// allow consumers of this class to observe events - remove themselves as an observer
bool LLEmbeddedBrowserWindow::remObserver(LLEmbeddedBrowserWindowObserver* observer)
{
    return d->mEventEmitter.remObserver(observer);
}

// used by observers of this class to get the current URI
const std::string LLEmbeddedBrowserWindow::getCurrentUri()
{
    return QString(d->mPage->mainFrame()->url().toEncoded()).toStdString();
}

// utility method that is used by observers to retrieve data after an event
const qint16 LLEmbeddedBrowserWindow::getPercentComplete()
{
    return d->mPercentComplete;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getStatusMsg()
{
    return d->mStatusText;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getClickLinkHref()
{
    std::string result;
    // This function doesn't seem to be used?
    qDebug() << __FUNCTION__ << "Not implemented";
    //return d->mClickHref;
    return result;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getClickLinkTarget()
{
    std::string result;
    // This function doesn't seem to be used?
    qDebug() << __FUNCTION__ << "Not implemented";
    //return d->mClickTarget;
    return result;
}

// render a page into memory and grab the window
// TODO: 0,0, browser width, browser height is always passed in right now
//       need to make this work with arbitrary rects (i.e. the dirty rect)
unsigned char* LLEmbeddedBrowserWindow::grabWindow(int x, int y, int width, int height)
{
    d->mImage = QImage(d->mPage->viewportSize(), QImage::Format_RGB32);
    QPainter painter(&d->mImage);
    QRegion clip(x, y, width, height);
    d->mPage->mainFrame()->render(&painter, clip);
    painter.end();
    if (!d->mFlipBitmap)
    {
        d->mImage = d->mImage.mirrored();
    }
    d->mImage = QGLWidget::convertToGLFormat(d->mImage);
    d->mPageBuffer = d->mImage.bits();
    return d->mPageBuffer;
}

// return the buffer that contains the rendered page
unsigned char* LLEmbeddedBrowserWindow::getPageBuffer()
{
    return d->mPageBuffer;
}

qint16 LLEmbeddedBrowserWindow::getBrowserWidth()
{
    return d->mImage.width();
}

qint16 LLEmbeddedBrowserWindow::getBrowserHeight()
{
    return d->mImage.height();
}

qint16 LLEmbeddedBrowserWindow::getBrowserDepth()
{
    return 4;
}

qint32 LLEmbeddedBrowserWindow::getBrowserRowSpan()
{
    return 4 * getBrowserWidth();
}

bool LLEmbeddedBrowserWindow::navigateTo(const std::string uri)
{
    QUrl url = QUrl(QString::fromStdString(uri));
    navigateStop();
    d->mPage->mainFrame()->load(url);
    return true;
}

bool LLEmbeddedBrowserWindow::canNavigateBack()
{
    return d->mPage->history()->canGoBack();
}

void LLEmbeddedBrowserWindow::navigateStop()
{
    d->mPage->triggerAction(QWebPage::Stop);
}

void LLEmbeddedBrowserWindow::navigateBack()
{
    d->mPage->triggerAction(QWebPage::Back);
}

bool LLEmbeddedBrowserWindow::canNavigateForward()
{
    return d->mPage->history()->canGoForward();
}

void LLEmbeddedBrowserWindow::navigateForward()
{
    d->mPage->triggerAction(QWebPage::Forward);
}

void LLEmbeddedBrowserWindow::navigateReload()
{
    d->mPage->triggerAction(QWebPage::Reload);
}

// set the size of the browser window
bool LLEmbeddedBrowserWindow::setSize(qint16 width, qint16 height)
{
    d->mPage->setViewportSize(QSize(width, height));
    d->mPageBuffer = NULL;
    return true;
}

bool LLEmbeddedBrowserWindow::flipWindow(bool flip)
{
    d->mFlipBitmap = flip;
    return true;
}

void LLEmbeddedBrowserWindow::mouseLeftDoubleClick(qint16 x, qint16 y)
{
    QMouseEvent event(QEvent::MouseButtonDblClick, QPoint(x, y), Qt::LeftButton, 0, 0);
    qApp->sendEvent(d->mPage, &event);
}

void LLEmbeddedBrowserWindow::mouseDown(qint16 x, qint16 y)
{
    QMouseEvent event(QEvent::MouseButtonPress, QPoint(x, y), Qt::LeftButton, 0, 0);
    qApp->sendEvent(d->mPage, &event);
}

void LLEmbeddedBrowserWindow::mouseUp(qint16 x, qint16 y)
{
    QMouseEvent event(QEvent::MouseButtonRelease, QPoint(x, y), Qt::LeftButton, 0, 0);
    qApp->sendEvent(d->mPage, &event);
}

void LLEmbeddedBrowserWindow::mouseMove(qint16 x, qint16 y)
{
    if (x == 0 && y == 0)
    {
        return;
    }
    QMouseEvent event(QEvent::MouseMove, QPoint(x, y), Qt::NoButton, 0, 0);
    qApp->sendEvent(d->mPage, &event);
}

// utility methods to set an error message so something else can look at it
void LLEmbeddedBrowserWindow::scrollByLines(qint16 lines)
{
    int currentScrollValue = d->mPage->mainFrame()->scrollBarValue(Qt::Vertical);
    d->mPage->mainFrame()->setScrollBarValue(Qt::Vertical, currentScrollValue + lines);
}

// accept a (mozilla-style) keycode
void LLEmbeddedBrowserWindow::keyPress(qint16 key_code)
{
    Qt::Key key;
    QChar text;

    switch (key_code)
	{
		case LL_DOM_VK_CANCEL:			key = Qt::Key_Cancel;		break;
		case LL_DOM_VK_HELP:			key = Qt::Key_Help;		break;
		case LL_DOM_VK_BACK_SPACE:		key = Qt::Key_Backspace;	break;
		case LL_DOM_VK_TAB:			key = Qt::Key_Tab;		break;
		case LL_DOM_VK_CLEAR:			key = Qt::Key_Clear;		break;
		case LL_DOM_VK_RETURN:			key = Qt::Key_Return;		break;
		case LL_DOM_VK_ENTER:			key = Qt::Key_Enter;		break;
		case LL_DOM_VK_SHIFT:			key = Qt::Key_Shift;		break;
		case LL_DOM_VK_CONTROL:			key = Qt::Key_Control;		break;
		case LL_DOM_VK_ALT:			key = Qt::Key_Alt;		break;
		case LL_DOM_VK_PAUSE:			key = Qt::Key_Pause;		break;
		case LL_DOM_VK_CAPS_LOCK:		key = Qt::Key_CapsLock;		break;
		case LL_DOM_VK_ESCAPE:			key = Qt::Key_Escape;		break;
		case LL_DOM_VK_SPACE:			key = Qt::Key_Space;		break;
		case LL_DOM_VK_PAGE_UP:			key = Qt::Key_PageUp;		break;
		case LL_DOM_VK_PAGE_DOWN:		key = Qt::Key_PageDown;		break;
		case LL_DOM_VK_END:			key = Qt::Key_End;		break;
		case LL_DOM_VK_HOME:			key = Qt::Key_Home;		break;
		case LL_DOM_VK_LEFT:			key = Qt::Key_Left;		break;
		case LL_DOM_VK_UP:			key = Qt::Key_Up;		break;
		case LL_DOM_VK_RIGHT:			key = Qt::Key_Right;		break;
		case LL_DOM_VK_DOWN:			key = Qt::Key_Down;		break;
		case LL_DOM_VK_PRINTSCREEN:		key = Qt::Key_Print;		break;
		case LL_DOM_VK_INSERT:			key = Qt::Key_Insert;		break;
		case LL_DOM_VK_DELETE:			key = Qt::Key_Delete;		break;
		case LL_DOM_VK_CONTEXT_MENU:	        key = Qt::Key_Menu;		break;

		default:
			key = (Qt::Key)key_code;
			text = QChar(key_code);
		break;
    }

    {
        QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier, text);
        qApp->sendEvent(d->mPage, &event);
    }
    {
        QKeyEvent event(QEvent::KeyRelease, key, Qt::NoModifier, text);
        qApp->sendEvent(d->mPage, &event);
    }
}

// accept keyboard input that's already been translated into a unicode char.
void LLEmbeddedBrowserWindow::unicodeInput(quint32 unicode_char)
{
    Qt::Key key = Qt::Key_unknown;
    QChar input((uint)unicode_char);

    QKeyEvent press_event(QEvent::KeyPress, key, Qt::NoModifier, input);
    qApp->sendEvent(d->mPage, &press_event);
    QKeyEvent release_event(QEvent::KeyRelease, key, Qt::NoModifier, input);
    qApp->sendEvent(d->mPage, &release_event);
}

// give focus to the browser so that input keyboard events work
void LLEmbeddedBrowserWindow::focusBrowser(bool focus_browser)
{
    QFocusEvent event(focus_browser ? QEvent::FocusIn : QEvent::FocusOut, Qt::OtherFocusReason);
    qApp->sendEvent(d->mPage, &event);
}

void LLEmbeddedBrowserWindow::setWindowId(int window_id)
{
    d->mWindowId = window_id;
}

int LLEmbeddedBrowserWindow::getWindowId()
{
    return d->mWindowId;
}

std::string LLEmbeddedBrowserWindow::evaluateJavascript(std::string script)
{
    QString q_script = QString::fromStdString(script);
    QString result = d->mPage->mainFrame()->evaluateJavaScript(q_script).toString();
    return result.toStdString();
}

bool LLEmbeddedBrowserWindow::set404RedirectUrl(std::string redirect_url)
{
    d->m404RedirectUrl = redirect_url;
    return true;
}

bool LLEmbeddedBrowserWindow::clr404RedirectUrl()
{
    d->m404RedirectUrl = std::string();
    return true;
}

void LLEmbeddedBrowserWindow::setNoFollowScheme(std::string scheme)
{
    Q_UNUSED(scheme);
    d->mNoFollowScheme = QString::fromStdString(scheme);
    // The scheme part of the url is what is before '://'
    d->mNoFollowScheme = d->mNoFollowScheme.mid(0,d->mNoFollowScheme.indexOf("://"));
}

std::string LLEmbeddedBrowserWindow::getNoFollowScheme()
{
    return d->mNoFollowScheme.toStdString();
}

