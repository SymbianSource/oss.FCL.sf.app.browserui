function valbutton(thisform) 
{

	var selectedOption = -1;
	for (i=thisform.blockpopupbutton.length-1; i > -1; i--) 
	{
		if (thisform.blockpopupbutton[i].checked)
	  {
	  	selectedOption = i; 
			i = -1;
		}
	}
	
	window.pageController.savePopupSettings(thisform.blockpopupbutton[selectedOption].value);
	alert("Data saved");
}
function initPopupForm()
{
	var index = popupState();
	blockpopup.blockpopupbutton[index].checked = true;
		
}

window.onload = initPopupForm;

function popupState()
{
	  if(window.pageController.getPopupSettings() == true)
				return 0;
		else
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
function clearSelected(field)
{
	if(field[0].checked == true) // Cache
  	  window.pageController.deleteCache();
	if(field[1].checked == true) // Cookies
	{
		  window.pageController.deleteCookies();
  }
	if(field[2].checked == true) // .dat files
	{ 
		window.pageController.deleteDataFiles();
  }

uncheckAll(field);
alert("Data cleared");
}