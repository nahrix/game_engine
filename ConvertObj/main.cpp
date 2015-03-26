////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")


//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <fstream>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
using namespace std;


//////////////
// TYPEDEFS //
//////////////
typedef struct
{
	float x, y, z;
}VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;

struct VertexOutputType
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
	D3DXVECTOR3 normal;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
void GetModelFilename(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);


//////////////////
// MAIN PROGRAM //
//////////////////
int main()
{
	bool result;
	char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;


	// Read in the name of the model file.
	GetModelFilename(filename);

	// Read in the number of vertices, tex coords, normals, and faces so that the data structures can be initialized with the exact sizes needed.
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if(!result)
	{
		return -1;
	}

	// Display the counts to the screen for information purposes.
	cout << endl;
	cout << "Vertices: " << vertexCount << endl;
	cout << "UVs:      " << textureCount << endl;
	cout << "Normals:  " << normalCount << endl;
	cout << "Faces:    " << faceCount << endl;

	// Now read the data from the file into the data structures and then output it in our model format.
	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if(!result)
	{
		return -1;
	}

	// Notify the user the model has been converted.
	cout << "\nFile has been converted." << endl;
	cout << "\nDo you wish to exit (y/n)? ";
	cin >> garbage;

	return 0;
}


void GetModelFilename(char* filename)
{
	bool done;
	ifstream fin;


	// Loop until we have a file name.
	done = false;
	while(!done)
	{
		// Ask the user for the filename.
		cout << "Enter model filename: ";

		// Read in the filename.
		cin >> filename;

		// Attempt to open the file.
		fin.open(filename);

		if(fin.good())
		{
			// If the file exists and there are no problems then exit since we have the file name.
			done = true;
		}
		else
		{
			// If the file does not exist or there was an issue opening it then notify the user and repeat the process.
			fin.clear();
			cout << endl;
			cout << "File " << filename << " could not be opened." << endl << endl;
		}
	}

	return;
}


bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;
    int face;


	// Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while(!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ') { vertexCount++; }
			if(input == 't') { textureCount++; }
			if(input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if(input == 'f')
		{
			fin.get(input);
			if(input == ' ')
            {
				bool space = true;
                face = 0;
                while (input != '\n')
                {
                    fin.get(input);

                    if (input == ' ')
						space = true;
					else if (space && (input != '\n'))
					{
						space = false;
						face++;
					}
					else
						space = false;
                }

                faceCount += (face - 2);
            }
		}
		
		// Otherwise read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}


bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;
    int buf1, buf2, buf3, buf4, buf5, buf6;


	// Initialize the four data structures.
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if(!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if(!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if(!faces)
	{
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if(input == ' ') 
			{ 
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++; 
			}

			// Read in the texture uv coordinates.
			if(input == 't') 
			{ 
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++; 
			}

			// Read in the normals.
			if(input == 'n') 
			{ 
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++; 
			}
		}

		// Read in the faces.
		if(input == 'f') 
		{
			fin.get(input);
			if(input == ' ')
			{
                fin >> buf1 >> input2 >> buf2 >> input2 >> buf3
                    >> buf4 >> input2 >> buf5 >> input2 >> buf6;

                while (input != '\n')
                {
                    faces[faceIndex].vIndex3 = buf1;
                    faces[faceIndex].tIndex3 = buf2;
                    faces[faceIndex].nIndex3 = buf3;
                    faces[faceIndex].vIndex2 = buf4;
                    faces[faceIndex].tIndex2 = buf5;
                    faces[faceIndex].nIndex2 = buf6;

                    fin >> buf4 >> input2 >> buf5 >> input2 >> buf6;

                    faces[faceIndex].vIndex1 = buf4;
                    faces[faceIndex].tIndex1 = buf5;
                    faces[faceIndex].nIndex1 = buf6;

                    faceIndex++;

                    while ((fin.peek() == ' ') || (fin.peek() == '\n'))
                    {
                        fin.get(input);
                    }
                }
			}
		}

		// Read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();


	int* swapRank = new int[faceCount];
	int highestRank, highestIndex;
	FaceType tempFace;

	for (int i = 0; i < faceCount; i++)
	{
		ZeroMemory(swapRank, sizeof(int) * faceCount);
		highestRank = 0;
		highestIndex = 0;

		for (int j = 0; j < faceCount; j++)
		{
			if (i == j)
				continue;

			if ((faces[i].vIndex1 == faces[j].vIndex1) || (faces[i].vIndex1 == faces[j].vIndex2) || (faces[i].vIndex1 == faces[j].vIndex3))
				swapRank[j]++;
			if ((faces[i].vIndex2 == faces[j].vIndex1) || (faces[i].vIndex2 == faces[j].vIndex2) || (faces[i].vIndex2 == faces[j].vIndex3))
				swapRank[j]++;
			if ((faces[i].vIndex3 == faces[j].vIndex1) || (faces[i].vIndex3 == faces[j].vIndex2) || (faces[i].vIndex3 == faces[j].vIndex3))
				swapRank[j]++;
			
			if (swapRank[j] > highestRank)
			{
				highestRank = swapRank[j];
				highestIndex = j;
			}

			if (highestRank >= 2)
				break;
		}

		if (highestRank > 0)
		{
			tempFace = faces[i];
			faces[i] = faces[highestIndex];
			faces[highestIndex] = tempFace;
		}
	}

    vertexCount = faceCount * 3;
    VertexOutputType outputVertex;

	// Open the output file.
	fout.open("model.bin", ios_base::out | ios_base::binary | ios_base::trunc);
    if (fout)
    {
        // Write header data, such as the number of vertices, and indices
        fout.write(reinterpret_cast<const char*>(&vertexCount), sizeof(int));
        fout.write(reinterpret_cast<const char*>(&faceCount), sizeof(int));

        for (int i = 0; i < faceIndex; i++)
        {
            vIndex = faces[i].vIndex1 - 1;
		    tIndex = faces[i].tIndex1 - 1;
		    nIndex = faces[i].nIndex1 - 1;

            outputVertex.position = D3DXVECTOR3(vertices[vIndex].x, vertices[vIndex].y, vertices[vIndex].z);
            outputVertex.texture = D3DXVECTOR2(texcoords[tIndex].x, texcoords[tIndex].y);
            outputVertex.normal = D3DXVECTOR3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z);
            fout.write(reinterpret_cast<const char*>(&outputVertex), sizeof(VertexOutputType));

		    vIndex = faces[i].vIndex2 - 1;
		    tIndex = faces[i].tIndex2 - 1;
		    nIndex = faces[i].nIndex2 - 1;

		    outputVertex.position = D3DXVECTOR3(vertices[vIndex].x, vertices[vIndex].y, vertices[vIndex].z);
            outputVertex.texture = D3DXVECTOR2(texcoords[tIndex].x, texcoords[tIndex].y);
            outputVertex.normal = D3DXVECTOR3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z);
            fout.write(reinterpret_cast<const char*>(&outputVertex), sizeof(VertexOutputType));

		    vIndex = faces[i].vIndex3 - 1;
		    tIndex = faces[i].tIndex3 - 1;
		    nIndex = faces[i].nIndex3 - 1;

		    outputVertex.position = D3DXVECTOR3(vertices[vIndex].x, vertices[vIndex].y, vertices[vIndex].z);
            outputVertex.texture = D3DXVECTOR2(texcoords[tIndex].x, texcoords[tIndex].y);
            outputVertex.normal = D3DXVECTOR3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z);
            fout.write(reinterpret_cast<const char*>(&outputVertex), sizeof(VertexOutputType));
        }
    }
    fout.close();

	// Write out the file header that our model format uses.
	fout << "Vertex Count: " << (faceCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;
    
	// Release the four data structures.
	if(vertices)
	{
		delete [] vertices;
		vertices = 0;
	}
	if(texcoords)
	{
		delete [] texcoords;
		texcoords = 0;
	}
	if(normals)
	{
		delete [] normals;
		normals = 0;
	}
	if(faces)
	{
		delete [] faces;
		faces = 0;
	}

	return true;
}