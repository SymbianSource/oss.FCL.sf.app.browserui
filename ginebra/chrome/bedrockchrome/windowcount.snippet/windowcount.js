/* This snippet is used to display the circles indicating the number of windows open.
 * Current window is indicated using filled in circle and others by an empty circle.
 * In portrait mode, the snippet is placed below the status bar while in landscape
 * mode, it is overlaid on the status bar
 * Note: When overlaying the snippet there were some issues repainting when the background was set to none
 * and hence a table with three cells are being used - the width of first and last cells match the
 * area we need to see from the status bar and their backgrounds are set to none. The middle cell is
 * used to display the circles and its bakground changes between black and white based 
 * on the display mode.
 */

//
// INIT the WindowCount snippet
//
function WindowCountBar(id)
{
   
    this.id = id;

    this.begin = 
        '<table id="wcrow" align=center   >'+
        '<tr>'+
        '<td id="wcfirst" style="background-color: transparent;">'+
        '</td>'+
        '<td  id="wccontent">';

    this.filledimg = 
        '<img class="wcicon" src="windowcount.snippet/icons/filledcircle.png" >' ;

    this.emptyimg = 
        '<img class="wcicon" src="windowcount.snippet/icons/emptycircle.png" >' ;

    this.end = 
        '</td>'+
        '<td id="wclast" style="background-color: transparent;">'+
        '</td>'+
        '</tr>'+
        '</table>' ;

    this.InitWCBar = function()  {
        this.wcUpdateWindowHtml();
        this.setId();
    }

    /* To identify the class. This class can also be identified through the
     * global variable wcbar which saves the instance when initialized
     * in chrome.html
     */
    this.setId = function () {
        el =  document.getElementById(this.id);
        if(el.wcChrome) {
            delete eldom.wcChrome;
        }
        el.wcChrome = this;

    }

    this.wcUpdateWindowHtml = function() {

        //window.chrome.alert("wcUpdateWindowHtml "+ window.pageController.pageCount());

        /* Generate html based on the number of windows open */
        switch(window.pageController.pageCount()) {
            case 1:
                this.wcOneWindow();
                break;
            case 2:
                this.wcTwoWindows();
                break;
            case 3:
                this.wcThreeWindows();
                break;
            case 4:
                this.wcFourWindows();
                break;
            case 5:
                this.wcFiveWindows();
                break;
            default :
                break;
        }
        this.setProps();

   }

    this.wcOneWindow = function() {

        var htmlText = '' +
                this.begin + this.filledimg + this.end;
 
        var el = document.getElementById("wcrow");
        if (el ) {
            el.innerHTML = htmlText; 
        }
        else {
            document.write(htmlText);
        }
    }

    this.wcTwoWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.end;

        this.setHtmlText(htmlText)


    }

    this.wcThreeWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.emptyimg+ this.end;
        this.setHtmlText(htmlText)
    }

    this.wcFourWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.emptyimg + this.emptyimg +  this.end;

        this.setHtmlText(htmlText)
    }

    this.wcFiveWindows = function() {
        var htmlText = '' +
                this.begin + this.emptyimg + this.emptyimg + this.emptyimg + this.emptyimg + this.emptyimg + this.end;
    
        this.setHtmlText(htmlText)
    }

    this.setHtmlText = function(htmlText) {

        var el = document.getElementById("wcrow");
        if (el ) {
            el.innerHTML = htmlText; 
        }
        else {
            document.write(htmlText);
        }
        this.setCurrentIndex();
    }

    this.setCurrentIndex  = function(){
        var el = document.getElementById("wcrow");
        Icons = el.getElementsByTagName("img");
 
        Icons[window.pageController.currentPageIndex].setAttribute('src', "windowcount.snippet/icons/filledcircle.png");
    }


    this.setSnippetPosition = function(mode) {
        if (window.snippets.WindowCountBarId ) {
            if (mode == "portrait") {
                window.snippets.WindowCountBarId.anchor = "AnchorTop";
                window.snippets.WindowCountBarId.anchorOffset = 27;
                document.getElementById("WindowCountBarId").style.backgroundColor='white';

            
            }
            else if (mode == "landscape") {
                window.snippets.WindowCountBarId.anchor = "AnchorNone";
                window.snippets.WindowCountBarId.setPosition(0, 0);
                document.getElementById("WindowCountBarId").style.backgroundColor='transparent';
            }

        }
    }


    /* Private method that sets the properties:
     * set the widht of first and last cells,
     * set the background of middle cell based on
     * display mode
     */
    this.setProps = function() {
        var leftW = document.getElementById("strength").offsetWidth +
                    document.getElementById("title").offsetWidth;

        var rightW = document.getElementById("clock").offsetWidth +
                    document.getElementById("battery").offsetWidth;
   
        if (window.snippets.WindowCountBarId ) {
            document.getElementById("wcfirst").width = leftW;
            document.getElementById("wclast").width = rightW;

            if (window.chrome.displayMode == "portrait" ) {
                document.getElementById("wccontent").style.backgroundColor='white';
            }
            else {
                // match status bar style (transparent doesn't repaint/refresh correctly)
                document.getElementById("wccontent").style.background='-webkit-gradient(linear, left top, left bottom,color-stop(0%,#111122),color-stop(100%,#111144))'; 
            }
        }

    }

    /* Initialize */
    this.InitWCBar();

    /* Slots */
    window.chrome.loadComplete.connect(
        function() {
            document.getElementById('WindowCountBarId').wcChrome.setSnippetPosition(window.chrome.displayMode);
            window.snippets.updateGeometry();
        }
    );

    /* Geometry is updated when there is a change in display mode. Here, we 
     * just need to set our attributes 
     */
    window.chrome.onDisplayModeChangeStart.connect(
        function(mode) {
            document.getElementById('WindowCountBarId').wcChrome.setSnippetPosition(mode);
        }
    );

    window.chrome.onDisplayModeChanged.connect(
        function(mode) {
            document.getElementById('WindowCountBarId').wcChrome.setProps();
        }
    );


}
