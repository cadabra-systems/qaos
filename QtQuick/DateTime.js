.pragma library

.import QtQuick 2.15 as QtQuick

function formatTimePoint(timepoint)
{
	var milliseconds = parseInt((timepoint % 1000) / 100), seconds = Math.floor((timepoint / 1000) % 60), minutes = Math.floor((timepoint / (1000 * 60)) % 60), hours = Math.floor((timepoint / (1000 * 60 * 60)) % 24);
	hours = (hours < 10) ? "0" + hours : hours;
	minutes = (minutes < 10) ? "0" + minutes : minutes;
	seconds = (seconds < 10) ? "0" + seconds : seconds;
	/// @???
	return (hours !== "00" ? (hours + ":") : "") + minutes + ":" + seconds;
}
