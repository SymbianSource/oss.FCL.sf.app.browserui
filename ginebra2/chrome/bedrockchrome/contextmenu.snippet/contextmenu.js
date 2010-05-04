function printProp(x) {
    window.app.debug(x + ":");
    for (property in x) {
        window.chrome.alert("  " + property + ": " + x[property]);
    }
}

function historyViewContextEvent(e) {
    window.chrome.alert("history context: " + e + " " + e.itemIndex + 
                        " x=" + e.pos.x + " y=" + e.pos.y);
    printProp(e);
}

window.chrome.chromeComplete.connect(chromeLoadComplete);


function chromeLoadComplete() {
       
    window.snippets.ContextMenuId.externalMouseEvent.connect(
        function(type, name, description) {
        	
            if ((name == "QGraphicsSceneMouseReleaseEvent") || (name == "QGraphicsSceneResizeEvent")){
            	  window.snippets.ContextMenuId.hide();                                    
                window.snippets.ContextMenuId.dontShow = true;
                setTimeout ( 'window.snippets.ContextMenuId.dontShow = false', 500 ); // reset the flag in 0.5 sec.
            } 
        }
    );
 }

function ContextMenu()
{
	 // attach internal funcs
    this.write = writeContextMenu;

    // do setup
    this.write();
}
////

function showBookmarkView()
{
    if(window.views.WebView.BookmarkTreeView == undefined) {
        window.views.WebView.createSuperPage("BookmarkTreeView", true);
    }
    window.views.WebView.BookmarkTreeView.load(chrome.baseDirectory + "bookmarkview.superpage/BookmarkView.html");

    // Show it.
    window.views.WebView.zoomFactor = 1.0;
    window.views.WebView.showSuperPage("BookmarkTreeView");
}

function showSettingsView()
{
	   if(window.views.WebView.SettingsView == undefined) {
        window.views.WebView.createSuperPage("SettingsView", true);
    }
    window.views.WebView.SettingsView.load(chrome.baseDirectory + "settingsview.superpage/SettingsView.html");

    // Show it.
    window.views.WebView.zoomFactor = 1.0;
    window.views.WebView.showSuperPage("SettingsView");
}

function showHistoryView()
{
    if(window.views.WebView.BookmarkHistoryView == undefined) {
	        window.views.WebView.createSuperPage("BookmarkHistoryView", true);
	  }
	  window.views.WebView.BookmarkHistoryView.load(chrome.baseDirectory + "historyview.superpage/historyView.html");
     
    // Show it.
    window.views.WebView.zoomFactor = 1.0;
    window.views.WebView.showSuperPage("BookmarkHistoryView");
}

function goToRecentUrlView () {
	  showHistoryView();
	  window.ViewStack.switchView("BookmarkHistoryView", "WebView");
    }

function goToBookmarkView () {
    showBookmarkView();
    window.ViewStack.switchView("BookmarkTreeView", "WebView");
    }
    
function goToSettingsView () {
    showSettingsView();
    window.ViewStack.switchView("SettingsView", "WebView");
    //window.snippets.UrlSearchChromeId.hide(false);       
}

// "Private" methods
function writeContextMenu() {
    var html =
    '<div class="loadImagesTop">'+
    '</div>'+
    '<div class="loadImagesCenter">'+
    '</div>'+
    '<div class="loadImagesBottom">'+
    '</div>'+
    
    '<div class="menuItem menuTop">'+
      '<div class="menuItemContents" onmouseup="goToBookmarkView(); window.snippets.ContextMenuId.hide()">'+
        '<img STYLE="vertical-align: middle; padding-left: 20px; padding-right: 15px;" src="contextmenu.snippet/icons/menu_icon_bookmarks.png">'+
        '<span class="menuItemLabel">'+          
          window.localeDelegate.translateText("content_view_menu_bookmarks")+
        '</span>'+
      '</div>'+
    '</div>'+

    '<div class="menuItem menuBot">'+
      '<div class="menuItemContents" onmouseup="goToRecentUrlView();window.snippets.ContextMenuId.hide()">'+
        '<img STYLE="vertical-align: middle; padding-left: 20px; padding-right: 15px;" src="contextmenu.snippet/icons/menu_icon_settings.png">'+
        '<span class="menuItemLabel">'+
        window.localeDelegate.translateText("content_view_menu_history")+
        '</span>'+
      '</div>'+
    '</div>';
   
  document.write(html);
}



