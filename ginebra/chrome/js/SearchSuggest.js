//Constructor
function SearchSuggest(){
}

var searchArray = new Array(6);
var arrayLimit = 6;
var usercallback;

SearchSuggest.prototype.loadYahoo = function(query){
    var yUrl = "http://search.yahooapis.com/WebSearchService/V1/relatedSuggestion?";
    var yAppId = "appid=rv9RNS3V34GEz5NvGPBClnQaRVtN1tO57Di0OxX24nfHx1iqNA_QfahxpBoJJw--";
    var yOutput = "&output=json";
    var yResults = "&results = 5";
    var yCallBack = "&callback=SearchSuggest.prototype.processYahoo&query=";
    var yahooSuggest = yUrl + yAppId + yOutput + yCallBack + query;
    
    var headID = document.getElementsByTagName("head")[0];         
    var newScript = document.createElement('script');
    newScript.type = 'text/javascript';
    newScript.src = yahooSuggest;
    headID.appendChild(newScript);
}

SearchSuggest.prototype.processYahoo = function(resultObject){
    searchArray = [];
    if(resultObject.ResultSet && resultObject.ResultSet.Result){
        var n = arrayLimit < resultObject.ResultSet.Result.length ? arrayLimit : resultObject.ResultSet.Result.length;
        for(var i=0; i<n; i++) {
            searchArray[i] = resultObject.ResultSet.Result[i];
        }
    }
    usercallback(searchArray);
}

SearchSuggest.prototype.loadGoogle = function(query){
    var gUrl = "http://suggestqueries.google.com/complete/search?hl=en&json=t&jsonp=";
    var gCallBack = "SearchSuggest.prototype.processGoogle&q=";
    var googleSuggest = gUrl + gCallBack + query;
      
    var headID = document.getElementsByTagName("head")[0];         
    var newScript = document.createElement('script');
    newScript.type = 'text/javascript';
    newScript.src = googleSuggest;
    headID.appendChild(newScript);
}

SearchSuggest.prototype.processGoogle = function(resultObject){
    searchArray = [];
    if(resultObject){
        var n = arrayLimit < resultObject[1].length ? arrayLimit : resultObject[1].length;
        searchArray[0] = resultObject[0];
        for(var i=1; i<n; i++) {
            searchArray[i] = resultObject[1][i];
        }
    }
    usercallback(searchArray);
}

SearchSuggest.prototype.searchSuggest = function(queryString, getSearchUrl){
    //load yahoo suggest database. this has a limit of 5k hits/day per ip address.
    //this.loadYahoo(queryString);
    //load google suggest database
    this.loadGoogle(queryString);
    usercallback = getSearchUrl;
}
