//clearhistorydialog.js

function clearHistoryDialog()
{
	window.bookmarksManager.confirmHistoryClear.connect(showClearHistoryDialog);
	  
    this.write = writeClearHistoryDialog;
    // do setup
    this.write();
    
}

function writeClearHistoryDialog()
{
	  var message = window.localeDelegate.translateText("txt_browser_history_delete_are_you_sure");
	  //Following string to be localized - BR-2979 
	  //var message = "Clear All History ?";
    var html =
        '<div class="top"></div>'+
        '<div class="body">'+
               '<div class="textLabel">'+message +'</div>' +
               '<div class="controls">' +
                 '<div type="button" onmouseup="clearAllHistoryDialog();" class="clearDoneButton"></div>'+ 
                 '<div type="button" onmouseup="clearHistoryDialogIdHide();" class="clearCancelButton"></div>'+         
                 '</div>'+ 
          '</div>' +  /*body*/
        '<div class="bottom"></div>';

  document.write(html);
}

function showClearHistoryDialog() {
  try{
	    window.snippets.ClearHistoryDialogId.show(false);
  }catch(e){ alert(e); }
} 

function clearAllHistoryDialog()
{
    window.bookmarksManager.clearHistory();
    window.views.WebView.reload();    
    window.snippets.ClearHistoryDialogId.hide();
}

function clearHistoryDialogIdHide(){
    window.snippets.ClearHistoryDialogId.hide();
}
