function printProp(x) {
    window.chrome.alert(x + ":");
    for (property in x) {
        window.chrome.alert(property + ": " + x[property]);
    }
}

window.webView.contextEvent.connect(
    function(e) {
        window.chrome.alert("web context22222222222: " + e + " " + e.pos.x);
        window.snippets.ContextMenuBgId.show();
    }
);

window.historyView.contextEvent.connect(
    function(e) {
        window.chrome.alert("history context: " + e + " " + e.itemIndex + 
                            " x=" + e.pos.x + " y=" + e.pos.y);
        printProp(e);
    }
);

window.chrome.loadComplete.connect(
    
);

function ContextMenuBg()
{
    // attach internal funcs
    this.write = writeContextMenuBg;

    // do setup
    this.write();
}

function goToRecentUrlView () {
         window.viewStack.switchView("bookmarkHistoryView", "webView");
    }

function goToBookmarkView () {
         window.viewStack.switchView("bookmarkTreeView", "webView");
    }

// "Private" methods
function writeContextMenuBg() {
    var html =
    '<div class="box">' +
        
       
        '<div class="boxBody">' +

          '<div id="Settings" class="show">' +
            '<ul>' +
              '<li onmouseup="goToBookmarkView();">' + window.localeDelegate.translateText("bookmarks_bookmarks") + '</li>' +
              '<li onmouseup="goToRecentUrlView();">'+ window.localeDelegate.translateText("content_view_menu_history") + '</li>' +     
//              '<li onmouseup="window.chrome.clearCache(); window.chrome.clearCookies();window.chrome.clearHistory();">' + window.localeDelegate.translateText("settings_clear_all") + '</li>' +
            '</ul>' +
          '</div>' +
        '</div>' +
    '</div>'; 
  document.write(html);
}

