#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>
#include "clock.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.1415

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
myColor myColorRed = { 1.0f,0.0f,0.0f,0.0f };
myColor myColorGreen = { 0.0f,1.0f,0.0f,0.0f };
myColor myColorBlue = { 0.0f, 0.0f, 1.0f, 0.0f };
myColor myColorWhite = { 1.0f,1.0f,1.0f,0.0f };
myColor myColorBlack = { 0.0f,0.0f,0.0f,0.0f };
int sec;
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

	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Display(void)
{
	// Function
	void DrawThickColoredCircleOutline(float, float, myColor, int);
	void DrawFilledCircle(float, myColor, myColor, int);
	void DrawMarks(float, float, float, myColor, int);
	void DrawMinuteNeedle(float, myColor);
	void DrawHourNeedle(float, myColor);
	void DrawSecondNeedle(float, myColor);
	// Code
	SYSTEMTIME time;
	GetLocalTime(&time);
	int second = time.wSecond;
	float minute = time.wMinute;
	float hour = time.wHour;
	hour = (int)hour % 12;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 110.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawThickColoredCircleOutline(30, 2, myColorBlack, 100);
	DrawFilledCircle(30, myColorBlue, myColorBlack, 100);
	DrawMarks(30.0f, 32.0f, 2.0f, myColorWhite, 60);
	DrawMarks(30.0f, 32.0f, 4.0f, myColorRed, 12);
	//DrawMarks(28.0f, 26.0f, 5.0f, myColorWhite, 12);
	DrawMarks(28.0f, 24.0f, 6.0f, myColorWhite, 4);
	glPushMatrix();
	glRotatef(0.5f*(60.0f*hour+minute), 0.0f, 0.0f, -1.0f);
	DrawHourNeedle(18.0f, myColorWhite);
	glPopMatrix();
	glPushMatrix();
	glRotatef(6.0f * minute, 0.0f, 0.0f, -1.0f);
	DrawMinuteNeedle(22.0f, myColorWhite);
	glPopMatrix();
	glPushMatrix();
	glRotatef(6.0f * second, 0.0f, 0.0f, -1.0f);
	DrawSecondNeedle(24.0f, myColorRed);
	if (second != sec)
	{
		PlaySound(MAKEINTRESOURCE(IDR_TICK), NULL, SND_RESOURCE);
		sec = second;
	}
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
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
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

void DrawThickColoredCircleOutline(float rad, float thickness, myColor color, int slices)
{
	float angle = 0.0f;
	float segments = (float)2 * PI / (float)slices;
	int outerRad = rad + thickness;
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(color.R,color.G,color.B,color.A);
	for (angle; angle < 2.0f*PI; angle += segments)
	{
		glVertex3f(rad*cos(angle), rad*sin(angle), 0.0f);
		glVertex3f(outerRad*cos(angle), outerRad*sin(angle), 0.0f);
	}
	glEnd();
}

void DrawFilledCircle(float rad, myColor innerColor, myColor outerColor, int slices)
{
	float angle = 0.0f;
	float segments = (float)2 * PI / (float)slices;
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(innerColor.R, innerColor.G, innerColor.B, innerColor.A);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor4f(outerColor.R, outerColor.G, outerColor.B, outerColor.A);
	for (angle; angle < 2.0f*PI; angle+=segments)
	{
		glVertex3f(rad*cos(angle), rad*sin(angle), 0.0f);
	}
	glEnd();
}

void DrawMarks(float innerRad, float outerRad, float thickness, myColor color, int numberOfMarks)
{
	float angle = 0.0f;
	float segments = (float)2 * PI / (float)numberOfMarks;
	glLineWidth(thickness);
	glBegin(GL_LINES);
	glColor4f(color.R, color.G, color.B, color.A);
	for (angle; angle < 2.0f*PI; angle += segments)
	{
		glVertex3f(innerRad*cos(angle), innerRad*sin(angle), 0.0f);
		glVertex3f(outerRad*cos(angle), outerRad*sin(angle), 0.0f);
	}
	glEnd();
}

void DrawMinuteNeedle(float size, myColor color)
{
	glBegin(GL_TRIANGLES);
	glColor4f(color.R, color.G, color.B, color.A);
	glVertex3f(size, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-3.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glEnd();
}

void DrawHourNeedle(float size, myColor color)
{
	glBegin(GL_TRIANGLES);
	glColor4f(color.R, color.G, color.B, color.A);
	glVertex3f(size, 0.0f, 0.0f);
	glVertex3f(0.0f, 2.0f, 0.0f);
	glVertex3f(0.0f, -2.0f, 0.0f);
	glVertex3f(-3.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 2.0f, 0.0f);
	glVertex3f(0.0f, -2.0f, 0.0f);
	glColor4f(0.3f, 0.3f, 0.3f, 0.0f);
	/*glVertex3f(size/1.5, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
	glVertex3f(size / 2.4, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.4f, 0.0f);
	glVertex3f(0.0f, -0.4f, 0.0f);*/
	glEnd();
}

void DrawSecondNeedle(float size,myColor color)
{
	void DrawFilledCircle(float, myColor, myColor, int);

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor4f(color.R, color.G, color.B, color.A);
	glVertex3f(size, 0.0f, 0.0f);
	glVertex3f(-3.0f, 0.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(-3.0f, 0.4f, 0.0f);
	glVertex3f(-3.0f, -0.4f, 0.0f);
	glVertex3f(-8.0f, -0.4f, 0.0f);
	glVertex3f(-8.0f, 0.4f, 0.0f);
	glEnd();
	DrawFilledCircle(1.0f, color, color, 30);
}
