
var tbCb;

/* create all functions with toolbar.* */
//
// INIT the webView Toolbar
//

/* Fade functions - modified the 3rdparty solution*/
var FadeAnim = {

    fade : function (id) {
        opacity = 96;
        this.fadeLoop(id, opacity);
    },
    fadeLoop : function (id, opacity) {
        var o = document.getElementById(id);
        if (opacity >= 5) {
            FadeAnim.setOpacity(o, opacity);
            opacity -= 4;
            window.setTimeout("FadeAnim.fadeLoop('" + id + "', " + opacity + ")", 50);
        } else {
//                        o.style.display = "none";
            o.style.visibility = 'hidden';
            //window.chrome.alert("Menu is " +  o.style.visibility);
            tbCb("hideCplt");
              
        }
    },
    setOpacity : function (o, opacity) {
        o.style.filter = "alpha(style=0,opacity:" + opacity + ")";      // IE
        o.style.KHTMLOpacity = opacity / 100;                           // Konqueror
        o.style.MozOpacity = opacity / 100;                                     // Mozilla (old)
        o.style.opacity = opacity / 100;                                        // Mozilla (new)
    },

    show : function (id) {
        opacity = 4;
        this.showLoop(id, opacity);
    },
    showLoop : function (id, opacity) {
        var o = document.getElementById(id);
        if (opacity <= 100) {
            FadeAnim.setOpacity(o, opacity);
            opacity += 4;
            window.setTimeout("FadeAnim.showLoop('" + id + "', " + opacity + ")", 50);
        }
        else {
            tbCb("showCplt");
        }
    }
}

/* Utility functions */
function Util() {
    
    /* Utility function to strip off the given string
     */
    function convertToValue(num, str) {

        var i = num.indexOf(str);
        var value = 0;
        if (i > -1) {
            value = num.slice(0, i);
        }
        return Number(value);
    }

    /* Get the first button if of the toolbar specified by 'view' */
    function firstBtn(tbId) {
        var rowid = getRowId(tbId);
        return (document.getElementById(rowid).firstChild.id);
    }

    /* Get the last button if of the toolbar specified by 'view' */
    function lastBtn(tbId) {
        var rowid = getRowId(tbId);
        return (document.getElementById(rowid).lastChild.id);
    }

    /* Get the row id of the toolbar specified by 'view' */
    function getRowId(tbId) {
       var id;
        switch( tbId) {
            case  "WebViewToolbarId":
                id = "webViewRow";
                break;
            case  "WebViewMiniToolbarId":
                id = "webViewMiniRow";
                break;
            case  "HistoryViewToolbarId":
                id = "historyRow";
                break;
            case  "BookmarkViewToolbarId":
                id = "bookmarkRow";
                break;
            case  "BookmarkHistoryViewToolbarId":
                id = "bookmarkHistoryRow";
                break;
            case  "WindowViewToolbarId":
                id = "windowRow";
                break;
            default:
                break;
        }
        return id;

    }

    /* This function calculates the margin-right needed */
    this.setMarginRight = function(tbId) {

        /* Get the total number of images in the toolbar */
        var firstIcon = firstBtn(tbId);
    
        var el = document.getElementById(getRowId(tbId));
        Icons = el.getElementsByTagName("img");

        numIcons = Icons.length;
        /* Get total width and total width od all icons */
        var  iconW = document.getElementById(firstIcon).offsetWidth;
        var  totalW =  document.getElementById(tbId).offsetWidth;
        var totalIconW= (iconW * numIcons);
        var firstobj = document.getElementById(firstIcon);
        var lastObj = document.getElementById(lastBtn(tbId));

        switch (numIcons) {
            case 1:
                /* Only one icon */
                var leftMarginStr=document.defaultView.getComputedStyle(firstobj, '').getPropertyValue("margin-left");
                var leftMargin = convertToValue(leftMarginStr, "px");
        
                var marginRight = totalW - totalIconW - leftMargin;
                Icons[0].style.marginRight = marginRight + "px";
                break;
            
        default:
            /* More than two buttons */
            var leftMarginStr=document.defaultView.getComputedStyle(firstobj, '').getPropertyValue("margin-left");
            var rightmarginStr=document.defaultView.getComputedStyle(lastObj, '').getPropertyValue("margin-right");
            var leftMargin = convertToValue(leftMarginStr, "px");
            var rightMargin = convertToValue(rightmarginStr, "px");
            var totalMargin = rightMargin + leftMargin ;


            /* Now determine remaining space */
            var marginRight = (totalW - totalIconW - totalMargin)/(numIcons - 1);
            for (var i = 0; i < numIcons; i++ ) {
    
                if (i !== (numIcons-1)  ){
                    Icons[i].style.marginRight = marginRight + "px";
                }
            }
            break;
        }

    }
}

/* create all functions with toolbar.* */
//
// INIT the webView Toolbar
//

function WebViewToolbar()
{

    var utils = new Util();
    var menuTimeoutCb = _hideToolbar;
    var menuTimeoutId = 0;
    tbCb = _handleToolbarMenuCb;


    //Private methods
    //write webview toolbar HTML code to document
    function _writeWebViewToolbar() {
    var html = ''+
        '<span id="webViewRow" style="width:100%">'+
        '<img class="toolBarBtn firstButton" id="backButton" >'+
        '<span id="menu" >'+
        '<img class="toolBarBtn" id="zoomIn"   >'+
        '<img class="toolBarBtn" id="zoomOut" >'+
        '<img class="toolBarBtn" id="winButton" >'+
        '<img class="toolBarBtn" id="contextMenuButton" >'+
        '</span>'+
        '<img class="toolBarBtn lastButton" id="mvButton">'+
        '</span>';
        document.write(html);

        // We can immediately set up the simple buttons
        // ActionButtons have to wait until the page is loaded
        var index = window.pageController.pageCount();
        var base = "toolbar2.snippet/fjicons/windows" + index;
        new SimpleButton("winButton",
                         base + ".png",
                         base + "_pushed.png",
                         base + ".png",
                         _goToWindowView);

       new SimpleButton("mvButton",
                         "toolbar2.snippet/fjicons/mostVisited.png",
                         "toolbar2.snippet/fjicons/mostVisited_pushed.png",
                         "toolbar2.snippet/fjicons/mostVisited.png",
                         _activateMostVisited);



        new SimpleButton("contextMenuButton",
                          "toolbar2.snippet/fjicons/menu.png",
                          "toolbar2.snippet/fjicons/menu_pushed.png",
                          "toolbar2.snippet/fjicons/menu.png",
                          _contextMenuF);
//       document.getElementById("menu").style.visibility = 'hidden';
       utils.setMarginRight("WebViewToolbarId");
   }

    /* Adds the back action button  using ActionButton from ../../js/ActionButton.js */
    function _setActions () {
        new ActionButton("backButton",
                     "toolbar2.snippet/fjicons/back.png",
                     "toolbar2.snippet/fjicons/back_pushed.png",
                     "toolbar2.snippet/fjicons/back_disabled.png",
                     window.pageController.actions.back);

        new ActionButton("zoomIn",
                     "toolbar2.snippet/fjicons/zoom+.png",
                     "toolbar2.snippet/fjicons/zoom+_pushed.png",
                     "toolbar2.snippet/fjicons/zoom+_disabled.png",
                     window.webView.actions.zoomIn);


        new ActionButton("zoomOut",
                     "toolbar2.snippet/fjicons/zoom-.png",
                     "toolbar2.snippet/fjicons/zoom-_pushed.png",
                     "toolbar2.snippet/fjicons/zoom-_disabled.png",
                     window.webView.actions.zoomOut);

        /* Add 'addWindow' action */
        new ActionButton("addWindow",
                         "toolbar2.snippet/fjicons/addwindow.png",
                         "toolbar2.snippet/fjicons/addwindow_pushed.png",
                         "toolbar2.snippet/fjicons/addwindow_disabled.png",
                         window.viewManager.WindowView.actions.addWindow);
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
            window.snippets.ContextMenuId.setPosition(83,270);
        else //landscape
            window.snippets.ContextMenuId.setPosition(223,130);

        window.snippets.ContextMenuId.show();
        window.snippets.ContextMenuId.zValue = 10;
           
        timeoutId = setTimeout ( 'window.snippets.ContextMenuId.hide(); window.snippets.ContextMenuBgId.hide()', 6000 ); // close the menu after 6 secs
//        _showToolbar();
    }

    /* Update the windows icon based on the number of windows open */
    function _changeWindowIcon () {
        var index = window.pageController.pageCount();
        var base = "toolbar2.snippet/fjicons/windows" + index;

        document.getElementById('winButton').button.updateImages(base + ".png",
                                                                 base + "_pushed.png",
                                                                 base + ".png");
        //window.snippets.repaint("WebViewToolbarId");

    }

    function _goToWindowView () {        
        window.viewStack.switchView("WindowView", "webView");
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

    function _activateMostVisited () {
        
    }

    function _chromeLoadComplete () {
        _setActions();
        _changeWindowIcon();

        /* Connect to pageCreated signal */
        window.pageController.pageCreated.connect(_changeWindowIcon);

        /*
        window.pageController.loadStarted.connect(_showToolbar);
        window.pageController.loadFinished.connect(startMenuHideTimer);
        window.pageController.pageScrollRequested.connect(_showToolbar);
        */
    }

    function _showToolbar()  {
        var el = document.getElementById("menu");
        //window.chrome.alert("show Toolbar " +  el.style.visibility);
        if (el.style.visibility != 'visible') {
            document.getElementById("menu").style.visibility = 'visible';
            FadeAnim.show("menu");
        }
    }

    function startMenuHideTimer() {

        //window.chrome.alert("startMenuHideTimer");
        var el = document.getElementById("menu");
        if (el.style.visibility == 'visible' && (!menuTimeoutId) ) {
            //window.chrome.alert("startMenuHideTimer: started timer");
            //menuTimeoutId = setTimeout ('_hideToolbar()', 1000);
            menuTimeoutId = setTimeout (menuTimeoutCb, 3000);
        }
    }

    function _hideToolbar()  {
        //window.chrome.alert("hideToolbar " +  document.getElementById("menu").style.visibility);

        var el = document.getElementById("menu");

        clearTimeout(menuTimeoutId);
        menuTimeoutId = 0;

        if (el.style.visibility != 'hidden' ) {
            FadeAnim.fade("menu");
            //window.chrome.alert("hide Toolbar " +  document.getElementById("menu").style.visibility);
        }
    }

    function _handleToolbarMenuCb(x) {

        if (x == "showCplt" ) {
            startMenuHideTimer();
        }
        else if (x == "hideCplt"){

        }
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
        _changeWindowIcon();
        window.chrome.alert("_pageChanged");
        document.getElementById('WindowCountBarId').wcChrome.wcUpdateWindowHtml();
    }

    function _updateToolbar() {
       utils.setMarginRight("WebViewToolbarId");

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
    window.chrome.onDisplayModeChanged.connect(_updateToolbar);
    window.chrome.viewPortResize.connect(_updateToolbar);

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
                          "toolbar2.snippet/fjicons/back.png",
                          "toolbar2.snippet/fjicons/back_pushed.png",
                          "toolbar2.snippet/fjicons/back.png",
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
            '<td class="toolBarBtnCell" style="width=50%;"><img class="toolBarBtn" id="addToBookMark"></td>'+
            '</tr>'+
            '</table>';

         document.write(tbhtml);


         /* add back button as a simple button */
         new SimpleButton("goBacktoWebViewBM",
                          "toolbar2.snippet/fjicons/back.png",
                          "toolbar2.snippet/fjicons/back_pushed.png",
                          "toolbar2.snippet/fjicons/back.png",
                          _goBackFromBookmarkView);

         /* add bookmark button as a simple button */
         new SimpleButton("addToBookMark",
                         "toolbar2.snippet/fjicons/addwindow.png",
                         "toolbar2.snippet/fjicons/addwindow_pushed.png",
                         "toolbar2.snippet/fjicons/addwindow.png",
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
                          "toolbar2.snippet/fjicons/back.png",
                          "toolbar2.snippet/fjicons/back_pushed.png",
                          "toolbar2.snippet/fjicons/back.png",
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
            '<td class="toolBarBtnCell" style="width=50%;"><img class="toolBarBtn" id="addWindow"></td>'+
            '</tr>'+
            '</table>';


         document.write(tbhtml);

         /* add back as simple button, add window is an action button */
         new SimpleButton("goBacktoWebViewWin",
                          "toolbar2.snippet/fjicons/back.png",
                          "toolbar2.snippet/fjicons/back_pushed.png",
                          "toolbar2.snippet/fjicons/back.png",
                          _goBackFromWindowView);

       }

    _goBackFromWindowView = function() {
        window.viewStack.switchView("webView", "WindowView");
    }
    _writeWindowToolbar();
}



