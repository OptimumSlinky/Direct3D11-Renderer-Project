#pragma once
#include "framework.h"

using namespace DirectX;
using namespace std;

// Grid vertex structure
struct GridVertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

// Container to store grid vertexes
std::vector <GridVertex> gridlines;

// Function to draw individual gridlines
void DrawLine(XMFLOAT3 firstPosition, XMFLOAT3 secondPosition, XMFLOAT4 color)
{
	gridlines.push_back({ firstPosition, color });
	gridlines.push_back({ secondPosition, color });
}

// Function to draw the full grid 
void DrawGrid()
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
			DrawLine({ x,0,y }, { x + gridSize,0,y }, { .5,.5,.5,1 });
		else
			DrawLine({ x,0,y }, { x + gridSize,0,y }, { 1,1,1,1 });
		y += yS;
	}
	y = -gridSize / 2.0f;
	x = -gridSize / 2.0f;
	for (int i = 0; i <= lineCount; i++)
	{
		if (i != lineCount / 2)
			DrawLine({ x,0,y }, { x ,0,y + gridSize }, { .5,.5,.5,1 });
		else
			DrawLine({ x,0,y }, { x ,0,y + gridSize }, { 1,1,1,1 });
		x += xS;
	}
}