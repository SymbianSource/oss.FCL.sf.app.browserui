
//Bookmark add dialog

function addBookmarkDialog()
{
    this.write = writeAddBookmarkDialog;

    // do setup
    this.write();
}

// "Private" methods
function writeAddBookmarkDialog() 
{
    var html =
		'<div id="BookmarkDialogOverlay"></div>'+
		'<div id="BookmarkDialogContainer">'+
     	'<form name = "bookmarkAddDialog" id="bookmarkAddForm" class="bookmarkAddFormPortriat">'+
				'<table cellpadding="3" cellspacing="3" class="dialogBox"> '+
		 			' <tr><td colspan="2"> '+
      			' <input name = "TitleTextBox" type ="text" class="textInputUI" id = "titleTextBoxId" OnFocus="selectElementOnFocus(this);"  onblur="titleFieldLostFocus();" maxlength="256" />'+
      		'</td></tr>'+
      		'<tr><td colspan="2"> '+
      			'  <input name = "UrlTextBox" type ="text"  id = "urlTextBoxId" OnFocus="selectElementOnFocus(this);" onblur="urlFieldLostFocus();" class="textInputUI" maxlength="256" /> '+
		  		'</td></tr>'+
		  		'<tr>'+
			  		'<td> '+
			  			'<input name="Ok" type="button" value="Done" class="doneButton" onmouseup="addBookmark();"/> '+
			  		'</td>'+
			  		'<td>'+
			  			'<input name="Cancel" type="button" value="Cancel" class="cancelButton" onmouseup="bookmarkDialogIdHide();"/>'+
			  		'</td> '+
		  		'</tr> '+
	   		'</table>'+
  		'</form>'+
		'</div>';
  document.write(html);
}


function bookmarkDialogIdHide(){
	document.getElementById("BookmarkDialogOverlay").style.display = 'none';
	document.getElementById("BookmarkDialogContainer").style.display = 'none';
	document.getElementById("bookmarkAddForm").style.display = 'none';
	window.snippets.BookmarkDialogId.hide();
}


function launchBookmarkAddDialog()
{
			try{
			var bmtitle = document.bookmarkAddDialog.TitleTextBox.value = window.pageController.currentDocTitle;
			var bmurl = document.bookmarkAddDialog.UrlTextBox.value = window.pageController.currentDocUrl;
				
			if(bmtitle == "")
				document.bookmarkAddDialog.TitleTextBox.value = "Title";
			
			if(bmurl == "")
				document.bookmarkAddDialog.UrlTextBox.value = "Url";
			
			window.chrome.aspectChanged.connect(_updateBookmarkDialogGoemetry);
			
			document.getElementById("BookmarkDialogOverlay").style.display = 'block';
			document.getElementById("BookmarkDialogContainer").style.display = 'block';
			document.getElementById("bookmarkAddForm").style.display = 'block';
			
			var dialog = document.getElementById("bookmarkAddForm");;
			
			if (window.chrome.displayMode == "portrait")
			    dialog.className = "bookmarkAddFormPortriat"
			else //landscape
			    dialog.className = "bookmarkAddFormLandScape"
			
			window.snippets.BookmarkDialogId.repaint();
			window.snippets.BookmarkDialogId.show(false);
			window.snippets.BookmarkDialogId.zValue = 15;
		}catch(e){ alert(e); }
			
}

function addBookmark () 
{
		//get title and url from the dailog
		var bmtitle = document.bookmarkAddDialog.TitleTextBox.value;
		var bmurl= document.bookmarkAddDialog.UrlTextBox.value;
		

		//hide bookmraks dialog
		document.getElementById("BookmarkDialogOverlay").style.display = 'none';
		document.getElementById("BookmarkDialogContainer").style.display = 'none';
		document.getElementById("bookmarkAddForm").style.display = 'none';
		window.snippets.BookmarkDialogId.hide();
		
		//add a bookmark to the bookmark database   
		var errCode = window.bookmarksManager.addBookmark(bmurl,bmtitle);
		if(errCode == -2) {
			  alert("Bookmark Already Present");
		    return;
		}
		else if(errCode == -3){
			  alert("Bookmark Url Is Empty");
		    return;
		}
		else if(errCode != -0){
			  alert("General Error");
		    return;
		}			  
		
		try {
			window.views.WebView.reload();
		} catch(E) { alert(E); }
			  
}

function selectElementOnFocus(el)
{
	el.scrollRight = el.length+1;
	el.select();
}

function titleFieldLostFocus()
{
	var bmtitle = document.bookmarkAddDialog.TitleTextBox.value;
	if(bmtitle == "")
		document.bookmarkAddDialog.TitleTextBox.value = "Title";
}

function urlFieldLostFocus()
{
	var bmurl= document.bookmarkAddDialog.UrlTextBox.value;
	if(bmurl == "")
		document.bookmarkAddDialog.UrlTextBox.value = "Url";
}

function _updateBookmarkDialogGoemetry(displayMode)
{
			var dialog = document.getElementById("bookmarkAddForm");;
			
			if (window.chrome.displayMode == "portrait")
			    dialog.className = "bookmarkAddFormPortriat"
			else //landscape
			    dialog.className = "bookmarkAddFormLandScape"
}
