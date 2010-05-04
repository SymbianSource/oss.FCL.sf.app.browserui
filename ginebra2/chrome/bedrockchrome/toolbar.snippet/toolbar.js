/* create all functions with toolbar */

//
// INIT the webView Toolbar
//

function ToolbarSnippet()
{
  this.setContainerWidth = function(leftid, rightid, id) {
      var lcbWidth = document.getElementById(leftid).offsetWidth;
      var rcbWidth = document.getElementById(rightid).offsetWidth;
      var totalW =   window.chrome.displaySize.width;
      
      var fcWidth = totalW - lcbWidth - rcbWidth;
      var fcWidthPercent = (fcWidth/totalW) * 100 ;
      fcWidthPercent = fcWidthPercent.toFixed(2);
      
      el =  document.getElementById(id);
      el.style.width = fcWidthPercent +"%";
  }

  /*
  ** Main View Buttons
  */
  function LeftCornerButton() {
      this.writeButton = function() {
          document.write('<img  class="webToolbarBtn"id="backButton" src="toolbar.snippet/icons/icon_back.png">');       
      }
  }
  function RightCornerButton() { 
      this.writeButton = function() {
          document.write('<img  class="webToolbarBtn"id="mvButton">');  

		  new ToggleButton("mvButton", "MostVisitedViewId",
                         "toolbar.snippet/icons/icon_mostvisited.png",
                         "toolbar.snippet/icons/icon_mostvisited_pressed.png",
                         "toolbar.snippet/icons/icon_mostvisited_disabled.png",
                         _activateMostVisited);                                       
      }
      function _activateMostVisited() {
          window.snippets.ZoomBarId.hide(); // hide Zoom Bar while showing most visited
		  window.snippets.MostVisitedViewId.toggleVisibility();
      }      
  }

  function WinButton() {
    this.writeButton = function() {
        document.write('<img class="webToolbarBtn" id="winButton">');        
        new SimpleButton("winButton",
                         "toolbar.snippet/icons/icon_windows1.png",
                         "toolbar.snippet/icons/icon_windows1_pressed.png",
                         "toolbar.snippet/icons/icon_windows1.png",
                         _activateWindows);
    }
    this.changeWindowIcon = function() {
        var index = window.pageController.pageCount();
        var base = "toolbar.snippet/icons/icon_windows" + index;
        
        //window.app.debug("BASE "+ base);
        document.getElementById('winButton').button.updateImages(base + ".png",
                                                                 base + "_pressed.png",
                                                                 base + ".png");
    }

    function _activateWindows() {
	  window.snippets.ZoomBarId.hide(); // hide Zoom Bar while showing windows view
	  window.snippets.MostVisitedViewId.hide();
      window.app.debug("_activateWindows");
      if(window.views.current().objectName == "WebView") {
        window.ViewStack.switchView("WindowView", "WebView");
      }
      else {
        window.ViewStack.switchView("WebView", "WindowView");
      }   
    }
  }

  function MenuButton() {
    this.writeButton = function() {
        document.write('<img class="webToolbarBtn" id="contextMenuButton">');
        new ToggleButton("contextMenuButton", "ContextMenuId",
                         "toolbar.snippet/icons/icon_menu.png",
                         "toolbar.snippet/icons/icon_menu_pressed.png",
                         "toolbar.snippet/icons/icon_menu_disabled.png",
                         _contextMenuF);
   }

   function _contextMenuF () {
       window.app.debug("_contextMenuF");
       if(window.snippets.ContextMenuId.dontShow)
       {
           window.snippets.ContextMenuId.dontShow = false;
           return;
       }
       
       window.snippets.ZoomBarId.hide(); // Don't show Zoom Bar while showing Context Menu
       window.snippets.MostVisitedViewId.hide();
       window.snippets.ContextMenuId.repaint();
       window.snippets.ContextMenuId.zValue = 10;
       window.snippets.ContextMenuId.show();
   }
  }
  
  /*!
    Class to handle displaying the zoom button. The zoom button provides access
    to the zoom bar.
  */
  function ZoomButton() {
      this.writeButton = function() {
          document.write('<img class="webToolbarBtn" id="zoomBarButton">');
          
          // create simple button to access zoom bar
          this.theButton = new ToggleButton("zoomBarButton", "ZoomBarId",
                       "toolbar.snippet/icons/icon_zoom.png",
                       "toolbar.snippet/icons/icon_zoom_pressed.png",
                       "toolbar.snippet/icons/icon_zoom_disabled.png",
                       _zoomBarF);
    }
    
    //! Handles zoom button presses.
    function _zoomBarF()
    {
        window.snippets.MostVisitedViewId.hide();
        // toggle visibility of zoom bar
        window.snippets.ZoomBarId.toggleVisibility();
    }
    
    //! Set enabled state of zoom button.
    /*!
      \param enabled new enabled state of button
    */
    this.setEnabled = function(enabled)
    {
      this.theButton.setEnabled(enabled);
    }
  } //end of class ZoomButton

  /*
  ** Windows View Toolbar Buttons
  */
  function WinBackButton() {
      this.writeButton = function() {
          window.app.debug("WinBackButton:writeButton");
          document.write('<img class="webToolbarBtn" id="goBacktoWebViewWin">');
          new SimpleButton("goBacktoWebViewWin",
                           "toolbar.snippet/icons/icon_back.png",
                           "toolbar.snippet/icons/icon_back_pressed.png",
                           "toolbar.snippet/icons/icon_back.png",
                           _goBackFromWindowView);
      }
      _goBackFromWindowView = function() {
          window.ViewStack.switchView("WebView", "WindowView");
      }
  }
  function WinAddButton() {
      this.writeButton = function() {
          document.write('<img class="webToolbarBtn" id="addWindow" src="toolbar.snippet/icons/icon_windowsadd.png">');
      }
  }
  
  /*
  ** Bookmarks Toolbar Buttons
  */
  function BMBackButton() {
      this.writeButton = function() {
          document.write('<img  class="webToolbarBtn" id="goBacktoWebViewBM">');
          new SimpleButton("goBacktoWebViewBM",
                           "toolbar.snippet/icons/icon_back.png",
                           "toolbar.snippet/icons/icon_back_pressed.png",
                           "toolbar.snippet/icons/icon_back.png",
                           _goBackFromBookmarkView);
      }
      _goBackFromBookmarkView = function() {
          window.views.WebView.showNormalPage();		
          window.ViewStack.switchView( "WebView","BookmarkTreeView");
          views.WebView.gesturesEnabled = true;
      }
  }
  function BMAddButton() { 
      this.writeButton = function() {
          document.write('<img  class="webToolbarBtn"id="addToBookMark">');
          new SimpleButton("addToBookMark",
			               "toolbar.snippet/icons/icon_addbookmark.png",
			               "toolbar.snippet/icons/icon_addbookmark_pressed.png",
			               "toolbar.snippet/icons/icon_addbookmark_disabled.png",
			               _showBookmarkAddDialog);
      }
      _showBookmarkAddDialog = function() {
          launchBookmarkAddDialog();
      }
  }

  /*
  ** Recent URLs Toolbar Buttons
  */
  function RecentURLBackButton() {
      this.writeButton = function() {
          document.write('<img  class="webToolbarBtn" id="goBacktoWebViewRecent">');
          new SimpleButton("goBacktoWebViewRecent",
                           "toolbar.snippet/icons/icon_back.png",
                           "toolbar.snippet/icons/icon_back_pressed.png",
                           "toolbar.snippet/icons/icon_back.png",
                           _goBackFromRecentUrlView);
      }			                          
	  _goBackFromRecentUrlView = function() {
          window.views.WebView.showNormalPage();
          window.ViewStack.switchView("WebView", "BookmarkHistoryView");
      }
  }
  function RecentURLClearallButton() {
      this.writeButton = function() {
          document.write('<img  class="webToolbarBtn"id="clearHistory">');
          new SimpleButton("clearHistory",
                           "toolbar.snippet/icons/icon_clearall.png",
                           "toolbar.snippet/icons/icon_clearall_pressed.png",
                           "toolbar.snippet/icons/icon_clearall_disabled.png",
                           _clearHistory);
	  }
      _clearHistory = function() {
    	  if(confirm("Are you sure you want to permanently delete your history?"))
          {
              window.bookmarksManager.clearHistory();
              window.views.WebView.reload();
          }
      }
  }

	

  /*
  ** Settings Toolbar Buttons
  */
  function SettingsBackButton() {
    this.writeButton = function() {
        document.write('<img  class="webToolbarBtn" id="goBacktoWebViewSettings">');
        new SimpleButton("goBacktoWebViewSettings",
                         "toolbar.snippet/icons/icon_back.png",
                         "toolbar.snippet/icons/icon_back_pressed.png",
                         "toolbar.snippet/icons/icon_back.png",
                         _goBackFromSettingsView);
    }			
    
      
                              
    _goBackFromSettingsView = function() {
        window.views.WebView.showNormalPage();
        window.ViewStack.switchView("WebView", "SettingsView");
        //window.snippets.UrlSearchChromeId.show(false);  
        
    }
  }
  function SettingsSaveButton() {
    this.writeButton = function() {
        document.write('<img  class="webToolbarBtn" id="SaveSettings">');
        new SimpleButton("SaveSettings",
                         "toolbar.snippet/icons/icon_clearall.png",
                         "toolbar.snippet/icons/icon_clearall_pressed.png",
                         "toolbar.snippet/icons/icon_clearall_disabled.png",
                         _SaveSet); 
    }
    _SaveSet = function() {
        window.views.WebView.showNormalPage();
        window.ViewStack.switchView("WebView", "SettingsView");	
        //window.snippets.UrlSearchChromeId.show(false);    
    }
  }

  /*
  ** Utility functions
  */
  function _chromeLoadComplete() {
    _setActions();
    winbutton.changeWindowIcon();
    window.pageController.pageChanged.connect(winbutton.changeWindowIcon);
  }

  function _chromeAspectChanged() {
    var lcbWidth = document.getElementById("LCToolbarChromeId").offsetWidth;
    var rcbWidth = document.getElementById("RCToolbarChromeId").offsetWidth;
    var totalW =   window.chrome.displaySize.width;

    var fcWidth = totalW - lcbWidth - rcbWidth;
    var fcWidthPercent = (fcWidth/totalW) * 100 ;
    fcWidthPercent = fcWidthPercent.toFixed(2);

    // Clear popups if aspect ratio changes
    window.snippets.ContextMenuId.hide();
    window.snippets.ZoomBarId.hide();
    
    el =  document.getElementById("ButtonContainer");
    el.style.width = fcWidthPercent +"%";

  }

  function _setActions() {
      new ActionButton("backButton",
                       "toolbar.snippet/icons/icon_back.png",
                       "toolbar.snippet/icons/icon_back_pressed.png",
                       "toolbar.snippet/icons/icon_back_disabled.png",
                       window.pageController.actions.back);
      
      /* Add 'addWindow' action */
      new ActionButton("addWindow",
                       "toolbar.snippet/icons/icon_windowsadd.png",
                       "toolbar.snippet/icons/icon_windowsadd_pressed.png",
                       "toolbar.snippet/icons/icon_windowsadd_disabled.png",
                       window.views.WindowView.actions.addWindow);
  }
 	
  function _pageLoadComplete(ok) {
      if(ok) {
          window.bookmarksManager.addHistory(window.pageController.currentDocUrl,window.pageController.currentDocTitle);
    }
  }

  function _pageLoadStarted() {
	  window.snippets.MostVisitedViewId.hide();	
      //snippets.ButtonContainer.show();
  }
  
  // Main toolbar button instances
  var lc = new LeftCornerButton();
  var rc = new RightCornerButton();
  var menubutton = new MenuButton();
  var zoombutton = new ZoomButton();
  var winbutton = new WinButton();
  
  // Windows View toolbar instances
  var winBackButton = new WinBackButton();
  var winAddButton = new WinAddButton();

  // Bookmarks View toolbar instances
  var bmBackButton = new BMBackButton();
  var bmAddButton = new BMAddButton();

  // Recent URL View toolbar instances
  var recentBackButton = new RecentURLBackButton();
  var recentClearallButton = new RecentURLClearallButton();

  // Settings View toolbar instances
  var settingsBackButton = new SettingsBackButton();
  var settingsSaveButton = new SettingsSaveButton();

  // Toolbar button write accessor methods
  this.writeLeftCornerButton = function() { lc.writeButton(); }
  this.writeRightCornerButton = function() { rc.writeButton(); }
  this.writeMenuButton = function() { menubutton.writeButton(); } 
  this.writeWinButton = function() { winbutton.writeButton(); }
  this.writeZoomButton = function() { zoombutton.writeButton(); }  
  this.writeWinBackButton = function() { winBackButton.writeButton(); }
  this.writeWinAddButton = function() { winAddButton.writeButton(); }
  this.writeBMBackButton = function() { bmBackButton.writeButton(); }
  this.writeBMAddButton = function() { bmAddButton.writeButton(); }
  this.writeRecentBackButton = function() { recentBackButton.writeButton(); }
  this.writeRecentClearallButton = function() { recentClearallButton.writeButton(); }
  this.writeSettingsBackButton = function() { settingsBackButton.writeButton(); }
  this.writeSettingsSaveButton = function() { settingsSaveButton.writeButton(); }

  //! Set enabled state of zoom button.
  /*!
    \param enabled new enabled state of button
  */
  this.setZoomEnabled = function(enabled)
  {
    zoombutton.setEnabled(enabled);
  }
 
  window.chrome.chromeComplete.connect(_chromeLoadComplete);
  window.chrome.prepareForGeometryChange.connect(_chromeAspectChanged);
  window.pageController.loadFinished.connect(_pageLoadComplete);
  window.pageController.loadStarted.connect(_pageLoadStarted);


  window.ViewStack.activateWebView.connect(_activateWebView);
  window.ViewStack.deActivateWebView.connect(_deActivateWebView);
  window.ViewStack.activateWindowView.connect(_activateWindowView);
  window.ViewStack.deActivateWindowView.connect(_deActivateWindowView);
  window.ViewStack.activateBookmark.connect(_activateBookmarkView);
  window.ViewStack.deActivateBookmark.connect(_deActivateBookmarkView);
  window.ViewStack.activateBookMarkHistory.connect(_activateRecentUrlView);
  window.ViewStack.deActivateBookMarkHistory.connect(_deActivateRecentUrlView);
  window.ViewStack.activateSettingsView.connect(_activateSettingsView);
  window.ViewStack.deActivateSettingsView.connect(_deActivateSettingsView);
    
  function _activateWebView () {
      window.snippets.WebViewToolbarId.show(false);
  }

  function _deActivateWebView () {
      window.snippets.WebViewToolbarId.hide(false);
  }

  function _activateWindowView () {
      window.snippets.WindowViewToolbarId.show();
  }

  function _deActivateWindowView () {
      window.snippets.WindowViewToolbarId.hide();
      /* Set the windows icon based on the number of windows */
      winbutton.changeWindowIcon();
  }
  
  function _activateSettingsView () {
      window.snippets.SettingsViewToolbarId.show();
  }

  function _deActivateSettingsView () {
      window.snippets.SettingsViewToolbarId.hide();
  }

  function _activateBookmarkView () {
      window.snippets.BookmarkViewToolbarId.show();
  }

  function _deActivateBookmarkView () {
      window.snippets.BookmarkViewToolbarId.hide();
  }

  function _activateRecentUrlView () {
      window.snippets.RecentUrlViewToolbarId.show();
  }

  function _deActivateRecentUrlView () {
      window.snippets.RecentUrlViewToolbarId.hide();
  }

} //end of class WebToolbar

