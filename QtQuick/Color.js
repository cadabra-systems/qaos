.pragma library

.import QtQuick 2.15 as QtQuick

function generate(str)
{
	var i, hash = 0;
	for (i = 0; i < str.length; i++) {
		hash = str.charCodeAt(i) + ((hash << 5) - hash);
	}
	var color = '#';
	for (i = 0; i < 3; i++) {
	var value = (hash >> (i * 8)) & 0xFF;
		color += ('00' + value.toString(16)).substr(-2);
	}
	return color;
}

function isLight(color)
{
	/// @note If hex --> Convert it to RGB: http://gist.github.com/983661
	var hex = color.toString();
	hex = +("0x" + hex.slice(1).replace(hex.length < 5 && /./g, '$&$&'));

	var r = hex >> 16;
	var g = hex >> 8 & 255;
	var b = hex & 255;
	var hsp = Math.sqrt(0.299 * (r * r) + 0.587 * (g * g) +	0.114 * (b * b));

	return (hsp > 127.5);
}
