#version 330

in vec2 TexCoord0;
in vec3 normal;
in vec3 pos;
out vec4 color;

uniform sampler2D level[6];
uniform int colMode;
uniform vec3 ca;

vec4 PhongLighting(vec4 texture){
	vec3 light_pos = vec3(1.f,1.f,-1.f);
	vec4 cl_amb = vec4(.5f, .5f, .5f, 1.0f);
	vec4 cl_diff = texture;
	vec4 cl_spec = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	float inten_amb = 0.4f;
	float inten_diff = 1.2f;
	float inten_spec = 2.0f;
	vec3 norm = normalize(normal);
	vec3 lg_dir = normalize(vec3(0.f, -1.f, 1.f));
	vec3 view_dir = normalize(ca - pos);
	vec3 refl_dir = reflect(lg_dir, norm); 

	vec4 amb = cl_amb * inten_amb;
	vec4 diff = cl_diff * max(0.0, dot(norm, -lg_dir)*0.5f + 0.5f)*inten_diff;
	vec4 spec = cl_spec * pow(max(0.0, dot(refl_dir, view_dir)), 30.0f) * inten_spec;

	return amb + diff + spec;
}

float IntensityBasedLighting(){
	vec3 light_pos = vec3(4.f,4.f,1.5f);
	float inten_amb = 0.4f;
	float inten_diff = 1.2f;
	float inten_spec = 2.0f;
	vec3 norm = normalize(normal);
	vec3 lg_dir = normalize(vec3(-1.f, -1.f, 1.f));
	vec3 view_dir = normalize(ca - pos);
	vec3 refl_dir = reflect(lg_dir, norm); 

	float amb = inten_amb;
	float diff = max(0.0, dot(norm, -lg_dir)*0.5f + 0.5f)*inten_diff;
	float spec = pow(max(0.0, dot(refl_dir, view_dir)), 30.0f) * inten_spec;

	return amb + diff + spec;
}

void main(){
	if(colMode == 1){
		color = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	}else{
		//float factor = (dot(normalize(normal), -normalize(vec3(0.f, -1.f, 1.f))) * 0.5 + 0.5) * 12.0f;
		float factor = IntensityBasedLighting() * 10.0f;
		vec4 tempcol = vec4(0.f);
		if(factor < 7.f)
			tempcol = texture2D(level[5],TexCoord0.st * 6.0);
		else if(factor < 8.f)
			tempcol = texture2D(level[4],TexCoord0.st * 4.5);
		else if(factor < 9.f)
			tempcol = texture2D(level[3],TexCoord0.st * 4.5);
		else if(factor < 10.f)
			tempcol = texture2D(level[2],TexCoord0.st * 4.5);
		else if(factor < 10.5f)
			tempcol = texture2D(level[1],TexCoord0.st * 4.5);
		else
			tempcol = texture2D(level[0],TexCoord0.st * 4.5);

		//color = PhongLighting(vec4(tempcol.rgb, 1.0f));
		color = vec4(tempcol.rgb, 1.0f);
	}
}
