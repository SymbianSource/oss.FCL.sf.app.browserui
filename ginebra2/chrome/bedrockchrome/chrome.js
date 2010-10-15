var BOOKMARKVIEW_PAGE = ":/bookmarkview/BookmarkView.html";
var SETTINGSVIEW_PAGE = ":/settingsview/SettingsView.html";
var HISTORYVIEW_PAGE = ":/historyview/historyView.html";
var SUGGESTS_PAGE = "qrc:///suggests/suggests.html";

var preLoadBookmarksDone=0;

document.getElementsByClassName = function(class_name) {
    var docList = this.all || this.getElementsByTagName('*');
    var matchArray = new Array();

    /*Create a regular expression object for class*/
    var re = new RegExp("(?:^|\\s)"+class_name+"(?:\\s|$)");
    for (var i = 0; i < docList.length; i++) {
        if (re.test(docList[i].className) ) {
            matchArray[matchArray.length] = docList[i];
        }
    }

	return matchArray;
}

function calcToolbarPopAnchorOffset(anchoredSnippet,
                                    anchorToSnippet,
                                    xCenterOffset,
                                    yBotGap)
{
    //
    // For popup-chrome (menu, zoomBtn) they are aligned by default
    // to the upper left-hand corners.
    //
    // This will calculate the offset needed to center the popup above the
    // button, or make it slightly off-center
    //
    // xCenterOffset = make x param this much off center
    // yBotGap = how much gap should there be between top of anchorTo,
    //           and bottom of anchored
    //
    anchorToGeom = anchorToSnippet.getGeometry();
    anchoredGeom = anchoredSnippet.getGeometry();
    xOffset = -(0.5 * anchoredGeom.width) +
        (0.5 * anchorToGeom.width) +
        xCenterOffset;
    yOffset = -anchoredGeom.height - yBotGap;
    return([xOffset,yOffset]);
}

function hidePopups(){
	
	  if (window.snippets.MostVisitedViewId.isVisible())
          window.snippets.MostVisitedViewId.hide();
           
    if (window.snippets.SuggestsChromeId.isVisible()) {
            window.snippets.SuggestsChromeId.hide(false);
            views.WebView.enabled = true;
            views.WebView.unfreeze();
    }
    
    if(app.ui() != "maemo5_ui") 
	    return;
	        
    snippets.BookmarkDialogId.hide();
}

// Display the super-page with the given name and path.
function showSuperPage(pageName, path) {

    if (window.views.WebView[pageName] == undefined) {
        window.views.WebView.createSuperPage(pageName, true);
    		window.views.WebView[pageName].load(/*chrome.baseDirectory +*/ path);
    }
    else {
    	// FIXME: this is a workaround temporary solution
    	// this is used to render CSS styles for super pages 
    	// instead of just popping them up from ViewStack which somehow prevent proper CSS styling on page display
    	window.views.WebView[pageName].load(path);
 	}
 
    if (!window.views.WebView.bedrockTiledBackingStoreEnabled()){
        window.views.WebView.setSavedZoomValueInView(window.views.WebView.zoomFactor);
		window.views.WebView.zoomFactor = 1.0;
    }
    //window.views.WebView.showSuperPage(pageName);
    window.ViewStack.switchView(pageName, "WebView");

    if (!window.views.WebView.bedrockTiledBackingStoreEnabled())
        window.views.WebView.touchNav.doubleClickEnabled = false;
        
    hidePopups();
}

function chrome_showBookmarksView() {
    showSuperPage("BookmarkTreeView", BOOKMARKVIEW_PAGE);
}

function chrome_showHistoryView() {
    showSuperPage("BookmarkHistoryView", HISTORYVIEW_PAGE);
}

function chrome_showWindowsView() {
		if(app.ui() != "maemo5_ui") 
    	window.snippets.ZoomBarId.hide(); // hide Zoom Bar while showing windows view
    window.snippets.MostVisitedViewId.hide();
    window.ViewStack.switchView("WindowView", "WebView");
}

function chrome_showSettingsView() {
    showSuperPage("SettingsView", SETTINGSVIEW_PAGE);
}

function chrome_showBasicMenu() {

    cm_TheContextMenu.show(viewMenu_getWebViewContextMenuData());

}

function chrome_addBookmark() {
    launchBookmarkDialog(window.pageController.currentDocTitle,window.pageController.currentDocUrl,-1,0);
}

function chrome_cancelMenu() {

    cm_TheContextMenu.cancel();
}

// Center the given snippet on-screen.
function centerSnippet(snippet) {
    var x = (chrome.displaySize.width - snippet.geometry.width) / 2;
    var y = (chrome.displaySize.height - snippet.geometry.height) / 2;
    snippet.setPosition(x, y);
}

function getChildById(el, childId) {
    var children = el.getElementsByTagName("*");
    for (var i = 0; i < children.length; i++) {
        var child = children[i];
        if (child.id == childId)
            return child;
    }
    return undefined;
}

function onActivateBookmarkView() {
	  snippets.BookmarkViewToolbarId.enabled = true;
	  try {
    window.bookmarksController.launchBookmarkEditDailog.connect(showBookmarkEditDialog);
  	}
  	catch (e){;} // just in case bookmarksHistoryManager is not ready yet
  
}
function preLoad()
{   
    preloadSuperPage("BookmarkTreeView", BOOKMARKVIEW_PAGE);
    preloadSuperPage("SettingsView", SETTINGSVIEW_PAGE); 
    preLoadBookmarksDone=1;
}
function _updateHistory()
{
	preloadSuperPage("BookmarkHistoryView", HISTORYVIEW_PAGE);
}

function _updateBookmarks()
{
    preloadSuperPage("BookmarkTreeView", BOOKMARKVIEW_PAGE);
}

// chrome_popupShownCount keeps a count of how many popups are currently being shown so that
// we can re-enable the appropriate UI elements only when the last one is hidden.
var chrome_popupShownCount = 0;

// Called when a PopupChromeItem is displayed.
function onPopupShown(id) {
    if(chrome_popupShownCount == 0) {
        // Disable snippets etc. that should be greyed-out while the popup is shown. 
        if (app.layoutType() == "tenone") {
            snippets.TitleUrlId.enabled = false;
        }
        else {
            snippets.UrlSearchChromeId.enabled = false;    
        }    
        views.WebView.enabled = false;
        views.WebView.freeze();
        
        // Note: this can be expanded as needed.  We may want to disable all snippets except
        // for the status bar and the one who's id was passed in.
    }
    chrome_popupShownCount++;
    
    if(preLoadBookmarksDone==0)
    {
    	preLoad();
    }
}

// Called when a PopupChromeItem is hidden.
function onPopupHidden(id) {
    chrome_popupShownCount--;
    if(chrome_popupShownCount == 0) {
        // Re-enable snippets etc. that were greyed-out while popups were being shown.
        if (app.layoutType() == "tenone") {
            snippets.TitleUrlId.enabled = true;
        }
        else {
            snippets.UrlSearchChromeId.enabled = true;
        }
        views.WebView.enabled = true;
        views.WebView.unfreeze();
    }
    if(chrome_popupShownCount < 0) app.debug("onPopupHidden: error, chrome_popupShownCount invalid");
}

function preloadSuperPage(pageName, path) {
    if (window.views.WebView[pageName] == undefined) {
        window.views.WebView.createSuperPage(pageName, true);
    }
    window.views.WebView[pageName].load(/*chrome.baseDirectory +*/ path);
}

//var chrome_blockPopupsAction;

// Handle Block/Allow Popups action.
function chrome_toggleBlockPopups() {
    hidePopups();
    pageController.savePopupSettings(!pageController.getPopupSettings());
    chrome_blockPopupsAction.text = chrome_getBlockPopupsActionText();
}

// Returns the text for the Block/Allow Popups action based on the the current setting in the page controller.
function chrome_getBlockPopupsActionText() {
    return window.localeDelegate.translateText(pageController.getPopupSettings()
                                               ? "txt_browser_content_view_menu_page_allow_popups"
                                               : "txt_browser_content_view_menu_page_block_popups");
}

function chrome_setZoomButtonActions() {
    snippets.ZoomInButtonSnippet.connectAction("zoomIn", "WebView", true, true);
    snippets.ZoomOutButtonSnippet.connectAction("zoomOut", "WebView",  true, true);
}

// Create an action and add it to the Maemo menu bar.
function chrome_addMaemoMenuBarAction(text, callback) {
    var action = app.createAction(text);
    action.triggered.connect(callback);
    app.addMenuBarAction(action);
    return action;
}
function chrome_updateMaemoMenuBarItems() { 
  app.setMenuBarEnabled(!views.WebView.currentPageIsSuperPage()); 
}

// Create actions that appear in the Maemo system menu at the top of the screen.
function chrome_createMaemoMenuBarItems() {
    // Global menu items.
    chrome_addMaemoMenuBarAction(window.localeDelegate.translateText("txt_browser_content_view_menu_nav_bookmarks"),
                                 chrome_showBookmarksView);
    chrome_addMaemoMenuBarAction(window.localeDelegate.translateText("txt_browser_content_view_menu_page_add_bookmark"),
                                 function() { hidePopups(); launchBookmarkDialog(pageController.currentDocTitle, pageController.currentDocUrl,-1,0); } );
    chrome_addMaemoMenuBarAction(window.localeDelegate.translateText("txt_browser_content_view_menu_nav_history"),
                                 chrome_showHistoryView);
    chrome_blockPopupsAction = chrome_addMaemoMenuBarAction(chrome_getBlockPopupsActionText(),
                                                            chrome_toggleBlockPopups);
    chrome_addMaemoMenuBarAction(window.localeDelegate.translateText("txt_browser_content_view_menu_page_share"),
                                 function() { hidePopups(); pageController.share(pageController.currentDocUrl); } );
    chrome_addMaemoMenuBarAction(window.localeDelegate.translateText("txt_browser_content_view_menu_nav_settings"),
                                 chrome_showSettingsView);
}
function onChromeActivated() {
	
	snippets.SuggestsChromeId.hide(false);
	
  if(!views.WebView.currentPageIsSuperPage()) {
      views.WebView.enabled = true;
      views.WebView.unfreeze();
  }
  snippets.MostVisitedViewId.hide();
    
	if(app.ui() == "maemo5_ui") {
    snippets.UrlSearchChromeId.grabFocus();            
  }
}

function onChromeComplete() {

    if (app.ui() == "orbit_ui") {
        snippets.StatusBarChromeId.hide();
    }
    if (app.layoutType() == "tenone") {
        window.snippets.TitleUrlId.anchorToView("top");
        
        // Disable content view while Most Visited view is shown.
    		snippets.MostVisitedViewId.shown.connect(
        	function() {
           	views.WebView.enabled = false;
           	views.WebView.unfreeze(); 
        	}
    		);

    		// Enable content view when Most Visited view is hidden.
    		snippets.MostVisitedViewId.hidden.connect(
        	function() {
            views.WebView.enabled = true;
        	}
    		);  

    }
    else if(app.ui() == "maemo5_ui") {
        
        chrome_createMaemoMenuBarItems();

        // Update the menu items when the page type changes.
        views.WebView.normalPageShown.connect(chrome_updateMaemoMenuBarItems);
        views.WebView.superPageShown.connect(chrome_updateMaemoMenuBarItems);
        window.snippets.UrlSearchChromeId.anchorToView("top");
        chrome_setZoomButtonActions();   
        
        // Disable content view while Most Visited view is shown.
    		snippets.MostVisitedViewId.shown.connect(
        	function() {
           	views.WebView.enabled = false;
           	snippets.UrlSearchChromeId.enabled = false;
        	}
    		);

    		// Enable content view when Most Visited view is hidden.
    		snippets.MostVisitedViewId.hidden.connect(
        	function() {
            views.WebView.enabled = true;
            snippets.UrlSearchChromeId.enabled = true;
        	}
    		);   
        
    }
    else {
        window.snippets.UrlSearchChromeId.anchorToView("top");
    }

    window.snippets.WebViewToolbarId.menuButtonSelected.connect(chrome_showBasicMenu);
    window.snippets.BookmarkViewToolbarId.addBookmarkSelected.connect(chrome_addBookmark);
    window.snippets.WebViewToolbarId.menuButtonCanceled.connect(chrome_cancelMenu);

		// Dismiss Most Visited view when it gets an external click event.
    	snippets.MostVisitedViewId.externalMouseEvent.connect(
        function(type, name, description) {
             if(name == "MouseClick") {
                // Use timer here to hide the view even when the Most Visited page button is clicked,
                // otherwise the button re-displays the view.
                setTimeout('snippets.MostVisitedViewId.hide()', 100);
             }
        });

    //window.snippets.ButtonContainer.setVisibilityAnimator("G_VISIBILITY_FADE_ANIMATOR");
  if(app.ui() != "maemo5_ui"){
    result = new Array;
    result = calcToolbarPopAnchorOffset(window.snippets.ZoomBarId,
                                        window.snippets.ZoomButtonSnippet,
                                        0,10);
    window.snippets.ZoomBarId.anchorTo("ZoomButtonSnippet",result[0],result[1]);
  }

    snippets.SuggestsChromeId.url = /*chrome.baseDirectory +*/ SUGGESTS_PAGE;
    
    window.ViewStack.activateBookmark.connect(onActivateBookmarkView);

    chrome.popupShown.connect(onPopupShown);
    chrome.popupHidden.connect(onPopupHidden);
    
    if(app.ui() == "maemo5_ui")
    	chrome.chromeActivated.connect(onChromeActivated);
    
    window.pageController.loadFinished.connect(_updateHistory);
    window.pageController.loadFinishedForBackgroundWindow.connect(_updateHistory);
    window.bookmarksController.bookmarksCleared.connect(_updateBookmarks);
    window.historyManager.historyCleared.connect(_updateHistory);
    
}

// For debugging: prints all properties and functions attached to a given object.
function printProp(x) {
    var str = "-------------\n" + x + " properties:\n";
    for (property in x) {
        str += ("\t" + property + ": " + x[property] + "\n");
    }
    app.debug(str);
}

