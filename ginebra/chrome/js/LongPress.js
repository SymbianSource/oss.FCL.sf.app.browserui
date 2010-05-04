//
// Use LongPress() to set a function callback for a long-press on a particular DOM element.
//   Example:
//        <javascript ...> 
//          new LongPress("btnId", function() { alert("hello"); });
//        </javascript>
//        ...
//        <img id="btnId" ... />
//
function LongPress(elementId, callback) {
	this.el = document.getElementById(elementId);
    this.callback = callback;
    this.initialX = 0;
    this.initialY = 0;

	if(!this.el) {
		window.chrome.alert("LongPress: element " + elementId + " not found");
		return;
	}

    this.onTimerFired = function() {
        window.chrome.alert("onTimerFired");
        this.callback.call();
        this.cancelTimer();
        this.unregisterMouseMove();
    }

    this.startTimer = function() {
        this.cancelTimer();
        this.timer = window.setTimeout(createDelegate(this, this.onTimerFired), 1500);
	}

    this.cancelTimer = function() {
        if(this.timer) {
            window.clearTimeout(this.timer);
            this.timer = null;
        }
    }

    // Stop tracking mouse movements.
    this.unregisterMouseMove= function() {
        this.el.onmousemove = null;
    }

    this.cancel = function() {
        window.chrome.alert("cancel");
        this.cancelTimer();
        this.unregisterMouseMove();
    }

    // If the mouse has moved too much it's not considered a long press, so cancel.
    this.onMouseMove = function() {
        //window.chrome.alert("LongPress::onMouseMove " + this + " event=" + window.event +
        //                    " " + window.event.clientX + "," + window.event.clientY);
        if(Math.abs(this.initialX - window.event.clientX) > 4 ||
           Math.abs(this.initialY - window.event.clientY) > 4) {
            this.cancel();
        }
    }

    // Start tracking the mouse and save the initial mouse coords.
    this.onMouseDown = function() {
        window.chrome.alert("LongPress::onMouseDown " + this);
        this.isDown = true;
        this.initialX = window.event.clientX;
        this.initialY = window.event.clientY;
		this.el.onmousemove = this.onMouseMove.bind(this);
        this.startTimer();
    }

	this.el.onmousedown = this.onMouseDown.bind(this);

    // Cancel tracking on mouse up and out events, ie. not a long press.
	this.el.onmouseup = this.cancel.bind(this);
	this.el.onmouseout = this.cancel.bind(this);
}
