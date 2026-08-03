cmake_minimum_required(VERSION 3.12.4)

queen_add_qml_module(
		QaosQtQuickView STATIC

		URI Qaos.QtQuick.View
		VERSION 0.1
		QML_FILES
			IconImage.qml
			RoundImage.qml
)
