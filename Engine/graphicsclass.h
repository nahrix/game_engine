////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#pragma comment(lib, "rpcrt4.lib")

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "textureshaderclass.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include <unordered_map>
#include <string>


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();

    string LoadBitmapResource(WCHAR*, int, int);
    string LoadModelResource(char*, WCHAR*);

    int getScreenWidth();
    int getScreenHeight();

    void setScreenWidth();
    void setScreenHeight();

	bool Frame(float, float, float, float, float, float);
	bool Render(float, float, float);

public:
	D3DClass* m_D3D;
    int m_screenWidth, m_screenHeight;
	CameraClass* m_Camera;
	LightShaderClass* m_LightShader;
    TextureShaderClass* m_TextureShader;
	LightClass* m_Light;
    unordered_map<string, BitmapClass*>* m_Bitmaps;
    unordered_map<string, ModelClass*>* m_Models;
};

#endif