function Suggests()
{
    // attach internal funcs
    this.write = writeSuggests;

    // do setup
    this.write();

    this.showSuggests = function() {
        window.chrome.alert("showSuggests");
        //window.snippets.updateGeometry();
        window.snippets.SuggestsId.repaint();

     	var curPos = window.snippets.SuggestsId.getPosition();
        window.chrome.alert(curPos.x);
        if (curPos.y == 0)
            window.snippets.SuggestsId.setPosition(5,68);
        window.snippets.SuggestsId.show();
        window.snippets.SuggestsId.zValue = 10;
    }

    this.hideSuggests = function() {
        window.snippets.SuggestsId.hide();
    }

    this.setWidth = function(width) {
        document.getElementById("SuggestsId").style.width = width;
        //window.chrome.alert("set width:" + document.getElementById("SuggestsId").offsetWidth);
    }

    this.setHeight = function(height) {
        document.getElementById("SuggestsId").style.height = height;
        //window.chrome.alert("set height:" + document.getElementById("SuggestsId").offsetHeight);
    }

    this.removeAllItems = function() {
        var parentList = document.getElementById("SuggestsUListId");
        while (parentList.childNodes[0]) {
            parentList.removeChild(parentList.childNodes[0]);
        }
        this.setHeight(0);
    }

    this.addItem = function(str) {
        this.setHeight(document.getElementById("SuggestsId").offsetHeight + 26); // FIXME 26 is the row height
        var parentList = document.getElementById("SuggestsUListId");
        var item = document.createElement("li");
        item.innerHTML = str;
        item.onclick=function() { window.chrome.alert(item.innerHTML + " is selected."); window.snippets.SuggestsId.hide();}
        item.onmouseover=function() { item.style.backgroundColor = 'Aquamarine'; }
        item.onmouseout=function() { item.style.backgroundColor = ''; }
        parentList.appendChild(item);
    }
}

// "Private" methods
function writeSuggests() {
    var html =
    '<div class="suggestsBox">' +
        '<div class="suggestBoxBody">' +
          '<div id="SuggestsListId" class="show">' +
            '<ul id="SuggestsUListId">' +
            '</ul>' +
          '</div>' +
        '</div>' +
    '</div>'; 
  document.write(html);
}

