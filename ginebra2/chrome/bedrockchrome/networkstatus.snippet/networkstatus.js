/*!
  \file networkstatus.js This module contains the NetworkStatus class.
*/

function NetworkStatus()
{   
	   // attach internal funcs
    this.setup = setupPage;

    // do setup
    this.setup();

    function onChromeComplete() 
    {
        window.pageController.pageLoadFinished.connect(
        function(ok) {
        	processLoadStatus(ok); 
        	var networkTimeoutId; 
	        if (!ok && !window.pageController.loadCanceled ) {         	
        		showNetworkStatus();   
        		networkTimeoutId = setTimeout ( 'hideNetworkStatus()', 2000 ); 
 	        }  	        
        }                
        );
    } //End of onChromeComplete handler
           
   window.chrome.chromeComplete.connect(onChromeComplete);    
}

// "Private" methods
function processLoadStatus(ok) {
	  
    var htmlText = "Page load ok"; 
    if (!ok) 
        htmlText = window.localeDelegate.translateText("page_load_failed"); 
        var ele = document.getElementById("NetworkStatusTextId");
	if (ele) {
            ele.innerHTML = htmlText; 
	}  
	else {
	  	document.write(htmlText); 
	}
	  
}

// "Private" methods
function setupPage() {
//	  var image = 'networkstatus.snippet/icons/network_status_icon.png';
	    
    var html =
        '<div class="networkStatusBox">' +
              '<table><tr>' +
//                '<td class="networkStatusIcon">' +
//                    '<img src="' + image + '" alt="">' +
//                '</td>' +
                '<td class="networkStatusText" id="NetworkStatusTextId">' +
                    'Replace with localized message text' +
                '</td>' +
            '</tr></table>' +
        '</div>';
  document.write(html);
}

// "Private" method
function showNetworkStatus() {
    window.snippets.NetworkStatusChromeId.setPosition(10,80); 
 	  window.snippets.NetworkStatusChromeId.show(true);
 	  window.snippets.NetworkStatusChromeId.repaint();
} 	            

// "private" method 
function hideNetworkStatus() {
    window.snippets.NetworkStatusChromeId.setPosition(10,80); 
 	  window.snippets.NetworkStatusChromeId.hide();
 	  window.snippets.NetworkStatusChromeId.repaint(); 	        	
}
