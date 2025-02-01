
// Only support few bones first.
#define MAX_BONES 10
/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix boneMatrices[MAX_BONES];
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    uint4 boneIndices : BONE_INDICES;
    float4 boneWeights : BONE_WEIGHTS;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SkinnedVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
    
    // Calculate animated position in object space.
    uint4 indices = input.boneIndices;
    
    output.position = mul(input.position, boneMatrices[indices.x]) * input.boneWeights.x
    + mul(input.position, boneMatrices[indices.y]) * input.boneWeights.y
    + mul(input.position, boneMatrices[indices.z]) * input.boneWeights.z
    + mul(input.position, boneMatrices[indices.w]) * input.boneWeights.w;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader. for interplate
    output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only. 3x3!! don't move!!!
    //output.normal = mul(input.normal, (float3x3) boneMatrices[indices.x]) * input.boneWeights.x
    //+ mul(input.normal, (float3x3) boneMatrices[indices.y]) * input.boneWeights.y
    //+ mul(input.normal, (float3x3) boneMatrices[indices.z]) * input.boneWeights.z
    //+ mul(input.normal, (float3x3) boneMatrices[indices.w]) * input.boneWeights.w;
    //output.normal = mul(input.normal, (float3x3) worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(input.normal);

    return output;
}