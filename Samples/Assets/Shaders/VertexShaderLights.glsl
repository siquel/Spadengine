#version 440

in vec3 inPosition;
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;
in vec2 inTexcoords;

out vec2 texcoords;
out vec3 normals;
out vec3 tangentFragPos;
out mat3 TBNVout;

layout (std140, binding = 0) uniform MVPUniform
{
	mat4 PV;
	mat4 M;
};

void main()
{
	gl_Position = PV * M * vec4(inPosition, 1.0);
	vec3 fragPos = vec3(M * vec4(inPosition, 1.0));
	texcoords = inTexcoords;
	mat3 normalMatrix = transpose(inverse(mat3(M)));
	vec3 T = normalize(normalMatrix * inTangent);
	vec3 B = normalize(normalMatrix * inBitangent);
	vec3 N = normalize(normalMatrix * inNormal);
	
	mat3 TBN = transpose(mat3(T, B, N));
	tangentFragPos = TBN * fragPos;
	
	TBNVout = TBN;
	normals = inNormal;
}