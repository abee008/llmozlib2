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

#ifndef LLEMBEDDEDBROWSERWINDOW_H
#define LLEMBEDDEDBROWSERWINDOW_H

#include <Qt/qglobal.h>
#include <QtGui/QtGui>

#include <string>
#include <list>
#include <algorithm>

#include "llmozlib2.h"

///////////////////////////////////////////////////////////////////////////////
// manages the process of storing and emitting events that the consumer
// of the embedding class can observe
template< class T >
class LLEmbeddedBrowserWindowEmitter
{
    public:
        LLEmbeddedBrowserWindowEmitter() { };
        ~LLEmbeddedBrowserWindowEmitter() { };

        typedef typename T::EventType EventType;
        typedef std::list< T* > ObserverContainer;
        typedef void( T::*observerMethod )( const EventType& );

        ///////////////////////////////////////////////////////////////////////////////
        //
        bool addObserver( T* observerIn )
        {
            if ( ! observerIn )
                return false;

            if ( std::find( observers.begin(), observers.end(), observerIn ) != observers.end() )
                return false;

            observers.push_back( observerIn );

            return true;
        };

        ///////////////////////////////////////////////////////////////////////////////
        //
        bool remObserver( T* observerIn )
        {
            if ( ! observerIn )
                return false;

            observers.remove( observerIn );

            return true;
        };

        ///////////////////////////////////////////////////////////////////////////////
        //
        void update( observerMethod method, const EventType& msgIn )
        {
            typename std::list< T* >::iterator iter = observers.begin();

            while( iter != observers.end() )
            {
                ( ( *iter )->*method )( msgIn );

                ++iter;
            };
        };

    protected:
        ObserverContainer observers;
};

class LLEmbeddedBrowser;

////////////////////////////////////////////////////////////////////////////////
// class for a "window" that holds a browser - there can be lots of these
class WebPage;
class LLEmbeddedBrowserWindow
{
public:
    LLEmbeddedBrowserWindow();
    virtual ~LLEmbeddedBrowserWindow();

    // housekeeping
    //nsresult createBrowser( void* nativeWindowHandleIn, qint32 widthIn, qint32 heightIn, nsIWebBrowser** aBrowser );
    void setParent( LLEmbeddedBrowser* parentIn ) { mParent = parentIn; };
    bool setSize( qint16 widthIn, qint16 heightIn );
    void focusBrowser( bool focusBrowserIn );
    void scrollByLines( qint16 linesIn );
    void setWindowId( int windowIdIn );
    int getWindowId();

    // random accessors
    const qint16 getPercentComplete();
    const std::string getStatusMsg();
    const std::string getCurrentUri();
    const std::string getClickLinkHref();
    const std::string getClickLinkTarget();

    // memory buffer management
    unsigned char* grabWindow( int xIn, int yIn, int widthIn, int heightIn );
    bool flipWindow( bool flip );
    unsigned char* getPageBuffer();
    qint16 getBrowserWidth();
    qint16 getBrowserHeight();
    qint16 getBrowserDepth();
    qint32 getBrowserRowSpan();

    // set background color that you see in between pages - default is white but sometimes useful to change
    void setBackgroundColor( const quint8 redIn, const quint8 greenIn, const quint8 blueIn );

    // change the caret color (we have different backgrounds to edit fields - black caret on black background == bad)
    void setCaretColor( const quint8 redIn, const quint8 greenIn, const quint8 blueIn );

    // can turn off updates to a page - e.g. when it's hidden by your windowing system
    void setEnabled( bool enabledIn );

    // navigation
    void navigateStop();
    bool navigateTo( const std::string uriIn );
    bool canNavigateBack();
    void navigateBack();
    bool canNavigateForward();
    void navigateForward();
    void navigateReload();

    // javascript access/control
    std::string evaluateJavascript( std::string scriptIn );

    // redirection when you hit a missing page
    bool set404RedirectUrl( std::string redirect_url );
    bool clr404RedirectUrl();

    // mouse & keyboard events
    void mouseDown( qint16 xPosIn, qint16 yPosIn );
    void mouseUp( qint16 xPosIn, qint16 yPosIn );
    void mouseMove( qint16 xPosIn, qint16 yPosIn );
    void mouseLeftDoubleClick( qint16 xPosIn, qint16 yPosIn );
    void keyPress( qint16 keyCode );
    void unicodeInput( quint32 uni_char );

    // allow consumers of this class and to observe browser events
    bool addObserver( LLEmbeddedBrowserWindowObserver* observerIn );
    bool remObserver( LLEmbeddedBrowserWindowObserver* observerIn );

    // accessor/mutator for scheme that browser doesn't follow - e.g. secondlife.com://
    void setNoFollowScheme( std::string schemeIn );
    std::string getNoFollowScheme();

private:
    friend class WebPage;
    WebPage *page;
    QImage image;
    LLEmbeddedBrowser* mParent;
    LLEmbeddedBrowserWindowEmitter< LLEmbeddedBrowserWindowObserver > mEventEmitter;

    qint16 mPercentComplete;
    std::string mStatusText;
    std::string mCurrentUri;
    std::string mClickHref;
    std::string mClickTarget;
    std::string mNoFollowScheme;
    int mWindowId;
    unsigned char* mPageBuffer;
    std::string m404RedirectUrl;
    bool mEnabled;
    bool mFlipBitmap;
};

#endif // LLEMBEDEDDBROWSERWINDOW_H
