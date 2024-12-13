.pragma library

.import QtQuick 2.15 as QtQuick

function makeURLParamsObject(target)
{
	var retval = {};
	var query = (typeof(target) === 'object' ? target.toString() : target).split('?')[1];
	if (!query) {
		return retval;
	}
	query = query.split('#')[0];
	var arr = query.split('&');
	for (var i = 0; i < arr.length; i++) {
		var a = arr[i].split('=');
		var param_name = a[0];
		var param_value = typeof (a[1]) === 'undefined' ? true : a[1];
		param_name = param_name.toLowerCase();
		if (typeof param_value === 'string') {
			param_value = param_value.toLowerCase();
		}
		if (param_name.match(/\[(\d+)?\]$/)) {
			var key = param_name.replace(/\[(\d+)?\]/, '');
			if (!retval[key]) {
				retval[key] = [];
			}
			if (param_name.match(/\[\d+\]$/)) {
				var index = /\[(\d+)\]/.exec(param_name)[1];
				retval[key][index] = param_value;
			} else {
				retval[key].push(param_value);
			}
		} else {
			if (!retval[param_name]) {
				retval[param_name] = param_value;
			} else if (retval[param_name] && typeof retval[param_name] === 'string') {
				retval[param_name] = [retval[param_name]];
				retval[param_name].push(param_value);
			} else {
				retval[param_name].push(param_value);
			}
		}
	}
	return retval;
}
