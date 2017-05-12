# NPR-Sketch-Scene-based-on-Lighting-Intensity
NPR Sketch Scene based on Lighting Intensity with OpenGL

## Overview
<img src="https://drive.google.com/uc?id=0B-N_s7wEHZ8LbDM5am5aMjBhZ00" width="400"> <img src="https://drive.google.com/uc?id=0B-N_s7wEHZ8LYm9QcHlyWHBnT2s" width="400">

The lighting is actually "fake", I do common Phong lighting in fragment shader but only to get the ligthing factors respectively for ambient, diffuse, and specular, then get the sum of all of them.
~~~
float IntensityBasedLighting(){
	...

	float amb = inten_amb;
	float diff = max(0.0, dot(norm, -lg_dir)*0.5f + 0.5f)*inten_diff;
	float spec = pow(max(0.0, dot(refl_dir, view_dir)), 30.0f) * inten_spec;

	return amb + diff + spec;
}
~~~

Based on the factor(sum of amb, diff, and spec), render to different textures rather than draw lighting color directly, for example, triangle with higher value of factor will have lighter texture(with less desity of lines in this case).
