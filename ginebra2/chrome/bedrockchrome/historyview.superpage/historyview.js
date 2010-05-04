var _prevExpandedUL = null;
var _prevExpandedATag;

function openHistoryElement(historyURL)
{
    window.views.WebView.showNormalPage();
	  window.ViewStack.switchView( "WebView","BookmarkHistoryView");
      	  
		// Laod a page to chrome view
		window.views.WebView.loadUrlToCurrentPage(historyURL);
	//	views.WebView.gesturesEnabled = true;

}

function updateHistoryList()
{
	var snippetId = document.getElementById('HistoryViewId'); 
	snippetId.innerHTML = "";
	
	try
		{
				//clearHistoryList();
				//Get historys data from database
				var folderNamesJSN = window.bookmarksManager.getHistoryFoldersJSON();
				var folderObjects = eval('(' + folderNamesJSN + ')');
				var mainUL = document.createElement('ul');
				
				mainUL.id =  'folderMenu';
				var subUlId;	
				for(i=0; i < folderObjects.length; i++)
				{
					  subUlId = "subUlId_"+i;
					  
						var mainLI  = document.createElement('li');
						var aTag  = document.createElement('a');
						var subUL = document.createElement('ul');

						// Expand First Item & Hide all the other Items
						if(i > 0)
						{	
								aTag.className = 'closed';
								subUL.style.display = 'none';
						}
						else
						{
							    aTag.className = 'opened';
								subUL.style.display = 'block';
								_prevExpandedATag = aTag;
								_prevExpandedUL = subUL;
						}

						aTag.href = "#";
						aTag.innerHTML = '<div></div>'+folderObjects[i];
						aTag.onclick = eval('( function(){ toggleHistoryFolder(this, "#'+subUlId+'"); } )');


						mainLI.appendChild(aTag);

						var folderDataJSN = window.bookmarksManager.getHistoryFolderJSON(folderObjects[i]);
						var folderDataObject = eval('(' + folderDataJSN + ')');
						
						subUL.id = subUlId;
						
						
						for(j=0; j<folderDataObject.length; j++)
						{
							var recenturl = folderDataObject[j].urlVal;
							
							var recenttitle = folderDataObject[j].titleVal;
							var recenttime = folderDataObject[j].dateVal+'<br/>'+folderDataObject[j].timeVal;
							
							recenturl = recenturl.replace('http://','');
							recenturl = recenturl.replace('www.','');
							recenturl = recenturl.replace('file:///','');
							
							//trim the bm title if the number of characters are more than 35.
							if(recenttitle.length > 30)
							{
								recenttitle = recenttitle.substring(0, 30);
								recenttitle += '...';
							}

							if(recenturl.length > 35)
							{
								recenturl = recenturl.substring(0, 35);
								recenturl += '...';
							}
							
							//<img src="'+bmfavicon+'">
								var subLI = document.createElement('li');
								subLI.innerHTML = '<a href="#" onclick="openHistoryElement(\''+folderDataObject[j].urlVal+'\');">'+
																	'<div class="HitsoryElement">'+
																	'<span class="aTime">'+recenttime+'</span>'+
																	'<span class="aTitle">'+recenttitle+'</span>'+
																	'<br/>'+
																	'<span class="aUrl">'+recenturl+'</span></div>';
								subUL.appendChild(subLI);
						}
						
						mainLI.appendChild(subUL);
						mainUL.appendChild(mainLI);
						
				}
				var snippetId = document.getElementById('HistoryViewId'); 
				snippetId.innerHTML = "";
				snippetId.appendChild(mainUL);

		} catch(E) { alert(E); }

}

function toggleHistoryFolder(aTag, subUlId){
	try{
	    aTag.className = (aTag.className == 'opened') ? 'closed' : 'opened';
      $(subUlId).toggle();
      //$(subUlId).toggle("blind", {}, 175);
      
      if(_prevExpandedUL != null && _prevExpandedUL != subUlId && _prevExpandedATag.className == 'opened'){
		    collapseFolder(_prevExpandedATag,_prevExpandedUL);
	    }
	    _prevExpandedUL   = subUlId;  
	    _prevExpandedATag = aTag;
  
	}  catch(E) { alert(E); }
}

function collapseFolder(aTag,subUlId)
{
try{
	    aTag.className = (aTag.className == 'opened') ? 'closed' : 'opened';
      //$(subUlId).toggle("blind", {}, 175);
      $(subUlId).toggle();
      
    }  catch(E) { alert(E); }
}
