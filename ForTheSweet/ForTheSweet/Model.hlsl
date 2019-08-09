#define NUM_DIRECTION_LIGHTS 1

Texture2DArray gBoxTextured : register(t0);
Texture2D gEffectTexture : register(t3);

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
    matrix gmtxGameObject : packoffset(c0);
    uint gnMaterial : packoffset(c4);
}

cbuffer cbAnimateInfo : register(b3)
{
    matrix gmtxObject;
    matrix gBoneTransforms[96];
    uint gnMat;
    matrix gmtxShadowplayer;
}

cbuffer cbShadowInfo : register(b4)
{
    matrix gmtxtest;
    matrix gmtxShadow;
    uint gnMatr;
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

///////////////////////////////////////////     MODEL       /////////////////////////////////////////////

VS_MODEL_TEXTURED_OUTPUT VSDynamicModel(VS_MODEL_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tanL = float3(0.0f, 0.0f, 0.0f);
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    weights[0] = input.weight.x;
    weights[1] = input.weight.y;
    weights[2] = input.weight.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransforms[input.index[i]]).xyz;
        normalL += weights[i] * mul(input.normal, (float3x3) gBoneTransforms[input.index[i]]).xyz;
        tanL += weights[i] * mul(input.tan, (float3x3) gBoneTransforms[input.index[i]]).xyz;
    }
    
	// Right + Texture
    output.normalW = mul(normalL, (float3x3) gmtxObject);
    output.positionW = (float3) mul(float4(posL, 1.0f), gmtxObject);
    output.position = mul(mul(mul(float4(posL, 1.0f), gmtxObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDynamicModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	    
    float3 Light_Position = float3(0.f, 1.f, 0.f);
    float3 c_Light1 = float3(0.5f, -1.0f, 0.4f);

    float3 ambient = float3(0.7f, 0.7f, 0.7f);
    float3 diffuse = float3(0.5f, 0.5f, 0.5f);
    float3 specular = float3(1.0f, 1.0f, 1.0f);

    float constant = 1.0f;
    float linear_ = 0.09f;
    float quadratic = 0.0032f;
       
    // ambient
    float3 ambient_ = ambient * cColor.rgb;
    
    // diffuse
    input.normalW = normalize(input.normalW);
    float3 lightDir = normalize(-c_Light1); // -c_Light1
    float diff = clamp(dot(input.normalW, lightDir), 0.0, 1.0);
    float3 diffuse_ = diffuse * diff * cColor.rgb;

    //specular
    float3 CameraDir_test = float3(0.0f, -1.0f, 0.2f); // gvCameraPosition
    float3 viewDir = normalize(gvCameraPosition - input.position.xyz); //positionW
    float3 reflectDir = reflect(-lightDir, input.normalW);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // 32 : shiness
    float3 specular_test = float3(1.0, 1.0, 1.0);
    float3 specular_ = specular * spec * specular_test; //cColor.rgb;
    
    float distance = length(Light_Position - input.position.xyz);
    float attenuation = 1.0 / (constant + linear_ * distance + quadratic * (distance * distance));

    //ambient_ *= attenuation;
    //diffuse_ *= attenuation;
    //specular_ *= attenuation;

    float3 result = ambient_ + diffuse_ + specular_;

    output.color = float4(result, cColor.w); // cColor.w); //(newColor * cColor.xyz, cColor.w);
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

VS_MODEL_TEXTURED_OUTPUT VSStaticModel(VS_MODEL_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

	//for (int i = 0; i < NUM_DIRECTION_LIGHTS; i++) output.ShadowPosH[i] = mul(float4(output.positionW, 1.0f), gmtxShadowProjection[i]);
       
    return (output);
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

///////////////////////////////////     MESH    ///////////////////////////////////

VS_MODEL_TEXTURED_OUTPUT VSWave(VS_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;
    
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    float4 position_ = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    
    float3 mewpos = output.positionW;

    float sin_test = gnMaterial / 50.f;

    float additionalValueX = mewpos.x;
    float additionalValueY = mewpos.y;
    float additionalValueZ = mewpos.z;
	
    float periodX = 1.0 + (1.0 - additionalValueY) * 0.5;
    float periodY = 1.0 + additionalValueX * 0.1;
    float periodZ = 1.0 + additionalValueZ; // * 0.5;
	
    float valueX = (additionalValueY * 2 * 3.141592) - sin_test;
    float valueY = (additionalValueX * 2 * 3.141592) - sin_test;
    float valueZ = (additionalValueZ * 2 * 3.141592) - sin_test;

    float sinValueX = sin(valueX) * 0.08;
    float sinValueY = sin(valueY) * 0.05;
    float sinValueZ = sin(valueZ) * 0.05;

    //position_.y = position_.y * ((1.0 - additionalValueX) * 0.5 + 0.5);
    
    //position_.x = position_.x - sinValueX * additionalValueX;
    position_.y = position_.y + sinValueY * additionalValueX;
    //position_.z = position_.z + sinValueX * additionalValueZ;

    output.position = position_;
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSWaveModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    float3 Light_Position = float3(0.f, 1.f, 0.f);
    float3 c_Light1 = float3(0.5f, -1.0f, 0.4f);

    float3 ambient = float3(0.7f, 0.7f, 0.7f);
    float3 diffuse = float3(0.5f, 0.5f, 0.5f);
    float3 specular = float3(1.0f, 1.0f, 1.0f);

    float constant = 1.0f;
    float linear_ = 0.09f;
    float quadratic = 0.0032f;
       
    float3 ambient_ = ambient * cColor.rgb;
    
    input.normalW = normalize(input.normalW);
    float3 lightDir = normalize(-c_Light1); // -c_Light1
    float diff = clamp(dot(input.normalW, lightDir), 0.0, 1.0);
    float3 diffuse_ = diffuse * diff * cColor.rgb;
    
    float3 CameraDir_test = float3(0.0f, -1.0f, 0.2f); // gvCameraPosition
    float3 viewDir = normalize(gvCameraPosition - input.position.xyz); //positionW
    float3 reflectDir = reflect(-lightDir, input.normalW);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // 32 : shiness
    float3 specular_test = float3(1.0, 1.0, 1.0);
    float3 specular_ = specular * spec * specular_test; //cColor.rgb;
    
    float distance = length(Light_Position - input.position.xyz);
    float attenuation = 1.0 / (constant + linear_ * distance + quadratic * (distance * distance));

    float3 result = ambient_ + diffuse_ + specular_;

    output.color = float4(result, 0.7f);
    
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSMagic(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

VS_MODEL_TEXTURED_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;
    
    output.normalW = input.normal; //mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDiffused(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    //float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    //input.normalW = normalize(input.normalW);
	
    float4 cColor = float4(input.normalW, 1.f);

    output.color = float4(1.f, 0.5f, 0.f, 1.0f); // cColor;
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSBoundBox(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	    
    input.normalW = normalize(input.normalW);
       
    output.color = float4(1.f, 0.f, 0.f, 1.0f);
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

//////////////////////////////////////////////////      Shadow      //////////////////////////////////////

VS_MODEL_TEXTURED_OUTPUT VSShadow(VS_MODEL_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    //output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxShadow);
    //output.position = mul(mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection), gmtxShadow);
    output.uv = input.uv;

	//for (int i = 0; i < NUM_DIRECTION_LIGHTS; i++) output.ShadowPosH[i] = mul(float4(output.positionW, 1.0f), gmtxShadowProjection[i]);
       
    return (output);
};

VS_MODEL_TEXTURED_OUTPUT VSDynamicShadow(VS_MODEL_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tanL = float3(0.0f, 0.0f, 0.0f);
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    weights[0] = input.weight.x;
    weights[1] = input.weight.y;
    weights[2] = input.weight.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransforms[input.index[i]]).xyz;
        normalL += weights[i] * mul(input.normal, (float3x3) gBoneTransforms[input.index[i]]).xyz;
        tanL += weights[i] * mul(input.tan, (float3x3) gBoneTransforms[input.index[i]]).xyz;
    }
    
	// Right + Texture
    output.normalW = mul(normalL, (float3x3) gmtxObject);
    output.positionW = (float3) mul(float4(posL, 1.0f), gmtxObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    //output.position = mul(mul(mul(float4(posL, 1.0f), gmtxShadowplayer), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSShadow(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	    
    input.normalW = normalize(input.normalW);
       
    output.color = float4(0.f, 0.f, 0.f, 0.5f); // 그림자 이므로 반투명한 검은색
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSReverseShadow(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	    
    input.normalW = normalize(input.normalW);
       
    output.color = float4(0.0f, 0.f, 0.f, 0.0f); // 그림자 이므로 반투명한 검은색
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

///////////////////////////                  EFFECT                  ////////////////////////////////////////////////////

VS_MODEL_TEXTURED_OUTPUT VSLightning(VS_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;
    
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = float2(
    input.uv.x / 8 + float(gnMaterial) / float(8),
    input.uv.y / 1 + float(0) / float(1)
    );
    
    return (output);
}

VS_MODEL_TEXTURED_OUTPUT VSSkillEffect_1(VS_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;
    
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = float2(
    input.uv.x / 4 + float(gnMaterial) % float(4),
    input.uv.y / 4 + float(gnMaterial) / float(4)
    );
    
    return (output);
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSEffect(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    
    float2 uv = input.uv;
    float4 cColor = gEffectTexture.Sample(gDefaultSamplerState, uv);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};
