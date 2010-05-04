/*!
  \file zoombar.js This module contains the ZoomBar class.
*/

/*!
  Class to handle displaying the zoom bar. The zoom bar is displayed when the 
  user presses the zoom button on the toolbar. It provides access to the zoom
  in and out functions. It is hidden when the main toolbar is hidden on user inactivity
  
  \param webtb web toolbar needed to set state of zoom button
*/
function ZoomBar(webtb)
{
    var timeoutId = 0; // inactivity timer ID
    var ZB_TIMEOUT = 5000; // hide zoombar after 5 secs
    var zooming = false; // true when in the process of zooming
    var enabled = true; // zooming enabled flag - initially enabled
    
    // Private Methods
    //! Write zoom bar HTML code to document.
    function _zoombar_write() {
        var html = ''+
            '<div id="zoomBarDiv">'+
            '<img class="zoomBarBtn" id="zoomBarBtnIn">'+
            '<img class="zoomBarBtn" id="zoomBarBtnOut">'+
            '</div>';
        document.write(html);
    }

    //! Create zoom in & out buttons.
    function _setActions() {
        new ActionButton("zoomBarBtnIn",
                         "zoombar.snippet/icons/icon_zoom+.png",
                         "zoombar.snippet/icons/icon_zoom+_pressed.png",
                         "zoombar.snippet/icons/icon_zoom+_disabled.png",
                         window.views.WebView.actions.zoomIn,
                         true,true,true);

        new ActionButton("zoomBarBtnOut",
                         "zoombar.snippet/icons/icon_zoom-.png",
                         "zoombar.snippet/icons/icon_zoom-_pressed.png",
                         "zoombar.snippet/icons/icon_zoom-_disabled.png",
                         window.views.WebView.actions.zoomOut,
                         true,true,true);
        
        // reset toolbar timeout each time a zoom is triggered
//        window.views.WebView.actions.zoomIn.triggered.connect(
//            function(checked) {handleZoom();});
//        window.views.WebView.actions.zoomOut.triggered.connect(
//            function(checked) {handleZoom();});
        // Connect to each action's "changed" signal.
        window.views.WebView.actions.zoomIn.changed.connect(
            createDelegate(this, handleChange));
        window.views.WebView.actions.zoomOut.changed.connect(
            createDelegate(this, handleChange));
    }
    
    //! Set inactivity timer when not zooming.
    function handleZoom()
    {
        // zoom activation toggles zoom on/off
        zooming = !zooming; // toggle zoom state
        clearTimeout(timeoutId);
        if (!zooming) {
            // close zoom after 5 secs
            timeoutId = setTimeout('window.snippets.ZoomBarId.hide()', ZB_TIMEOUT);
        }
    }
    
    //! Start inactivity timer when zoom bar is shown.
    function handleShow()
    {
        timeoutId = setTimeout('window.snippets.ZoomBarId.hide()', ZB_TIMEOUT); // close zoom after 5 secs
    }
    
    //! Clear inactivity timer when zoom bar is hidden.
    function handleHide()
    {
        clearTimeout(timeoutId);
        zooming = false; // ensure zooming state doesn't get out of sync
    }
    
    //! Handle action object changes. In particular we are interested in
    //! changes to the enabled state of the object.
    function handleChange() {
        var saveEnabled = enabled;
        
        // enable zoom button if either zoom-in or zoom-out action enabled
        if (window.views.WebView.actions.zoomIn.enabled
            || window.views.WebView.actions.zoomOut.enabled) {
            enabled = true;
        } else {
            enabled = false;
        }
        
        // if state changed update web toolbar zoom button state
        if (saveEnabled != enabled) {
            // enabled state changed
            webtb.setZoomEnabled(enabled);
        }
    }
    function handleLoadStarted() {
        window.snippets.ZoomBarId.hide()
    }

    //! After chrome is loaded, create zoombar buttons and setup show/hide 
    //! handlers.
    function _chromeLoadComplete() {
        _setActions();
        window.pageController.loadStarted.connect(handleLoadStarted);
//        window.snippets.ZoomBarId.shown.connect(handleShow);
//        window.snippets.ZoomBarId.hidden.connect(handleHide);
    }

    _zoombar_write();

    window.chrome.chromeComplete.connect(_chromeLoadComplete);
}



 
