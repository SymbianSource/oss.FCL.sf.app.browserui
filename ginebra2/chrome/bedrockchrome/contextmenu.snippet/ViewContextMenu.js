// This file contains the functions used to create the context menu for the browser's
// main content web view.


 // Context menu spec:
 // ------------------
 //Images---Options to be shown under an "Links/Pictures" tab
 // Save
 // Share
 //
 //Images that are also links---Options to be shown under an "Links/Pictures" tab
 // Open link in new window
 // Save
 // Share
 //
 //URLs-- Options to be shown under a "Links/Picture" tab
 // Open link in new window
 // Share
 //
 // Note: the options offered for a link or a picture or a combo
 // link/picture are slightly different, but the right set should
 // always be shown under a single tab contextual to what's been
 // pressed.
 //
 // If no link or picture has been long-pressed and the user is in
 // the CMB - say to see page options or the view switcher--the user
 // can still press on the Links/Pictures tab. Because there is no
 // known context to present options, we instead show menu items for
 // the combined link/image object as grayed out with text on top
 // approximating this: "Long-press on a web link or picture to use
 // these actions."

function viewMenu_getImageItems(imageUrl) {
    var items  = new Array();
    items =
        [
         {
             "text": window.localeDelegate.translateText("txt_browser_content_view_menu_image_save_image"), // "Save Image",
             "onclick": function() {
                 if (window.downloads != null) {
                     window.downloads.downloadImage(imageUrl);
                 } else {
                     alert("Save " + imageUrl);
                 }
             },
         },
        ]
    ;
    return items;
}

function viewMenu_getLinkItems(linkUrl) {
    const maxWindowCount = 5;
    var browserSchemeFound = true;
    // schemes other than http(s) and file may be handled by other app
    var browserSchemePattern = /^(http|https|file):/i;
    var matchFound = false;
    var errorUrl= "javascript:"
    var disableLink = false;
    
    if (linkUrl.substring(0,errorUrl.length) == errorUrl) {
        matchFound = true;
    }
    
    if (!browserSchemePattern.test(linkUrl)) {
        // scheme not directly handled by browser and therefore not displayed 
        // in browser window
        browserSchemeFound = false;
    }
    
    if ((pageController.pageCount() >= maxWindowCount)
        || matchFound || !browserSchemeFound) {
        disableLink = true;
    }
    
    var items = new Array();
    items =
        [
         {
            "text": window.localeDelegate.translateText("txt_browser_content_view_menu_link_open_link"), // "Open Link In New Window",
            "onclick": function() {
            pageController.LoadInNewWindow(linkUrl);
          },
         "disabled": disableLink ? "true" : "false",
         },
        ]
    ;
    return items;
}

function viewMenu_getNavMenuData(current) {
    return {
         "text": window.localeDelegate.translateText("txt_browser_content_view_menu_tab_navigation"), // "Navigation",
         "iconHighlighted": "/contextmenu/nav_selected.png",
         "icon": "/contextmenu/nav_deselected.png",
         "current": current ? "true" : "false",
         "menuItems":
             [
               {
                 "text": window.localeDelegate.translateText("txt_browser_content_view_menu_nav_bookmarks"),
                 "onclick": chrome_showBookmarksView,
               },
               {
                 "text": window.localeDelegate.translateText("txt_browser_content_view_menu_nav_history"),
                 "onclick": chrome_showHistoryView,
               },
               {
                 "text": window.localeDelegate.translateText("txt_browser_content_view_menu_nav_windows"),
                 "onclick": chrome_showWindowsView,
               },
               {
                 "text": window.localeDelegate.translateText("txt_browser_content_view_menu_nav_settings"),
                 "onclick": chrome_showSettingsView,
               },
               {
                 "text": window.localeDelegate.translateText("txt_browser_content_view_menu_nav_exit"),
                 "onclick": window.app.quit,
               },               
             ],
    };
}

function viewMenu_getTextEditMenuData(isContentSelected) {

    var items = new Array();
    items = [
         {
            "text": "copy",
            "disabled" : isContentSelected ? "false" : "true",
            "onclick": function() {
                pageController.copy();
            },
         },
         {
             "text": "cut",
             "disabled" : isContentSelected ? "false" : "true",
             "onclick": function() {
                 pageController.cut();
             },
         },
        ];

    items = items.concat( [ {
                 "text": "paste",
                 "disabled": pageController.hasTextOnClipBoard() ? "false" : "true",
                 "onclick": function() {
                     pageController.paste();
                   },
               },]);

    return {
         "text": "",
         "current": "true",
         "menuItems": items,
    };
}

function viewMenu_getPageMenuData(current) {

    var popupsBlocked =  pageController.getPopupSettings();
    var tempUrl = pageController.currentDocUrl;
    return {
         "text": window.localeDelegate.translateText("txt_browser_content_view_menu_tab_page"), // "Page",
         "iconHighlighted": "/contextmenu/page_selected.png",
         "icon": "/contextmenu/page_deselected.png",
         "current": current ? "true" : "false",
         "menuItems":
             [
               {
                 "text": window.localeDelegate.translateText("txt_browser_content_view_menu_page_add_bookmark"), // "Add Bookmark",
                 "onclick": function() {
                   launchBookmarkDialog(pageController.currentDocTitle, pageController.currentDocUrl,-1,0);
                 },
               },
               {
                 "text": (popupsBlocked ) ?
                    window.localeDelegate.translateText("txt_browser_content_view_menu_page_allow_popups"): 
                    window.localeDelegate.translateText("txt_browser_content_view_menu_page_block_popups"), // "Block/Allow Pop-ups"
                  "onclick": function() {
                    pageController.savePopupSettings(!popupsBlocked);
                 },
               },
               {
                 "text": window.localeDelegate.translateText("txt_browser_content_view_menu_page_share"), // "Share",
                 "disabled": tempUrl ? "false" : "true",
                 "onclick": function() {
                 pageController.share(tempUrl);    
                },                 
               },
             ],
    };
}

// Return a data structure representing the context menu for the main web view based
// on the information in contextEvent.
function viewMenu_getWebViewContextMenuData(contextEvent) {
    var tabList = new Array;
    var linkUrl = contextEvent != undefined ? contextEvent.linkUrl : "";
    var imageUrl = contextEvent != undefined ? contextEvent.imageUrl : "";
    var isLink = linkUrl != "";
    var isImage = imageUrl != "";
    var isLinkOrImage = isLink || isImage;
    var isContentEditable = contextEvent != undefined ? contextEvent.isContentEditable : false;
    var isContentSelected = contextEvent != undefined ? contextEvent.isContentSelected : false;

    if (isContentEditable) {
         var menuList = new Array;
         menuList = menuList.concat(viewMenu_getTextEditMenuData(isContentSelected));
         return {
             "menus": menuList
         };
    }

    // Build Navigation tab and its menu.
    if (contextEvent == undefined)
      tabList = tabList.concat(viewMenu_getNavMenuData(true));
    else
      tabList = tabList.concat(viewMenu_getNavMenuData(false));

    var linkMenuItems = new Array();

	// Get link related menu items.
    if (isLink || !isLinkOrImage)
        linkMenuItems = linkMenuItems.concat(viewMenu_getLinkItems(linkUrl));

    // Get image related menu items.
    if (isImage || !isLinkOrImage)
        linkMenuItems = linkMenuItems.concat(viewMenu_getImageItems(imageUrl));

   
    linkMenuItems = linkMenuItems.concat(
         [
             {
             	// Show "Share" for Image or empty area, "Share Link" for link in the menu item.
		     	"text": (isImage || !isLinkOrImage) ?
                	window.localeDelegate.translateText("txt_browser_content_view_menu_page_share"): 
                    window.localeDelegate.translateText("txt_browser_content_view_menu_link_share_link"),//Share Link	
                 "onclick": function() {
                  if ( isImage || !isLinkOrImage)
                      {
               	      pageController.share(imageUrl); 
                      }
                  else
                      { 
             	        pageController.share(linkUrl); 
         	      }
                 },
             }
         ]);

    // Build image/link tab and menu data.
    tabList = tabList.concat( {
         "text": (isLink && isImage || !isLink && !isImage)
            ? window.localeDelegate.translateText("txt_browser_content_view_menu_tab_link_image") // "Link/image"
            : (isLink
               ? window.localeDelegate.translateText("txt_browser_content_view_menu_tab_link") // "Link"
               : window.localeDelegate.translateText("txt_browser_content_view_menu_tab_image")), // "image"
         "icon": "/contextmenu/links_deselected.png",
         "iconHighlighted": "/contextmenu/links_selected.png",
         "current": isLinkOrImage ? "true" : "false",
         "disabled": isLinkOrImage ? "false" : "true",
         "menuItems": linkMenuItems,
    });

    // Build Page tab and menu data.
    if (contextEvent == undefined)
      tabList = tabList.concat(viewMenu_getPageMenuData(false));
    else
      tabList = tabList.concat(viewMenu_getPageMenuData(!isLinkOrImage));

    // Combine the tabs and return them.
    return {
        "tabs": tabList
    };
}

// Return a data structure representing the context menu for the edit widget
function viewMenu_getEditWidgetContextMenuData(isContentSelected, snippetId) {
    var menuItems = new Array();
    menuItems = [
         {
            "text": "copy",
            "disabled" : isContentSelected ? "false" : "true",
            "onclick": function() {
                window.snippets[snippetId].copy();
            },
         },
         {
             "text": "cut",
             "disabled" : isContentSelected ? "false" : "true",
             "onclick": function() {
                 window.snippets[snippetId].cut();
             },
         },
        ];

    menuItems = menuItems.concat( [ {
                 "text": "paste",
                 "disabled": pageController.hasTextOnClipBoard() ? "false" : "true",
                 "onclick": function() {
                     window.snippets[snippetId].paste();
                 },
               },]);

    return {
        "menus": [
                   { "text": "",
                     "current": "true",
                     "menuItems": menuItems,
                   },
                 ]
    };
}

// Initialize the WebView's context menu.
function viewMenu_initWebViewContextMenu() {
    // Connect to the view's contextEvent signal.
    views.WebView.contextEvent.connect(
        function(contextEvent) {
            //printProp(contextEvent);
            var menuData = viewMenu_getWebViewContextMenuData(contextEvent);
            if (menuData.menus != undefined && menuData.menus[0].menuItems.length == 0)
                return;
            cm_TheContextMenu.editingSnippet = undefined;
            cm_TheContextMenu.show(menuData);
        }
    );

    // Connect to the snippets' contextEvent signal
    for(var i = 0; i < chrome.snippets.length; i++) {
        var snippet = chrome.snippets[i];
        if (snippet.editable() && !snippet.useNativeCopyPasteMenu()) {
            //alert(snippet);
            snippet.contextEvent.connect(
                function(isContentSelected, snippetId) {
                    var menuData = viewMenu_getEditWidgetContextMenuData(isContentSelected, snippetId);
                    if (menuData.menus != undefined && menuData.menus[0].menuItems.length == 0)
                        return;
                    cm_TheContextMenu.editingSnippet = window.snippets[snippetId];
                    cm_TheContextMenu.show(menuData);
                }
            );
        }
    }

}

chrome.chromeComplete.connect(viewMenu_initWebViewContextMenu);
