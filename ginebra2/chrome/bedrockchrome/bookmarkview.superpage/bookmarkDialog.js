var _dailogFlag = 0;
var _OriginalTitle = 0;

function bookmarkDialog()
{
   this.write = writeBookmarkDialog;

   // do setup
    this.write();
}

function writeBookmarkDialog()
{
    var html =
               '<div class="top"></div>'+
                '<div class="body">'+
                   '<div class="bookmarkTextLabel" id="bookmarkDialogTitle">Add Bookmark</div>'+
                   '<div class="GinebraSnippet" id="BookmarkDialogTitleId" data-GinebraNativeClass="TextEditSnippet" data-GinebraVisible="true"></div>'+ 
                   '<div class="GinebraSnippet" id="BookmarkDialogUrlId" data-GinebraNativeClass="TextEditSnippet" data-GinebraVisible="true"></div>'+
                   '<div class="controls">' +
                      '<div type="button"  onmouseup="bookmarkOperation();" class="bookmarkDoneButton"></div>'+     
                       '<div type="button" onmouseup="bookmarkDialogIdHide();" class="bookmarkCancelButton"></div>'+  
                    '</div>'+ /*controls*/
                '</div>'+  /*body*/
                '<div class="bottom"></div>';
  document.write(html);
}

function bookmarkDialogIdHide(){
    window.snippets.BookmarkDialogId.hide();
    snippets.BookmarkViewToolbarId.enabled = true;
    snippets.WebViewToolbarId.enabled = true;
}

function launchBookmarkDialog(bmtitle, bmurl, dialogFlag)
{
    try{
        snippets.BookmarkViewToolbarId.enabled = false;
        snippets.WebViewToolbarId.enabled = false;

        if (dialogFlag == 0) {
                var dlgTitle = document.getElementById("bookmarkDialogTitle");
                  dlgTitle.firstChild.nodeValue= window.localeDelegate.translateText("txt_browser_input_dial_add_bm");                 
           }
           else if (dialogFlag == 1) {
                var dlgTitle = document.getElementById("bookmarkDialogTitle");
                dlgTitle.firstChild.nodeValue= window.localeDelegate.translateText("txt_browser_input_dial_edit_bm");
           }

        _dailogFlag = dialogFlag;
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

        window.snippets.BookmarkDialogId.show(false);

        window.snippets.BookmarkDialogTitleId.selectAll();
        
       }catch(e){ alert(e); }

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
    var errCode;

    if (_dailogFlag == 0)
       errCode = window.bookmarksManager.addBookmark(bmtitle,bmurl);
    else if (_dailogFlag == 1)
       errCode = window.bookmarksManager.modifyBookmark(_OriginalTitle,bmtitle,bmurl);
    
    
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
    window.snippets.BookmarkDialogUrlId.unselect();
}

function urlFieldGainedFocus()
{
    window.snippets.BookmarkDialogTitleId.unselect();
}


function showBookmarkEditDialog(bmtitle,bmurl) {
    launchBookmarkDialog(bmtitle,bmurl,1);
}
