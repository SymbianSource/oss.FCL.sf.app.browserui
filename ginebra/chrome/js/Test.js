
// This file contains functions to be used for testing and logging.

var loadStartTime = new Date().getTime();
var loadCount = 0;

// Setup callback for loadStarted signal.
// Called when a new page has started loading.
//
window.pageController.loadStarted.connect(
    function() {
        loadStartTime = new Date().getTime();
        window.chrome.log("loadStarted:");
    }
);

// Setup callback for URL changed signal.
// Called when a new page has started loading.
//
window.pageController.currentPageUrlChanged.connect(
    function(url) {
        window.chrome.log("currentPageUrlChanged: " + url);
    }
);

// Setup callback for loadFinished signal.
// Called when the current page has finished loading.  
//   'ok' parameter is false if there was an error (not very reliable).
//
window.pageController.loadFinished.connect(
    function(ok) {
        window.chrome.log("loadFinished: " +
            "load count: " + loadCount + " " +
            "load time: " + (new Date().getTime() - loadStartTime) + "ms " +
            "ok=" + ok + " " + window.pageController.currentDocUrl);
        //window.pageController.currentLoad("http://www.google.com/search?q=help");
        
        loadCount++;
        
        // Load another document.  This will effectively keep loading the same document forever.
        //setTimeout("window.pageController.currentLoad('http://doc.trolltech.com/4.6/classes.html')",1000);
        //setTimeout("window.pageController.currentLoad('http://www.google.com/search?q=help')",1000);
    }
);

// Setup callback for loadProgress signal.
// Called periodically during the load process.  
//    'percent' parameter indicates how much of the document has been loaded (0-100).
//
//window.pageController.loadProgress.connect(
//    function(percent) {
//        window.chrome.log("loadProgress: percent=" + percent);
//        window.chrome.alert("loadProgress: percent=" + percent);
//    }
//);

// Setup callback for the chrome's loadComplete signal.
// Called when the chrome finishes loading.
//
window.chrome.loadComplete.connect(
    function() {
        window.chrome.log("chrome.loadComplete");
    }
);
           
  