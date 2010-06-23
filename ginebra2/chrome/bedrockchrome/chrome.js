
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

// Display the super-page with the given name and path.
function showSuperPage(pageName, path) {
    if (window.views.WebView[pageName] == undefined) {
        window.views.WebView.createSuperPage(pageName, true);
    }
    window.views.WebView[pageName].load(chrome.baseDirectory + path);

    // Show it.
    window.views.WebView.zoomFactor = 1.0;
    window.views.WebView.showSuperPage(pageName);
    window.ViewStack.switchView(pageName, "WebView");
}

function chrome_showBookmarksView() {
    app.debug("chrome_showBookmarksView");
    showSuperPage("BookmarkTreeView", "bookmarkview.superpage/BookmarkView.html");
}

function chrome_showHistoryView() {
    app.debug("chrome_showHistoryView");
    showSuperPage("BookmarkHistoryView", "historyview.superpage/historyView.html");
//    showHistoryView();
}

function chrome_showWindowsView() {
    app.debug("chrome_showWindowsView");
    window.snippets.ZoomBarId.hide(); // hide Zoom Bar while showing windows view
    window.snippets.MostVisitedViewId.hide();
    window.ViewStack.switchView("WindowView", "WebView");
}

function chrome_showSettingsView() {
    app.debug("chrome_showSettingsView");
    showSuperPage("SettingsView", "settingsview.superpage/SettingsView.html");
}

function chrome_showBasicMenu() {
    if (!snippets.ContextMenuId.dontShow) {
        cm_TheContextMenu.show(viewMenu_getWebViewContextMenuData());
    }
}

function chrome_addBookmark() {
    launchBookmarkDialog(window.pageController.currentDocTitle,window.pageController.currentDocUrl,0);
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
    window.bookmarksManager.launchBookmarkEditDailog.connect(showBookmarkEditDialog);
}

// Called when a PopupChromeItem is displayed.
function onPopupShown(id) {
    // Disable snippets etc. that should be greyed-out while the popup is shown. 
    snippets.UrlSearchChromeId.enabled = false;
    views.WebView.enabled = false;
    views.WebView.freeze();
    
    // Note: this can be expanded as needed.  We may want to disable all snippets except
    // for the urlSearchBar and the one who's id was passed in.
}

// Called when a PopupChromeItem is hidden.
function onPopupHidden(id) {
    // Re-enable snippets etc. that were greyed-out while the popup is shown.
    snippets.UrlSearchChromeId.enabled = true;
    views.WebView.enabled = true;
    views.WebView.unfreeze();
}

function onChromeComplete(){
    if (app.ui() != "orbit_ui") {
        snippets.StatusBarChromeId.show();
    }

    window.snippets.WebViewToolbarId.menuButtonSelected.connect(chrome_showBasicMenu);
    window.snippets.BookmarkViewToolbarId.addBookmarkSelected.connect(chrome_addBookmark);
    window.snippets.UrlSearchChromeId.anchorToView("top");
    window.snippets.WebViewToolbarId.menuButtonCanceled.connect(chrome_cancelMenu);

    //window.snippets.ButtonContainer.setVisibilityAnimator("G_VISIBILITY_FADE_ANIMATOR");

    result = new Array;
    result = calcToolbarPopAnchorOffset(window.snippets.ZoomBarId,
                                        window.snippets.ZoomButtonSnippet,
                                        0,10);
    window.snippets.ZoomBarId.anchorTo("ZoomButtonSnippet",result[0],result[1]);

    snippets.SuggestsChromeId.url = chrome.baseDirectory + "suggests.snippet/suggests.html";
    
    window.ViewStack.activateBookmark.connect(onActivateBookmarkView);

    chrome.popupShown.connect(onPopupShown);
    chrome.popupHidden.connect(onPopupHidden);
}

// For debugging: prints all properties and functions attached to a given object.
function printProp(x) {
    var str = "-------------\n" + x + " properties:\n";
    for (property in x) {
        str += ("\t" + property + ": " + x[property] + "\n");
    }
    app.debug(str);
}
