// @name      Slowly Fade
// @version   0.88
// @author    Richard D. LeCour
// @namespace http://www.richardsramblings.com/?p=486

var opacity = 96; // Avoid starting at 100% due to Mozilla bug
var slowly = {
	fade : function (id) {
		opacity = 96;
		this.fadeLoop(id, opacity);
	},
	fadeLoop : function (id, opacity) {
		var o = document.getElementById(id);
		if (opacity >= 5) {
			slowly.setOpacity(o, opacity);
			opacity -= 4;
			window.setTimeout("slowly.fadeLoop('" + id + "', " + opacity + ")", 50);
		} else {
			o.style.display = "none";
		}
	},
	setOpacity : function (o, opacity) {
		o.style.filter = "alpha(style=0,opacity:" + opacity + ")";	// IE
		o.style.KHTMLOpacity = opacity / 100;				// Konqueror
		o.style.MozOpacity = opacity / 100;					// Mozilla (old)
		o.style.opacity = opacity / 100;					// Mozilla (new)
	},
	
	show : function (id) {
		opacity = 4;
		this.showLoop(id, opacity);
		},
	showLoop : function (id, opacity) {
		//alert('opacity: ' + opacity);
		var o = document.getElementById(id);
		if (opacity <= 100) {
			
			slowly.setOpacity(o, opacity);
			opacity += 4;
			window.setTimeout("slowly.showLoop('" + id + "', " + opacity + ")", 50);
		} 
	}
}
