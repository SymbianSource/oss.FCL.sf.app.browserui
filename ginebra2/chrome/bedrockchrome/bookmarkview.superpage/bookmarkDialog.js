var _dialogFlag = 0;
var _OriginalTitle = 0;
var selected_src = "/bookmarkview/yes_selected.png";
var deselected_src = "/bookmarkview/yes_deselected.png";

document.getElementsByClassName = function(class_name) {
    var docList = this.all || this.getElementsByTagName('*');
    var matchArray = new Array();

    /*Create a regular expression object for class*/
    var re = new RegExp("(?:^|\\s)"+class_name+"(?:\\s|$)");
    for (var i = 0; i < docList.length; i++) {
        if (re.test(docList[i].className) ) {
            matchArray[matchArray.length] = docList[i];
        }
    }

	return matchArray;
}

function bookmarkDialog()
{
    this.write = writeBookmarkDialog;

    // do setup
    this.write();

    if (app.serviceFramework() == "mobility_service_framework") {
        $("bookmarkCheckboxTextLabelContainer").css("display", "inline");
    }

    chrome.chromeComplete.connect(createDelegate(this,
        function() {
            var snippet = window.snippets["BookmarkDialogId"];
            chrome.aspectChanged.connect(createDelegate(this,
                    function(a) {
                        centerBookmarkDialog();
                    })
            );
        })
    );
}

function writeBookmarkDialog()
{
    var html =
               '<div class="top"></div>'+
                '<div class="body">'+
                   '<div class="bookmarkTextLabel" id="bookmarkDialogTitle">Add Bookmark</div>'+
                   '<div class="GinebraSnippet" id="BookmarkDialogTitleId" data-GinebraNativeClass="TextEditSnippet" data-GinebraVisible="true"></div>'+ 
                   '<div class="GinebraSnippet" id="BookmarkDialogUrlId" data-GinebraNativeClass="TextEditSnippet" data-GinebraVisible="true"></div>'+
                   '<div class="bookmarkCheckboxTextLabelContainer">' +
                   '	<input style="vertical-align:middle;" type="image" src="' + deselected_src + '" id="bookmarkCheckboxId" />' +
                   '	<label id="bookmarkCheckboxTextLabel">Add shortcut to home screen</label>' +
                   '</div>'+
                   '<div>' + 
                   '<input type="hidden" id="BookmarkDialogBookmarkId" name="BookmarkDialogBookmarkId" value=""/></div>'+
                   '<div class="controls">' +
                   '	<div type="button"  onmouseup="bookmarkOperation();" class="bookmarkDoneButton"></div>'+     
                   '	<div type="button" onmouseup="bookmarkDialogIdHide();" class="bookmarkCancelButton"></div>'+  
                   '</div>'+ /*controls*/
                '</div>'+  /*body*/
                '<div class="bottom"></div>';
    document.write(html);
    $('#bookmarkCheckboxId').click(function() {
    	toggleCheckbox($(this));
    });
   
    $('#bookmarkCheckboxTextLabel').click(function() {
    	toggleCheckbox($('#bookmarkCheckboxId'));
    });
}

function toggleCheckbox(elem) {
    img_src = elem.getAttribute('src');
    if ( img_src == deselected_src ) {

        elem.src = selected_src;

    } else {

        elem.src = deselected_src;

    }
}

function bookmarkDialogIdHide(){
    window.snippets.BookmarkDialogId.hide();
    snippets.BookmarkViewToolbarId.enabled = true;
    snippets.WebViewToolbarId.enabled = true;
    
    if (app.serviceFramework() == "mobility_service_framework") 
    {
        elem = $("#bookmarkCheckboxId");
        if (elem)
        {
            if ( elem.src == selected_src )
            {
                elem.src = deselected_src;
            }
        }
    }    
}

function launchBookmarkDialog(bmtitle, bmurl, bmid, dialogFlag)
{
    try{
        snippets.BookmarkViewToolbarId.enabled = false;
        snippets.BookmarkDialogId.zValue = 100;
        snippets.WebViewToolbarId.enabled = false;

        if (dialogFlag == 0) {
            if (app.serviceFramework() == "mobility_service_framework") {
                $(".bookmarkCheckboxTextLabelContainer").css("display", "inline"); 
            }
            var dlgTitle = $("#bookmarkDialogTitle");
            dlgTitle.text(window.localeDelegate.translateText("txt_browser_input_dial_add_bm"));
            var chkboxTitle = $("#bookmarkCheckboxTextLabel");
            chkboxTitle.val(window.localeDelegate.translateText("txt_browser_bookmarks_also_add_to_home_screen"));
        }
        else if (dialogFlag == 1) {
             $("bookmarkCheckboxTextLabelContainer").css("display", "none");
             var dlgTitle = $("#bookmarkDialogTitle");
             dlgTitle.text(window.localeDelegate.translateText("txt_browser_input_dial_edit_bm"));
        }
       
        _dialogFlag = dialogFlag;
        _OriginalTitle = bmtitle;
        window.snippets.BookmarkDialogTitleId.lostFocus.connect(titleFieldLostFocus);
        window.snippets.BookmarkDialogUrlId.lostFocus.connect(urlFieldLostFocus);
        window.snippets.BookmarkDialogTitleId.gainedFocus.connect(titleFieldGainedFocus);
        window.snippets.BookmarkDialogUrlId.gainedFocus.connect(urlFieldGainedFocus);
        // set max text length
        // window.snippets.BookmarkDialogTitleId.setMaxTextLength(30);
        var hints = window.snippets.BookmarkDialogUrlId.getTextOptions();
        hints |= 2; // Qt::ImhNoAutoUppercase 0x2
        window.snippets.BookmarkDialogUrlId.setTextOptions(hints);
        if (bmtitle == "")
            window.snippets.BookmarkDialogTitleId.text = "Title";
        else
            window.snippets.BookmarkDialogTitleId.text = bmtitle;    
  
        if (bmurl == "")
            window.snippets.BookmarkDialogUrlId.text = "Url";
        else
           window.snippets.BookmarkDialogUrlId.text = bmurl;
        document.getElementById('BookmarkDialogBookmarkId').value = bmid;

        centerBookmarkDialog();

        window.snippets.BookmarkDialogId.show(false);
        
       }catch(e){ alert(e); }

}

function centerBookmarkDialog() {
        
        var statusBarHeight = window.snippets.StatusBarChromeId.visible ? window.snippets.StatusBarChromeId.geometry.height : 0;
        
        var snippet = window.snippets["BookmarkDialogId"];
        var x = (window.chrome.displaySize.width - snippet.geometry.width) / 2;
        
        // Center the menu in the space between status bar and tool bar
        var y = (window.chrome.displaySize.height - statusBarHeight - window.snippets.WebViewToolbarId.geometry.height - document.getElementById("BookmarkDialogId").offsetHeight)/2;
        snippet.setPosition(x, (y+statusBarHeight));
}

function bookmarkOperation()
{
    snippets.BookmarkViewToolbarId.enabled = true;
    snippets.WebViewToolbarId.enabled = true;
    //get title and url from the dialog
    var bmtitle = window.snippets.BookmarkDialogTitleId.text;
    var bmurl = window.snippets.BookmarkDialogUrlId.text;
    //Hide the dialog
    window.snippets.BookmarkDialogId.hide();
    //Update the database
    var errCode = 0;

    if (_dialogFlag == 0) {
        var bmid = window.bookmarksController.addBookmark(bmtitle,bmurl);
        if (bmid < 0) {
            alert("Unknown error adding bookmark");
            return;
        }
        if (app.serviceFramework() == "mobility_service_framework") 
        {
            elem = document.getElementById("bookmarkCheckboxId");
            if (elem)
            {
                if ( elem.src == selected_src )
                {
                    errCode = window.hsBookmarkPublishClient.addWidget(bmtitle, bmurl);
                    elem.src = deselected_src;
                }
            }
        } 
    }
    else if (_dialogFlag == 1) {
        var bmid = document.getElementById('BookmarkDialogBookmarkId').value;
        errCode = window.bookmarksController.modifyBookmark(bmid,bmtitle,bmurl);
    }
    
    if (errCode == -3){
	     alert("Bookmark Url Is Empty");
	     return;
    }
    else if (errCode != -0){
	       alert("General Error");
	       return;
    }
     
	
}

function titleFieldLostFocus()
{
    var bmtitle = window.snippets.BookmarkDialogTitleId.text;
    if (bmtitle == "")
       window.snippets.BookmarkDialogTitleId.text = "Title";
}

function urlFieldLostFocus()
{
    var bmurl= window.snippets.BookmarkDialogUrlId.text;
    if (bmurl == "")
       window.snippets.BookmarkDialogUrlId.text = "Url";
}


function titleFieldGainedFocus()
{
    //window.snippets.BookmarkDialogUrlId.unselect();
}

function urlFieldGainedFocus()
{
    //window.snippets.BookmarkDialogTitleId.unselect();
}


function showBookmarkEditDialog(bmtitle,bmurl,id) {
    launchBookmarkDialog(bmtitle,bmurl,id,1);
}
