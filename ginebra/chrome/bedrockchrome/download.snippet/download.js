/* js for download progress */

var debugDownloadSignals = true;

// Generate HTML to populate download DIV element.

function downloadProgressHTML()
{
    var html =
        '<div class="DownloadFile" id="DownloadFile"></div>' +
        '<div class="DownloadMeter" id="DownloadMeter" style="width: 200px">' +
            '<div class="DownloadProgress" id="DownloadProgress" style="width: 0px"></div>' +
            '<div class="DownloadCounter" id="DownloadCounter" style="left: 210px"></div>' +
        '</div>';

    // FIXME ;;; replace form with pure div/css button

    html +=
        '<form name="DownloadForm" action="">' +
            '<input type="button" name="DownloadButton" value="Clear" ' +
            'onClick="window.snippets.DownloadId.hide();">' +
        '</form>';

    return html;
}

function downloadErrorHTML(file, error)
{
    var html =
        '<div class="DownloadFile" id="DownloadFile">' + file + '</div>' +
        '<div class="DownloadError" id="DownloadError">ERROR: ' + error + '</div>';

    return html;
}

function writeDownloadEmpty()
{
    document.write(downloadProgressHTML());
}

// Initialize download DIV.

function Download()
{
    this.write = writeDownloadEmpty;

    this.write();
}

// Update HTML.

function setDownloadHTML(html)
{
    var dlElem = document.getElementById("DownloadId");
    dlElem.innerHTML = html;
}

function setDownloadFile(file)
{
    var dlFile = document.getElementById("DownloadFile");
    dlFile.innerHTML = file;
}

function setDownloadPercent(percent)
{
    var outer = document.getElementById("DownloadMeter");
    var inner = document.getElementById("DownloadProgress");
    var counter = document.getElementById("DownloadCounter");

    var outerWidthStyle = outer.style.width;
    var innerWidthStyle = inner.style.width;
    var counterLeftStyle = counter.style.left;

    var outerWidthNum = parseInt(outerWidthStyle);
    var innerWidthNum = parseInt(innerWidthStyle);
    var counterLeftNum = parseInt(counterLeftStyle);

    innerWidthNum = Math.floor(outerWidthNum * percent / 100.0);
    innerWidthStyle = innerWidthStyle.replace(/^\d+/, innerWidthNum);

    counterLeftNum = outerWidthNum + 10 - innerWidthNum;
    counterLeftStyle = counterLeftStyle.replace(/^\d+/, counterLeftNum);

    inner.style.width = innerWidthStyle;
    counter.style.left = counterLeftStyle;
    counter.style.width = "" + (innerWidthNum + 50) + "px";

    counter.innerHTML = "" + percent + "%";
}

function setDownloadSize(size, sofar, percent)
{
    if (size >= 0) {
        setDownloadPercent(percent);
    } else {
        var counter = document.getElementById("DownloadCounter");
        counter.innerHTML = "" + sofar + " bytes";
    }
}

// Connect to relevant ChromeJSObject signals.

window.chrome.onDownloadCreated.connect(
    function(id) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadCreated");
            window.chrome.alert("id=" + id);
        }
        window.snippets.DownloadId.show();
        setDownloadHTML(downloadProgressHTML());
    }
);

window.chrome.onDownloadStarted.connect(
    function(id, file, size) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadStarted");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
        }
        setDownloadFile(file);
    }
);

window.chrome.onDownloadProgress.connect(
    function(id, file, size, sofar, percent) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadProgress");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
            window.chrome.alert("sofar=" + sofar);
            window.chrome.alert("percent=" + percent);
        }
        setDownloadFile(file);
        setDownloadSize(size, sofar, percent);
    }
);

window.chrome.onDownloadFinished.connect(
    function(id, file, size, sofar, percent) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadFinished");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
            window.chrome.alert("sofar=" + sofar);
            window.chrome.alert("percent=" + percent);
        }
        setDownloadFile(file);
        setDownloadSize(size, sofar, percent);
    }
);

window.chrome.onDownloadPaused.connect(
    function(id, file, size, sofar, percent, error) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadPaused");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
            window.chrome.alert("sofar=" + sofar);
            window.chrome.alert("percent=" + percent);
            window.chrome.alert("error=" + error);
        }
        setDownloadHTML(downloadErrorHTML(file, error));
    }
);

window.chrome.onDownloadCancelled.connect(
    function(id, file, size, sofar, percent, error) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadCancelled");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
            window.chrome.alert("sofar=" + sofar);
            window.chrome.alert("percent=" + percent);
            window.chrome.alert("error=" + error);
        }
        setDownloadHTML(downloadErrorHTML(file, error));
    }
);

window.chrome.onDownloadFailed.connect(
    function(id, file, size, sofar, percent, error) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadFailed");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
            window.chrome.alert("sofar=" + sofar);
            window.chrome.alert("percent=" + percent);
            window.chrome.alert("error=" + error);
        }
        setDownloadHTML(downloadErrorHTML(file, error));
    }
);

window.chrome.onDownloadNetworkLoss.connect(
    function(id, file, size, sofar, percent, error) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadNetworkLoss");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
            window.chrome.alert("sofar=" + sofar);
            window.chrome.alert("percent=" + percent);
            window.chrome.alert("error=" + error);
        }
        setDownloadHTML(downloadErrorHTML(file, error));
    }
);

window.chrome.onDownloadError.connect(
    function(id, file, size, sofar, percent, error) {
        if (debugDownloadSignals) {
            window.chrome.alert("JS received signal onDownloadError");
            window.chrome.alert("id=" + id);
            window.chrome.alert("file=" + file);
            window.chrome.alert("size=" + size);
            window.chrome.alert("sofar=" + sofar);
            window.chrome.alert("percent=" + percent);
            window.chrome.alert("error=" + error);
        }
        setDownloadHTML(downloadErrorHTML(file, error));
    }
);
