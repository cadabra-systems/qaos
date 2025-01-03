import QtQuick 2.15

Image {
	id: rootImage

	layer.enabled: true
	layer.effect: ShaderEffect {
		property variant src: rootImage

		vertexShader:
		"
			uniform highp mat4 qt_Matrix;
			attribute highp vec4 qt_Vertex;
			attribute highp vec2 qt_MultiTexCoord0;
			varying highp vec2 coord;
			void main()
			{
				coord = qt_MultiTexCoord0;
				gl_Position = qt_Matrix * qt_Vertex;
			}
		"
		fragmentShader:
		"
			#ifdef GL_ES
				precision mediump float;
			#endif

			varying highp vec2 coord;
			uniform sampler2D src;
			uniform lowp float qt_Opacity;
			void main()
			{
				float d = distance(coord, vec2(0.5, 0.5));
				if(d > 0.5) {
					gl_FragColor = vec4(0, 0, 0, 0) * qt_Opacity;
					return;
				}
				lowp vec4 tex = texture2D(src, coord);
				gl_FragColor = vec4(tex.rgb, tex.a) * qt_Opacity;
			}
		"
	}
}
