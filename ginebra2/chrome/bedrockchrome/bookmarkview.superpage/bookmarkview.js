var _prevOpenedBookmarkControl = null;
var previousSortEle = null;

function BookmarkToggleControls(ele){
	try {
		
			if(_sortingEnabled && ele.parentNode.parentNode!=previousSortEle) 
			{
				_sortingEnabled = false;
				previousSortEle.className = 'li.no-sort';
				views.WebView.gesturesEnabled = true;
				return false;
			}		
			else if(_sortingEnabled && (ele.parentNode.parentNode==previousSortEle))
			{
				return false;
			}
			
			ele.parentNode.className = (ele.parentNode.className == 'controlIn') ? 'controlOut' : 'controlIn';
			if(ele.parentNode.className == 'controlIn') {
				ele.className = 'collapsed';
				ele.parentNode.childNodes[1].className = "collapsedIcons";
				ele.parentNode.parentNode.childNodes[1].style.width = (parseInt(window.innerWidth)-115)+"px";
			}
	 		else {		
				ele.className = 'expanded';
				ele.parentNode.childNodes[1].className = "expandedIcons";
				ele.parentNode.parentNode.childNodes[1].style.width = (parseInt(window.innerWidth)-195)+"px";
			}

		if(_prevOpenedBookmarkControl != null && _prevOpenedBookmarkControl != ele)
			BookmarkHideControl(_prevOpenedBookmarkControl);
			
	}catch(e) { alert(e); }
	_prevOpenedBookmarkControl = ele;
}


function BookmarkHideControl(ele){
	ele.parentNode.className = 'controlIn';
 	ele.className = 'collapsed';
}


function deleteBookmark(r,bmtitle)
{
	window.bookmarksManager.deleteBookmark(bmtitle);
	//ToDo : check for error code
	r.parentNode.parentNode.parentNode.removeChild(r.parentNode.parentNode);
}


function openUrl(ele, newUrl) {
		// DragStart & DragEnd listeners are defined at bottom
		if(_sortingEnabled && (ele.parentNode!=previousSortEle)) 
		{
			_sortingEnabled = false;
			previousSortEle.className = 'li.no-sort';
			views.WebView.gesturesEnabled = true;
			return false;
		}
		else if(_sortingEnabled && (ele.parentNode==previousSortEle))
			{
				return false;
			}
		else if(__dragendFlag) 
		{
			__dragendFlag = false;
			return false;
		}		

    window.views.WebView.showNormalPage();
    window.ViewStack.switchView( "WebView","BookmarkTreeView");
      	  
		// Laod a page to chrome view
		window.views.WebView.loadUrlToCurrentPage(newUrl);
		views.WebView.gesturesEnabled = true;
}


//'<div class="Title"  onclick="openUrl(\''+bmurl+'\')">'+
//'<div class="collapsedIcons"><img src="icons/edit-icon.gif" width="56" height="56" class="controlIcon">'+
//background-image: url(\''+bmfavicon+'\');

function createBookmarkElement(bmtitle,bmfullurl,bmtrimmedurl, idValue)
{
		var _width = parseInt(window.innerWidth)-115;
		
		var li=document.createElement('li');
		li.id = idValue;
		li.innerHTML = 	  
		'<div class="controlIn">'+
		  '<div class="collapsed" onClick="javascript:BookmarkToggleControls(this);"></div>'+
			'<img class="controlIcon" src="icons/delete-icon.gif" width="56" height="56" vspace="7" hspace="5"  onclick="deleteBookmark(this, \''+bmtitle+'\')">'+
		'</div>'+
		'<div class="Title" style="width:'+_width+'px;" onclick="openUrl(this,\''+bmfullurl+'\')">'+
			'<span class="aTitle">'+bmtitle+'</span>'+
			'<br/>'+
			'<span class="aUrl">'+bmtrimmedurl+'</span>'+
		'</div>';
		
		return li;
}

function clearBookmarkList()
{
	var list = document.getElementById('bookmarkListTree'); 
	while( list.hasChildNodes() ) { 
		list.removeChild( list.lastChild ); 
	} 
}

var __dragendFlag = false;

function longtapbm(ele)
{ 
  if(_sortingEnabled)
  {
			_sortingEnabled = false;
			previousSortEle.className = 'li.no-sort';
			views.WebView.gesturesEnabled = true;
			return false;
  }
  	
  _sortingEnabled = true;
  _sortEleOffsetTop = ele.offsetTop;
  views.WebView.gesturesEnabled = false;
	ele.className = 'sort';
	previousSortEle = ele;
	$("#bookmarkListTree").sortable('enable');

}

// Set to the LongPress object that is currently waiting for activation,
// ie. it has gotten a mouse-down event and has its timer running.
var __currentLongPress;

function longPressStarted(lp)
{
    // Remember the current LongPress object so we can cancel it if scrolling
    // starts.
    __currentLongPress = lp;
}

function handlePanStarted(type) 
{
    // Scrolling started, cancel the LongPress.
    if(__currentLongPress != undefined) {
        __currentLongPress.cancel();
        __currentLongPress = undefined;
    }
}

function createBookmarkView()
{
		try
		{
			__currentLongPress = undefined;
			clearBookmarkList();
			//Get bookmarks data from database
			var bookmakrData = window.bookmarksManager.getBookmarksJSON();
			var myObject = eval('(' + bookmakrData + ')');
			var ul=document.getElementById('bookmarkListTree');
			for(x=0; x<myObject.length; x++)
			{
				var bmurl = myObject[x].urlvalue;
				var bmtitle = myObject[x].title;
				var bmtrimmedurl = bmurl.replace('http://','');
				bmtrimmedurl = bmtrimmedurl.replace('www.','');
				bmtrimmedurl = bmtrimmedurl.replace('file:///','');
				
				//create element and add it to bookmark view   
				var li = createBookmarkElement(bmtitle,bmurl,bmtrimmedurl, x);
				li.className = 'no-sort';
				ul.appendChild(li);
				ul.childNodes[0].focus();
				
				new LongPress(li.id, longtapbm, longPressStarted);
			}
		} catch(E) { alert(E); }
		dragInit();
}

function _updateBookmarkViewGoemetry(displayMode)
{
	try{
		var _list = document.getElementsByClassName('Title');
		for(var i=0; i<_list.length; i++){
			 if(_list[i].parentNode.childNodes[0].className == 'controlIn'){
			 	_list[i].style.width = (parseInt(window.innerWidth)-115)+"px";
			}
			 else{		
			 		_list[i].style.width = (parseInt(window.innerWidth)-195)+"px";
			 }
		}
	}catch(e){ alert(e); }

}

function launchBookmarkView()
{
	try{
        window.chrome.aspectChanged.connect(_updateBookmarkViewGoemetry);
        		
        // Get Bookmarks from the database		
        createBookmarkView();

        // Watch for the start of scrolling in the view so we can cancel the 
        // long presses to prevent them from interfere with scrolling.  Otherwise
        // the long press callback will fire and trigger sortable mode.        
        views.WebView.startingPanGesture.connect(handlePanStarted);
        
    }catch(e){ alert(e); }	
}

function dragInit()
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
			 _sortingEnabled = false;	
	     views.WebView.gesturesEnabled = true;
			 $("#bookmarkListTree").sortable('disable');
	
				var sortedItemId = ui.item[0].id;
				var _a = $('#bookmarkListTree').sortable('toArray');
				for(var i=0; i<_a.length; i++)
				{
					if(sortedItemId == _a[i])
					{
						var li = document.getElementById(sortedItemId);
						li.className = 'no-sort';
						var div = li.childNodes[1];
						window.bookmarksManager.reorderBokmarks(div.childNodes[0].firstChild.nodeValue,i);
					}
				}
			}
	});
	
	$("#bookmarkListTree").sortable('disable');
	$("#bookmarkListTree").disableSelection();
		
}
