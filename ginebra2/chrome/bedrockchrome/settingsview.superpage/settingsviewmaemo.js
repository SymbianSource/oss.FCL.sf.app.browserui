var icon_src = "/settingsview/broom_sweep.png";
var icon_action_src = "/settingsview/broom_sweep.gif";
var selected_src = "/settingsview/yes_selected.png";
var deselected_src = "/settingsview/yes_deselected.png";

// sets localaized text constant
function localizeString(logicalString) {

    document.getElementById(logicalString).innerHTML = window.localeDelegate.translateText(logicalString);

}

// get settings Browser history value
function saverestoreState() {

    return window.pageController.getSaverestoreSettings();

}

// gets settings Character encoding value
function enciodingState() {

    return window.pageController.getTextEncoding();

}

// writes settings Character encoding value to pageController
function setValue(selectedValue) {

    window.pageController.setSettingsLoaded(1);    
    window.pageController.setTextEncoding(selectedValue);

}

// sets settings Character encoding value
function setEncodingState() {

    var encString = enciodingState();

    var list = document.getElementById("formbutton");
    if (list) {
    
		for (var i = 0; i < list.length; i++ ) {

		    if ( list.options[i].text == encString ) {

				list.options[i].selected = true;
				return i;

	    	}
		}
    }

    return 0;

}

// settings view form initialization
function initForms() {

    if ( saverestoreState() )
		$("#saverestore").attr("src", selected_src);
    else
		$("#saverestore").attr("src", deselected_src);
   
    setEncodingState();

    $("#id1").attr("src", icon_src);
    $("#id2").attr("src", icon_src);
    $("#id3").attr("src", icon_src);
    $("#id4").attr("src", icon_src);
    //$("#id5").attr("src", icon_src);
    $("#id6").attr("src", icon_src);
   
    localizeString("txt_browser_settings_general_settings");
    localizeString("txt_browser_settings_general_settings_character_encoding");
    localizeString("txt_browser_settings_general_settings_save_browser_history");
    localizeString("txt_browser_settings_clear_data");
    localizeString("txt_browser_settings_clear_data_cache");
    localizeString("txt_browser_settings_clear_data_cookies");
    localizeString("txt_browser_settings_clear_data_history");
    localizeString("txt_browser_settings_clear_data_bookmarks");
    //localizeString("txt_browser_settings_clear_data_form_data");
    localizeString("txt_browser_settings_clear_data_all");
    localizeString("txt_browser_settings_general_settings_about");
   	
}

window.onload = initForms;

// execute action according to setting value
function clearSelected(field) {

    window.pageController.setSettingsLoaded(2);      
            
    if (field == "cache") { // Cache
 
		window.pageController.deleteCache();
		$("#id1").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);
		return;

    } else if (field == "cookies") { // Cookies
    
		window.pageController.deleteCookies();
		$("#id2").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);	
		return;
  
    } else if (field == "history") { // History

		window.historyManager.clearHistory();
		$("#id3").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);  
		return;    				        			 

    } else if (field == "bookmarks") { // Bookmarks

		window.bookmarksController.clearAll();
		$("#id4").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);
		return;

    } else if (field == "all") { // everything

		$("#id6").attr("src", icon_action_src);
		setTimeout("initForms();deleteData()", 2000);  
		return;

    } else {

		return;

    }
    
}

// clear all data represented by settings
function deleteData() {

    window.pageController.deleteCache();
    window.pageController.deleteCookies();
    window.historyManager.clearHistory();
    window.bookmarksController.clearAll();
    window.pageController.deleteDataFiles();
}

// save Browser history
function saveSR(img_src) {

    var flag;
    if ( img_src == deselected_src ) {

		$("#saverestore").attr("src", selected_src);
		flag = 1;

    } else {

		$("#saverestore").attr("src", deselected_src);
		flag = 0;

    }
    window.pageController.saveSaverestoreSettings(flag);
    window.pageController.setSettingsLoaded(1);

}
