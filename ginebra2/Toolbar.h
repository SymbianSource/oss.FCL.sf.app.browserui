/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description: Global header for toolbar snippets
*
*/
#ifndef __GINEBRA_TOOLBAR_H
#define __GINEBRA_TOOLBAR_H

namespace GVA {

  enum ContentViewActions {

      CONTENT_VIEW_ACTION_BACK,
      CONTENT_VIEW_ACTION_ZOOM,
      CONTENT_VIEW_ACTION_MOSTVISITED,
      CONTENT_VIEW_ACTION_MENU,
      CONTENT_VIEW_ACTION_TOGGLETB,
      CONTENT_VIEW_ACTION_MAX
  };

  enum WindowViewActions {

      WINDOW_VIEW_ACTION_BACK,
      WINDOW_VIEW_ACTION_ADD,
      WINDOW_VIEW_ACTION_MAX
  };

  enum BookmarksViewActions {
      BOOKMARKS_VIEW_ACTION_BACK,
      BOOKMARKS_VIEW_ACTION_ADD,
      BOOKMARKS_VIEW_ACTION_MAX
  };

  enum RecentURLViewActions {
      RECENTURL_VIEW_ACTION_BACK,
      RECENTURL_VIEW_ACTION_CLEARALL,
      RECENTURL_VIEW_ACTION_MAX
  };

  enum SettingsViewActions {
      SETTINGS_VIEW_ACTION_BACK,
      SETTINGS_VIEW_ACTION_FEEDBACK,
      SETTINGS_VIEW_ACTION_MAX
  };

  struct ToolbarActions_t {

       int actionId;
       QString actionName;
       QString activeImg;
       QString disabledImg;
       QString selectedImg;
       QString id;
  };


#define TOOLBAR_POPUP_INACTIVITY_DURATION 5000
#define TOOLBAR_ZOOMBAR_INACTIVITY_DURATION 8000

#define DUAL_TOOLBAR_NUM_BUTTONS 2

#define WINDOW_TOOLBAR_BACK "goBack"
#define WINDOW_TOOLBAR_ADD "addWindow"

#define BOOKMARKS_TOOLBAR_BACK "goBack"
#define BOOKMARKS_TOOLBAR_ADD "addBookmark"

#define RECENTURL_TOOLBAR_BACK "goBack"
#define RECENTURL_TOOLBAR_CLEARALL "clearAll"

#define SETTINGS_TOOLBAR_BACK "goBack"
#define SETTINGS_TOOLBAR_FEEDBACK "giveFeedback"

#define CONTENT_TOOLBAR_NUM_BUTTONS 3
#define CONTENT_TOTOLBAR_BACK "Back"
#define CONTENT_TOTOLBAR_ZOOM "zoom"
#define CONTENT_TOTOLBAR_MENU "menu"
#define CONTENT_TOTOLBAR_MOSTVISITED "mostvisited"
#define CONTENT_TOTOLBAR_TOGGLETB "toggletb"

#define TOOLBAR_WEB_VIEW  "WebView"
#define TOOLBAR_WINDOWS_VIEW  "WindowView"
#define TOOLBAR_BOOKMARKS_VIEW "BookmarkTreeView"
#define TOOLBAR_RECENTURL_VIEW "BookmarkHistoryView"
#define TOOLBAR_SETTINGS_VIEW "SettingsView"

} // end of namespace GVA

#endif // __GINEBRA_TOOLBAR_H
