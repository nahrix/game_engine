#include "bufferclass.h"

BufferClass::BufferClass()
{
	m_dynamicIndexCount = 0;
	m_staticIndexCount = 0;
	m_streamingIndexCount = 0;

	m_dynamicIndexBuffer = 0;
	m_dynamicVertexBuffer = 0;
	m_staticIndexBuffer = 0;
	m_staticVertexBuffer = 0;
	m_streamingIndexBuffer = 0;
	m_streamingVertexBuffer = 0;

	m_dynamicVertices = 0;
	m_staticVertices = 0;
	m_streamingVertices = 0;
}

BufferClass::~BufferClass()
{
	Shutdown();
}

int BufferClass::GetDynamicIndexCount()
{
	return m_dynamicIndexCount;
}

bool BufferClass::Initialize(D3DClass* D3D)
{
	m_D3D = D3D;

	m_dynamicVertices = new VertexType::Default[0];
	m_staticVertices = new VertexType::Default[0];
	m_streamingVertices = new VertexType::Default[0];

	return true;
}

bool BufferClass::AddModel(VertexType::Default* vertices, int indices)
{
	VertexType::Default *tempVertices;
	int newIndexCount;
	int offset;
	int size = sizeof(VertexType::Default);

	// Update the vertex array with the new values (including resizing the array)
	newIndexCount = m_dynamicIndexCount + indices;
	offset = sizeof(VertexType::Default) * m_dynamicIndexCount;
	tempVertices = new VertexType::Default[newIndexCount];
	ZeroMemory(tempVertices, sizeof(VertexType::Default) * newIndexCount);
	memcpy(tempVertices, m_dynamicVertices, sizeof(VertexType::Default) * m_dynamicIndexCount);
	memcpy(tempVertices + m_dynamicIndexCount, vertices, sizeof(VertexType::Default) * indices);
	m_dynamicIndexCount = newIndexCount;
	delete m_dynamicVertices;
	m_dynamicVertices = tempVertices;

	UpdateBuffer(m_dynamicVertexBuffer, m_dynamicIndexBuffer, m_dynamicVertices, m_dynamicIndexCount);

	return true;
}

bool BufferClass::UpdateBuffer(ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, VertexType::Default*& vertices, int& indexCount)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	ID3D11Device* device;
	int i;

	device = m_D3D->GetDevice();

	if (vertexBuffer)
		vertexBuffer->Release();

	if (indexBuffer)
		indexBuffer->Release();

	// Create the index array.
	indices = new unsigned long[indexCount];
	if(!indices)
	{
		return false;
	}
    
	// Load the index array with data.
	for(i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}
    
	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType::Default) * indexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete [] indices;
	indices = 0;

	return true;
}

void BufferClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType::Default); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_dynamicVertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_dynamicIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void BufferClass::Shutdown()
{
	if (m_dynamicIndexBuffer)
	{
		m_dynamicIndexBuffer->Release();
		m_dynamicIndexBuffer = 0;
	}

	if (m_dynamicVertexBuffer)
	{
		m_dynamicVertexBuffer->Release();
		m_dynamicVertexBuffer = 0;
	}

	if (m_staticIndexBuffer)
	{
		m_staticIndexBuffer->Release();
		m_staticIndexBuffer = 0;
	}

	if (m_staticVertexBuffer)
	{
		m_staticVertexBuffer->Release();
		m_staticVertexBuffer = 0;
	}
		
	if (m_streamingIndexBuffer)
	{
		m_streamingIndexBuffer->Release();
		m_streamingIndexBuffer = 0;
	}

	if (m_streamingVertexBuffer)
	{
		m_streamingVertexBuffer->Release();
		m_streamingVertexBuffer = 0;
	}
	
	if (m_dynamicVertices)
	{
		delete[] m_dynamicVertices;
		m_dynamicVertices = 0;
	}
	
	if (m_staticVertices)
	{
		delete[] m_staticVertices;
		m_staticVertices = 0;
	}

	if (m_streamingVertices)
	{
		delete[] m_streamingVertices;
		m_streamingVertices = 0;
	}
		
}