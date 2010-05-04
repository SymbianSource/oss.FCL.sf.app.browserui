/*!
  \file urlsearch.js This module contains the UrlSearch class.
*/

var enumObj = new Object();
enumObj.state = {Loading:0, Editing:1, Reloadable:2}

function UrlSearch(lastUrl)
{   
    function onChromeComplete() 
    {
        // When restoring session we're already loading when chrome complete signal is sent
        // So update button and progress to reflect the loading state
        switch (window.pageController.loadState) {
        case enumObj.state.Loading:
            urlBarButton.setButton();
            urlBarProgressBar.setProgress("5%");
            break;
        default:
            urlBarProgressBar.setProgress("0%");
            break;
        }
        //window.snippets.FaviconSnippet.anchorTo("UrlSearchChromeId", 1,5);
        //window.snippets.FaviconSnippet.connectToWebpageController();
        window.pageController.pageUrlChanged.connect(
        function(url) {
            //window.chrome.alert(url);
            //url is coming from loadContoller. So need not set it back
            urlBarTextBox.setTextBoxValue(url);
        }
        );

        window.pageController.pageLoadStarted.connect(
        function() {
            //window.chrome.alert("Load start");
            //window.app.debug("Load start" + window.pageController.currentRequestedUrl);
            window.chrome.slideView(100);
            urlBarProgressBar.setProgress("5%");
            urlBarButton.setButton();
            urlBarTextBox.setTextBoxValue(window.pageController.currentRequestedUrl); 
        }
        );

        window.pageController.pageLoadProgress.connect(
        function(percent) {
            urlBarProgressBar.handlePageLoadProgress(percent);
        }
        );

        window.pageController.pageLoadFinished.connect(
        function(ok) {

        //window.chrome.alert("pageLoadFinished: ");
        // If the load was finished normally and not due to user stopping it,
        // simulate progress completion
        if(!window.pageController.loadCanceled) {
            urlBarProgressBar.setProgress("99%");
        }
        setTimeout("urlsearch.clearPBar()", 500);
        __blockDeselect = false;
        urlBarTextBox.setFocus(false);
        urlBarButton.setButton();
      
        if (window.views.current().type == "webView") {
            if (window.snippets.UrlSearchChromeId && window.pageController.contentsYPos > 0) {
                window.chrome.slideView(-100);
            }
        }
        }
        );
  
		    window.pageController.pageCreated.connect(
		    function() {
		    	  window.chrome.slideView(100);
		    	}
		    );
  

    window.pageController.pageChanged.connect(
    function() {
        //window.chrome.alert("Page  changed");
        // Set the values of the new page 
        setUrlSearchValues();
    }
    );

    // Connect View Manager current view changed signal to slot.
    window.views.currentViewChanged.connect(
        function() {
            if (window.views.current().type ==  "webView") {
                /*window.chrome.alert("currentViewChanged  window.views.current().type =" + window.views.current().type + "Pos " + window.pageController.contentsYPos == 0  + "Loading still: " + pageController.isPageLoading);*/
                // When we change back to content view, show the urlsearch bar if either we are
                // at the top or we are still loading the page (in this case, we will hide the urlsearch bar
                // if needed on getting loadFinished
                if (!window.views.WebView.currentPageIsSuperPage() && 
                    (window.pageController.contentsYPos <= 0 || pageController.isPageLoading) ){
                    // show url bar with no animation
                    //showUrlBar();
                    window.chrome.slideView(100);
                }
                else {
                	window.chrome.slideView(-100);
                } 
                //set the correct values based on the window 
                //window.chrome.alert("currentViewChanged " + window.pageController.loadText);
                setUrlSearchValues();
                 
                // place focus in urlsearch bar when returning from adding a new window in windows view
                if (!window.views.WebView.currentPageIsSuperPage() &&
                    (window.pageController.loadText == "")) {
                    __blockDeselect = false;
                    window.snippets.UrlSearchChromeId.grabFocus();
                    document.getElementById("urlBox").focus();
                    return;
                 }
             } else {
                 //Save url text box value
                 window.pageController.urlTextChanged(urlBarTextBox.getTextBoxValue());
             
                 // Remove progress bar and url text field value so that 
                 // incorrect values are not seen before we can update when we come back 
                 urlBarTextBox.setTextBoxValue("");
                 urlBarProgressBar.setProgress("0%");
                 //hideUrlBar();
                 window.chrome.slideView(-100);
             }
     
        }
    );


window.ViewStack.currentViewChanged.connect(
        function() {
            if (window.views.current().type ==  "webView") {
                /*window.chrome.alert("currentViewChanged  window.views.current().type =" + window.views.current().type + "Pos " + window.pageController.contentsYPos == 0  + "Loading still: " + pageController.isPageLoading);*/
                // When we change back to content view, show the urlsearch bar if either we are
                // at the top or we are still loading the page (in this case, we will hide the urlsearch bar
                // if needed on getting loadFinished
                if (!window.views.WebView.currentPageIsSuperPage() && 
                    (window.pageController.contentsYPos <= 0 || pageController.isPageLoading) ){
                    // show url bar with no animation
                    //showUrlBar();
                    window.chrome.slideView(100);                    
                }
                else {
                	window.chrome.slideView(-100);                	
                } 
                //set the correct values based on the window 
                //window.chrome.alert("currentViewChanged " + window.pageController.loadText);
                setUrlSearchValues();
                 
                // place focus in urlsearch bar when returning from adding a new window in windows view
                if (window.pageController.loadText == "") {
                    __blockDeselect = false;
                    window.snippets.UrlSearchChromeId.grabFocus();
                    document.getElementById("urlBox").focus();
                    return;
                 }
             } else {
                 //Save url text box value
                 window.pageController.urlTextChanged(urlBarTextBox.getTextBoxValue());
             
                 // Remove progress bar and url text field value so that 
                 // incorrect values are not seen before we can update when we come back 
                 urlBarTextBox.setTextBoxValue("");
                 urlBarProgressBar.setProgress("0%");
                 //hideUrlBar();
                 window.chrome.slideView(-100);
             }
     
        }
    );


    // Connect snippets CR signal to slot.
    window.chrome.symbianCarriageReturn.connect(
        function() {
            if (urlBarTextBox.hasFocus()) {
                // When we get load started, the button will be changed
                loadToMainWindow(urlBarTextBox.getTextBoxValue());
            }
        }
    );

    } //End of onChromeComplete handler


    /*!
      Class to handle displaying and updating the URL text box.
    */
    function UrlTextBox()
    {
    }
    
    //! UrlTextBox Public Methods
    
    UrlTextBox.prototype.getTextBoxValue = function()
    {
        return document.gotoUrlForm.textBox.value;
    }
    
    UrlTextBox.prototype.setTextBoxValue = function(text)
    {
        //window.chrome.alert("setTextBoxValue" + text);
        document.gotoUrlForm.textBox.value = text;
    }
    
    // Unused Method
    UrlTextBox.prototype.textBoxFocus = function()
    {
        document.gotoUrlForm.textBox.focus();
    }
    
    UrlTextBox.prototype.hasFocus = function()
    {
        return document.getElementById("urlBox").hasFocus;
    }
    
    UrlTextBox.prototype.setFocus = function(hasFocus)
    {
        document.getElementById("urlBox").hasFocus = hasFocus;
    }
    
    /*!
      Class to handle displaying and updating the URL tri-state button.
    */
    function UrlTriStateButton()
    {
    }
    
    //! UrlTriStateButton Public Methods
    UrlTriStateButton.prototype.setButton = function()
    {
        //window.app.debug("setButton ");
        btnDom = document.getElementById("tristateBtn").button;
        switch (window.pageController.loadState) {
        case enumObj.state.Loading:
            btnDom.updateImages("urlsearch.snippet/icons/stop_btn.png",
                                    "urlsearch.snippet/icons/stop_btn_pressed.png",
                                    "urlsearch.snippet/icons/stop_btn.png");
            break;
        case enumObj.state.Editing:
            btnDom.updateImages("urlsearch.snippet/icons/go_btn.png",
                                "urlsearch.snippet/icons/go_btn_pressed.png",
                                "urlsearch.snippet/icons/go_btn.png");
            break;
         case enumObj.state.Reloadable:
            btnDom.updateImages("urlsearch.snippet/icons/refresh_btn.png",
                                    "urlsearch.snippet/icons/refresh_btn_pressed.png",
                                    "urlsearch.snippet/icons/refresh_btn.png");
            break;
         default:
            window.App.debug("Incorrect state");
            break;
        }

        document.getElementById("tristateBtn").button.updateButton();

        // Don't need repaint anymore?
        //if (window.snippets.UrlSearchChromeId) {
        //    window.snippets.UrlSearchChromeId.repaint();
        //}
    }

    // Unused Method
    UrlTriStateButton.prototype.changeTriState = function(a)
    {
       if (this.getValue() != a) {
            this.setValue(a);
            /* Shouln't have to do this check. This function should be 
             * called only after chrome has completed loading which 
             * means that all snippets have also been created.
             */
            if (window.snippets.UrlSearchChromeId) {
                window.snippets.UrlSearchChromeId.repaint();
            } 
       }
    }

    /*!
      Class to handle displaying and updating the URL progress bar.
    */
    function UrlProgressBar()
    {
        this.oldpercent = 0;
    }
    
    //! UrlProgressBar Public Methods
    
    UrlProgressBar.prototype.setProgress = function(widthStr)
    {
        document.getElementById("PBar").style.width = widthStr;
    }
    
    UrlProgressBar.prototype.handlePageLoadProgress = function(percent)
    {
        this.updateProgress(percent);
    }
    
    UrlProgressBar.prototype.updateProgress = function(percent)
    {
        if (percent != 100) {
            //window.chrome.alert("Load prog "+percent);
            var parentWidth = document.getElementById("textEntryCell").offsetWidth;
            this.setProgress((parentWidth * percent)/100 + "px");
        } else {
            //clearProgressBar();
            this.setProgress("0%");
            oldpercent = 0;
        }
    }
    
    // UrlSearch Private Member Variables
    var animator;
    var animating = false;
    var defaultPos;
    var defaultHeight;
    var animateDuration = 500;
    var __blockDeselect = false;
    var urlBarTextBox = new UrlTextBox();
    var urlBarButton = new UrlTriStateButton();
    var urlBarProgressBar = new UrlProgressBar();

    // UrlSearch Private Methods
    function _urlsearch_write(lastUrl) {
        var html = '' +
            '<div id="urlsearchSnippet">' +

                    '<div class = "tableMain" id="mainLayoutTable">' +
                        '<div class = "tableRow" id="mainLayoutRow">' +
                                    '<div class = "tableCell mainLayoutCell" id = "inputboxl"></div>' +
                            '<div class = "mainLayoutCell" id = "inputboxc">' +
                                '<form name = "gotoUrlForm">' +
            
                                '<div class = "tableRow">' +
                                    '<div class = "tableCell" id = "textEntryCell">' +
                                    '<input type ="text" id = "urlBox" name = "textBox" value="' + lastUrl + '" '  +
                                    'maxlength="256" '+
                                    		'OnFocus="urlsearch.focusElement(this,event)" '+
                                    		'OnMouseUp="return urlsearch.mouseUpElement(this,event)" '+
                                    		'OnMouseDown="urlsearch.mouseDownElement(this,event)" '+
                                    		'OnBlur="urlsearch.mouseOutElement(this,event)" '+
                                    		'OnKeyUp="urlsearch.resetTriState();"/>' +
                                      	'<div id="PBar"></div>' +
                                      '</div>' +
                                    '<div class = "tableCell" id = "tristateBtnCell">' +
                                    '<img id="tristateBtn">' +
                                        '</div>' +
                                '</div>' +

                                '</form>' +
                            '</div>' +
                        '<div class = "tableCell" id = "inputboxr"></div>' +
                        '</div>' +
                    '</div>' +
                        '</div>';

        document.write(html);
    }
    
    function loadToMainWindow(gotourl){
        var gotourl = window.pageController.guessUrlFromString(gotourl);
        urlBarTextBox.setTextBoxValue(gotourl);
        window.pageController.currentLoad(gotourl);
        window.pageController.urlTextChanged(gotourl);

    }
    
    function setUrlSearchValues()
    {
          //window.app.debug("seturlsearchvalues")
        urlBarTextBox.setTextBoxValue(window.pageController.loadText);
        urlBarButton.setButton();

        //window.chrome.alert("setProgress " + "New: " +  window.pageController.loadProgressValue +  " Old Value: " + oldpercent );
        var percent = window.pageController.loadProgressValue;
        
        urlBarProgressBar.updateProgress(percent);
        if (percent == 100) {
            __blockDeselect = false;
            urlBarTextBox.setFocus(false);
        }
        //window.snippets.UrlSearchChromeId.repaint();
    }

    function chromeAlert(){
        window.chrome.alert("chrome Alert!");
    }
    
    // Public Methods
    this.focusElement = function(el, event)
    {
        el.select();
        __blockDeselect = true;
        document.getElementById("urlBox").scrollLeft = 1000;
        urlBarTextBox.setFocus(true);
    }
		
		this.mouseOutElement = function(el, event)
    {
        __blockDeselect = false;
        el.scrollRight = 0;
        urlBarTextBox.setFocus(false); 
    }
    
    this.mouseDownElement = function(el, event)
    {
        // Clear __justSelected in case the element gained focus through some non-mouse event
        // and still has focus.
         __blockDeselect = false;
    }

    this.mouseUpElement = function(el, event)
    {
        // Return false to prevent Qt from deselecting the text if we've just selected it
        // in focusElement(), return true otherwise.

        var result = !__blockDeselect;
        __blockDeselect = false;
        return result;
    }

    this.clearPBar = function()
    {
        urlBarProgressBar.setProgress("0%");
    }

    this.pushTriState = function()
    {
        //window.app.debug("pushTriState ");
        switch (window.pageController.loadState) {
        case enumObj.state.Loading:
            //window.chrome.alert("In loading state, stop loading");
            window.pageController.currentStop();
            setTimeout("urlsearch.clearPBar()", 50);
            break;
        case enumObj.state.Editing:
            //window.chrome.alert("In editing state, start loading");
            loadToMainWindow(urlBarTextBox.getTextBoxValue());
            break;
        case enumObj.state.Reloadable:
            //window.chrome.alert("In reloadable state, start reloading");
            window.pageController.currentReload();
            break;
        default:
            window.chrome.alert("Incorrect state");
            break;
        }
        urlBarButton.setButton();
    }

    this.resetTriState = function()
    {
        //window.app.debug("resetTriState");
        if(window.pageController.loadState == enumObj.state.Reloadable) {
            urlBarTextBox.setFocus(true);
            //window.chrome.alert("Change to editing");
            window.pageController.setLoadState(enumObj.state.Editing);
            urlBarButton.setButton();
        }
    }

    this.updateSuggests = function()
    {
       var text = urlBarTextBox.getTextBoxValue();
       if (text.length == 0) {  // hide the suggests if no text
           searchSuggests.hideSuggests();
           return;
       }

       if (text.length >= 8) { // only for demo
           searchSuggests.hideSuggests();
           return
       }

       searchSuggests.removeAllItems();
       searchSuggests.addItem(text);
       var i = 0;
       var n = 100;
       for (i = text.length; i < 8; i += 1, n += 100)
           searchSuggests.addItem(text + n);
       searchSuggests.showSuggests();
    }

    // UrlSearch Constructor
    window.chrome.chromeComplete.connect(onChromeComplete);    
    // do setup
    _urlsearch_write(lastUrl);

    // Init the button
    new SimpleButton("tristateBtn",
                     "urlsearch.snippet/icons/goto.png",
                     "urlsearch.snippet/icons/goto_pushed.png",
                     "urlsearch.snippet/icons/goto.png",
                     this.pushTriState);

}

