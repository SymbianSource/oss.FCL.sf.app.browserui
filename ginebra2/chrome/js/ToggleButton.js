// Call ToggleButton to wire an HTML button (typically an <img> tag) to a JS callback
// when you want the button pressed state to be tied to a Ginebra snippets 
// visible attribute.
// 
//
// Params:
//   id - the id of the button.  Ex:  <img id="buttonId" .../>
//   snippetId - the id of the ginebra snippet to which the button state should be tied
//   upImg - the path to the "up" image for the button.
//   downImg - the path to the "down" image for the button.
//   disabledImg - the path to the "disabled" image for the button.
//   handler - the javascript callback to be called when triggered
//   triggerOnMouseUp- if true the handler will be called on mouse-up, defaults to true.
//   triggerOnMouseDown - if true the handler will be called on mouse-down, defaults to false.
//   triggerOnMouseOut - if true the handler will be called on mouse-out, defaults to false.
//
function ToggleButton(id, snippetId, upImg, downImg, disabledImg, handler, triggerOnMouseUp, triggerOnMouseDown, triggerOnMouseOut) {
    this.id = id;
    this.snippetId = snippetId;
    this.upImg = upImg;
    this.downImg = downImg;
    this.disabledImg = disabledImg;
    this.handler = handler;
    this.isDown = false;
    this.enabled = true;
    this.triggerOnMouseUp= triggerOnMouseUp == undefined ? true : triggerOnMouseUp;
    this.triggerOnMouseDown = triggerOnMouseDown == undefined ? false : triggerOnMouseDown;
    this.triggerOnMouseOut = triggerOnMouseOut == undefined ? false : triggerOnMouseOut;

    // attach this class to the item as "button" for later access
    // delete/cleanup existing button (if any) and attach this as button
    dom = document.getElementById(this.id);
    if (dom.button) {
        delete dom.button;
    }
    dom.button = this;

    //! Provide easy access to button element.
    this.element = function() {
        return document.getElementById(this.id);
    }

    //! Allow client to change button images after object creation.
    this.updateImages = function(upImg, downImg, disabledImg) {
        this.upImg = upImg;
        this.downImg = downImg;
        this.disableImg = disabledImg;
        this.updateButton();
    }

    //! Updates button image to appropriate up/down/disabled icon.
    this.updateButton = function() {
        if (this.enabled) {
            // button is enabled
            if (this.isDown) {
                // button is down
                this.element().src = this.downImg;
            } else {
                // button is up
                this.element().src = this.upImg;
            }
        } else {
            // button is disabled
            this.element().src = this.disabledImg;
        }
    }

    //! Enables/Disables button.
    this.setEnabled = function(state) {
        this.enabled = state;
        this.updateButton();
    }
    
    //! Handler for the mouse down event.
    this.onMouseDown = function() {
        if (this.triggerOnMouseDown && this.enabled) {
            this.handler();
        }
    }
    
    //! Handler for the mouse up event.
    this.onMouseUp = function() {
        if (this.triggerOnMouseUp && this.enabled) {
            this.handler();
        }
    }

    //! Handler for the mouse out event.
    this.onMouseOut = function() {
        if (this.isDown) {
            if (this.triggerOnMouseOut && this.enabled) {
                this.handler();
            }
        }
    }
    
    //! Handler for the snippet shown signal.
    this.onSnippetShown = function() {
        // When the snippet is shown the button should be down.
        this.isDown = true;
        this.updateButton.call(this);
    }
    
    //! Handler for the snippet hidden signal.
    this.onSnippetHidden = function() {
        // When the snippet is hidden the button should be up.
        this.isDown = false;
        this.updateButton.call(this);
    }
    
    //! After chrome is loaded, setup shown/hidden handlers.
    this.chromeLoadComplete = function() {
        // The up/down state of this button is tied to the visibility state of
        // the specified snippet.
        window.snippets[this.snippetId].shown.connect(this.onSnippetShown.bind(this));
        window.snippets[this.snippetId].hidden.connect(this.onSnippetHidden.bind(this));
    }
 
    // Make sure we can find the element.
    if (!this.element()) {
        alert("ToggleButton: element not found, " + id);
        return;
    }
    
    // Set up element event handlers.
    this.element().onmousedown = this.onMouseDown.bind(this);
    this.element().onmouseup = this.onMouseUp.bind(this);
    this.element().onmouseout = this.onMouseOut.bind(this);

    // Set the initial state of the button.
    this.updateButton();

    // can't access chrome snippets until chrome load complete
    window.chrome.chromeComplete.connect(this.chromeLoadComplete.bind(this));
}

