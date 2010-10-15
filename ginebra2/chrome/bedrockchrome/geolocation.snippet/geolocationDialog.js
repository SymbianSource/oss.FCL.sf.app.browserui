/*!
  \file geolocationDialog.js This module contains GeolocationDialog class which
  prompts user for accepting/rejection geolocation request.
*/
function GeolocationDialog() {
	  var _frame;
	  var _page;
	
    var onChromeComplete = function()
    {
        // Watch for geolocation permission request. ".bind(this)" is used to resolve amibiguous this pointer.
        window.pageController.requestGeolocationPermission.connect(
            function(frame, page, domain) {
       
        	      _frame = frame;
        	      _page = page;
        	      
        	      this.showDialog(domain);
        	      
            }.bind(this)
        );
    }
    
    //! Create the html/DOM elements for the Dialog.
    this.setupPage = function() {
        var html =
            '<div class="top"></div>' +
              '<div class="body">' +
                '<div class="geolocationTextLabel" id="geolocationDialogTitle">Location Permissions</div>' +
                '<div class="geolocationWarningLabel" id="geolocationDialogWarning"></div>' +
                '<div class="geolocationCheckboxTextLabel"><input type="checkbox" id="geolocationCheckboxId"/>' +
                '<label id="geolocationCheckboxTextLabelId" onmouseup="GeolocationDialog.toggleCheckbox();"> Save This Setting</label></div>' +
                '<div class="controls">' +
                  '<div type="button" onmouseup="GeolocationDialog.permissionGranted();" class="geolocationAcceptButton"></div>' + 
                  '<div type="button" onmouseup="GeolocationDialog.permissionDenied();" class="geolocationRejectButton"></div>' +
                '</div>' + /*controls*/
              '</div>' + /*body*/
            '<div class="bottom"></div>';
        document.write(html);
    }
    
    //! Show the Dialog to display the request domain and ask user for permission.
    this.showDialog = function(domain) {
    	  snippets.BookmarkViewToolbarId.enabled = false;
        snippets.WebViewToolbarId.enabled = false;
        
        // Translate these texts
        var dialogTitle = document.getElementById("geolocationDialogTitle");
        dialogTitle.firstChild.nodeValue = window.localeDelegate.translateText("txt_browser_location_dialog_location_permissions");
               
    	  var dialogWarning = document.getElementById("geolocationDialogWarning");
    	  dialogWarning.innerHTML = "Allow " + domain + " to Use Your Location?";
	      dialogWarning.firstChild.nodeValue = window.localeDelegate.translateText("txt_browser_location_dialog_allow");
	      
	      var checkboxTitle = document.getElementById("geolocationCheckboxTextLabelId");
        checkboxTitle.firstChild.nodeValue= window.localeDelegate.translateText("txt_browser_location_dialog_save_setting");
	      
        window.snippets.GeolocationDialogId.show(false);
    }
    
    //! Hide the Dialog.
    this.hideDialog = function() {
        window.snippets.GeolocationDialogId.hide();
        
        snippets.BookmarkViewToolbarId.enabled = true;
        snippets.WebViewToolbarId.enabled = true;
    }
    
    //! Allow the text label for the checkbox to toggle the checkbox
    GeolocationDialog.toggleCheckbox = function() {
        document.getElementById("geolocationCheckboxId").checked = !document.getElementById("geolocationCheckboxId").checked;
    }.bind(this)
    
    //! Grant the permission.
    GeolocationDialog.permissionGranted = function() {
	      window.pageController.setGeolocationPermission(_frame, _page, true,
	          document.getElementById("geolocationCheckboxId").checked);
	          
	      document.getElementById("geolocationCheckboxId").checked = false;
	      
	      this.hideDialog();
    }.bind(this)

    //! Deny the permission.
    GeolocationDialog.permissionDenied = function() {
	      window.pageController.setGeolocationPermission(_frame, _page, false,
	          document.getElementById("geolocationCheckboxId").checked);
	          
	      document.getElementById("geolocationCheckboxId").checked = false;
	      
	      this.hideDialog();
    }.bind(this)
    
    this.setupPage();
    
    chrome.chromeComplete.connect(onChromeComplete.bind(this));
}
