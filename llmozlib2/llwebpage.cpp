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

#include "llwebpage.h"

#include <qevent.h>
#include <qnetworkrequest.h>
#include <qurl.h>

//#define LINKTARGETPATCH

LLWebPage::LLWebPage(QObject *parent)
    : QWebPage(parent)
{
}

bool LLWebPage::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
        currentPoint = ((QMouseEvent*)event)->pos();
    else if (event->type() == QEvent::MouseButtonRelease)
        currentPoint = QPoint();
    return QWebPage::event(event);
}

bool LLWebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if (request.url().scheme() == mNoFollowScheme)
    {
        emit noFollowSchemeUrl(request.url());
        return false;
    }
    bool accepted = QWebPage::acceptNavigationRequest(frame, request, type);
    if (accepted && type == QWebPage::NavigationTypeLinkClicked) {
        QUrl url = request.url();
        QString target;
#ifdef LINKTARGETPATCH
        QWebHitTestResult hitTest = mainFrame()->hitTestContent(currentPoint);
        target = hitTest.linkTarget();
#endif
        emit targetUrlClicked(url, target);
    }
    return accepted;
}


