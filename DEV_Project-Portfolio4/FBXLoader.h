#pragma once

#include "framework.h"
#include <string>

using namespace std;

// funtime random normal
#define RAND_NORMAL XMFLOAT3(rand()/float(RAND_MAX),rand()/float(RAND_MAX),rand()/float(RAND_MAX))

FbxManager* gSdkManager;
float fbxScale = 1.25f;

struct fbxJoint
{
	FbxNode* node; int parental_index;
};

struct transformJoint
{
	float global_transform[16]; int parent_index;
};

struct keyframe
{
	double time;
	std::vector <FbxAMatrix> joints;
};

struct animClip
{
	double duration;
	std::vector <keyframe> frames;
};

std::vector <fbxJoint> JointArray;
std::vector <transformJoint> TransformJointArray;

void GetMatrixTranforms(FbxNode* node, vector <fbxJoint> joints, vector<transformJoint> xformJoints)
{
	for (size_t i = 0; i < node->GetChildCount(); i++)
	{
		transformJoint newJoint;
		for (size_t i = 0; i < 16; i++)
		{
			// Need to get all 16 numbers out of FBX Matrix into float array
			//0  1  2  3
			//0  1  2  3   0
			//4  5  6  7   1
			//8  9  10 11  2
			//12 13 14 15  3

			int width = 4;
			int row = i / width;
			int col = i % width;
			FbxAMatrix nodeMatrix = joints[i].node->EvaluateGlobalTransform();
			newJoint.global_transform[i] = nodeMatrix[row][col];
			xformJoints.push_back(newJoint);
		}

	}
}

void GetMatrixTranformsKF(FbxNode* node, vector <fbxJoint> joints, keyframe KF, FbxTime time)
{
	for (size_t i = 0; i < node->GetChildCount(); i++)
	{
		for (size_t i = 0; i < 16; i++)
		{
			// Need to get all 16 numbers out of FBX Matrix into float array
			//0  1  2  3
			//0  1  2  3   0
			//4  5  6  7   1
			//8  9  10 11  2
			//12 13 14 15  3

			int width = 4;
			int row = i / width;
			int col = i % width;
			FbxAMatrix nodeMatrix = joints[i].node->EvaluateGlobalTransform(time);
			KF.joints.push_back(nodeMatrix);
		}

	}
}

void GetAnimationData(FbxScene* fbxScene)
{
	FbxPose* bindPose = nullptr;
	int bindIndex;

	int scenePoseCount = fbxScene->GetPoseCount();
	for (size_t i = 0; i < scenePoseCount; i++)
	{
		FbxPose* currentPose = fbxScene->GetPose(i);
		if (currentPose->IsBindPose())
		{
			bindIndex = i;
			bindPose = currentPose;
		}
	}

	int poseCount = bindPose->GetCount();
	int skeletonRoot;
	FbxSkeleton* skelly = nullptr;
	for (size_t i = 0; i < poseCount; i++)
	{
		FbxNode* currentNode = bindPose->GetNode(i);
		if (currentNode->GetSkeleton() != nullptr)
		{
			FbxSkeleton* currentSkelly = currentNode->GetSkeleton();
			if (currentSkelly->IsSkeletonRoot())
			{
				skeletonRoot = i;
				skelly = currentSkelly;
			}
		}
	}

	fbxJoint skellyRoot;
	skellyRoot.node = skelly->GetNode();
	skellyRoot.parental_index = -1;
	JointArray.push_back(skellyRoot);

	FbxNode* rootNode = skelly->GetNode();
	for (size_t i = 0; i < rootNode->GetChildCount(); i++)
	{
		fbxJoint currentJoint;
		FbxNode* currentNode = rootNode;
		if (currentNode->GetSkeleton())
		{
			currentJoint.node = currentNode->GetChild(i);
			currentJoint.parental_index = i;
			JointArray.push_back(currentJoint);
		}
	}

	//GetMatrixTranforms(rootNode, JointArray, TransformJointArray);

	// Get animation stack
	FbxAnimStack* animStack = fbxScene->GetCurrentAnimationStack();

	// Get timespan of the animations
	FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();

	// Get the duration
	FbxTime duration = timeSpan.GetDuration();

	// Get the number of frames in the animation for the desired time mode
	long numFrames = duration.GetFrameCount(FbxTime::EMode::eFrames24);

	// Create animation clip to fill
	animClip baseAnim;

	for (size_t i = 1; i < numFrames; i++)
	{
		keyframe newKF;
		FbxTime keytime;
		keytime.SetFrame(numFrames, FbxTime::EMode::eFrames24);
		newKF.time = keytime.GetSecondCount();

		for (size_t i = 0; i < JointArray.size(); i++)
		{
			fbxJoint* currentJoint;
			currentJoint = &JointArray[i];
			GetMatrixTranformsKF(currentJoint->node, JointArray, newKF, keytime);
			baseAnim.frames.push_back(newKF);
		}
	}
}

void LoadUVInformation(FbxMesh* pMesh, vector<SimpleVertex>& UVstorage)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		// UV Index Counter
		int uv_Index = 0;

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file
					// Copy texture coordinates (already unindexed)
					UVstorage[uv_Index].Tex.x = lUVValue[0];
					UVstorage[uv_Index].Tex.y = 1 - lUVValue[1];
					uv_Index++;
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file
						// Copy texture coordinates (already unindexed)
						UVstorage[uv_Index].Tex.x = lUVValue[0];
						UVstorage[uv_Index].Tex.y = 1 - lUVValue[1];
						uv_Index++;
						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}

// FBX mesh process function declaration 
void ProcessFBXMesh(FbxNode* Node, SimpleMesh<SimpleVertex>& simpleMesh)
{
	int childrenCount = Node->GetChildCount();
	cout << "\nName:" << Node->GetName();

	// Check each child node for a FBX Mesh
	for (int i = 0; i < childrenCount; i++)
	{
		FbxNode* childNode = Node->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();

		// Found a mesh on this node
		if (mesh != NULL)
		{
			cout << "\nMesh:" << childNode->GetName();

			// Get index count from mesh
			int numVertices = mesh->GetControlPointsCount();
			cout << "\nVertex Count:" << numVertices;

			// Resize the vertex vector to size of this mesh
			simpleMesh.vertexList.resize(numVertices);

			//================= Process Vertices ===============
			for (int j = 0; j < numVertices; j++)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				simpleMesh.vertexList[j].Pos.x = (float)vert.mData[0] / fbxScale;
				simpleMesh.vertexList[j].Pos.y = (float)vert.mData[1] / fbxScale;
				simpleMesh.vertexList[j].Pos.z = (float)vert.mData[2] / fbxScale;

				// Generate random normal for first attempt at getting to render
				// simpleMesh.vertexList[j].Normal = RAND_NORMAL;
			}

			int numIndices = mesh->GetPolygonVertexCount();
			cout << "\nIndices Count:" << numIndices;

			// No need to allocate int array, FBX does for us
			int* indices = mesh->GetPolygonVertices();

			// Fill indiceList
			simpleMesh.indexList.resize(numIndices);
			memcpy(simpleMesh.indexList.data(), indices, numIndices * sizeof(int));

			// Get normals Array from the mesh
			FbxArray<FbxVector4> normalsVec;
			mesh->GetPolygonVertexNormals(normalsVec);
			cout << "\nNormalVec Count:" << normalsVec.Size();

			// Declare new vector for the expanded vertex data
			// NOTE: Size is numIndices not numVertices
			vector<SimpleVertex> vertexListExpanded;
			vertexListExpanded.resize(numIndices);

			// Load UV information
			LoadUVInformation(mesh, vertexListExpanded);

			// Align (expand) vertex array and set the normals
			for (int k = 0; k < numIndices; k++)
			{
				// Copy original vertex position to the new vector
				// by using index to look up correct vertex
				// This is the "UNINDEXING" step
				vertexListExpanded[k].Pos.x = simpleMesh.vertexList[indices[k]].Pos.x;
				vertexListExpanded[k].Pos.y = simpleMesh.vertexList[indices[k]].Pos.y;
				vertexListExpanded[k].Pos.z = simpleMesh.vertexList[indices[k]].Pos.z;

				// copy normal data directly; no need to unindex
				vertexListExpanded[k].Normal.x = (float)normalsVec.GetAt(k)[0];
				vertexListExpanded[k].Normal.y = (float)normalsVec.GetAt(k)[1];
				vertexListExpanded[k].Normal.z = (float)normalsVec.GetAt(k)[2];
			}

			// make new indices to match the new vertexListExpanded
			vector<int>indicesList;
			indicesList.resize(numIndices);
			for (int l = 0; l < numIndices; l++)
			{
				indicesList[l] = l; // index == count (literally)
			}

			// copy working data to the global SimpleMesh
			simpleMesh.indexList = indicesList;
			simpleMesh.vertexList = vertexListExpanded;

			//================= Texture ========================================

			int materialCount = childNode->GetSrcObjectCount<FbxSurfaceMaterial>();
			for (int index = 0; index < materialCount; index++)
			{
				FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)childNode->GetSrcObject<FbxSurfaceMaterial>(index);
				if (material != NULL)
				{
					cout << "\nMaterial: " << material->GetName() << endl;

					// Only gets sDiffuse material type; need to traverse Standard Material Property by its name to get all possible textures
					FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

					// Check if Layeredtextures
					int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
					if (layeredTextureCount > 0)
					{
						for (int j = 0; j < layeredTextureCount; j++)
						{
							FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
							int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

							for (int k = 0; k < lcount; k++)
							{
								FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxTexture>(k));

								// Then you can get all texture properties, including name
								const char* textureName = texture->GetFileName();
								cout << textureName << endl;
							}
						}
					}
					else
					{
						// Get textures directly
						int textureCount = prop.GetSrcObjectCount<FbxTexture>();
						for (int j = 0; j < textureCount; j++)
						{
							FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxTexture>(j));

							// Get all texture properties + name
							const char* textureName = texture->GetFileName();
							cout << textureName << endl;
							FbxProperty p = texture->RootProperty.Find("Filename");
							cout << p.Get<FbxString>() << endl;
						}
					}
				}
			}
		}

		// Did not find a mesh so recurse
		else
		{
			ProcessFBXMesh(childNode, simpleMesh);
		}
	}
}

void InitFBX()
{
	gSdkManager = FbxManager::Create();

	// create an IOSettings object
	FbxIOSettings* ios = FbxIOSettings::Create(gSdkManager, IOSROOT);
	gSdkManager->SetIOSettings(ios);
}

string GetFileName(const string& fname)
{
	// Look for '\\'
	char sep = '/';

	size_t i = fname.rfind(sep, fname.length());
	if (i != string::npos)
	{
		return (fname.substr(i + 1, fname.length() - i));
	}

	else
	{
		sep = '\\';
		size_t i = fname.rfind(sep, fname.length());
		if (i != string::npos)
		{
			return (fname.substr(i + 1, fname.length() - i));
		}
	}
	return ("");
}

void LoadFBX(const std::string& filename, SimpleMesh<SimpleVertex>& simpleMesh, std::string& textureFilename)
{
	const char* ImportFileName = filename.c_str();

	// Create a scene
	FbxScene* lScene = FbxScene::Create(gSdkManager, "");

	FbxImporter* lImporter = FbxImporter::Create(gSdkManager, "");

	// Initialize the importer by providing a filename.
//	const bool lImportStatus = lImporter->Initialize(ImportFileName, -1, gSdkManager->GetIOSettings());
	if (!lImporter->Initialize(ImportFileName, -1, gSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Import the scene.
	bool lStatus = lImporter->Import(lScene);

	// Destroy the importer
	lImporter->Destroy();

	// Process the scene and build DirectX Arrays
	ProcessFBXMesh(lScene->GetRootNode(), simpleMesh);

	// Animation
	// GetAnimationData(lScene);

	// Optimize the mesh
	// MeshUtils::Compactify(simpleMesh);

	// Destroy the (no longer needed) scene
	lScene->Destroy();
}


