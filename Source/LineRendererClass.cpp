#include "LineRendererClass.h"

LineRendererClass::LineRendererClass()
{
	m_Lines.clear();
}

LineRendererClass::~LineRendererClass()
{
}

bool LineRendererClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//AddLine(XMFLOAT3(0, 0.1, 0), XMFLOAT3(1.2, 0.1, 0), XMFLOAT4(1.0, 0, 0, 1.0));
	/*m_Lines.clear();
	AddLine(XMFLOAT3(0, 0, 0), XMFLOAT3(2.5, 0, 0), XMFLOAT4(1.0, 0, 0, 1.0));
	AddLine(XMFLOAT3(0, 0, 0), XMFLOAT3(-2.5, 0, 0), XMFLOAT4(0, 1.0, 0, 1.0));
	AddLine(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 2.5, 0), XMFLOAT4(1.0, 1.0, 0, 1.0));
	AddLine(XMFLOAT3(0, 0, 0), XMFLOAT3(0, -2.5, 0), XMFLOAT4(1.0, 0, 1.0, 1.0));*/



	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	// 1.Compile the vertex and pixel shaders.				
	// 2.Create Input Layout for the vertex shader.		   struct for vertex input	
	// 3.Create the constant buffer for the vertex shader. MVP matrix.
	bool result = m_ColorShader->Initialize(device, hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void LineRendererClass::Shutdown()
{
	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

bool LineRendererClass::Render(ID3D11DeviceContext* context, ID3D11Device* device, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{

	//MyLine line{ XMFLOAT3(0,0,0),XMFLOAT3(0,2.5,0),XMFLOAT4(1.0,0,0,1.0) };
	int lineCount = m_Lines.size();
	int vertexCount = lineCount * 2;

	// create vertex array
	std::vector<VertexType> vertices(vertexCount);
	for (int i = 0; i < lineCount; i++) {
		vertices[i * 2] = { m_Lines[i].start, m_Lines[i].color };
		vertices[i * 2 + 1] = { m_Lines[i].end, m_Lines[i].color };
	}


	// create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;


	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices.data();

	// this step vertex data would be copied to gpu.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set Matrix
	m_ColorShader->Render(context, device, vertexCount, worldMatrix, viewMatrix, projectionMatrix, true);


	return true;
}

void LineRendererClass::AddLine(XMFLOAT3 start, XMFLOAT3 end, XMFLOAT4 color, bool random)
{
	XMFLOAT4 colors[] = {
		XMFLOAT4(1.0, 0, 0, 1.0),
		XMFLOAT4(0, 1.0, 0, 1.0),
		XMFLOAT4(1.0, 1.0, 0, 1.0),
		XMFLOAT4(1.0, 0, 1.0, 1.0) };

	MyLine line;
	line.start = start;
	line.end = end;
	if (random)
	{
		int currentColor = m_Lines.size() % 4;
		line.color = colors[currentColor];
	}
	else line.color = color;


	m_Lines.push_back(line);
}

void LineRendererClass::ClearLines()
{
	m_Lines.clear();
}
