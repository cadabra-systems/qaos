import QtQuick 2.15
import QtGraphicalEffects 1.15

Image {
	id: rootImage

	property color color: "black"

	smooth: true

	layer.enabled: status != Image.Null
	layer.effect: ColorOverlay {
		color: rootImage.color
		antialiasing: true
	}
}
