var __prevOpenedBookmarkControl = null;
var __previousSortEle = null;
var __bookmarkCount= 0;
var __bookmarkEditElement= null;
var __dragendFlag = false;
// Set to the LongPress object that is currently waiting for activation,
// ie. it has gotten a mouse-down event and has its timer running.
var __currentLongPress;
var __cancelLinkOpening = false;

function _enableSorting(ele) {

    ele.className = 'sort';
		
		var evt = document.createEvent("MouseEvents");
		evt.initMouseEvent("mousedown", true, true, window,
		  0, 0, 0, gInitialX, gInitialY, false, false, false, false, 0, null);
		var canceled = !ele.dispatchEvent(evt);
    
  	views.WebView.gesturesEnabled = false;

    ele.childNodes[0].className = 'controlInSort';
    ele.childNodes[0].childNodes[0].className = 'collapsedSort';

  __sortingEnabled = true;
  __sortEleOffsetTop = ele.offsetTop;
  __previousSortEle = ele;
    
    ele.className = 'highlight';

  }

function _disableSorting(ele) {

    views.WebView.gesturesEnabled = true;
    
    ele.className = 'no-sort';
    ele.childNodes[0].className = 'controlIn';
    ele.childNodes[0].childNodes[0].className = 'collapsed';

    __sortingEnabled = false;
    __sortEleOffsetTop = 0;
    __previousSortEle = null;
}

function _dragInit()
{
    $("#bookmarkListTree").sortable({
        placeholder: 'ui-state-highlight',
        axis:'y',
      cancel : 'li.no-sort',
      scrollSensitivity : 40,
      scrollSpeed : 40,
        start: function(event, ui) {
            __dragendFlag = true;
            },
      update: function(event, ui) {
                var sortedItemId = ui.item[0].id;
                var _a = $('#bookmarkListTree').sortable('toArray');
                for (var i=0; i<_a.length; i++)
                {
                    if (sortedItemId == _a[i])
                    {
                        var li = document.getElementById(sortedItemId);
                        _disableSorting(li);

                        var div = li.childNodes[1];
                        window.bookmarksManager.reorderBokmarks(div.childNodes[0].firstChild.nodeValue,i);
                    }
                }
            }
    });

    $("#bookmarkListTree").sortable('enable');
    $("#bookmarkListTree").disableSelection();
}

function _longPress(ele)
{
  if (__sortingEnabled)
  {
            _disableSorting(__previousSortEle)
            return false;
  }

  if(__prevOpenedBookmarkControl)
  	_bookmarkHideControl(__prevOpenedBookmarkControl);
  	
    _enableSorting(ele);
}

function _longPressStarted(lp)
{
    // Remember the current LongPress object so we can cancel it if scrolling
    // starts.
    __currentLongPress = lp;
    __cancelLinkOpening = false;
}

function _handlePanStarted(type)
{
		__cancelLinkOpening = true;
		
    // Scrolling started, cancel the LongPress.
      if (__currentLongPress != undefined) {
        __currentLongPress.cancel();
        __currentLongPress = undefined;
    }
}


function _bookmarkHideControl(ele){
    ele.childNodes[0].className = 'controlIn';
    ele.childNodes[0].childNodes[0].className = 'collapsed';
    ele.childNodes[1].style.width = (parseInt(window.innerWidth)-70)+"px";
}

function _bookmarkToggleControls(ele){
    try {

            if (__sortingEnabled && ele.parentNode.parentNode!=__previousSortEle)
            {
                _disableSorting(__previousSortEle)
                return false;
            }
            else if (__sortingEnabled && (ele.parentNode.parentNode==__previousSortEle))
            {
                return false;
            }

            ele.parentNode.className = (ele.parentNode.className == 'controlIn') ? 'controlOut' : 'controlIn';
            if (ele.parentNode.className == 'controlIn') {
                ele.className = 'collapsed';
                ele.parentNode.parentNode.childNodes[1].style.width = (parseInt(window.innerWidth)-70)+"px";
            }
            else {
                ele.className = 'expanded';
                ele.parentNode.parentNode.childNodes[1].style.width = (parseInt(window.innerWidth)-220)+"px";
            }

        if (__prevOpenedBookmarkControl != null && __prevOpenedBookmarkControl != ele.parentNode.parentNode)
            _bookmarkHideControl(__prevOpenedBookmarkControl);

    }catch(e) { alert(e); }

    __prevOpenedBookmarkControl = ele.parentNode.parentNode;
}

function _addNewBookmark(bmtitle,bmurl)
{
	  if(__prevOpenedBookmarkControl)
  		_bookmarkHideControl(__prevOpenedBookmarkControl);

    var ul=document.getElementById('bookmarkListTree');
    for (x=0; x< ul.childNodes.length; x++)
        {
        	 var li_element= ul.childNodes[x];
        	 var bm_title= li_element.childNodes[1].childNodes[0].innerText; 
        	 
        	 if(bmtitle.toLowerCase() ==  bm_title.toLowerCase())
        	 {
        	 	ul.removeChild(li_element);
        	 	break;
        	 }
        	 
        }
    
    if(__previousSortEle    != null)
        _disableSorting(__previousSortEle)

    __bookmarkCount += 1;
    //create element and add it to bookmark view
    var ul=document.getElementById('bookmarkListTree');
    var dbgTitle = bmtitle.replace(/'/g, "&#39");
  dbgTitle = dbgTitle.replace(/"/g, "&#34");
    var li = _createBookmarkElement(dbgTitle,bmurl,__bookmarkCount);
		li.className = 'no-sort';
    ul.insertBefore(li, ul.childNodes[0]);
    ul.childNodes[0].focus();
		new LongPress(li.id, _longPress, _longPressStarted);

    $('html, body').animate({
                        scrollTop: 0}, 1000);
}

function _editBookmark(bmtitle,bmurl)
{
  if(__prevOpenedBookmarkControl)
  	_bookmarkHideControl(__prevOpenedBookmarkControl);

	var ul=document.getElementById('bookmarkListTree');
	for (x=0; x< ul.childNodes.length; x++)
        {
        	 var li_element= ul.childNodes[x];
        	 var bm_title= li_element.childNodes[1].childNodes[0].innerText; 
        	 
        	 if(bmtitle.toLowerCase() == bm_title.toLowerCase() &&
        	   __bookmarkEditElement.childNodes[1].childNodes[0].innerText.toLowerCase() != bmtitle.toLowerCase())
        	 {
        	 	ul.removeChild(li_element);
        	 	break;
        	 }
        	 
         }
    __bookmarkEditElement.childNodes[1].childNodes[0].innerText  = bmtitle;
    __bookmarkEditElement.childNodes[1].childNodes[2].innerText  = bmurl;
}

function _launchEditBookmark(r,bmtitle,bmurl)
{
    __bookmarkEditElement = r.parentNode.parentNode;
    window.bookmarksManager.launchEditBookmark(bmtitle,bmurl);
}

function _deleteBookmark(r,bmtitle)
{ 
  window.bookmarksManager.deleteBookmark(bmtitle);
    //ToDo : check for error code
    r.parentNode.parentNode.parentNode.removeChild(r.parentNode.parentNode);
  
}


function _openUrl(ele, newUrl) {
        // DragStart & DragEnd listeners are defined at bottom
        if (__sortingEnabled && (ele.parentNode!=__previousSortEle))
        {
            _disableSorting(__previousSortEle)
            return false;
        }
        else if (__sortingEnabled && (ele.parentNode==__previousSortEle))
            {
                return false;
            }
        else if (__dragendFlag)
        {
            __dragendFlag = false;
            return false;
        }
        else if(__cancelLinkOpening)
        {
        	__cancelLinkOpening = false;
        	return false;
        }

    window.views.WebView.showNormalPage();
    window.ViewStack.switchView( "WebView","BookmarkTreeView");

        // Laod a page to chrome view
        window.views.WebView.loadUrlToCurrentPage(newUrl);
        views.WebView.gesturesEnabled = true;
}

function _updateBookmarkViewGoemetry(displayMode)
{
    try{
        var _list = document.getElementsByClassName('Title');
        for (var i=0; i<_list.length; i++){
             if (_list[i].parentNode.childNodes[0].className == 'controlIn'){
                _list[i].style.width = (parseInt(window.innerWidth)-70)+"px";
            }
             else{
                    _list[i].style.width = (parseInt(window.innerWidth)-220)+"px";
             }
        }
    }catch(e){ alert(e); }

}

function _createBookmarkElement(bmtitle,bmfullurl,idValue)
{
        var _width = parseInt(window.innerWidth)-70;
        var escapedTitle = bmtitle.replace(/&#39/g, "\\'"); 
        var li=document.createElement('li');
        li.id = idValue;
        li.innerHTML =
        '<div class="controlIn">'+
          '<div class="collapsed" onClick="javascript:_bookmarkToggleControls(this);"></div>'+
            '<img src="icons/edit_btn.png" width="56" height="56" vspace="7" hspace="5" '+
              'onclick="_launchEditBookmark(this,this.parentNode.parentNode.childNodes[1].childNodes[0].innerText,'+
                                                                     'this.parentNode.parentNode.childNodes[1].childNodes[2].innerText)">'+
            '<img src="icons/delete_btn.png" width="56" height="56" vspace="7" hspace="5" '+
               'onclick="_deleteBookmark(this, \''+escapedTitle+'\')">'+
        '</div>'+
        '<div class="Title" style="width:'+_width+'px;" onclick="_openUrl(this,this.childNodes[2].innerText);">'+
            '<span class="aTitle">'+bmtitle+'</span>'+
            '<br/>'+
            '<span class="aUrl">'+bmfullurl+'</span>'+
        '</div>';

        return li;
}

function _createBookmarkView()
{
        try
        {
            __currentLongPress = undefined;
            //Get bookmarks data from database
            var bookmakrData = window.bookmarksManager.getBookmarksJSON();
            var myObject = eval('(' + bookmakrData + ')');
            var ul=document.getElementById('bookmarkListTree');
            for (x=0; x<myObject.length; x++)
            {
                var bmurl = myObject[x].urlvalue;
                var bmtitle = myObject[x].title;

                //create element and add it to bookmark view
                var li = _createBookmarkElement(bmtitle,bmurl, x);
                li.className = 'no-sort';
                ul.appendChild(li);
                ul.childNodes[0].focus();

                new LongPress(li.id, _longPress, _longPressStarted);
                __bookmarkCount = x;
            }
        } catch(E) { alert(E); }

        _dragInit();
}

function launchBookmarkView()
{
    try{
        window.chrome.aspectChanged.connect(_updateBookmarkViewGoemetry);
        window.bookmarksManager.bookmarkEntryAdded.connect(_addNewBookmark);
        window.bookmarksManager.bookmarkEntryModified.connect(_editBookmark);

        // Get Bookmarks from the database
        _createBookmarkView();

        // Watch for the start of scrolling in the view so we can cancel the
        // long presses to prevent them from interfere with scrolling.  Otherwise
        // the long press callback will fire and trigger sortable mode.
        views.WebView.startingPanGesture.connect(_handlePanStarted);

    }catch(e){ alert(e); }
}