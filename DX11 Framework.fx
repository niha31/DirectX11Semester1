//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);
TextureCube SkyMap;

cbuffer ConstantBuffer : register( b0 )
{
	//PACK_SEAM
    float4x4 mWorld;

	//PACK_SEAM
    float4x4 mView;

	//PACK_SEAM
    float4x4 mProjection;

	//PACK_SEAM
    float4 DiffuseMtrl;

	//PACK_SEAM
    float4 DiffuseLight;

	//PACK_SEAM
    float3 LightVecW;
    float padding1;

	//PACK_SEAM
    float4 AmbientMtrl;

	//PACK_SEAM
    float4 AmbientLight;

	//PACK_SEAM
    float4 SpecularMtrl;

	//PACK_SEAM
    float4 SpecularLight;

	//PACK_SEAM
    float3 EyePosW; // Camera position in world space
    float SpecularPower;

	//PACK_SEAM

    float gameTime;
    int wavePattern;
    float2 padding2;


}

cbuffer cbSkybox
{
    float4x4 WVP;
    float4x4 World;
};

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 WorldPos : POSITION0;
    float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct SKYMAP_VS_OUTPUT    //output structure for skymap vertex shader
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

//------------------------------------------------------------------------------------
// Vertex Shader - Implements Gouraud Shading using Diffuse lighting only
//------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    if (wavePattern)
    {
        float height = 0.3f * (sin(Pos.x * gameTime * 0.25f) + sin(Pos.z * gameTime * 0.12f));
        Pos.y = height;
    }
        
    output.Pos = mul(Pos, mWorld);
	output.Tex = Tex;
    output.WorldPos = output.Pos;

    output.Pos = mul(output.Pos, mView);
    output.Pos = mul(output.Pos, mProjection);

    float3 normalW = mul(float4(NormalL, 0.0f), mWorld).xyz;
    output.Norm = normalize(normalW);

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	// Compute the ambient, diffuse, and specular terms separately.

    // Sum all the terms together and copy over the diffuse alpha.
    float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);

    float3 ambient = textureColour * AmbientMtrl * AmbientLight;

	float diffuseAmount = max(dot(LightVecW, input.Norm), 0.0f);
    float3 diffuse = diffuseAmount * (textureColour * DiffuseMtrl * DiffuseLight).rgb;

	// Compute the vector from the vertex to the eye position.

    float3 toEye = normalize(EyePosW - input.WorldPos.xyz);

	// Compute the reflection vector.

    float3 r = reflect(-LightVecW, input.Norm);

	// Determine how much (if any) specular light makes it
	// into the eye.
    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);
    float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rgb;


    float4 returnColour;
    returnColour.rgb = ambient + diffuse + specular;
    returnColour.a = DiffuseMtrl.a;

    return returnColour;

}


SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION, float3 normal : NORMAL, float2 inTexCoord : TEXCOORD)
{
    SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT) 0;

    //Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
    output.Pos = mul(float4(inPos, 1.0f), WVP).xyww;

    output.texCoord = inPos;

    return output;
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
    return SkyMap.Sample(samLinear, input.texCoord);
}