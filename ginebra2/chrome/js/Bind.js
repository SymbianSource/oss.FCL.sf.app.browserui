
Function.prototype.bind = function(obj) {
    var method = this,
    temp = function() {
        return method.apply(obj, arguments);
    };
 
    return temp;
} 

function createDelegate(object, method) {
    var shim = function()
        {
            method.apply(object, arguments);
        }

    return shim;
}
