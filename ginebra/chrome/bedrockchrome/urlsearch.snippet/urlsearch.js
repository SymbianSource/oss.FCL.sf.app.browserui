/*!
  \file urlsearch.js This module contains the UrlSearch class.
*/

var enumObj = new Object();
enumObj.state = {Loading:0, Editing:1, Reloadable:2}

/* The following "classes" are psudo classes since javascript doesn't have 
   true classes. The prototype object is a property of all JavaScript
   objects and can be used to impart properties to each instance of a
   class - these properties become public and can't access private class 
   members. Another way to create a public method is to add code like the
   following inside of the class constructor.
   
   // create a public method named clearPBar
   this.clearPBar = function()
   {
   }
   
   Methods created this way can access private members. Syntax-wise, 
   private member variables and methods are created in the usual fasion but 
   within the class constructor. Semantically they are simulated in JavaScript 
   using closures.
*/

/*!
  Class to handle displaying and updating the URL search bar. Only 1 search 
  bar should be created for the browser. This class is not designed to be code
  space efficient for creating multiple URL search bar objects.
*/
function UrlSearch(lastUrl)
{   
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
        //window.chrome.alert("setButton " + window.pageController.loadState);
        btnDom = document.getElementById("tristateBtn").button;
        switch (window.pageController.loadState) {
        case enumObj.state.Loading:
            btnDom.updateImages("urlsearch.snippet/icons/stop.png",
                                "urlsearch.snippet/icons/stop_pushed.png",
                                "urlsearch.snippet/icons/stop.png");
            break;
        case enumObj.state.Editing:
            btnDom.updateImages("urlsearch.snippet/icons/goto.png",
                                "urlsearch.snippet/icons/goto_pushed.png",
                                "urlsearch.snippet/icons/goto.png");
            break;
        case enumObj.state.Reloadable:
            btnDom.updateImages("urlsearch.snippet/icons/reload.png",
                                "urlsearch.snippet/icons/reload_pushed.png",
                                "urlsearch.snippet/icons/reload.png");
            break;
        default:
            window.chrome.alert("Incorrect state");
            break;
        }
        
        document.getElementById("tristateBtn").button.updateButton();
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
        //window.chrome.alert("Load prog "+percent);
        if ((window.viewManager.currentView.type == "webView")  &&
            (this.oldpercent != percent)) {
            this.oldpercent = percent;
            //window.chrome.alert("Load prog "+percent);
            var parentWidth = document.getElementById("PWrap").offsetWidth;
            this.setProgress((parentWidth * percent)/100 + "px");
        }
    }
    
    UrlProgressBar.prototype.updateProgress = function(percent)
    {
        if (percent != 100) {
            //window.chrome.alert("Load prog "+percent);
            var parentWidth = document.getElementById("PWrap").offsetWidth;
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
    var searchSuggestEngine = new SearchSuggest();
    // UrlSearch Private Methods
    
    function _urlsearch_write(lastUrl) {
          var html = ''+
            '<form name = "gotoUrlForm">' +
              '<table id="UrlSearchTable">' +
              '<tr>' +
                '<td id="faviconCell"><img id = "favicon" src="urlsearch.snippet/icons/defaultFavicon.png"></td>'+
                '<td>' +
                  '<div id="PWrap">' +
                  '<input type ="text" id = "urlBox" name = "textBox" value="' + lastUrl + '" '  +
                  'maxlength="256" '+
                  'OnFocus="urlsearch.focusElement(this,event)" '+
                  'OnMouseUp="return urlsearch.mouseUpElement(this,event)" '+
                  'OnMouseDown="urlsearch.mouseDownElement(this,event)" '+
                  'OnKeyUp="urlsearch.resetTriState();urlsearch.querySuggests();"/>' +
                  '<div id="PBar"></div>' +
                  '</div>' +
                '</td>' +
                '<td id="tristateBtnCell"' +
                  '<img id="tristateBtn">' +
                '</td>' +
              '</tr>' +
              '</table>' +
            '</form>';

        document.write(html);
    }
    
    function loadToMainWindow(gotourl){
        // use a single call to guess url and goto url
        // (could do as separate calls, but we try to eliminate lag with a single call)

        //var correctedUrl = window.chrome.guessAndGotoUrl(gotourl);
        //urlBarTextBox.setTextBoxValue(correctedUrl);

        var gotourl = window.chrome.guessUrlFromString(gotourl);
        window.pageController.currentLoad(gotourl);
        urlBarTextBox.setTextBoxValue(gotourl);
        window.pageController.urlTextChanged(gotourl);
    }
    
    function loadSearchUrl(gotourl){
        var gotourl = window.chrome.searchUrl(gotourl);
        window.pageController.currentLoad(gotourl);
        urlBarTextBox.setTextBoxValue(gotourl);
        window.pageController.urlTextChanged(gotourl);
    }

    function chromeAlert(){
        window.chrome.alert("chrome Alert!");
    }

    function canSeeUrl()
    {
        return (!window.snippets.UrlSearchChromeId.isHiding && window.snippets.UrlSearchChromeId.isVisible());
    }

    function showUrlBar()
    {
        if (!canSeeUrl()) {
            //window.chrome.alert("showUrlbar");
            window.snippets.UrlSearchChromeId.zValue =  window.snippets.StatusBarChromeId.zValue - 1;
            window.snippets.UrlSearchChromeId.setPosition(0,defaultPos);
            window.snippets.UrlSearchChromeId.show(false);
            window.chrome.updateViewPort();
        }
    }

    function hideUrlBar()
    {
        //window.chrome.alert("hideUrlbar");
        if (canSeeUrl()) {
            window.snippets.UrlSearchChromeId.hide(false);
        }
    }

    function slideUrlOut() {
        //window.chrome.alert("slideUrlOut");
        if (canSeeUrl()) {
            window.snippets.UrlSearchChromeId.zValue =  window.snippets.StatusBarChromeId.zValue - 1;
            animating = true;
            //window.snippets.UrlSearchChromeId.isHiding = true;
            animator = window.snippets.UrlSearchChromeId.animate(animateDuration).translateBy(0, -1 * defaultHeight).start();
            animator.updated.connect(animationUpdated);
            animator.finished.connect(hideAnimationFinished);
        }
    }

    function slideUrlIn()
    {
        /* show it at right position and zorder first */
        window.snippets.UrlSearchChromeId.zValue =  window.snippets.StatusBarChromeId.zValue - 1;
        window.snippets.UrlSearchChromeId.setPosition(0, defaultPos-defaultHeight);
        window.snippets.UrlSearchChromeId.show(false);
        //window.snippets.UrlSearchChromeId.isHiding = false;

        animating = true;
        animator = window.snippets.UrlSearchChromeId.animate(animateDuration).translateBy(0, defaultHeight).start();
        animator.updated.connect(animationUpdated);
        animator.finished.connect(showAnimationFinished);
    }
    
    function setUrlSearchValues()
    {
        //window.chrome.alert("setUrlSearchValues " + window.pageController.loadText);
        urlBarTextBox.setTextBoxValue(window.pageController.loadText);
        urlBarButton.setButton();

        //window.chrome.alert("setProgress " + "New: " +  window.pageController.loadProgressValue +  " Old Value: " + oldpercent );
        var percent = window.pageController.loadProgressValue;
        
        urlBarProgressBar.updateProgress(percent);
        if (percent == 100) {
            __blockDeselect = false;
            urlBarTextBox.setFocus(false);
        }
        window.snippets.UrlSearchChromeId.repaint();
    }

    function animationUpdated(value)
    {
        window.chrome.updateViewPort();
    }

    function hideAnimationFinished()
    {
        //window.chrome.alert("hideAnimationFinished");
        animating = false;
        window.snippets.UrlSearchChromeId.isHiding = true;
    }

    function showAnimationFinished()
    {
        //window.chrome.alert("showAnimationFinished");
        animating = false;
        window.snippets.UrlSearchChromeId.isHiding = false;
    }
    
    // Public Methods
    this.focusElement = function(el, event)
    {
        el.select();
        __blockDeselect = true;
        document.getElementById("urlBox").scrollLeft = 1000;
        urlBarTextBox.setFocus(true);
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
        //window.chrome.alert("pushTriState " + window.pageController.loadState);
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
        if(window.pageController.loadState == enumObj.state.Reloadable) {
            urlBarTextBox.setFocus(true);
            //window.chrome.alert("Change to editing");
            window.pageController.setLoadState(enumObj.state.Editing);
            urlBarButton.setButton();
        }
    }

    this.querySuggests = function()
    {
        /* Disable suggests
        var text = urlBarTextBox.getTextBoxValue();
        //window.chrome.alert("call searchSuggest " + text);
        searchSuggests.removeAllItems();
        searchSuggests.hideSuggests();
        searchSuggestEngine.searchSuggest(text, this.updateSuggests);
        */
    }

    this.updateSuggests = function(result)
    {
        var curText = urlBarTextBox.getTextBoxValue();
        searchSuggests.removeAllItems();
        if (result.length > 1 && curText == result[0]){
            for(var i = 1; i < result.length; i++) {
                //window.chrome.alert(result[i]);
                searchSuggests.addItem(result[i]);
            }
        searchSuggests.showSuggests();
        }
        
        
            
    }

    // UrlSearch Constructor
    
    // do setup
    _urlsearch_write(lastUrl);


    // Init the button
    new SimpleButton("tristateBtn",
                     "urlsearch.snippet/icons/goto.png",
                     "urlsearch.snippet/icons/goto_pushed.png",
                     "urlsearch.snippet/icons/goto.png",
                     this.pushTriState);
    urlBarButton.setButton();
    
    // Note that in the slots below the "this" object is never used directly.
    // This is because they don't have access to "this" as they are called
    // externaly.
    
    // Connect Chrome load complete signal to slot.
    window.chrome.loadComplete.connect(
        function () {
            defaultPos = window.snippets.UrlSearchChromeId.anchorOffset;
            defaultHeight = UrlSearchChromeId.offsetHeight;
        }
        );

    // Connect various Page Controller signals to slots.
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
            urlBarProgressBar.setProgress("5%");
            urlBarButton.setButton();
            
            /* show url bar if it's hiding in contentView */
            if (window.viewManager.currentView.type == "webView") {
                if (window.snippets.UrlSearchChromeId) {
                    showUrlBar();
                }
            }
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

            /* start hide animation if contentYpos is not zero */
            if (window.viewManager.currentView.type == "webView") {
               if (window.snippets.UrlSearchChromeId && window.pageController.contentsYPos > 0) {
                   slideUrlOut();
                }
            }
        }
        );

    window.pageController.pageIconChanged.connect(
        function() {
            window.chrome.alert("FAVICON changed");
        }
        );

    window.pageController.pageCreated.connect(
        function() {
            //window.chrome.alert("add new window");
            showUrlBar();
        }
        );

    window.pageController.pageChanged.connect(
        function() {

         //window.chrome.alert("Page  changed");
         // Set the values of the new page 
         setUrlSearchValues();
        }
        );

    window.pageController.pageScrollPositionZero.connect(
        function() {
            var contentYPos = window.pageController.contentsYPos;
            //window.chrome.alert("pageScrollRequested contentYPos = " + contentYPos);
            if (contentYPos > 0) {
                if(canSeeUrl() && !animating && !pageController.isPageLoading) {
                    slideUrlOut();
                    //window.chrome.updateViewPort();
                }
            }
            else {
                if(!canSeeUrl() && !animating) {
                    slideUrlIn();
                }
            }
        }
        );
    
    // Connect snippets CR signal to slot.
    window.snippets.symbianCarriageReturn.connect(
        function() {
            if (urlBarTextBox.hasFocus()) {
                // When we get load started, the button will be changed
                loadToMainWindow(urlBarTextBox.getTextBoxValue());
            }
        }
        );

    // Connect View Manager current view changed signal to slot.
    window.viewManager.currentViewChanged.connect(
        function() {
             if (window.viewManager.currentView.type ==  "webView") {
                 /*window.chrome.alert("currentViewChanged  window.viewManager.currentView.type =" + window.viewManager.currentView.type + "Pos " + window.pageController.contentsYPos == 0  + "Loading still: " + pageController.isPageLoading);*/
                 // When we change back to content view, show the urlsearch bar if either we are
                 // at the top or we are still loading the page (in this case, we will hide the urlsearch bar
                 // if needed on getting loadFinished
                 if (window.pageController.contentsYPos == 0 || pageController.isPageLoading) {
                     // show url bar with no animation
                     showUrlBar();
                 }
                 //set the correct values based on the window 
                 //window.chrome.alert("currentViewChanged " + window.pageController.loadText);
                 setUrlSearchValues();
                 
                 // place focus in urlsearch bar when returning from adding a new window in windows view
                 if (window.pageController.loadText == "") {
                     __blockDeselect = false;
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
                 hideUrlBar();
             }
     
        }
        );
}

