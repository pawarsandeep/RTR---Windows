#include <Windows.h>

#include <gl/GL.h>
#include <gl\GLU.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"glu32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variables
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeIsPressed = false;
bool gbFullscreen = false;

// Red Light
GLfloat redLight_ambient[4] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat redLight_diffuse[4] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat redLight_specular[4] = { 1.0f,0.0f,0.0f,0.0f };

// Green Light
GLfloat greenLight_ambient[4] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat greenLight_diffuse[4] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat greenLight_specular[4] = { 0.0f,1.0f,0.0f,1.0f };

// Blue Light
GLfloat blueLight_ambient[4] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat blueLight_diffuse[4] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat blueLight_specular[4] = { 0.0f,0.0f,1.0f,1.0f };

// Light Positions
GLfloat redLigh_position[3] = { 0.0f,0.0f,0.0f };
GLfloat greenLight_position[3] = { 0.0f,0.0f,0.0f };
GLfloat blueLight_position[3] = { 0.0f,0.0f,0.0f };

// Material
GLfloat material_ambient[4] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat material_diffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[4] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;

GLfloat redLightAngle = 0.0f;
GLfloat greenLightAngle = 0.0f;
GLfloat blueLightAngle = 0.0f;

GLUquadric *quadric = NULL;

GLboolean bLightEnabled = GL_FALSE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpsCmdLine, int iCmdShow)
{
	// Function declarations
	void Initialize(void);
	void Animate(void);
	void Display(void);
	void Uninitialize(void);

	// Variable declarations
	WNDCLASSEX wndClass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("myOGL_FFP");
	bool bDone = false;

	// Code
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpszClassName = szClassName;
	wndClass.lpszMenuName = NULL;
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndClass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Fixed Function Pipeline: Lights with Normals"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;
	Initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				if (gbEscapeIsPressed == true)
				{
					bDone = true;
				}
				else
				{
					Animate();
					Display();
				}
			}
		}
	}
	Uninitialize();
	return((int)msg.wParam);
}

// WndProc
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function declarations
	void Resize(int, int);
	void ToggleFullscreen(void);
	void Uninitialize(void);
	// Code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
		break;
	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeIsPressed == false)
				gbEscapeIsPressed = true;
			break;
		case 0x46: // f or F
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		case 0x4C: // l or L
			if (bLightEnabled == GL_TRUE)
			{
				glDisable(GL_LIGHTING);
				bLightEnabled = GL_FALSE;
			}
			else
			{
				glEnable(GL_LIGHTING);
				bLightEnabled = GL_TRUE;
			}
			break;
		case 0x54: // t or T
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_CLOSE:
			Uninitialize();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// Variable declarations
	MONITORINFO mi;

	// Code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void Initialize(void)
{
	// Function declarations
	void Resize(int, int);

	// Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Lighting
	// Red
	glLightfv(GL_LIGHT0, GL_AMBIENT, redLight_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, redLight_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, redLight_specular);
	glEnable(GL_LIGHT0);
	// Green
	glLightfv(GL_LIGHT1, GL_AMBIENT, greenLight_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, greenLight_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, greenLight_specular);
	glEnable(GL_LIGHT1);
	// Blue
	glLightfv(GL_LIGHT2, GL_AMBIENT, blueLight_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, blueLight_specular);
	glEnable(GL_LIGHT2);

	// Material
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

	quadric = gluNewQuadric();

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width > height)
	{
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	}
	else
	{
		gluPerspective(45.0f, (GLfloat)height / (GLfloat)width, 0.1f, 100.0f);
	}
}

void Display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glRotatef(redLightAngle, 1.0f, 0.0f, 0.0f);
	redLigh_position[1] = redLightAngle;
	glLightfv(GL_LIGHT0, GL_POSITION, redLigh_position);
	glPopMatrix();
	glPushMatrix();
	glRotatef(greenLightAngle, 0.0f, 1.0f, 0.0f);
	greenLight_position[0] = greenLightAngle;
	glLightfv(GL_LIGHT1, GL_POSITION, greenLight_position);
	glPopMatrix();
	glPushMatrix();
	glRotatef(blueLightAngle, 0.0f, 0.0f, 1.0f);
	blueLight_position[0] = blueLightAngle;
	glLightfv(GL_LIGHT2, GL_POSITION, greenLight_position);
	glPopMatrix();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 0.75, 15, 15);
	glPopMatrix();
	SwapBuffers(ghdc);
}

void Uninitialize(void)
{
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	gluDeleteQuadric(quadric);
	quadric = NULL;

	DestroyWindow(ghwnd);
}

void Animate(void)
{
	redLightAngle += 1.0f;
	if (redLightAngle > 360.0f)
		redLightAngle -= 360.0f;
	greenLightAngle += 1.0f;
	if (greenLightAngle > 360.0f)
		greenLightAngle -= 360.0f;
	blueLightAngle += 1.0f;
	if (blueLightAngle > 360.0f)
		blueLightAngle -= 360.0f;
}