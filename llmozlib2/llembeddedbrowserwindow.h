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
 *   Benjamin Meyer (benjamin.meyer@torchmobile.com)
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

#include <string>
#include <list>
#include <algorithm>
#ifdef _MSC_VER
#include "pstdint.h"
#else
#include <stdint.h>        // Use the C99 official header
#endif

#include "llmozlib2.h"

class LLEmbeddedBrowser;

////////////////////////////////////////////////////////////////////////////////
// class for a "window" that holds a browser - there can be lots of these
class LLEmbeddedBrowserWindowPrivate;
class LLEmbeddedBrowserWindow
{
public:
    LLEmbeddedBrowserWindow();
    virtual ~LLEmbeddedBrowserWindow();

    // housekeeping
    void setParent(LLEmbeddedBrowser* parent);
    bool setSize(int16_t width, int16_t height);
    void focusBrowser(bool focus_browser);
    void scrollByLines(int16_t lines);
    void setWindowId(int window_id);
    int getWindowId();

    // random accessors
    int16_t getPercentComplete();
    std::string& getStatusMsg();
    std::string& getCurrentUri();
    std::string& getClickLinkHref();
    std::string& getClickLinkTarget();

    // memory buffer management
    unsigned char* grabWindow(int x, int y, int width, int height);
    bool flipWindow(bool flip);
    unsigned char* getPageBuffer();
    int16_t getBrowserWidth();
    int16_t getBrowserHeight();
    int16_t getBrowserDepth();
    int32_t getBrowserRowSpan();

    // set background color that you see in between pages - default is white but sometimes useful to change
    void setBackgroundColor(const uint8_t red, const uint8_t green, const uint8_t blue);

    // change the caret color (we have different backgrounds to edit fields - black caret on black background == bad)
    void setCaretColor(const uint8_t red, const uint8_t green, const uint8_t blue);

    // can turn off updates to a page - e.g. when it's hidden by your windowing system
    void setEnabled(bool enabledIn);

    // navigation
    bool userAction(LLMozLib::EUserAction action);
    bool userActionIsEnabled(LLMozLib::EUserAction action);
    bool navigateTo(const std::string uri);

    // javascript access/control
    std::string evaluateJavascript(std::string script);

    // redirection when you hit a missing page
    bool set404RedirectUrl(std::string redirect_url);
    bool clr404RedirectUrl();
    void load404RedirectUrl();

    // mouse & keyboard events
    void mouseDown(int16_t x, int16_t y);
    void mouseUp(int16_t x, int16_t y);
    void mouseMove(int16_t x, int16_t y);
    void mouseLeftDoubleClick(int16_t x, int16_t y);
    void keyPress(int16_t key_code);
    void unicodeInput(uint32_t unicode_char);

    // allow consumers of this class and to observe browser events
    bool addObserver(LLEmbeddedBrowserWindowObserver* observer);
    bool remObserver(LLEmbeddedBrowserWindowObserver* observer);
    int getObserverNumber();

    // accessor/mutator for scheme that browser doesn't follow - e.g. secondlife.com://
    void setNoFollowScheme(std::string scheme);
    std::string getNoFollowScheme();

private:
    friend class LLWebPage;
    friend class LLGraphicsScene;
    friend class LLWebView;
    LLEmbeddedBrowserWindowPrivate *d;

};
#endif // LLEMBEDEDDBROWSERWINDOW_H
