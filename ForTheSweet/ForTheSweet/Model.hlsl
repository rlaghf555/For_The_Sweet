#define NUM_DIRECTION_LIGHTS 1

const float3 c_Light1 = float3(500.0f, -500.0f, -500.0f);
const float3 ambient = float3(1.0f, 1.0f, 1.0f);
const float3 diffuseColor = float3(1.0f, 1.0f, 1.0f);
const float3 specColor = float3(1.0f, 1.0f, 1.0f);

const float3 c_CameraDir = float3(0.0f,0.0f, 0.0f);

const float a = 1.0f;
const float d = 0.5f;
const float s = 1.0f;

Texture2DArray gBoxTextured : register(t0);
SamplerState gDefaultSamplerState : register(s0);

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView;
	matrix gmtxProjection;
	matrix gmtxInvProjection;
	float3 gvCameraPosition;
};

cbuffer cbObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint			gnMaterial : packoffset(c4);
}

cbuffer cbAnimateInfo : register(b3)
{
	matrix      gmtxObject;
	matrix      gBoneTransforms[96];
	uint        gnMat;
}

struct VS_MODEL_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tan : TANGENT;
	float2 uv : TEXCOORD;
	uint4 index : BORNINDEX;
	float3 weight : WEIGHT;
	uint texindex : TEXINDEX;
};

struct VS_MODEL_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 color : SV_TARGET0;
	float4 nrmoutline : SV_TARGET1;
	float4 nrm : SV_TARGET2;
	float4 pos : SV_TARGET3;
};

VS_MODEL_TEXTURED_OUTPUT VSDynamicModel(VS_MODEL_INPUT input)
{
	VS_MODEL_TEXTURED_OUTPUT output;

	float3   posL = float3(0.0f, 0.0f, 0.0f);
	float3   normalL = float3(0.0f, 0.0f, 0.0f);
	float3   tanL = float3(0.0f, 0.0f, 0.0f);
	float   weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	weights[0] = input.weight.x;
	weights[1] = input.weight.y;
	weights[2] = input.weight.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	for (int i = 0; i < 4; ++i) {
		posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransforms[input.index[i]]).xyz;
		normalL += weights[i] * mul(input.normal, (float3x3)gBoneTransforms[input.index[i]]).xyz;
		tanL += weights[i] * mul(input.tan, (float3x3)gBoneTransforms[input.index[i]]).xyz;
	}
    
	// Right + Texture
	output.normalW = mul(normalL, (float3x3)gmtxObject);
	output.positionW = (float3)mul(float4(posL, 1.0f), gmtxObject);
	output.position = mul(mul(mul(float4(posL, 1.0f), gmtxObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDynamicModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    float3 v_normal = float3(input.normalW.xy, input.normalW.z * 0.2);

	input.normalW = normalize(input.normalW);
	
    float3 v_Pos = input.positionW;
    
    float3 lightDir = c_Light1 - v_Pos;
    float diffuse = clamp(dot(lightDir, v_normal), 0.0f, 1.0f);
    float3 reflectDir = reflect(lightDir, v_normal);
    float3 viewDir = v_Pos - c_CameraDir;
    float spec = clamp(dot(viewDir, reflectDir), 0.0f, 1.0f);
    spec = pow(spec, 12.0);

    float3 newColor = ambient * a + diffuseColor * diffuse * d + specColor * spec * s;

    output.color = cColor; //(newColor * cColor.xyz, cColor.w);
	//output.color = float4(1, 1, 1, 1);
	output.nrmoutline = float4(input.normalW, 1.0f);
	output.nrm = output.nrmoutline;
	output.pos = float4(input.positionW, 1.0f);
	
	return (output);
};

VS_MODEL_TEXTURED_OUTPUT VSStaticModel(VS_MODEL_INPUT input)
{
	VS_MODEL_TEXTURED_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

	//for (int i = 0; i < NUM_DIRECTION_LIGHTS; i++) output.ShadowPosH[i] = mul(float4(output.positionW, 1.0f), gmtxShadowProjection[i]);
       
	return(output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSCottonModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
    output.color.a = 0.7f;
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSWaveModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
    output.color.w = 0.7f;
	//output.color = float4(1, 1, 1, 1);
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tan : TANGENT;
    float2 uv : TEXCOORD;
    uint texindex : TEXINDEX;
};
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
};

VS_MODEL_TEXTURED_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;
    
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

/*
float4 PSDiffused(VS_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
	//output.color = float4(1, 1, 1, 1);
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
}*/

/*
// nPrimitiveID : 삼각형의 정보 
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSStaticModel(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	//float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	input.normalW = normalize(input.normalW);

	output.color = float4(0,1,0,1);
	output.nrmoutline = float4(input.normalW, 1.0f);
	output.nrm = output.nrmoutline;
	output.pos = float4(input.positionW, 1.f);
	return (output);
};
*/