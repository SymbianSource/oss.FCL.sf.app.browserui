var encodingArray = ['Automatic',         
            				'Big5',                
										'Big5-HKSCS',
										'CP949',
										'EUC-JP',
										'EUC-KR',
										'GB18030-0',
										'IBM-850',
										'IBM-866',
										'IBM-874',
										'ISO-2022-JP',
										'ISO-8859-1',
										'ISO-8859-2',
										'ISO-8859-3',
										'ISO-8859-4',
										'ISO-8859-5',
										'ISO-8859-6',
										'ISO-8859-7',
										'ISO-8859-8',
										'ISO-8859-9',
										'ISO-8859-10',
										'ISO-8859-13',
										'ISO-8859-14',
										'ISO-8859-15',
										'ISO-8859-16',
										'KOI8-R',
										'KOI8-U',
										'MuleLao-1',
										'ROMAN8',
										'Shift-JIS',
										'TIS-620',
										'TSCII',
										'UTF-8',
										'UTF-16',
										'UTF-16BE',
										'UTF-16LE',
										'UTF-32',
										'UTF-32BE',
										'UTF-32LE',
										'Windows-1250',
										'Windows-1251',
										'Windows-1252',
										'Windows-1253',
										'Windows-1254',
										'Windows-1255',
										'Windows-1256',
										'Windows-1257',
										'Windows-1258'];
										
var lastIndex = 47; // change it while adding new encoding strings										
										

function saveSR(value)
{
	  var sr1 = document.getElementById("saverestore1");
    var sr2 = document.getElementById("saverestore2");
    if(value == "yes")
    {
    	sr1.setAttribute("src", "icons/yes_selected.png");
    	sr2.setAttribute("src", "icons/no_deselected.png");
    	window.pageController.saveSaverestoreSettings(1);
    }
    else
    {
    	sr1.setAttribute("src", "icons/yes_deselected.png");
    	sr2.setAttribute("src", "icons/no_selected.png");
    	window.pageController.saveSaverestoreSettings(0);
    		
    }
    window.pageController.setSettingsLoaded(1);
    
    
    
}


function saveFP(value)
{
		var fp1 = document.getElementById("formpwd1");
    var fp2 = document.getElementById("formpwd2");
    if(value == "yes")
    {
    	fp1.setAttribute("src", "icons/yes_selected.png");
    	fp2.setAttribute("src", "icons/no_deselected.png");
    }
    else
    {
    	fp1.setAttribute("src", "icons/yes_deselected.png");
    	fp2.setAttribute("src", "icons/no_selected.png");
    		
    }
    window.pageController.setSettingsLoaded(1);
        
}


function initForms()
{
    //var index1 = popupState();
    //blockpopup.formbutton[index1].checked = true;
  
		var index1 = saverestoreState();
    
    var sr1 = document.getElementById("saverestore1");
    var sr2 = document.getElementById("saverestore2");
    if(index1 == 0)
    {
    	sr1.setAttribute("src", "icons/yes_selected.png");
    	sr2.setAttribute("src", "icons/no_deselected.png");
    }
    else
    {
    	sr1.setAttribute("src", "icons/yes_deselected.png");
    	sr2.setAttribute("src", "icons/no_selected.png");
    		
    }

    /*var index2 = saveFormpasswordState();
    
    var fp1 = document.getElementById("formpwd1");
    var fp2 = document.getElementById("formpwd2");
    if(index2 == 0)
    {
    	fp1.setAttribute("src", "icons/yes_selected.png");
    	fp2.setAttribute("src", "icons/no_deselected.png");
    }
    else
    {
    	fp1.setAttribute("src", "icons/yes_deselected.png");
    	fp2.setAttribute("src", "icons/no_selected.png");
    		
    }*/

    var index3;
    index3 = encodingState();    
    //this.options[this.selectedIndex].value
    encoding.formbutton[index3].selected = true;    
    
    
    var cs1 = document.getElementById("id1");
    var cs2 = document.getElementById("id2");
    var cs3 = document.getElementById("id3");
    var cs4 = document.getElementById("id4");
    var cs5 = document.getElementById("id5");
    var cs6 = document.getElementById("id6");
   
   	cs1.setAttribute("src", "icons/broom_sweep.png");
   	cs2.setAttribute("src", "icons/broom_sweep.png");
   	cs3.setAttribute("src", "icons/broom_sweep.png");
   	cs4.setAttribute("src", "icons/broom_sweep.png");
   	cs5.setAttribute("src", "icons/broom_sweep.png");
   	cs6.setAttribute("src", "icons/broom_sweep.png");  
   

    if(window.chrome.displayMode == "portrait")
      document.getElementById("cd2").width = "100%";
    else
    	document.getElementById("cd2").width = "100%";  //I need this line to handle some problems with landscape in the future. The value may be different  from 100%

}

window.onload = initForms;



function popupState()
{
      if (window.pageController.getPopupSettings() == true)
                return 0;
        else
                return 1;
}

function saverestoreState()
{
      if (window.pageController.getSaverestoreSettings() == true)
      {
          return 0;
        }
        else
        {
            return 1;
      }
}

function encodingState()
{
    var  encString;
    var index;
    encString = window.pageController.getTextEncoding();    
    index = findEncodingIndex(encString);
    return index;
}

function saveFormpasswordState()
{
    return 1;
}

function checkAll(field)
{
for (i = 0; i < field.length; i++)
field[i].checked = true ;
}
function uncheckAll(field)
{
for (i = 0; i < field.length; i++)
field[i].checked = false ;
}

function pausecomp(millis) 
{
	var date = new Date();
	var curDate = null;

	do { curDate = new Date(); } 
		while(curDate-date < millis);
} 

function clearSelected(field)
{
	  window.pageController.setSettingsLoaded(2);      
            
    if (field == "cache")
    { 
      window.pageController.deleteCache();
      var cs1 = document.getElementById("id1");
      cs1.setAttribute("src", "icons/broom_sweep.gif");
    }
      
    if (field == "cookies") // Cookies
    {
    	    var cs2 = document.getElementById("id2");
         	cs2.setAttribute("src", "icons/broom_sweep.gif");
      		window.pageController.deleteCookies();
      		//pausecomp(5000);     		
          
  	}

  if (field == "history") // History
  {

              window.bookmarksManager.clearHistory();
              var cs3 = document.getElementById("id3");
      				cs3.setAttribute("src", "icons/broom_sweep.gif");
      				
      				        			 
         			 
              //window.views.WebView.reload();

  }

  if (field == "bookmarks") // Bookmarks
  {

              window.bookmarksManager.clearBookmarks();
              var cs4 = document.getElementById("id4");
      				cs4.setAttribute("src", "icons/broom_sweep.gif");
              //window.views.WebView.reload();

   }

  if (field == "forms") // forms and passwords
  {

						 var cs5 = document.getElementById("id5");
      			 cs5.setAttribute("src", "icons/broom_sweep.gif");
             ;// place holder

   }

    if (field == "all") // everything
    {
    	window.pageController.deleteCache();
    	window.pageController.deleteCookies();
    	window.bookmarksManager.clearHistory();
    	window.bookmarksManager.clearBookmarks();
      window.pageController.deleteDataFiles();
      var cs6 = document.getElementById("id6");
      cs6.setAttribute("src", "icons/broom_sweep.gif");
    }

    setTimeout("initForms()", 2000);
}




function setValue(selectedIndex)
{
	//alert(this.options[this.selectedIndex].value);
	window.pageController.setSettingsLoaded(1);    
	window.pageController.setTextEncoding(encodingArray[selectedIndex]);
}

function findEncodingIndex(encodingString)
{
  for(var index = 0; index <= lastIndex; index++)
    if( encodingArray[index] == encodingString)
    	return index;
    	
  return (0);
}   	

