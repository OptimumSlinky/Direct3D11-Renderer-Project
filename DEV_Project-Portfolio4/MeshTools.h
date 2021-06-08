#pragma once

#include "framework.h"

using namespace DirectX;
using namespace std;

// Structures
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Normal;
};

template <typename T> struct SimpleMesh
{
	vector<T> vertexList;
	vector<int> indexList;
};

SimpleMesh<SimpleVertex> CreateCube()
{
	SimpleMesh<SimpleVertex> mesh;

	// Cube vertices
	mesh.vertexList =
	{
		// Top
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),	XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f) },

		// Bottom											
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)  },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),	 XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)  },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),	 XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),	 XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f) },

		// Left Face
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		 XMFLOAT2(0.0f, 1.0f),XMFLOAT3(-1.0f, 0.0f, 0.0f)		},
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	 XMFLOAT2(1.0f, 1.0f),XMFLOAT3(-1.0f, 0.0f, 0.0f)		},
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		 XMFLOAT2(1.0f, 0.0f),XMFLOAT3(-1.0f, 0.0f, 0.0f)		},
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		 XMFLOAT2(0.0f, 0.0f),XMFLOAT3(-1.0f, 0.0f, 0.0f)		},

		// Right Face											
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)	},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT2(0.0f, 1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)		},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT2(0.0f, 0.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)		},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)		},

		// Front Face											
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	 XMFLOAT2(0.0f, 1.0f),XMFLOAT3(0.0f, 0.0f, -1.0f)		},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		 XMFLOAT2(1.0f, 1.0f),XMFLOAT3(0.0f, 0.0f, -1.0f)		},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		 XMFLOAT2(1.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, -1.0f)		},
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		 XMFLOAT2(0.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, -1.0f)		},

		// Back Face											
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f),XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};

	// Cube indices
	mesh.indexList =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		11,10,9,

		12,13,14,
		15,12,14,

		16,18,17,
		18,16,19,

		20,21,22,
		20,22,23,
	};

	return mesh;
};