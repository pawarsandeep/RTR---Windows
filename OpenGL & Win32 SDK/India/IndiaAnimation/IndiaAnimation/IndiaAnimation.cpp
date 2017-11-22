#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "resource.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

// Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global structure declaration
struct myVertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct myColor
{
	GLfloat R;
	GLfloat G;
	GLfloat B;
	GLfloat A;
};


// Global variable declaration
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
myColor myColorRed = { 1.0f,0.0f,0.0f,1.0f };
myColor myColorGreen = { 0.0f,1.0f,0.0f,1.0f };
myColor myColorBlue = { 0.0f, 0.0f, 1.0f, 1.0f };
myColor myColorWhite = { 1.0f,1.0f,1.0f,1.0f };
myColor myColorSaffron = { 1.0f,0.6f,0.2f, 1.0f };
myColor myColorFlagGreen = { 0.074f,0.533f,0.031f,1.0f };

myVertex animFirstI = { -12.0f,0.0f,0.0f };
myVertex animN = { -4.0f,11.0f,0.0f };
float animD = 0.0f;
myVertex animSecondI = { 3.0f,-11.0f,0.0f };
myVertex animA = { 13.0f,0.0f,0.0f };
float animHorizontalLineLeftX = -12.f;
float animHorizontalLineRightX = -12.0f;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

int giScreenWidth;
int giScreenHeight;

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

// main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function prototype
	void Initialize(void);
	void Uninitialize(void);
	void Display(void);
	void Animate(void);

	// Variable declaration
	WNDCLASSEX wndClass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("RTROGL");
	bool bDone = false;
	giScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	giScreenHeight = GetSystemMetrics(SM_CYSCREEN);


	// Code
	// Initializing members of struct WNDCLASSEX
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = szClassName;
	wndClass.lpszMenuName = NULL;

	// Registering class
	RegisterClassEx(&wndClass);

	// Create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Fixed Function Pipeline Using Native Windowing: Double Buffer Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(giScreenWidth / 2) - 400,
		(giScreenHeight / 2) - 300,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	ghwnd = hwnd;

	// Initialize
	Initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Message loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
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
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
				Animate();
				Display();
			}
		}
	}
	Uninitialize();
	return((int)msg.wParam);
}

// WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function prototype
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
	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 0x46:
			if (gbFullScreen == false)
			{
				ToggleFullscreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullScreen = false;
			}
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// Variable declaration
	MONITORINFO  mi;
	int x, y;

	// Code
	if (gbFullScreen == false)
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
		// Code
		x = (giScreenWidth / 2) - ((wpPrev.rcNormalPosition.right - wpPrev.rcNormalPosition.left) / 2);
		y = (giScreenHeight / 2) - ((wpPrev.rcNormalPosition.bottom - wpPrev.rcNormalPosition.top) / 2);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void Initialize(void)
{
	// Function prototypes
	void Resize(int, int);
	// Variable Declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// Initialization of structure 'PIXELFORMATDESCRIPTOR'
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
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

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Resize(WIN_WIDTH, WIN_HEIGHT);

	PlaySound(MAKEINTRESOURCE(IDR_VANDE_MATARAM), NULL, SND_RESOURCE|SND_ASYNC);
}

void Display(void)
{
	// Function prototype
	void DrawLetterI(void);
	void DrawLetterN(void);
	void DrawLetterD(void);
	void DrawLetterA(void);
	void DrawHorizontalLine();
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -15.0f);

	glPushMatrix();
	glTranslatef(animFirstI.x,animFirstI.y,animFirstI.z);
	DrawLetterI();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(animN.x,animN.y,animN.z);
	DrawLetterN();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	DrawLetterD();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(animSecondI.x,animSecondI.y,animSecondI.z);
	DrawLetterI();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(animA.x,animA.y,animA.z);
	DrawLetterA();
	glPopMatrix();

	glPushMatrix();
	DrawHorizontalLine();
	glPopMatrix();

	SwapBuffers(ghdc);
}

void Resize(int width, int height)
{
	// Code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Uninitialize(void)
{
	// Uninitialization code

	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;
}

void DrawLetterI()
{
	// Function prototype
	void DrawLine(myVertex, myVertex, myColor, myColor, int);

	// Code
	DrawLine({ 0.0f,-4.0f,0.0f }, { 0.0f,4.0f,0.0f }, myColorFlagGreen, myColorSaffron, 5);
}

void DrawLetterN()
{
	// Function prototype
	void DrawLine(myVertex, myVertex, myColor, myColor, int);

	// Code
	DrawLine({ -1.0f,-4.0f,0.0f }, { -1.0f,4.0f,0.0f }, myColorFlagGreen, myColorSaffron, 5);
	DrawLine({ 1.0f,-4.0f,0.0f }, { -1.0f,4.0f,0.0f }, myColorFlagGreen, myColorSaffron, 5);
	DrawLine({ 1.0f,-4.0f,0.0f }, { 1.0f,4.0f,0.0f }, myColorFlagGreen, myColorSaffron, 5);
}

void DrawLetterD()
{
	// Function prototype
	void DrawLine(myVertex, myVertex, myColor, myColor, int);

	// Variable declaration
	myColor saffron = myColorSaffron;
	myColor flagGreen = myColorFlagGreen;

	// Code
	saffron.A = animD;
	flagGreen.A = animD;
	DrawLine({ -1.0f,-4.0f,0.0f }, { -1.0f,4.0f,0.0f }, flagGreen, saffron, 5); // Left vertical line
	DrawLine({ -1.1f,-4.0f,0.0f }, { 1.0f,-4.0f,0.0f }, flagGreen, flagGreen, 5); // Bottom horizontal line
	DrawLine({ -1.1f,4.0f,0.0f }, { 1.0f,4.0f,0.0f }, saffron, saffron, 5); // Top horizontal line
	DrawLine({ 1.0f,-4.05f,0.0f }, { 1.0f,4.05f,0.0f }, flagGreen, saffron, 5); // Right vertical line
}

void DrawLetterA()
{
	// Function prototype
	void DrawLine(myVertex, myVertex, myColor, myColor, int);

	// Code
	DrawLine({ -1.0f,-4.0f,0.0f }, { 0.0f,4.0f,0.0f }, myColorFlagGreen, myColorSaffron, 5); // Right cross line
	DrawLine({ 1.0f,-4.0f,0.0f }, { 0.0f,4.0f,0.0f }, myColorFlagGreen, myColorSaffron, 5); // Left cross line
}

void DrawHorizontalLine()
{
	// Function prototype
	void DrawLine(myVertex, myVertex, myColor, myColor, int);

	DrawLine({ animHorizontalLineLeftX,0.05f,0.0f }, { animHorizontalLineRightX,0.05f,0.0f }, myColorSaffron, myColorSaffron, 3); // Middle horizontal Saffron line
	DrawLine({ animHorizontalLineLeftX,0.0f,0.0f }, { animHorizontalLineRightX,0.0f,0.0f }, myColorWhite, myColorWhite, 3); // Middle horizontal White line
	DrawLine({ animHorizontalLineLeftX,-0.05f,0.0f }, { animHorizontalLineRightX,-0.05f,0.0f }, myColorFlagGreen, myColorFlagGreen, 3); // Middle horizontal Green line
}

void DrawLine(myVertex v1, myVertex v2, myColor c1, myColor c2, int width)
{
	glLineWidth(width);
	glBegin(GL_LINES);
	glColor4f(c1.R, c1.G, c1.B, c1.A);
	glVertex3f(v1.x, v1.y, v1.z);
	glColor4f(c2.R, c2.G, c2.B, c2.A);
	glVertex3f(v2.x, v2.y, v2.z);
	glEnd();
}

void Animate()
{
	if (animFirstI.x < -7.0f)
	{
		animFirstI.x += 0.001f;
	}
	else if (animN.y > 0.0f)
	{
		animN.y -= 0.001f;
	}
	else if (animD < 1.0f)
	{
		animD += 0.0005f;
	}
	else if (animSecondI.y < 0.0f)
	{
		animSecondI.y += 0.001f;
	}
	else if (animA.x > 6.0f)
	{
		animA.x -= 0.001f;
	}
	else if (animHorizontalLineRightX < 6.5f)
	{
		animHorizontalLineRightX += 0.005;
	}
	else if (animHorizontalLineLeftX < 5.5f)
	{
		animHorizontalLineLeftX += 0.005;
	}
}