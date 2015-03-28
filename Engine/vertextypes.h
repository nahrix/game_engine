#ifndef _VERTEX_TYPES_H_
#define _VERTEX_TYPES_H_

namespace VertexType
{
	#include <d3d11.h>
	#include <dxgi.h>
	#include <D3DX10math.h>

	struct Default
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
}

#endif