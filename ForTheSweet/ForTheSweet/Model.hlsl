#define NUM_DIRECTION_LIGHTS 1

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView;
	matrix gmtxProjection;
	matrix gmtxInvProjection;
	matrix gmtxShadowProjection[NUM_DIRECTION_LIGHTS];
	float3 gvCameraPosition;
};

cbuffer cbAnimateInfo : register(b3)
{
	matrix      gmtxObject;
	matrix      gBoneTransforms[96];
	uint      gnMat;
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
	float4 ShadowPosH[NUM_DIRECTION_LIGHTS] : POSITION1;
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
	for (int j = 0; j < NUM_DIRECTION_LIGHTS; j++)
		output.ShadowPosH[j] = mul(float4(output.positionW, 1.0f), gmtxShadowProjection[j]);
	output.uv = input.uv;

	return(output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDynamicModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	/*
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	input.normalW = normalize(input.normalW);
	*/

	output.color = float(1, 0, 0, 1);
	output.nrmoutline = float4(0, 0, 0, 0);
	output.nrm = output.nrmoutline;
	output.pos = float4(input.positionW, 1.0f);
	
	return (output);
};