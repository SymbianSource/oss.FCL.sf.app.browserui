function ZoomBar()
{
    // Private Methods
    //! Write status bar HTML code to document.
    function _zoombar_write() {
        var html = ''+
            '<div id="zoomBarSpan">'+
            '<img class="zoomBarBtn" id="zoomBarBtnIn">'+
            '<img class="zoomBarBtn" id="zoomBarBtnOut">'+
            '</div>';
        document.write(html);
    }

    function _setActions () {
        new ActionButton("zoomBarBtnIn",
                     "zoombar.snippet/icons/zoom+.png",
                     "zoombar.snippet/icons/zoom+_pushed.png",
                     "zoombar.snippet/icons/zoom+_disabled.png",
                     window.webView.actions.zoomIn);

        new ActionButton("zoomBarBtnOut",
                     "zoombar.snippet/icons/zoom-.png",
                     "zoombar.snippet/icons/zoom-_pushed.png",
                     "zoombar.snippet/icons/zoom-_disabled.png",
                     window.webView.actions.zoomOut);
    }

    function _chromeLoadComplete () {
        _setActions();
    }

    _zoombar_write();
    window.chrome.loadComplete.connect(_chromeLoadComplete);
}



 
