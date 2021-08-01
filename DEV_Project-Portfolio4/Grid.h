#pragma once
#include "framework.h"
#include "FBXLoader.h"

using namespace DirectX;
using namespace std;

// Grid vertex structure
struct DebugVertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

// Container to store grid vertexes
std::vector <DebugVertex> gridlines;
std::vector <DebugVertex> skeletonBones;

// Function to draw individual gridlines
void AddGridline(XMFLOAT3 firstPosition, XMFLOAT3 secondPosition, XMFLOAT4 firstColor, XMFLOAT4 secondColor)
{
	gridlines.push_back({ firstPosition, firstColor });
	gridlines.push_back({ secondPosition, secondColor });
}

void AddBone(XMFLOAT3 firstPosition, XMFLOAT3 secondPosition, XMFLOAT4 firstColor, XMFLOAT4 secondColor)
{
	skeletonBones.push_back({ firstPosition, firstColor });
	skeletonBones.push_back({ secondPosition, secondColor });
}

void GenerateBonesFromTransforms(std::vector<transformJoint>& transformArray)
{
	for (int i = 0; i < transformArray.size(); i++)
	{
		// int parentIndex = transformArray[i].parent_index;
		DebugVertex parent, newPoint;
		
		if (transformArray[i].parent_index != -1)
		{
			newPoint.position = { transformArray[i].global_transform[12], transformArray[i].global_transform[13], transformArray[i].global_transform[14] };
			newPoint.color = { 1.0f, 1.0f, 1.0f, 1.0f };
			parent.position = { transformArray[transformArray[i].parent_index].global_transform[12],
								transformArray[transformArray[i].parent_index].global_transform[13],
								transformArray[transformArray[i].parent_index].global_transform[14] };
			parent.color = { 1.0f, 1.0f, 1.0f, 1.0f };

			AddBone(newPoint.position, parent.position, newPoint.color, parent.color);
		}
	}
}

void GenerateTriads(std::vector <DebugVertex> bones)
{
	//RGB Triad for joints
	DebugVertex xAxis, yAxis, zAxis;
	xAxis.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	yAxis.color = { 0.0f, 1.0f, 0.0f, 1.0f };
	zAxis.color = { 0.0f, 0.0f, 1.0f, 1.0f };

	for (int i = 0; i < bones.size(); i++)
	{
		xAxis.position = { 1.0f, 0.0f, 0.0f };
		yAxis.position = { 0.0f, 1.0f, 0.0f };
		zAxis.position = { 0.0f, 0.0f, 1.0f };
		
		AddBone(bones[i].position, xAxis.position, xAxis.color, xAxis.color);
		AddBone(bones[i].position, yAxis.position, yAxis.color, yAxis.color);
		AddBone(bones[i].position, zAxis.position, zAxis.color, zAxis.color);
	}
}

// Function to draw the full grid 
void GenerateGrid()
{
	float gridSize = 10.0f;
	float gridSpacing = 0.5f;
	int lineCount = (int)(gridSize / gridSpacing);

	float x = -gridSize / 2.0f;
	float y = -gridSize / 2.0f;
	float xS = gridSpacing, yS = gridSpacing;

	y = -gridSize / 2.0f;
	for (int i = 0; i <= lineCount; i++)
	{
		if (i != lineCount / 2)
			AddGridline({ x,0,y }, { x + gridSize,0,y }, { .5,.5,.5,1 }, { .5,.5,.5,1 });
		else
			AddGridline({ x,0,y }, { x + gridSize,0,y }, { 1,1,1,1 }, { 1,1,1,1 });
		y += yS;
	}

	y = -gridSize / 2.0f;
	x = -gridSize / 2.0f;
	
	for (int i = 0; i <= lineCount; i++)
	{
		if (i != lineCount / 2)
			AddGridline({ x,0,y }, { x ,0,y + gridSize }, { .5,.5,.5,1 }, { .5,.5,.5,1 });
		else
			AddGridline({ x,0,y }, { x ,0,y + gridSize }, { 1,1,1,1 }, { 1,1,1,1 });
		x += xS;
	}
}