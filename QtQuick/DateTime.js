.pragma library

.import QtQuick 2.15 as QtQuick

function formatDate(input)
{
	var milliseconds = parseInt((input % 1000) / 100);
	var seconds = Math.floor((input / 1000) % 60);
	var minutes = Math.floor((input / (1000 * 60)) % 60);
	var hours = Math.floor((input / (1000 * 60 * 60)) % 24);
	hours = (hours < 10) ? "0" + hours : hours;
	minutes = (minutes < 10) ? "0" + minutes : minutes;
	seconds = (seconds < 10) ? "0" + seconds : seconds;
	/// @???
	return (hours !== "00" ? (hours + ":") : "") + minutes + ":" + seconds;
}

function shrinkDate(input)
{
	var now = new Date();
	var target = new Date(input);
	var format = "hh:mm";
	/// @todo now - target <= 2 hours
	if (target.getFullYear() !== now.getFullYear()) {
		format = "d MMM yyyy, hh:mm"
	} else if (target.getMonth() !== now.getMonth() || target.getDay() !== now.getDay()) {
		format = "d MMM, hh:mm"
	}
	return Qt.formatDateTime(target, format);
}
