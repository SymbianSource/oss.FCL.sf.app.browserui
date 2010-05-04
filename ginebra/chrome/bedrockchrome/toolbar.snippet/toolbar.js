

/* create all functions with toolbar.* */
//
// INIT the webView Toolbar
//

function WebViewToolbar()
{
    //Private methods
    //write webview toolbar HTML code to document
    function _writeWebViewToolbar() {
        var html = ''+
            '<table class="toolBarTable">' +
            '<tr>' +
            '<td class="toolBarBtnCell" style="width=15%;"><img class="toolBarBtn" id="backButton"></td>'+
            '<td><img src="toolbar.snippet/icons/btnBorder.png"></td>'+
            '<td class="toolBarBtnCell" style="width=15%;"><img class="toolBarBtn" id="zoomIn"></td>'+
            '<td><img src="toolbar.snippet/icons/btnBorder.png"></td>'+
            '<td class="toolBarBtnCell" style="width=15%;"><img class="toolBarBtn" id="zoomOut"></td>'+
            '<td><img src="toolbar.snippet/icons/btnBorder.png"></td>'+
            '<td class="toolBarBtnCell" style="width=15%;"><img class="toolBarBtn" id="winButton"></td>'+
            '<td><img src="toolbar.snippet/icons/btnBorder.png"></td>'+
             '<td class="toolBarBtnCell" style="width=15%;"><img class="toolBarBtn" id="contextMenuButton"></td>'+

            '</tr>'+
            '</table>';

        document.write(html);

        // We can immediately set up the simple buttons
        // ActionButtons have to wait until the page is loaded
        var index = window.pageController.pageCount();
        var base = "toolbar.snippet/icons/windows" + index;
        new SimpleButton("winButton",
                         base + ".png",
                         base + "_pushed.png",
                         base + ".png",
                         _goToWindowView);


        new SimpleButton("contextMenuButton",
                          "toolbar.snippet/icons/go-anywhere.png",
                          "toolbar.snippet/icons/go-anywhere_pushed.png",
                          "toolbar.snippet/icons/go-anywhere.png",
                          _contextMenuF);

   }

    /* Adds the back action button  using ActionButton from ../../js/ActionButton.js */
    function _setActions () {
        new ActionButton("backButton",
                     "toolbar.snippet/icons/back.png",
                     "toolbar.snippet/icons/back_pushed.png",
                     "toolbar.snippet/icons/back_disabled.png",
                     window.pageController.actions.back);

        new ActionButton("zoomIn",
                     "toolbar.snippet/icons/zoom+.png",
                     "toolbar.snippet/icons/zoom+_pushed.png",
                     "toolbar.snippet/icons/zoom+_disabled.png",
                     window.webView.actions.zoomIn);


        new ActionButton("zoomOut",
                     "toolbar.snippet/icons/zoom-.png",
                     "toolbar.snippet/icons/zoom-_pushed.png",
                     "toolbar.snippet/icons/zoom-_disabled.png",
                     window.webView.actions.zoomOut);

        /* Add 'addWindow' action */
        new ActionButton("addWindow",
                         "toolbar.snippet/icons/addwindow.png",
                         "toolbar.snippet/icons/addwindow_pushed.png",
                         "toolbar.snippet/icons/addwindow_disabled.png",
                         window.viewManager.windowView.actions.addWindow);
    }

    var timeoutId = 0;
   
    function _contextMenuF ()
    {
    	     var hideFlag = 0;
    	    
    	  	 hideFlag = window.snippets.ContextMenuId.getContextMenuFlag();
    	     
    	  	 if(hideFlag == 1)
    	  	 {
    	     	 window.snippets.ContextMenuBgId.hide();
    	     	 window.snippets.ContextMenuId.hide();
    	       return;
    	  	 }
    	     
    	     clearTimeout(timeoutId); 	
    	     
    	     window.snippets.ContextMenuBgId.setPosition(0,20);
           window.snippets.ContextMenuBgId.show();
           window.snippets.ContextMenuBgId.zValue = 1;
    	
    		   window.snippets.ContextMenuId.repaint();
    	     if (window.snippets.ContextMenuId.getDisplayMode() == "portrait")
    	       window.snippets.ContextMenuId.setPosition(90,200);
    	     else //landscape
    	     	 window.snippets.ContextMenuId.setPosition(250,100); 
    	
           
           window.snippets.ContextMenuId.show();
           window.snippets.ContextMenuId.zValue = 10;
          
           timeoutId = setTimeout ( 'window.snippets.ContextMenuId.hide(); window.snippets.ContextMenuBgId.hide()', 6000 ); // close the menu after 6 secs

    }

    /* Update the windows icon based on the number of windows open */
    function _changeWindowIcon () {
        var index = window.pageController.pageCount();
        var base = "toolbar.snippet/icons/windows" + index;

        document.getElementById('winButton').button.updateImages(base + ".png",
                                                                 base + "_pushed.png",
                                                                 base + ".png");
    }

    function _goToWindowView () {
        //window.chrome.alert("goToWindowView");
        window.viewStack.switchView("windowView", "webView");
    }

    function _goToGoAnywhereView () {
        /* Change to history view */
        window.viewStack.switchView("goAnywhereView", "webView");
    }

    function _goToBookmarkView () {
        /* Change to Bookmark view */
        window.viewStack.switchView("bookmarkTreeView", "webView");
    }

    function _goToRecentUrlView () {
         window.viewStack.switchView("bookmarkHistoryView", "webView");
    }

    function _chromeLoadComplete () {
        _setActions();
        _changeWindowIcon();

        /* Connect to pageCreated signal */
        window.pageController.pageCreated.connect(_changeWindowIcon);
    }

    function _activateBookmark () {
        window.chrome.toggleVisibility("BookmarkViewToolbarId");
    }

    function _deActivateBookmark () {
        window.chrome.toggleVisibility("BookmarkViewToolbarId");
    }

    function _activateHistory () {
        window.chrome.toggleVisibility("HistoryViewToolbarId");
    }

    function _deActivateHistory () {
        window.chrome.toggleVisibility("HistoryViewToolbarId");
    }

    function _activateBookMarkHistory () {
        window.chrome.toggleVisibility("BookmarkHistoryViewToolbarId");
    }

    function _deActivateBookMarkHistory () {
        window.chrome.toggleVisibility("BookmarkHistoryViewToolbarId");
    }

    function _activateWebView () {
        window.chrome.toggleVisibility("WebViewToolbarId");
    }

    function _deActivateWebView () {
        window.chrome.toggleVisibility("WebViewToolbarId");
    }

    function _activateWindowView () {

        window.chrome.alert("_activateWindowView");

        /* Show the window count snippet */
        document.getElementById('WindowCountBarId').wcChrome.wcUpdateWindowHtml();
        window.chrome.toggleVisibility("WindowCountBarId");

        window.chrome.toggleVisibility("WindowViewToolbarId");
    }

    function _deActivateWindowView () {

        window.chrome.toggleVisibility("WindowCountBarId");
        window.chrome.toggleVisibility("WindowViewToolbarId");

        /* Set the windows icon based on the number of windows */
        _changeWindowIcon();
    }

    function _pageChanged() {
        window.chrome.alert("_pageChanged");
        document.getElementById('WindowCountBarId').wcChrome.wcUpdateWindowHtml();
    }


    _writeWebViewToolbar();
    window.chrome.loadComplete.connect(_chromeLoadComplete);
    window.viewStack.activateBookmark.connect(_activateBookmark);
    window.viewStack.deActivateBookmark.connect(_deActivateBookmark);
    window.viewStack.activateHistory.connect(_activateHistory);
    window.viewStack.deActivateHistory.connect(_deActivateHistory);
    window.viewStack.activateBookMarkHistory.connect(_activateBookMarkHistory);
    window.viewStack.deActivateBookMarkHistory.connect(_deActivateBookMarkHistory);
    window.viewStack.activateWebView.connect(_activateWebView);
    window.viewStack.deActivateWebView.connect(_deActivateWebView);
    window.viewStack.activateWindowView.connect(_activateWindowView);
    window.viewStack.deActivateWindowView.connect(_deActivateWindowView);
    window.viewStack.pageChanged.connect(_pageChanged);

} //end of class webViewToolbar


// INIT the History View Toolbar
function HistoryViewToolbar()
{
    //Private methods
    //write historyview toolbar HTML code to document
    function _writeHistoryViewToolbar() {
        var tbhtml = ''+
        //'<span id="historyRow" style="width:100%">'+
        //'<img class="toolBarBtn firstButton" id="goBacktoWebViewHist">'+
        //'</span>';

        '<table class="toolBarTable">' +
        '<tr>' +
        '<td class="toolBarBtnCell" style="width=100%;"><img class="toolBarBtn" id="goBacktoWebViewHist"></td>'+
        '</tr>' +
        '</table>';
         document.write(tbhtml);

         /* add back button as a simple button */
         new SimpleButton("goBacktoWebViewHist",
                          "toolbar.snippet/icons/back.png",
                          "toolbar.snippet/icons/back_pushed.png",
                          "toolbar.snippet/icons/back.png",
                          _goBackFromGoAnywhereView);

    }

    _goBackFromGoAnywhereView = function() {
        window.viewStack.switchView("webView", "goAnywhereView");
    }

    _writeHistoryViewToolbar();

} //end of class HistoryViewToolbar

// INIT the bookmark View Toolbar
function BookmarkViewToolbar()
{
    //Private methods
    //write webview toolbar HTML code to document
    function _writeBookmarkViewToolbar() {
        var tbhtml = ''+
        //'<span id="bookmarkRow" style="width:100%">'+
        //'<img class="toolBarBtn firstButton" id="goBacktoWebViewBM">'+
        //'<img class="toolBarBtn lastButton"  id="addToBookMark">'+
        //'</span>';
        '<table class="toolBarTable">' +
            '<tr>' +
            '<td class="toolBarBtnCell" style="width=50%;"><img class="toolBarBtn" id="goBacktoWebViewBM"></td>'+
            '<td><img src="toolbar.snippet/icons/btnBorder.png"></td>'+
            '<td class="toolBarBtnCell" style="width=50%;"><img class="toolBarBtn" id="addToBookMark"></td>'+
            '</tr>'+
            '</table>';

         document.write(tbhtml);


         /* add back button as a simple button */
         new SimpleButton("goBacktoWebViewBM",
                          "toolbar.snippet/icons/back.png",
                          "toolbar.snippet/icons/back_pushed.png",
                          "toolbar.snippet/icons/back.png",
                          _goBackFromBookmarkView);

         /* add bookmark button as a simple button */
         new SimpleButton("addToBookMark",
                         "toolbar.snippet/icons/addwindow.png",
                         "toolbar.snippet/icons/addwindow_pushed.png",
                         "toolbar.snippet/icons/addwindow.png",
                         _addCurrentPageToBookMark);

    }

    function _goBackFromBookmarkView () {
        window.viewStack.switchView("webView", "bookmarkTreeView");
    }

    function _addCurrentPageToBookMark () {
        /* Change to history view */
        window.chrome.addCurrentPageToBookMark();
    }


    _writeBookmarkViewToolbar();

}

// INIT the Bookmark History View Toolbar
function BookmarkHistoryViewToolbar()
{
    /* BookmarkHist View Toolbar */
    function _writeBookmarkHistoryViewToolbar() {
        var tbhtml = ''+
        //'<span id="bookmarkHistoryRow" style="width:100%">'+
        //'<img class="toolBarBtn firstButton" id="goBacktoWebViewBMH">'+
        //'</span>';
        '<table class="toolBarTable">' +
            '<tr>' +
            '<td class="toolBarBtnCell" style="width=100%;"><img class="toolBarBtn" id="goBacktoWebViewBMH"></td>'+
            '</tr>'+
            '</table>';
         document.write(tbhtml);

         /* add back button as a simple button */
         new SimpleButton("goBacktoWebViewBMH",
                          "toolbar.snippet/icons/back.png",
                          "toolbar.snippet/icons/back_pushed.png",
                          "toolbar.snippet/icons/back.png",
                          _goBackFromRecentUrlView);
    }

    /* This function changes the view to webView and updates the layout correspondingly */
    _goBackFromRecentUrlView = function()
    {
        window.viewStack.switchView("webView", "bookmarkHistoryView");
    }

    _writeBookmarkHistoryViewToolbar();

}


// INIT the Window View Toolbar
function WindowViewToolbar()
{
    /* Window View Toolbar */
    function _writeWindowToolbar() {
        var tbhtml = ''+
            //    '<span id="windowRow" style="width:100%">'+
            //    '<img class="toolBarBtn firstButton" id="goBacktoWebViewWin">'+
            //    '<img class="toolBarBtn" id="addWindow">'+
            //    '</span>';
            '<table class="toolBarTable">' +
            '<tr>' +
            '<td class="toolBarBtnCell" style="width=50%;"><img class="toolBarBtn" id="goBacktoWebViewWin"></td>'+
            '<td><img src="toolbar.snippet/icons/btnBorder.png"></td>'+
            '<td class="toolBarBtnCell" style="width=50%;"><img class="toolBarBtn" id="addWindow"></td>'+
            '</tr>'+
            '</table>';


         document.write(tbhtml);

         /* add back as simple button, add window is an action button */
         new SimpleButton("goBacktoWebViewWin",
                          "toolbar.snippet/icons/back.png",
                          "toolbar.snippet/icons/back_pushed.png",
                          "toolbar.snippet/icons/back.png",
                          _goBackFromWindowView);

       }

    _goBackFromWindowView = function() {
        window.viewStack.switchView("webView", "windowView");
    }
    _writeWindowToolbar();
}



