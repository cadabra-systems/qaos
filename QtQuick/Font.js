.pragma library

.import QtQuick 2.15 as QtQuick

function describeFont(font)
{
	return (
			"familyName: %1, styleName: %2, "+
			"weight: %3, bold: %4, italic: %5, strikeout: %6, underline: %7, overline: %8, "+
			"pointSize: %9, pixelSize: %10, letterSpacing: %11, wordSpacing: %12, kerning: %13, "+
			"capitalization: %14, preferShaping: %15, hintingPreference: %16"
	)
	.arg(font.family)
	.arg(font.styleName)
	.arg(font.weight)
	.arg(font.bold)
	.arg(font.italic)
	.arg(font.strikeout)
	.arg(font.underline)
	.arg(font.overline)
	.arg(font.pointSize)
	.arg(font.pixelSize)
	.arg(font.letterSpacing)
	.arg(font.wordSpacing)
	.arg(font.kerning)
	.arg(font.capitalization)
	.arg(font.preferShaping)
	.arg(font.hintingPreference)
	;
}
