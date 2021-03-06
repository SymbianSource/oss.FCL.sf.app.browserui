/* js for download dialogs */

function writeDownloadDialog()
{
    var image = '/download/download_dialog_icon.png';

    var html =
        '<div class="downloadDialogBox">' +
            '<table><tr>' +
                '<td class="downloadDialogIcon">' +
                    '<img src="' + image + '" alt="">' +
                '</td>' +
                '<td class="downloadDialogText" id="DownloadDialogTextId">' +
                    'Replace with localized message text' +
                '</td>' +
            '</tr></table>' +
        '</div>';

    document.write(html);
}

var downloadTimeoutId = null;

var downloadTimeoutSeconds = 3;

function disableDownloadTimeout(caller)
{
    if (downloadTimeoutId != null) {
        clearTimeout(downloadTimeoutId);
        downloadTimeoutId = null;
    }
}

function hideDownloadDialog()
{
    window.snippets.DownloadDialogId.hide();

    // Clear timeout so we can use this function to hide
    // the download dialog in response to an external
    // mouse event and not just because of a timeout.

    disableDownloadTimeout("hideDownloadDialog");
}

function showDownloadDialog(messageHTML)
{
    // Clear timeout so we can replace an existing dialog
    // with the newest one without having hideDownloadDialog()
    // hide the wrong dialog.

    disableDownloadTimeout("showDownloadDialog");

    document.getElementById('DownloadDialogTextId').innerHTML = messageHTML;

    window.snippets.DownloadDialogId.setPosition(10,80);
    window.snippets.DownloadDialogId.show();
    window.snippets.DownloadDialogId.repaint();

    downloadTimeoutId = setTimeout(
            hideDownloadDialog,
            downloadTimeoutSeconds * 1000);
}

// This will get called for BOTH explicit download
// requests and for unsupported content downloads.
// In the former case the file argument may not be
// final as the download manager will rename files
// to avoid overwriting existing files in the download
// directory.
//
function onDownloadCreated(messageHTML)
{
    showDownloadDialog(messageHTML);
}

// This will get called for explicit download requests,
// i.e. requests generated by selecting "Save Link" or
// "Save Image" from the long-press menu.
//
// This will NOT get called for downloads executed in
// response to QWebPage unsupportedContent signals,
// as they get started by the QWebPage implementation
// before they get passed to the download manager.
//
function onDownloadStarted(messageHTML)
{
    showDownloadDialog(messageHTML);
}

function onDownloadSuccess(messageHTML)
{
    showDownloadDialog(messageHTML);
}

function onDownloadFailure(messageHTML)
{
    showDownloadDialog(messageHTML);
}

function onUnsupportedDownload(messageHTML)
{
    showDownloadDialog(messageHTML);
}

function connectDownloadSignals()
{
    window.downloads.downloadCreated.connect(onDownloadCreated);
    window.downloads.downloadStarted.connect(onDownloadStarted);
    window.downloads.downloadSuccess.connect(onDownloadSuccess);
    window.downloads.downloadFailure.connect(onDownloadFailure);
    window.downloads.unsupportedDownload.connect(onUnsupportedDownload);

    window.snippets.DownloadDialogId.externalMouseEvent.connect(
        function(type, name, description) {
            if ((name == "QGraphicsSceneMouseReleaseEvent") || (name == "QGraphicsSceneResizeEvent")) {
                hideDownloadDialog();
            }
        }
    );
}

if (window.downloads != null) {
    window.chrome.chromeComplete.connect(connectDownloadSignals);
}
