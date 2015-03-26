////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Models = 0;
	m_LightShader = 0;
    m_TextureShader = 0;
	m_Light = 0;
    m_Bitmaps = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
    m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
	
	// Create the model object.
	m_Models = new unordered_map<string, ModelClass*>;
	if(!m_Models)
	{
		return false;
	}
    
	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

    // Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
    
    // Create the texture shader object.
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }
    
    // Initialize the texture shader object.
    result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd, L"../engine/texture.vs", L"../engine/texture.ps");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
    }
    
	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}
    
    // Create the bitmap object.
	m_Bitmaps = new unordered_map<string, BitmapClass*>;
	if(!m_Bitmaps)
	{
		return false;
	}
    
	// Initialize the light object.
    m_Light->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.8f, -1.0f, 0.1f);
	m_Light->SetSpecularColor(0.85f, 1.0f, 0.98f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
    
	return true;
}

string GraphicsClass::LoadModelResource(char* meshPath, WCHAR* texturePath)
{
    ModelClass* model;
    UUID uuid;
    char* cuuid;
    string suuid;

    // Create the bitmap resource
    model = new ModelClass();
    if (!model->Initialize(m_D3D->GetDevice(), meshPath, texturePath))
    {
		return "error";
    }
    
    // Create a unique identifier for this resource
    UuidCreate(&uuid);
    UuidToStringA(&uuid, (RPC_CSTR*)&cuuid);
    suuid = cuuid;
    RpcStringFreeA((RPC_CSTR*)&cuuid);

    // Store the bitmap, accessible by the unique id
    m_Models->insert(make_pair(suuid, model));

    // Return a copy of the unique id as a string
    return suuid;
}

string GraphicsClass::LoadBitmapResource(WCHAR* filePath, int bitmapWidth, int bitmapHeight)
{
    BitmapClass* bitmap;
    UUID uuid;
    char* cuuid;
    string suuid;

    // Create the bitmap resource
    bitmap = new BitmapClass();
    if (!bitmap->Initialize(m_D3D->GetDevice(), filePath, bitmapWidth, bitmapHeight, m_screenWidth, m_screenHeight))
    {
        return "error";
    }
    
    // Create a unique identifier for this resource
    UuidCreate(&uuid);
    UuidToStringA(&uuid, (RPC_CSTR*)&cuuid);
    suuid = cuuid;
    RpcStringFreeA((RPC_CSTR*)&cuuid);

    // Store the bitmap, accessible by the unique id
    m_Bitmaps->insert(make_pair(suuid, bitmap));

    // Return a copy of the unique id as a string
    return suuid;
}


void GraphicsClass::Shutdown()
{
    // Release all bitmaps
    if (m_Bitmaps)
    {
        for (auto it = m_Bitmaps->begin(); it != m_Bitmaps->end(); it++)
        {
            it->second->Shutdown();
            delete it->second;
            it->second = 0;
        }

        m_Bitmaps->clear();
        delete m_Bitmaps;
        m_Bitmaps = 0;
    }

	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

    // Release the texture shader
    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

	// Release all models
	if (m_Models)
    {
        for (auto it = m_Models->begin(); it != m_Models->end(); it++)
        {
            it->second->Shutdown();
            delete it->second;
            it->second = 0;
        }

        m_Models->clear();
        delete m_Models;
        m_Models = 0;
    }

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

bool GraphicsClass::Frame(float rotationX, float rotationY, float rotationZ, float zoom, float moveX, float moveY)
{
	bool result;
    D3DXVECTOR3 cameraPos;
    
    cameraPos = m_Camera->GetPosition();
    cameraPos.z = zoom;
    cameraPos.x += moveX;
    cameraPos.y += moveY;
    m_Camera->SetPosition(cameraPos.x, cameraPos.y, cameraPos.z);
    
    result = Render(rotationX, rotationY, rotationZ);

    if (!result)
    {
        return false;
    }

	return true;
}

bool GraphicsClass::Render(float rotationX, float rotationY, float rotationZ)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, UIWorldMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
    m_D3D->GetOrthoMatrix(orthoMatrix);
    m_D3D->GetUIWorldMatrix(UIWorldMatrix);

    // Rotate the world matrix by the rotation value so that the triangle will spin.
    D3DXMatrixRotationYawPitchRoll(&worldMatrix, rotationX, rotationY, rotationZ);

    // Put all models and index buffers on the graphics pipeline to prepare them for drawing.
    for (auto it = m_Models->begin(); it != m_Models->end(); it++)
    {
        it->second->Render(m_D3D->GetDeviceContext());
        // Render the model using the light shader.
        result = m_LightShader->Render(m_D3D->GetDeviceContext(), it->second->GetIndexCount(), worldMatrix, viewMatrix,
            projectionMatrix, it->second->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
            m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
        if(!result)
	    {
		    return false;
	    }
    }

    // Disable the Z buffer for 2D rendering
    m_D3D->TurnZBufferOff();

    // Draw every bitmap in the array
    for (auto it = m_Bitmaps->begin(); it != m_Bitmaps->end(); it++)
    {
        // Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
        result = it->second->Render(m_D3D->GetDeviceContext(), 100, 100);
	    if(!result)
	    {
		    return false;
	    }

	    // Render the bitmap with the texture shader.
	    result = m_TextureShader->Render(m_D3D->GetDeviceContext(), it->second->GetIndexCount(), UIWorldMatrix, viewMatrix,
            orthoMatrix, it->second->GetTexture());
	    if(!result)
	    {
		    return false;
	    }
    }
    m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}