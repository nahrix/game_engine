#ifndef _BUFFERCLASS_H_
#define _BUFFERCLASS_H_

#include <DXGI.h>
#include <D3D11.h>
#include <d3dx10math.h>
#include <vector>
#include "modelclass.h"
#include "d3dclass.h"
#include "vertextypes.h"

class BufferClass
{
public:
	BufferClass();
	BufferClass(const BufferClass&);
	~BufferClass();

	bool Initialize(D3DClass*);

	bool AddModel(VertexType::Default*, int);

	int GetDynamicIndexCount();

	bool UpdateBuffer(ID3D11Buffer*&, ID3D11Buffer*&, VertexType::Default*&, int&);
	void RenderBuffers(ID3D11DeviceContext*);

	void Shutdown();

private:
	ID3D11Buffer	*m_dynamicVertexBuffer,
					*m_staticVertexBuffer,
					*m_streamingVertexBuffer,
					*m_dynamicIndexBuffer,
					*m_staticIndexBuffer,
					*m_streamingIndexBuffer;

	int	m_dynamicIndexCount,
		m_staticIndexCount,
		m_streamingIndexCount;

	D3DClass* m_D3D;

	VertexType::Default	*m_dynamicVertices,
						*m_staticVertices,
						*m_streamingVertices;
};

#endif