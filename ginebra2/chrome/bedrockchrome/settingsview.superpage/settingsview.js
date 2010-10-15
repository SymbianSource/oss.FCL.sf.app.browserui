// header arrow icons
var header_closed_src = "/settingsview/list_headerarrow_closed.png";
var header_open_src = "/settingsview/list_headerarrow_open.png";

// menu section backgrounds
var bg_closed_src = "/settingsview/list_headerbg_closed.png";
var bg_open_src = "/settingsview/list_headerbg_open.png";

// menu section item background
var bg_item_src = "/settingsview/list_itembg.png";
var bg_item_pressed_src = "/settingsview/list_itembg_pressed.png";

var icon_src = "/settingsview/broom_sweep.png";
var icon_action_src = "/settingsview/broom_sweep.gif";

var selected_src = "/settingsview/yes_selected.png";
var deselected_src = "/settingsview/yes_deselected.png";

var radio_on = "/settingsview/radio_on.png";
var radio_off = "/settingsview/radio_off.png";

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

var encodingOptionsInitialized = false;

// sets localized text constant
function localizeString(logicalString) {
	$("#"+logicalString).html(window.localeDelegate.translateText(logicalString));
}

// get settings Browser history value
function saverestoreState() {
	return window.pageController.getSaverestoreSettings();
}

// settings view form initialization
function initForms() {

	$("span#character_encoding_value").text(encodingState());
	
	if (saverestoreState())
		$("#saverestore").attr("src", selected_src);
	else
		$("#saverestore").attr("src", deselected_src);

	$("#id1").attr("src", icon_src).click(function(){clearSelected("cache");});
	$("#id2").attr("src", icon_src).click(function(){clearSelected("cookies");});
	$("#id3").attr("src", icon_src).click(function(){clearSelected("history");});
	$("#id4").attr("src", icon_src).click(function(){clearSelected("bookamrks");});
	$("#id5").attr("src", icon_src).click(function(){clearSelected("geolocation");});
	$("#id6").attr("src", icon_src).click(function(){clearSelected("all");});

	$(".encoding_options").hide();
		
	localizeString("txt_browser_settings_general_settings");
	localizeString("txt_browser_settings_general_settings_character_encoding");
	localizeString("txt_browser_settings_general_settings_save_browser_history");
	localizeString("txt_browser_settings_clear_data");
	localizeString("txt_browser_settings_clear_data_cache");
	localizeString("txt_browser_settings_clear_data_cookies");
	localizeString("txt_browser_settings_clear_data_history");
	localizeString("txt_browser_settings_clear_data_bookmarks");
	localizeString("txt_browser_settings_clear_data_location");
	// localizeString("txt_browser_settings_clear_data_form_data");
	localizeString("txt_browser_settings_clear_data_all");
	localizeString("txt_browser_settings_general_settings_about");

}

// execute action according to setting value
function clearSelected(field) {

	window.pageController.setSettingsLoaded(2);

	if (field == "cache") { // Cache

		$("#id1").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);
		window.pageController.deleteCache();		
		return;

	} else if (field == "cookies") { // Cookies

		$("#id2").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);
		window.pageController.deleteCookies();		
		return;
		
	} else if (field == "history") { // History
		
		$("#id3").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);  
		window.historyManager.clearHistory();		
		return;    				        			 

	} else if (field == "bookmarks") { // Bookmarks

		$("#id4").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);
		window.bookmarksController.clearAll();		
		return;

	} else if (field == "geolocation") { // GeoLocation

		$("#id5").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);
		window.geolocationManager.clearAllGeodata();
		return;

	} else if (field == "all") { // everything

		$("#id6").attr("src", icon_action_src);
		setTimeout("initForms()", 2000);  
		deleteData();
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
	window.geolocationManager.clearAllGeodata();
	window.pageController.deleteDataFiles();
}

// save Browser history
function saveSR(img_src) {

	var flag;
	if (img_src == deselected_src) {

		$("#saverestore").attr("src", selected_src);
		flag = 1;

	} else {

		$("#saverestore").attr("src", deselected_src);
		flag = 0;

	}
	window.pageController.saveSaverestoreSettings(flag);
	window.pageController.setSettingsLoaded(1);
}

//gets settings Character encoding value
function encodingState() {
	return window.pageController.getTextEncoding() || "Automatic"; 
}

function setEncodingState(value) {
	window.pageController.setTextEncoding(value);
	app.debug("Encoding State set to " + value);
}

function setupEncodingOptions() {
	var encodingOptions = $(".encoding_options");
	var prevOption = null;
	var option = null;
	var encState = encodingState();
	for(index in encodingArray) {
		option = $("<span class='encoding_option'>" +
				"	<label>" + encodingArray[index]  + "</label>" +
				"	<img class='contentCharacterEncodingItem' id='" + encodingArray[index]  + "' alt='' />" +
				"</span><br />");
		var img = option.find("img"); 
		if(img.attr('id') == encState) {
			img.attr("src", "url(" + radio_on + ")");
		}
		else { 
			img.attr("src", "url(" + radio_off + ")");
		}
    	
		if(prevOption)
			option.insertAfter(prevOption);
		else
			encodingOptions.html(option);

		prevOption = option;
	}
	
	$("span.encoding_option").click(function() {
		$('.encoding_options').find("#" + encodingState()).attr("src", "url(" + radio_off + ")");
		
		$(this).find('img').attr("src", "url(" + radio_on + ")");
		app.debug("option:" + $(this).find('img').attr('id') + " selected.");
		setEncodingState($(this).find('img').attr('id'));
		$('.encoding_options').slideUp();
		$('span#character_encoding_value').text(encodingState());
	});	
	
	encodingOptionsInitialized = true;
	app.debug("Encoding Options Initialized");
}

$(document).ready(function() {
	
	initForms();
	
	// HIDE THE DIVS ON PAGE LOAD
	var settingsLoaded = window.pageController.getSettingsLoaded();

	$('.encoding_options').hide();
	$('#selected_encoding_option').click(function() {
		if(!encodingOptionsInitialized) 
			setupEncodingOptions();
 		$('.encoding_options').slideToggle();
	});

	$('#saverestore').click(function() {
		saveSR( $(this).attr('src') ); 
		return false;
	});
	
    if(settingsLoaded == 0) {

        $("div.accordionContent").hide();

        // set menu section items background
        $("div.accordionContentItem").css("background-image", bg_item_src);
        // set menu sections background
        $('div.accordionButtonGS').css("background-image", "url(" + bg_closed_src + ")");
        $('div.accordionButtonCD').css("background-image", "url(" + bg_closed_src + ")");
        $('div.accordionButtonABT').css("background-image", "url(" + bg_closed_src + ")");

        // Fix margin between General and Clear Data accordion buttons
        $('div.accordionButtonCD').css("margin-top", "0px");

        // set header arrow image
        $("#im1").attr("src", header_closed_src);	
        $("#im2").attr("src", header_closed_src);
        $("#im3").attr("src", header_closed_src);

    } else if(settingsLoaded == 1) {

        $('div.accordionButtonGS').next().show();

        // set menu sections background
        $('div.accordionButtonGS').css("background-image", "url(" + bg_open_src + ")");
        $('div.accordionButtonCD').css("background-image", "url(" + bg_closed_src + ")");
        $('div.accordionButtonABT').css("background-image", "url(" + bg_closed_src + ")");

        // Fix margin between General and Clear Data accordion buttons
        $('div.accordionButtonCD').css("margin-top", "1px");

        // set header arrow image
        $("#im1").attr("src", header_open_src);
        $("#im2").attr("src", header_closed_src);  
        $("#im3").attr("src", header_closed_src);  

    } else if(settingsLoaded == 2) {

        $('div.accordionButtonCD').next().show();

        // set menu sections background
        $('div.accordionButtonGS').css("background-image", "url(" + bg_closed_src + ")");
        $('div.accordionButtonCD').css("background-image", "url(" + bg_open_src + ")");
        $('div.accordionButtonABT').css("background-image", "url(" + bg_closed_src + ")");
        // Fix margin between General and Clear Data accordion buttons
        $('div.accordionButtonCD').css("margin-top", "0px");

        // set header arrow image
        $("#im1").attr("src", header_closed_src);
        $("#im2").attr("src", header_open_src);  
        $("#im3").attr("src", header_closed_src);  

   } else if(settingsLoaded == 3) {

        $('div.accordionButtonABT').next().show();

        // set menu sections background
        $('div.accordionButtonGS').css("background-image", "url(" + bg_closed_src + ")");
        $('div.accordionButtonCD').css("background-image", "url(" + bg_closed_src + ")");
        $('div.accordionButtonABT').css("background-image", "url(" + bg_open_src + ")");

        // Fix margin between General and Clear Data accordion buttons
        $('div.accordionButtonCD').css("margin-top", "0px");

        // set header arrow image
        $("#im1").attr("src", header_closed_src);
        $("#im1").attr("src", header_closed_src);  
        $("#im1").attr("src", header_open_src);  

  }	  

  //ACCORDION BUTTON ACTION
  $('div.accordionButtonGS').click(function() {
       //$('div.accordionContent').slideUp('normal');
       //$(this).next().slideDown('normal');

       $(this).next().toggle();
       $('div.accordionButtonCD').next().hide();
       $('div.accordionButtonABT').next().hide();
         
       if( $("#im1").attr("src") == header_closed_src ) { // current state

            // set menu sections background
            $('div.accordionButtonGS').css("background-image", "url(" + bg_open_src + ")");
            $('div.accordionButtonCD').css("background-image", "url(" + bg_closed_src + ")");
            $('div.accordionButtonABT').css("background-image", "url(" + bg_closed_src + ")");

            // Fix margin between General and Clear Data accordion buttons
            $('div.accordionButtonCD').css("margin-top", "0px");

            // change list headerarrow icon
            $("#im1").attr("src", header_open_src);
            $("#im2").attr("src", header_closed_src);
            $("#im3").attr("src", header_closed_src);

       } else if( $("#im1").attr("src") == header_open_src ) {
 
            // set menu sections background
            $('div.accordionButtonGS').css("background-image", "url(" + bg_closed_src + ")");
            // change list headerarrow icon
            $("#im1").attr("src", header_closed_src);
       }       
  });

  $('div.accordionButtonCD').click(function() {

       $(this).next().toggle();
       $('div.accordionButtonGS').next().hide();
       $('div.accordionButtonABT').next().hide();
 
       if( $("#im2").attr("src") == header_closed_src ) { // current state

            // set menu sections background
            $('div.accordionButtonGS').css("background-image", "url(" + bg_closed_src + ")");
            $('div.accordionButtonCD').css("background-image", "url(" + bg_open_src + ")");
            $('div.accordionButtonABT').css("background-image", "url(" + bg_closed_src + ")");

            // Fix margin between General and Clear Data accordion buttons
            $('div.accordionButtonCD').css("margin-top", "1px");

            // change list headerarrow icon
            $("#im1").attr("src", header_closed_src);
            $("#im2").attr("src", header_open_src);
            $("#im3").attr("src", header_closed_src);
 		
       } else if( $("#im2").attr("src") == header_open_src ) {
 
            // set menu sections background
            $('div.accordionButtonCD').css("background-image", "url(" + bg_closed_src + ")");
            // change list headerarrow icon
            $("#im2").attr("src", header_closed_src);
       }

  	});

  	$('div.accordionButtonABT').click(function() {

  		$(this).next().toggle();
  		$('div.accordionButtonGS').next().hide();
		$('div.accordionButtonCD').next().hide();
    
		if( $("#im3").attr("src") == header_closed_src ) { // current state

            // set menu sections background
            $('div.accordionButtonGS').css("background-image", "url(" + bg_closed_src + ")");
            $('div.accordionButtonCD').css("background-image", "url(" + bg_closed_src + ")");
            $('div.accordionButtonABT').css("background-image", "url(" + bg_open_src + ")");

            // Fix margin between General and Clear Data accordion buttons
            $('div.accordionButtonCD').css("margin-top", "0px");

            // change list headerarrow icon
            $("#im1").attr("src", header_closed_src);
            $("#im2").attr("src", header_closed_src);
            $("#im3").attr("src", header_open_src);
 		
		} else if( $("#im3").attr("src") == header_open_src ) {
 
            // set menu sections background
            $('div.accordionButtonABT').css("background-image", "url(" + bg_closed_src + ")");
            // change list headerarrow icon
            $("#im3").attr("src", header_closed_src);

		}
  	});
  
    var UAString = navigator.userAgent;
    var brNameVer = "unknown";
    var UALen = 1;
    var nameStart = 1;
    UALen = UAString.length;
    nameStart = UAString.indexOf("NokiaBrowser");
    // test to see if "NokiaBrowser" is in the UA String at all
    if (nameStart < 0) {
        brNameVer = "NOT a NokiaBrowser Browser";
    } else {
        // crop string starting at "NokiaBrowser"
    	brNameVer = UAString.substring(nameStart, UALen);
    	// limit substring to "NokiaBrowser" and version only
    	brNameVer = brNameVer.substr(0, brNameVer.indexOf(" "));
    	// clean up string for presentation
    	brNameVer = brNameVer.replace("NokiaBrowser/", " Version: ");
    }
    $('#release_version').html(brNameVer + "<br />");
});