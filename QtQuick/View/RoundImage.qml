import QtQuick 2.15
import QtQuick.Effects

Image {
	id: rootImage

	layer.enabled: true
	layer.effect: MultiEffect {
		maskEnabled: true
		maskSource: ShaderEffectSource {
			sourceItem: Rectangle {
				width: rootImage.width
				height: rootImage.height
				radius: Math.min(width, height) / 2
			}
		}
	}
}
