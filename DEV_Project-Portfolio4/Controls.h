#pragma once
#include "framework.h"
#include "Definitions.h"

static float t = 0.0f;
float moveScale = 0.0015f;

// Update time
void UpdateTime()
{
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCurrent = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCurrent;
		t = (timeCurrent - timeStart) / 1000.0f;
	}
}

void UpdateAnimations()
{
	for (int currentFrame = 0; currentFrame < mageAnimations.frames.size(); currentFrame++)
	{
		// Move to next animation frame
		if (GetAsyncKeyState('2') && currentFrame <= mageAnimations.frames.size())
		{
			++currentFrame;
			std::vector<transformJoint> nextPose;
			FbxAMatrix keyframeMatrix = mageAnimations.frames[currentFrame].joints[currentFrame];
			int width = 4;
			transformJoint newJoint;

			for (int m = 0; m < 16; m++)
			{
				int row = m / width;
				int col = m % width;
				newJoint.global_transform[m] = keyframeMatrix[row][col];
				newJoint.parent_index = TransformJointArray[currentFrame].parent_index;
			}
			nextPose.push_back(newJoint);
			GenerateBonesFromTransforms(nextPose);
		}

		if (GetAsyncKeyState('1') && currentFrame >= 1)
		{
			// Move to previous animation frame
			--currentFrame;
			std::vector<transformJoint> prevPose;
			FbxAMatrix keyframeMatrix = mageAnimations.frames[currentFrame].joints[currentFrame];
			int width = 4;
			transformJoint newJoint;

			for (int m = 0; m < 16; m++)
			{
				int row = m / width;
				int col = m % width;
				newJoint.global_transform[m] = keyframeMatrix[row][col];
				newJoint.parent_index = TransformJointArray[currentFrame].parent_index;
			}
			prevPose.push_back(newJoint);
			GenerateBonesFromTransforms(prevPose);
		}

	}
}

void UpdateCamera()
{
	if (GetAsyncKeyState('W'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, 0, moveScale);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('S'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, 0, -moveScale);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('A'))
	{
		XMMATRIX temp = XMMatrixTranslation(-moveScale, 0, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('D'))
	{
		XMMATRIX temp = XMMatrixTranslation(moveScale, 0, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('Q'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationZ(-t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('E'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationZ(t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('R'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationX(-t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('F'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationX(t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('T'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, moveScale, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('G'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, -moveScale, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	// Mouse Look Implementation
	// Floats for storing movement deltas
	float deltaX;
	float deltaY;
	float mouseScale = 0.0015f;

	//Gateware black magic
	GW::GReturn result;
	result = MouseLook.GetMouseDelta(deltaY, deltaX);

	if (G_PASS(result) && result != GW::GReturn::REDUNDANT)
	{
		XMMATRIX tempX = XMMatrixRotationX(deltaX * mouseScale);
		g_Camera = XMMatrixMultiply(tempX, g_Camera); // X rotation complete; go do global Y

		// Do global Y here
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX tempY = XMMatrixRotationY(deltaY * mouseScale); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, tempY); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	// Stage 3: Convert updated camera back to View Space
	g_View = XMMatrixInverse(nullptr, g_Camera);
}