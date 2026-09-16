cmake_minimum_required(VERSION 3.12.4)

queen_add_qml_module(
		QaosQtQuick STATIC

		URI Qaos.QtQuick
		VERSION 0.1
		QML_FILES
			URL.js
			Font.js
			Color.js
			DateTime.js
			ListView.js
)
