var _backEnabled = false;

function onBackEnabled(enabled) 
{
    _backEnabled = enabled;
}

var _forwardEnabled = false;

function onForwardEnabled(enabled)  
{
    _forwardEnabled = enabled;
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

function onChromeComplete(){
    window.snippets.UrlSearchChromeId.anchorToView("top");
    
    window.views.WebView.backEnabled(onBackEnabled);
    window.views.WebView.forwardEnabled(onForwardEnabled);
    //window.snippets.ButtonContainer.setVisibilityAnimator("G_VISIBILITY_FADE_ANIMATOR");
    
    result = new Array;
    result = calcToolbarPopAnchorOffset(window.snippets.ZoomBarId,
                                        window.snippets.ZooomButtonSnippet,
                                        0,10);       
    window.snippets.ZoomBarId.anchorTo("ZooomButtonSnippet",result[0],result[1]);
    
    result = calcToolbarPopAnchorOffset(window.snippets.ContextMenuId,
                                        window.snippets.MenuButtonSnippet,
                                        0,10); 
    
    window.snippets.ContextMenuId.anchorTo("MenuButtonSnippet",result[0],result[1]);
}
