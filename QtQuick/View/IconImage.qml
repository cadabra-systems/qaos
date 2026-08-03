import QtQuick 2.15
import QtQuick.Effects

Image {
	id: rootImage

	property color color: "black"

	smooth: true

	layer.enabled: status != Image.Null
	// Flat recolor (Qt5 ColorOverlay parity): brightness 1.0 pushes the glyph to white while
	// keeping its alpha, then colorization tints that (now max-luminance) shape to the target —
	// so even a black source glyph becomes a solid `color`. Plain colorization alone is
	// luminance-preserving and leaves dark glyphs dark.
	layer.effect: MultiEffect {
		brightness: 1.0
		colorization: 1.0
		colorizationColor: rootImage.color
	}
}
