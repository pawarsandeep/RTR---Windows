#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include "timer_helper.h"
#include "Resources.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define cameraTransitionCounter 0.01f
#define upvectorTransitionCounter 0.0006f

#define PI 3.1415926535898f
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")


// Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void freeBufferMemory(struct Buffer *);
void initializeBufferPool(void);
void initializeFreeList(void);

// Global variable declaration
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
FILE *gfpFile = NULL;
int giScreenWidth;
int giScreenHeight;
int gislideIndex = 0;
int cameraPositionTracker = 1;  //used to track the camera position count in order to make desired modifications in buffer pool
bool sceneOneBufferMarkedBusy = false, sceneThreeTargetBufferMoved = false;
bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;
GLfloat gfCameraAngle = 0.0f, gfPositionTimer = 0.0f;
GLuint igTexCircuit, slides[9], freeTex[100], lockedTex[100], delayedWrite[100], queueHeaders[5];

//gluLookAt structure
struct LookAtStruct {
	struct Camera {
		GLdouble x, y, z;
		int reachedX, reachedY, reachedZ; //0 = did not reach the camera position, 1 = reached the camera position
	} camera;

	GLfloat counter;
	struct Center {
		GLdouble x, y, z;
		int reachedX, reachedY, reachedZ; //0 = did not reach the camera position, 1 = reached the camera position
	} center;

	struct UpVector {
		GLdouble x, y, z;
		int reachedX, reachedY, reachedZ; //0 = did not reach the camera position, 1 = reached the camera position
	} upvector;
	bool positionFlag;
	bool isSlidePosition;
	struct LookAtStruct *nextPosition;
} *positionListHead, *currentPosition, *tracker;



//buffer structure
struct Buffer {
	int value;           //buffer content
	int state;           //0 = on free list, 1 = on hash queue, 2 = is marked for delayed write

						 //translation matrix  for drawing the buffer
	GLfloat translateX;
	GLfloat translateY;
	GLfloat translateZ;

	//next and previous buffer links
	struct Buffer *nextBuffer;
	struct Buffer *prevBuffer;
} *firstHashQueueHead, *secondHashQueueHead, *thirdHashQueueHead, *fourthHashQueueHead, *freeListHead, *sceneTwoTargetBuffer,
*sceneThreeDelayedWriteBufferOne, *sceneThreeDelayedWriteBufferTwo, *sceneThreeTargetBuffer;

// main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function prototype
	void Initialize(void);
	void Uninitialize(void);
	void Display(void);
	void moveCamera(void);


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
				moveCamera();
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
	void initializeCameraPositionList(void);
	int LoadGLTextures(GLuint *, TCHAR[]);
	// Variable Declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	fopen_s(&gfpFile, "temp.txt", "w");
	// Initialization of structure 'PIXELFORMATDESCRIPTOR'
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
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

	initializeBufferPool();

	//initialize free list
	initializeFreeList();

	sceneTwoTargetBuffer = NULL;
	//initialize camera position list
	initializeCameraPositionList();
	//set camera at top view
	currentPosition = positionListHead;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// Load textures
	LoadGLTextures(&igTexCircuit, MAKEINTRESOURCE(IDBITMAP_CIRCUIT));
	/*LoadGLTextures(&slides[0], MAKEINTRESOURCE(IDBITMAP_SCENE_01));
	LoadGLTextures(&slides[1], MAKEINTRESOURCE(IDBITMAP_SCENE_02));
	LoadGLTextures(&slides[2], MAKEINTRESOURCE(IDBITMAP_SCENE_03));
	LoadGLTextures(&slides[3], MAKEINTRESOURCE(IDBITMAP_SCENE_04));
	LoadGLTextures(&slides[4], MAKEINTRESOURCE(IDBITMAP_SCENE_05));
	LoadGLTextures(&slides[5], MAKEINTRESOURCE(IDBITMAP_SCENE_HEADER));
	LoadGLTextures(&slides[6], MAKEINTRESOURCE(IDBITMAP_BUFFER_CACHE));
	LoadGLTextures(&slides[7], MAKEINTRESOURCE(IDBITMAP_BUFFER_HEADER));
	LoadGLTextures(&slides[8], MAKEINTRESOURCE(IDBITMAP_INDICES));*/

	LoadGLTextures(&slides[1], MAKEINTRESOURCE(IDBITMAP_SCENE_01));
	LoadGLTextures(&slides[2], MAKEINTRESOURCE(IDBITMAP_SCENE_02));
	LoadGLTextures(&slides[3], MAKEINTRESOURCE(IDBITMAP_SCENE_03));
	LoadGLTextures(&slides[4], MAKEINTRESOURCE(IDBITMAP_SCENE_04));
	LoadGLTextures(&slides[5], MAKEINTRESOURCE(IDBITMAP_SCENE_05));
	LoadGLTextures(&slides[6], MAKEINTRESOURCE(IDBITMAP_SCENE_HEADER));
	LoadGLTextures(&slides[0], MAKEINTRESOURCE(IDBITMAP_BUFFER_CACHE));
	LoadGLTextures(&slides[7], MAKEINTRESOURCE(IDBITMAP_BUFFER_HEADER));
	LoadGLTextures(&slides[8], MAKEINTRESOURCE(IDBITMAP_INDICES));

	LoadGLTextures(&lockedTex[3], MAKEINTRESOURCE(IDBITMAP_3_LOCKED));
	LoadGLTextures(&lockedTex[4], MAKEINTRESOURCE(IDBITMAP_4_LOCKED));
	LoadGLTextures(&lockedTex[5], MAKEINTRESOURCE(IDBITMAP_5_LOCKED));
	LoadGLTextures(&lockedTex[10], MAKEINTRESOURCE(IDBITMAP_10_LOCKED));
	LoadGLTextures(&lockedTex[17], MAKEINTRESOURCE(IDBITMAP_17_LOCKED));
	LoadGLTextures(&lockedTex[18], MAKEINTRESOURCE(IDBITMAP_18_LOCKED));
	LoadGLTextures(&lockedTex[28], MAKEINTRESOURCE(IDBITMAP_28_LOCKED));
	LoadGLTextures(&lockedTex[35], MAKEINTRESOURCE(IDBITMAP_35_LOCKED));
	LoadGLTextures(&lockedTex[50], MAKEINTRESOURCE(IDBITMAP_50_LOCKED));
	LoadGLTextures(&lockedTex[64], MAKEINTRESOURCE(IDBITMAP_64_LOCKED));
	LoadGLTextures(&lockedTex[97], MAKEINTRESOURCE(IDBITMAP_97_LOCKED));
	LoadGLTextures(&lockedTex[98], MAKEINTRESOURCE(IDBITMAP_98_LOCKED));
	LoadGLTextures(&lockedTex[99], MAKEINTRESOURCE(IDBITMAP_99_LOCKED));

	LoadGLTextures(&freeTex[3], MAKEINTRESOURCE(IDBITMAP_3_FREE));
	LoadGLTextures(&freeTex[4], MAKEINTRESOURCE(IDBITMAP_4_FREE));
	LoadGLTextures(&freeTex[5], MAKEINTRESOURCE(IDBITMAP_5_FREE));
	LoadGLTextures(&freeTex[10], MAKEINTRESOURCE(IDBITMAP_10_FREE));
	LoadGLTextures(&freeTex[17], MAKEINTRESOURCE(IDBITMAP_17_FREE));
	LoadGLTextures(&freeTex[18], MAKEINTRESOURCE(IDBITMAP_18_FREE));
	LoadGLTextures(&freeTex[28], MAKEINTRESOURCE(IDBITMAP_28_FREE));
	LoadGLTextures(&freeTex[35], MAKEINTRESOURCE(IDBITMAP_35_FREE));
	LoadGLTextures(&freeTex[50], MAKEINTRESOURCE(IDBITMAP_50_FREE));
	LoadGLTextures(&freeTex[64], MAKEINTRESOURCE(IDBITMAP_64_FREE));
	LoadGLTextures(&freeTex[97], MAKEINTRESOURCE(IDBITMAP_97_FREE));
	LoadGLTextures(&freeTex[98], MAKEINTRESOURCE(IDBITMAP_98_FREE));
	LoadGLTextures(&freeTex[99], MAKEINTRESOURCE(IDBITMAP_99_FREE));

	LoadGLTextures(&delayedWrite[3], MAKEINTRESOURCE(IDBITMAP_3_DELAYED));
	LoadGLTextures(&delayedWrite[4], MAKEINTRESOURCE(IDBITMAP_4_DELAYED));
	LoadGLTextures(&delayedWrite[5], MAKEINTRESOURCE(IDBITMAP_5_DELAYED));
	LoadGLTextures(&delayedWrite[10], MAKEINTRESOURCE(IDBITMAP_10_DELAYED));
	LoadGLTextures(&delayedWrite[17], MAKEINTRESOURCE(IDBITMAP_17_DELAYED));
	LoadGLTextures(&delayedWrite[18], MAKEINTRESOURCE(IDBITMAP_18_DELAYED));
	LoadGLTextures(&delayedWrite[28], MAKEINTRESOURCE(IDBITMAP_28_DELAYED));
	LoadGLTextures(&delayedWrite[35], MAKEINTRESOURCE(IDBITMAP_35_DELAYED));
	LoadGLTextures(&delayedWrite[50], MAKEINTRESOURCE(IDBITMAP_50_DELAYED));
	LoadGLTextures(&delayedWrite[64], MAKEINTRESOURCE(IDBITMAP_64_DELAYED));
	LoadGLTextures(&delayedWrite[97], MAKEINTRESOURCE(IDBITMAP_97_DELAYED));
	LoadGLTextures(&delayedWrite[98], MAKEINTRESOURCE(IDBITMAP_98_DELAYED));
	LoadGLTextures(&delayedWrite[99], MAKEINTRESOURCE(IDBITMAP_99_DELAYED));

	LoadGLTextures(&queueHeaders[0], MAKEINTRESOURCE(IDBITMAP_BLOCKNO_0));
	LoadGLTextures(&queueHeaders[1], MAKEINTRESOURCE(IDBITMAP_BLOCKNO_1));
	LoadGLTextures(&queueHeaders[2], MAKEINTRESOURCE(IDBITMAP_BLOCKNO_2));
	LoadGLTextures(&queueHeaders[3], MAKEINTRESOURCE(IDBITMAP_BLOCKNO_3));
	LoadGLTextures(&queueHeaders[4], MAKEINTRESOURCE(IDBITMAP_FREE_LIST_HEADER));
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void initializeCameraPositionList() {
	struct LookAtStruct *getCameraPosition(struct LookAtStruct *currentPosition, GLfloat cameraX,
		GLfloat cameraY, GLfloat cameraZ,
		GLfloat centerX, GLfloat centerY,
		GLfloat centerZ, GLfloat upvectorX,
		GLfloat upvectorY, GLfloat upVectorZ, GLfloat);
	positionListHead = (struct LookAtStruct *)malloc(sizeof(LookAtStruct));
	positionListHead->nextPosition = NULL;
	positionListHead->camera.x = positionListHead->camera.y = positionListHead->center.x = positionListHead->center.y = positionListHead->upvector.x = positionListHead->upvector.z = 0.0f;
	positionListHead->camera.z = -29.0f;
	positionListHead->upvector.y = 1.0f;
	positionListHead->center.z = -40.0f;
	positionListHead->positionFlag = false;
	positionListHead->isSlidePosition = true;
	positionListHead->counter = 10.0f;
	positionListHead->camera.reachedX = positionListHead->camera.reachedY = positionListHead->camera.reachedZ =
		positionListHead->center.reachedX = positionListHead->center.reachedY = positionListHead->center.reachedZ =
		positionListHead->upvector.reachedX = positionListHead->upvector.reachedY = positionListHead->upvector.reachedZ = 1;

	currentPosition = positionListHead;


	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	//positions for traversing hash queue
	//middle side view 2
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//first hash queue side view 3
	currentPosition = getCameraPosition(currentPosition, -25.0f, 6.0f, -20.0f, 0.0f, 6.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//till the end of first hash queue 4
	currentPosition = getCameraPosition(currentPosition, 2.0f, 6.0f, -20.0f, 27.0f, 6.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//reset to side middle view 5
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//second hash queue side view 6
	currentPosition = getCameraPosition(currentPosition, -25.0f, 2.0f, -20.0f, 2.0f, 2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//end of second hash queue 7
	currentPosition = getCameraPosition(currentPosition, 2.0f, 2.0f, -20.0f, 27.0f, 2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//reset to side middle view 8
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//third hash queue side view 9
	currentPosition = getCameraPosition(currentPosition, -25.0f, -2.0f, -20.0f, -2.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//end of third hash queue 10
	currentPosition = getCameraPosition(currentPosition, 2.0f, -2.0f, -20.0f, 27.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//reset to side middle view 11
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//fourth hash queue side view 12
	currentPosition = getCameraPosition(currentPosition, -25.0f, -6.0f, -20.0f, -2.0f, -6.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//end of fourth hash queue 13
	currentPosition = getCameraPosition(currentPosition, 2.0f, -6.0f, -20.0f, 27.0f, -6.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//freelist traversal starts from here 
	//go to top view 14
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	//side from free list header 15
	currentPosition = getCameraPosition(currentPosition, 0.0f, -25.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//point it to free list 16
	currentPosition = getCameraPosition(currentPosition, -2.0f, -25.0f, -20.0f, -2.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//17
	currentPosition = getCameraPosition(currentPosition, -2.0f, -10.0f, -20.0f, -2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//18
	currentPosition = getCameraPosition(currentPosition, 0.0f, -10.0f, -20.0f, 0.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//19
	currentPosition = getCameraPosition(currentPosition, 0.0f, -5.0f, -20.0f, 0.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//20
	currentPosition = getCameraPosition(currentPosition, 2.0f, -5.0f, -20.0f, 2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//21
	currentPosition = getCameraPosition(currentPosition, 2.0f, 5.0f, -20.0f, 2.0f, 20.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//come back 22 
	currentPosition = getCameraPosition(currentPosition, 2.0f, 0.0f, -20.0f, 2.0f, 15.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//move left 23
	currentPosition = getCameraPosition(currentPosition, -2.0f, 0.0f, -20.0f, -2.0f, 15.0f, -25.0f, 0.0f, 0.0f, 1.0f, 5.0f);
	//come back to 24
	currentPosition = getCameraPosition(currentPosition, 2.0f, -5.0f, -20.0f, 2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//25
	currentPosition = getCameraPosition(currentPosition, 6.0f, -5.0f, -20.0f, 6.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//26
	currentPosition = getCameraPosition(currentPosition, 6.0f, -10.0f, -20.0f, 6.0f, 5.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//back to top view 27
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);

	//scene 1 positions start from here
	//middle side view 28
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//left side view 29
	currentPosition = getCameraPosition(currentPosition, -25.0f, 6.0f, -20.0f, 0.0f, 6.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//3rd buffer in first hash queue from left side view. Change the buffer color to red after camera reaches this position 30
	currentPosition = getCameraPosition(currentPosition, -5.0f, 6.0f, -20.0f, 10.0f, 6.0f, -25.0f, 0.0f, 0.0f, 1.0f, 15.0f);
	//last position i.e. pos 1 (back to top view) 31
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 15.0f);


	//scene 2 positions start from here
	//middle side view 32
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//third hash queue 33
	currentPosition = getCameraPosition(currentPosition, -25.0f, -2.0f, -20.0f, 0.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//till the end of third hash queue 34
	currentPosition = getCameraPosition(currentPosition, 0.0f, -2.0f, -20.0f, 15.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//35
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	//side from free list header 36
	currentPosition = getCameraPosition(currentPosition, 0.0f, -25.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//point it to free list 37
	currentPosition = getCameraPosition(currentPosition, -2.0f, -25.0f, -20.0f, -2.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//point till first free buffer 38
	currentPosition = getCameraPosition(currentPosition, -2.0f, -15.0f, -20.0f, -2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//come back a little. Now move the free buffer from free list to hash queue 39
	currentPosition = getCameraPosition(currentPosition, -2.0f, -25.0f, -20.0f, -2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 30.0f);
	//40
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);

	//scene 3 positions start from here
	//moddle side view 41
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//third hash queue 42
	currentPosition = getCameraPosition(currentPosition, -25.0f, -2.0f, -20.0f, 0.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//till the end of third hash queue 43
	currentPosition = getCameraPosition(currentPosition, 0.0f, -2.0f, -20.0f, 15.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//44
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	//side from free list header 45
	currentPosition = getCameraPosition(currentPosition, 0.0f, -25.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//point it to free list 46
	currentPosition = getCameraPosition(currentPosition, -2.0f, -25.0f, -20.0f, -2.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//now start traversing till the free buffer is found i.e. green buffer 47
	currentPosition = getCameraPosition(currentPosition, -2.0f, -10.0f, -20.0f, -2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//48
	currentPosition = getCameraPosition(currentPosition, 0.0f, -10.0f, -20.0f, 0.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//49
	currentPosition = getCameraPosition(currentPosition, 0.0f, -5.0f, -20.0f, 0.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//50
	currentPosition = getCameraPosition(currentPosition, 2.0f, -5.0f, -20.0f, 2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//51
	currentPosition = getCameraPosition(currentPosition, 2.0f, 0.0f, -20.0f, 2.0f, 10.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//side from free list header 52
	currentPosition = getCameraPosition(currentPosition, 0.0f, -25.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 30.0f);
	//53
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);

	//scene 4 positions start from here
	//moddle side view 54
	currentPosition = getCameraPosition(currentPosition, -25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//third hash queue 55
	currentPosition = getCameraPosition(currentPosition, -25.0f, -2.0f, -20.0f, 0.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//till the end of third hash queue 56
	currentPosition = getCameraPosition(currentPosition, 0.0f, -2.0f, -20.0f, 15.0f, -2.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//57
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	//side from free list header 58
	currentPosition = getCameraPosition(currentPosition, 0.0f, -25.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//point it to free list 59
	currentPosition = getCameraPosition(currentPosition, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f);


}

struct LookAtStruct *getCameraPosition(struct LookAtStruct *currentPosition, GLfloat cameraX,
	GLfloat cameraY, GLfloat cameraZ,
	GLfloat centerX, GLfloat centerY,
	GLfloat centerZ, GLfloat upvectorX,
	GLfloat upvectorY, GLfloat upVectorZ, GLfloat positionTimer) {
	struct LookAtStruct *position = (struct LookAtStruct*)malloc(sizeof(LookAtStruct));
	position->camera.x = cameraX;
	position->camera.y = cameraY;
	position->camera.z = cameraZ;
	position->center.x = centerX;
	position->center.y = centerY;
	position->center.z = centerZ;
	position->upvector.x = upvectorX;
	position->upvector.y = upvectorY;
	position->upvector.z = upVectorZ;
	position->positionFlag = false;
	position->counter = positionTimer;
	position->isSlidePosition = false;
	position->camera.reachedX = position->camera.reachedY = position->camera.reachedZ =
		position->center.reachedX = position->center.reachedY = position->center.reachedZ =
		position->upvector.reachedX = position->upvector.reachedY = position->upvector.reachedZ = 1;
	position->nextPosition = NULL;
	currentPosition->nextPosition = position;
	currentPosition = position;
	return currentPosition;
}

void initializeFreeList() {
	struct Buffer * getFreeListBuffer(struct Buffer *, GLfloat, GLfloat);

	//free list header
	freeListHead = (struct Buffer *)malloc(sizeof(Buffer));
	freeListHead->nextBuffer = freeListHead->prevBuffer = NULL;
	freeListHead->translateX = -2.0f;
	freeListHead->translateY = -9.0f;
	freeListHead->translateZ = -25.0f;
	if (cameraPositionTracker != 53) {

		struct Buffer *lastBuffer = NULL;
		lastBuffer = freeListHead;

		//first corner point
		lastBuffer = getFreeListBuffer(lastBuffer, -2.0f, -4.0f);

		//second corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 0.0f, -4.0f);

		//third corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 0.0f, 0.0f);

		//fourth corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 2.0f, 0.0f);

		//fifth corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 2.0f, 8.0f);

		//sixth corner point
		lastBuffer = getFreeListBuffer(lastBuffer, -2.0f, 8.0f);

		//seventh corner point
		lastBuffer = getFreeListBuffer(lastBuffer, -2.0f, 4.0f);

		//eighth corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 6.0f, 4.0f);

		//ninth corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 6.0f, -4.0f);

		//tenth corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 8.0f, -4.0f);

		//eleventh corner point
		lastBuffer = getFreeListBuffer(lastBuffer, 8.0f, -9.0f);

		//last point
		lastBuffer = getFreeListBuffer(lastBuffer, -2.0f, -9.0f);
	}
}

//returns an instance of buffer by attaching it after the given last buffer
struct Buffer * getFreeListBuffer(struct Buffer *lastBuffer, GLfloat xOffset, GLfloat yOffset) {
	struct Buffer *buffer = (struct Buffer *)malloc(sizeof(Buffer));
	buffer->nextBuffer = buffer->prevBuffer = NULL;
	buffer->translateX = xOffset;
	buffer->translateY = yOffset;
	buffer->translateZ = -25.0f;
	lastBuffer->nextBuffer = buffer;
	buffer->prevBuffer = lastBuffer;
	lastBuffer = buffer;
	return lastBuffer;
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

	//free up the memory allocated to buffer pool
	freeBufferMemory(firstHashQueueHead);
	freeBufferMemory(secondHashQueueHead);
	freeBufferMemory(thirdHashQueueHead);
	freeBufferMemory(fourthHashQueueHead);
	freeBufferMemory(freeListHead);
	//freeCameraPositionList();
	fclose(gfpFile);
}

void freeBufferMemory(struct Buffer *head) {
	if (head != NULL) {
		struct Buffer * buffer = head;
		while (firstHashQueueHead != NULL) {
			buffer = firstHashQueueHead;
			firstHashQueueHead = firstHashQueueHead->nextBuffer;
			free(buffer);
			buffer->nextBuffer = NULL;
			buffer->prevBuffer = NULL;
			buffer = NULL;
		}
	}
}

void Display(void)
{
	// Function declaration
	void drawBufferPool();
	void drawFreeList();
	void modifySceneOneFreeList();
	void drawBuffer(struct Buffer*);
	void DrawContainer(GLfloat, GLuint);
	void DrawSlides();

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DrawContainer(52.0f, igTexCircuit);
	DrawSlides();
	drawFreeList();
	drawBufferPool();

	if (sceneThreeDelayedWriteBufferTwo != NULL)
		drawBuffer(sceneThreeDelayedWriteBufferTwo);
	if (sceneThreeDelayedWriteBufferOne != NULL)
		drawBuffer(sceneThreeDelayedWriteBufferOne);
	if (sceneThreeTargetBuffer != NULL) {
		drawBuffer(sceneThreeTargetBuffer);
	}
	if (sceneTwoTargetBuffer != NULL)
		drawBuffer(sceneTwoTargetBuffer);
	SwapBuffers(ghdc);
}



void drawBuffer(struct Buffer * currentBuffer) {
	gluLookAt(currentPosition->camera.x, currentPosition->camera.y, currentPosition->camera.z, currentPosition->center.x, currentPosition->center.y, currentPosition->center.z, currentPosition->upvector.x, currentPosition->upvector.y, currentPosition->upvector.z);
	//gluLookAt(-10.0f, 5.0f, -20.0f, 10.0f, 7.0f, -30.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(currentBuffer->translateX, currentBuffer->translateY, currentBuffer->translateZ);

	glBegin(GL_QUADS);
	//use different color grading for hash queue header, free buffer and busy buffer
	//front
	if (currentBuffer->state == 0)
		glColor3f(0.0f, 0.7f, 0.0f);
	if (currentBuffer->state == 1)
		glColor3f(0.7f, 0.0f, 0.0f);
	if (currentBuffer->state == 2)
		glColor3f(0.7f, 0.7f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	//right face
	if (currentBuffer->state == 0)
		glColor3f(0.0f, 0.35f, 0.0f);
	if (currentBuffer->state == 1)
		glColor3f(0.35f, 0.0f, 0.0f);
	if (currentBuffer->state == 2)
		glColor3f(0.35f, 0.35f, 0.0f);
	glVertex3f(0.6f, 0.6f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(0.6f, -0.6f, -1.0f);

	//back face
	if (currentBuffer->state == 0)
		glColor3f(0.0f, 0.35f, 0.0f);
	if (currentBuffer->state == 1)
		glColor3f(0.35f, 0.0f, 0.0f);
	if (currentBuffer->state == 2)
		glColor3f(0.35f, 0.35f, 0.0f);
	glVertex3f(0.6f, 0.6f, -1.0f);
	glVertex3f(-0.6f, 0.6f, -1.0f);
	glVertex3f(-0.6f, -0.6f, -1.0f);
	glVertex3f(0.6f, -0.6f, -1.0f);


	//left face
	if (currentBuffer->state == 0)
		glColor3f(0.0f, 0.35f, 0.0f);
	if (currentBuffer->state == 1)
		glColor3f(0.35f, 0.0f, 0.0f);
	if (currentBuffer->state == 2)
		glColor3f(0.35f, 0.35f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-0.6f, 0.6f, -1.0f);
	glVertex3f(-0.6f, -0.6f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	//top face
	if (currentBuffer->state == 0)
		glColor3f(0.0f, 0.35f, 0.0f);
	if (currentBuffer->state == 1)
		glColor3f(0.35f, 0.0f, 0.0f);
	if (currentBuffer->state == 2)
		glColor3f(0.35f, 0.35f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.6f, 0.6f, -1.0f);
	glVertex3f(-0.6f, 0.6f, -1.0f);

	//bottom face
	if (currentBuffer->state == 0)
		glColor3f(0.0f, 0.35f, 0.0f);
	if (currentBuffer->state == 1)
		glColor3f(0.35f, 0.0f, 0.0f);
	if (currentBuffer->state == 2)
		glColor3f(0.35f, 0.35f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-0.6f, -0.6f, -1.0f);
	glVertex3f(0.6f, -0.6f, -1.0f);
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void drawFreeList() {
	struct Buffer * buffer = freeListHead;
	bool freeListHeader = TRUE;
	do {

		if (freeListHeader) {
			gluLookAt(currentPosition->camera.x, currentPosition->camera.y, currentPosition->camera.z, currentPosition->center.x, currentPosition->center.y, currentPosition->center.z, currentPosition->upvector.x, currentPosition->upvector.y, currentPosition->upvector.z);
			freeListHeader = FALSE;
			glTranslatef(buffer->translateX, buffer->translateY, buffer->translateZ);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, queueHeaders[4]);
			glBegin(GL_QUADS);
			//front
			/*glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			*/
			
			//use different color grading for hash queue header, free buffer and busy buffer
			//front


			glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
			//right face
			glColor3f(0.0f, 0.7f, 0.0f);
			glVertex3f(0.6f, 0.6f, -1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(0.6f, -0.6f, -1.0f);

			//back face
			glColor3f(0.0f, 0.7f, 0.0f);
			glVertex3f(0.6f, 0.6f, -1.0f);
			glVertex3f(-0.6f, 0.6f, -1.0f);
			glVertex3f(-0.6f, -0.6f, -1.0f);
			glVertex3f(0.6f, -0.6f, -1.0f);


			//left face
			glColor3f(0.0f, 0.7f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-0.6f, 0.6f, -1.0f);
			glVertex3f(-0.6f, -0.6f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);

			//top face
			glColor3f(0.0f, 0.7f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(0.6f, 0.6f, -1.0f);
			glVertex3f(-0.6f, 0.6f, -1.0f);

			//bottom face
			glColor3f(0.0f, 0.7f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(-0.6f, -0.6f, -1.0f);
			glVertex3f(0.6f, -0.6f, -1.0f);
			glEnd();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

		}
		if (buffer->nextBuffer != NULL) {
			gluLookAt(currentPosition->camera.x, currentPosition->camera.y, currentPosition->camera.z, currentPosition->center.x, currentPosition->center.y, currentPosition->center.z, currentPosition->upvector.x, currentPosition->upvector.y, currentPosition->upvector.z);
			//gluLookAt(-10.0f, 5.0f, -20.0f, 10.0f, 7.0f, -30.0f, 0.0f, 0.0f, 1.0f);
			glLineWidth(5.0f);
			glColor3f(0.0f, 0.7f, 0.0f);
			glBegin(GL_LINES);
			glVertex3f(buffer->translateX, buffer->translateY, buffer->translateZ);
			glVertex3f(buffer->nextBuffer->translateX, buffer->nextBuffer->translateY, buffer->nextBuffer->translateZ);
			glEnd();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			buffer = buffer->nextBuffer;
		}
	} while (buffer->nextBuffer != NULL);
}

void drawBufferPool()
{
	void drawHashQueue(struct Buffer *);
	fprintf(gfpFile, "Head %d\n", firstHashQueueHead->value);
	drawHashQueue(firstHashQueueHead);
	drawHashQueue(secondHashQueueHead);
	drawHashQueue(thirdHashQueueHead);
	drawHashQueue(fourthHashQueueHead);
}

void modifySceneOneFreeList() {

	struct Buffer * getFreeListBuffer(struct Buffer *, GLfloat, GLfloat);
	struct Buffer *currentFreeListNode = freeListHead, *nodeTracker, *hashQueueTracker = firstHashQueueHead;
	int nodeCount = 1;

	//change state of the buffer
	while (hashQueueTracker->value != 4) {
		hashQueueTracker = hashQueueTracker->nextBuffer;
	}
	hashQueueTracker->state = 1;
	//go to 5th corner point
	while (nodeCount <= 5) {
		currentFreeListNode = currentFreeListNode->nextBuffer;
		nodeCount++;
	}
	currentFreeListNode->translateX = 2.0f;
	currentFreeListNode->translateY = 4.0f;

	currentFreeListNode = currentFreeListNode->nextBuffer;
	currentFreeListNode->translateX = -2.0f;
	currentFreeListNode->translateY = 4.0f;

	currentFreeListNode = currentFreeListNode->nextBuffer;
	currentFreeListNode->translateX = -2.0f;
	currentFreeListNode->translateY = 8.0f;

	currentFreeListNode = currentFreeListNode->nextBuffer;
	currentFreeListNode->translateX = 8.0f;
	currentFreeListNode->translateY = 8.0f;

	nodeTracker = currentFreeListNode->nextBuffer;
	currentFreeListNode = getFreeListBuffer(currentFreeListNode, 8.0f, 2.0f);
	currentFreeListNode = getFreeListBuffer(currentFreeListNode, 6.0f, 2.0f);
	currentFreeListNode->nextBuffer = nodeTracker;
	nodeTracker->prevBuffer = currentFreeListNode;


}


void moveCamera() {
	void markBufferBusy(struct Buffer *curbuffer, int searchValue);
	void moveFreeBuffertoHashQueue(struct Buffer*, int, struct Buffer *);
	void modifySceneOneFreeList();
	void moveBuffers(struct Buffer *, struct Buffer *, struct Buffer *);


	if (!currentPosition->isSlidePosition && currentPosition->nextPosition != NULL && gfPositionTimer >= currentPosition->counter) {

		//manipulate camera x coordinate to next position
		if (currentPosition->camera.x >= currentPosition->nextPosition->camera.x) {
			currentPosition->camera.x -= cameraTransitionCounter;
			currentPosition->nextPosition->camera.reachedX = 0;
		}
		else if (currentPosition->camera.x <= currentPosition->nextPosition->camera.x) {
			currentPosition->camera.x += cameraTransitionCounter;
			currentPosition->nextPosition->camera.reachedX = 0;
		}
		if (fabs(currentPosition->camera.x - currentPosition->nextPosition->camera.x) < 0.001f)
			currentPosition->nextPosition->camera.reachedX = 1;

		//manipulate camera y coordinate to next position
		if (currentPosition->camera.y >= currentPosition->nextPosition->camera.y) {
			currentPosition->camera.y -= cameraTransitionCounter;
			currentPosition->nextPosition->camera.reachedY = 0;
		}
		else if (currentPosition->camera.y <= currentPosition->nextPosition->camera.y) {
			currentPosition->camera.y += cameraTransitionCounter;
			currentPosition->nextPosition->camera.reachedY = 0;
		}
		if (fabs(currentPosition->camera.y - currentPosition->nextPosition->camera.y) < 0.001f)
			currentPosition->nextPosition->camera.reachedY = 1;

		//z
		if (currentPosition->camera.z >= currentPosition->nextPosition->camera.z) {
			currentPosition->camera.z -= cameraTransitionCounter;
			currentPosition->nextPosition->camera.reachedZ = 0;
		}
		else if (currentPosition->camera.z <= currentPosition->nextPosition->camera.z) {
			currentPosition->camera.z += cameraTransitionCounter;
			currentPosition->nextPosition->camera.reachedZ = 0;
		}
		if (fabs(currentPosition->camera.z - currentPosition->nextPosition->camera.z) < 0.001f)
			currentPosition->nextPosition->camera.reachedZ = 1;

		//center x
		if (currentPosition->center.x >= currentPosition->nextPosition->center.x) {
			currentPosition->center.x -= cameraTransitionCounter;
			currentPosition->nextPosition->center.reachedX = 0;
		}
		else if (currentPosition->center.x <= currentPosition->nextPosition->center.x) {
			currentPosition->center.x += cameraTransitionCounter;
			currentPosition->nextPosition->center.reachedX = 0;
		}
		if (fabs(currentPosition->center.x - currentPosition->nextPosition->center.x) < 0.001f)
			currentPosition->nextPosition->center.reachedX = 1;

		//center y
		if (currentPosition->center.y >= currentPosition->nextPosition->center.y) {
			currentPosition->center.y -= cameraTransitionCounter;
			currentPosition->nextPosition->center.reachedY = 0;
		}
		else if (currentPosition->center.y <= currentPosition->nextPosition->center.y) {
			currentPosition->center.y += cameraTransitionCounter;
			currentPosition->nextPosition->center.reachedY = 0;
		}
		if (fabs(currentPosition->center.y - currentPosition->nextPosition->center.y) < 0.001f)
			currentPosition->nextPosition->center.reachedY = 1;

		//center z
		if (currentPosition->center.z > currentPosition->nextPosition->center.z) {
			currentPosition->center.z -= cameraTransitionCounter;
			currentPosition->nextPosition->center.reachedZ = 0;
		}
		else if (currentPosition->center.z < currentPosition->nextPosition->center.z) {
			currentPosition->center.z += cameraTransitionCounter;
			currentPosition->nextPosition->center.reachedZ = 0;
		}
		if (fabs(currentPosition->center.z - currentPosition->nextPosition->center.z) < 0.001f)
			currentPosition->nextPosition->center.reachedZ = 1;

		//up vector x
		if (currentPosition->upvector.x > currentPosition->nextPosition->upvector.x) {
			currentPosition->upvector.x -= upvectorTransitionCounter;
			currentPosition->nextPosition->upvector.reachedX = 0;
		}
		else if (currentPosition->upvector.x < currentPosition->nextPosition->upvector.x) {
			currentPosition->upvector.x += upvectorTransitionCounter;
			currentPosition->nextPosition->upvector.reachedX = 0;
		}
		if (fabs(currentPosition->upvector.x - currentPosition->nextPosition->upvector.x) < 0.001f)
			currentPosition->nextPosition->upvector.reachedX = 1;

		//up vector y
		if (currentPosition->upvector.y > currentPosition->nextPosition->upvector.y) {
			currentPosition->upvector.y -= upvectorTransitionCounter;
			currentPosition->nextPosition->upvector.reachedY = 0;
		}
		else if (currentPosition->upvector.y < currentPosition->nextPosition->upvector.y) {
			currentPosition->upvector.y += upvectorTransitionCounter;
			currentPosition->nextPosition->upvector.reachedY = 0;
		}
		if (fabs(currentPosition->upvector.y - currentPosition->nextPosition->upvector.y) < 0.001f)
			currentPosition->nextPosition->upvector.reachedY = 1;

		//up vector z
		if (currentPosition->upvector.z > currentPosition->nextPosition->upvector.z) {
			currentPosition->upvector.z -= upvectorTransitionCounter;
			currentPosition->nextPosition->upvector.reachedZ = 0;
		}
		else if (currentPosition->upvector.z < currentPosition->nextPosition->upvector.z) {
			currentPosition->upvector.z += upvectorTransitionCounter;
			currentPosition->nextPosition->upvector.reachedZ = 0;
		}
		if (fabs(currentPosition->upvector.z - currentPosition->nextPosition->upvector.z) < 0.001f)
			currentPosition->nextPosition->upvector.reachedZ = 1;

		//if reached the next position then make current position as next position
		if (currentPosition->nextPosition->camera.reachedX  && currentPosition->nextPosition->camera.reachedY && currentPosition->nextPosition->camera.reachedZ &&
			currentPosition->nextPosition->center.reachedX && currentPosition->nextPosition->center.reachedY && currentPosition->nextPosition->center.reachedZ &&
			currentPosition->nextPosition->upvector.reachedX && currentPosition->nextPosition->upvector.reachedY && currentPosition->nextPosition->upvector.reachedZ) {
			currentPosition = currentPosition->nextPosition;
			cameraPositionTracker++;
			gfPositionTimer = 0.0f;

			//Change the buffer color to red to mark it busy after camera reaches the desired buffer in first hash queue
			if (cameraPositionTracker == 30 && !sceneOneBufferMarkedBusy) {
				markBufferBusy(firstHashQueueHead, 4);//takes hash queue head and the buffer content to identify the desired buffer
				modifySceneOneFreeList();
				sceneOneBufferMarkedBusy = true;
			}
		}
	}
	//scene 2: move the buffer from free list to hash queue. CHANGE TO 13
	if (cameraPositionTracker == 39) {
		moveFreeBuffertoHashQueue(fourthHashQueueHead, 3, thirdHashQueueHead);
	}

	//scene 3: move delayed write buffers out of the scene and free buffer to hash queue
	if (cameraPositionTracker == 52) {
		moveBuffers(firstHashQueueHead, secondHashQueueHead, fourthHashQueueHead);
	}

	//after scene 3 reset buffer pool
	if (cameraPositionTracker == 53 && !currentPosition->positionFlag) {
		currentPosition->positionFlag = true;

		//free up extra buffer memory used for scene 3
		if (sceneThreeTargetBuffer != NULL) {
			sceneThreeTargetBuffer->prevBuffer->nextBuffer = NULL;
			free(sceneThreeTargetBuffer);
			sceneThreeTargetBuffer->nextBuffer = sceneThreeTargetBuffer->prevBuffer = NULL;
		}


		//free up the memory allocated to buffer pool
		freeBufferMemory(firstHashQueueHead);
		freeBufferMemory(secondHashQueueHead);
		freeBufferMemory(thirdHashQueueHead);
		freeBufferMemory(fourthHashQueueHead);
		freeBufferMemory(freeListHead);

		initializeBufferPool();
		initializeFreeList();
	}
	//after scene 1 and 2 reset buffer pool
	if ((cameraPositionTracker == 40 || cameraPositionTracker == 31) && !currentPosition->positionFlag) {
		currentPosition->positionFlag = true;
		//free up the memory allocated to buffer pool
		freeBufferMemory(firstHashQueueHead);
		freeBufferMemory(secondHashQueueHead);
		freeBufferMemory(thirdHashQueueHead);
		freeBufferMemory(fourthHashQueueHead);
		freeBufferMemory(freeListHead);

		initializeBufferPool();
		initializeFreeList();

		if (sceneTwoTargetBuffer != NULL) {
			//remove the buffer which was added to third hash queue in scene two
			sceneTwoTargetBuffer->prevBuffer->nextBuffer = NULL;
			free(sceneTwoTargetBuffer);
			sceneTwoTargetBuffer->nextBuffer = NULL;
			sceneTwoTargetBuffer->prevBuffer = NULL;
		}
	}

	if (currentPosition->isSlidePosition && gfPositionTimer >= currentPosition->counter)
		currentPosition = currentPosition->nextPosition;
	gfPositionTimer += cameraTransitionCounter;
}

//scene 3: move the free buffer in hash queue and two delayed write buffers out of the scene
void moveBuffers(struct Buffer * freeBufferHashQueueTracker, struct Buffer * delayedWriteBufferOneHashQueueTracker, struct Buffer * delayedWriteBufferTwoHashQueueTracker) {
	if (sceneThreeTargetBuffer == NULL) {
		while (freeBufferHashQueueTracker->nextBuffer != NULL) {
			if (freeBufferHashQueueTracker->value == 4) {
				freeBufferHashQueueTracker->prevBuffer->nextBuffer = freeBufferHashQueueTracker->nextBuffer;
				freeBufferHashQueueTracker->nextBuffer->prevBuffer = freeBufferHashQueueTracker->prevBuffer;
				sceneThreeTargetBuffer = freeBufferHashQueueTracker;
				sceneThreeTargetBuffer->nextBuffer = NULL;
				sceneThreeTargetBuffer->prevBuffer = NULL;
				//fprintf(gfpFile, "In moveBuffer function, got the buffer\n");
				break;
			}
			freeBufferHashQueueTracker = freeBufferHashQueueTracker->nextBuffer;
		}
	}

	if (sceneThreeTargetBuffer != NULL) {
		//fprintf(gfpFile, "In moveBuffer function, got the buffer moving it\n");
		if (sceneThreeTargetBuffer->translateY >= -2.0f && sceneThreeTargetBuffer->translateZ <= -20.0f)
			sceneThreeTargetBuffer->translateZ += cameraTransitionCounter;
		if (sceneThreeTargetBuffer->translateZ >= -20.0f && sceneThreeTargetBuffer->translateX <= 10.0f)
			sceneThreeTargetBuffer->translateX += cameraTransitionCounter;
		if (sceneThreeTargetBuffer->translateX >= 10.0f && sceneThreeTargetBuffer->translateY >= -2.0f)
			sceneThreeTargetBuffer->translateY -= cameraTransitionCounter;
		if (sceneThreeTargetBuffer->translateY <= -2.0f && sceneThreeTargetBuffer->translateZ >= -25.0f)
			sceneThreeTargetBuffer->translateZ -= cameraTransitionCounter;
		if (sceneThreeTargetBuffer->translateZ <= -25.0f && sceneThreeTargetBuffer->state == 0) {
			sceneThreeTargetBuffer->state = 1;
			struct Buffer *tracker = thirdHashQueueHead;
			while (tracker->nextBuffer != NULL) {
				tracker = tracker->nextBuffer;
			}
			tracker->nextBuffer = sceneThreeTargetBuffer;
			sceneThreeTargetBuffer->prevBuffer = tracker;
		}
	}

	//move delayed write marked buffer out of the scene
	if (sceneThreeDelayedWriteBufferOne == NULL) {
		while (delayedWriteBufferOneHashQueueTracker->nextBuffer != NULL) {
			if (delayedWriteBufferOneHashQueueTracker->value == 5) {
				delayedWriteBufferOneHashQueueTracker->prevBuffer->nextBuffer = delayedWriteBufferOneHashQueueTracker->nextBuffer;
				delayedWriteBufferOneHashQueueTracker->nextBuffer->prevBuffer = delayedWriteBufferOneHashQueueTracker->prevBuffer;
				sceneThreeDelayedWriteBufferOne = delayedWriteBufferOneHashQueueTracker;
				sceneThreeDelayedWriteBufferOne->nextBuffer = sceneThreeDelayedWriteBufferOne->prevBuffer = NULL;
				break;
			}
			delayedWriteBufferOneHashQueueTracker = delayedWriteBufferOneHashQueueTracker->nextBuffer;
		}
	}

	if (sceneThreeDelayedWriteBufferOne != NULL) {
		if (sceneThreeDelayedWriteBufferOne->translateZ <= -20.0f)
			sceneThreeDelayedWriteBufferOne->translateZ += cameraTransitionCounter;
		if (sceneThreeDelayedWriteBufferOne->translateZ >= -20.0f && sceneThreeDelayedWriteBufferOne->translateX <= 20.0f)
			sceneThreeDelayedWriteBufferOne->translateX += cameraTransitionCounter;
		if (sceneThreeDelayedWriteBufferOne->translateX >= 20.0f) {
			free(sceneThreeDelayedWriteBufferOne);
			sceneThreeDelayedWriteBufferOne->nextBuffer = sceneThreeDelayedWriteBufferOne->prevBuffer = NULL;

		}
	}

	//move second delayed write marked buffer out of the scene
	if (sceneThreeDelayedWriteBufferOne != NULL
		&& sceneThreeDelayedWriteBufferOne->nextBuffer == NULL && sceneThreeDelayedWriteBufferTwo == NULL) {
		while (delayedWriteBufferTwoHashQueueTracker->nextBuffer != NULL) {
			if (delayedWriteBufferTwoHashQueueTracker->value == 3) {
				delayedWriteBufferTwoHashQueueTracker->prevBuffer->nextBuffer = delayedWriteBufferTwoHashQueueTracker->nextBuffer;
				delayedWriteBufferTwoHashQueueTracker->nextBuffer->prevBuffer = delayedWriteBufferTwoHashQueueTracker->prevBuffer;
				sceneThreeDelayedWriteBufferTwo = delayedWriteBufferTwoHashQueueTracker;
				sceneThreeDelayedWriteBufferTwo->nextBuffer = sceneThreeDelayedWriteBufferTwo->prevBuffer = NULL;
				break;
			}
			delayedWriteBufferTwoHashQueueTracker = delayedWriteBufferTwoHashQueueTracker->nextBuffer;
		}
	}

	if (sceneThreeDelayedWriteBufferTwo != NULL) {
		if (sceneThreeDelayedWriteBufferTwo->translateZ <= -20.0f)
			sceneThreeDelayedWriteBufferTwo->translateZ += cameraTransitionCounter;
		if (sceneThreeDelayedWriteBufferTwo->translateZ >= -20.0f && sceneThreeDelayedWriteBufferTwo->translateX <= 20.0f)
			sceneThreeDelayedWriteBufferTwo->translateX += cameraTransitionCounter;
		if (sceneThreeDelayedWriteBufferTwo->translateX >= 20.0f) {
			free(sceneThreeDelayedWriteBufferTwo);
			sceneThreeDelayedWriteBufferTwo->nextBuffer = sceneThreeDelayedWriteBufferTwo->prevBuffer = NULL;

		}
	}
}

void moveFreeBuffertoHashQueue(struct Buffer * sourceHashQueueTracker, int searchValue, struct Buffer *destinationHashQueueTracker) {

	if (sceneTwoTargetBuffer == NULL) {
		while (sourceHashQueueTracker->nextBuffer != NULL) {
			if (sourceHashQueueTracker->value == searchValue) {
				sourceHashQueueTracker->prevBuffer->nextBuffer = sourceHashQueueTracker->nextBuffer;
				sourceHashQueueTracker->nextBuffer->prevBuffer = sourceHashQueueTracker->prevBuffer;
				sceneTwoTargetBuffer = sourceHashQueueTracker;
				sceneTwoTargetBuffer->nextBuffer = NULL;
				sceneTwoTargetBuffer->prevBuffer = NULL;
				break;
			}
			sourceHashQueueTracker = sourceHashQueueTracker->nextBuffer;
		}
	}
	if (sceneTwoTargetBuffer != NULL) {
		if (sceneTwoTargetBuffer->translateY <= -4.0f && sceneTwoTargetBuffer->translateZ <= -20.0f)
			sceneTwoTargetBuffer->translateZ += cameraTransitionCounter;
		if (sceneTwoTargetBuffer->translateZ >= -20.0f && sceneTwoTargetBuffer->translateX <= 10.0f)
			sceneTwoTargetBuffer->translateX += cameraTransitionCounter;
		if (sceneTwoTargetBuffer->translateX >= 10.0f && sceneTwoTargetBuffer->translateY <= -2.0f)
			sceneTwoTargetBuffer->translateY += cameraTransitionCounter;
		if (sceneTwoTargetBuffer->translateY >= -2.0f && sceneTwoTargetBuffer->translateZ >= -25.0f)
			sceneTwoTargetBuffer->translateZ -= cameraTransitionCounter;
		if (sceneTwoTargetBuffer->translateZ <= -25.0f && sceneTwoTargetBuffer->state == 0) {
			sceneTwoTargetBuffer->state = 1;
			while (destinationHashQueueTracker->nextBuffer != NULL)
				destinationHashQueueTracker = destinationHashQueueTracker->nextBuffer;
			destinationHashQueueTracker->nextBuffer = sceneTwoTargetBuffer;
			sceneTwoTargetBuffer->prevBuffer = destinationHashQueueTracker;

		}

	}

}


void markBufferBusy(struct Buffer *curbuffer, int searchValue) {
	while (curbuffer->nextBuffer != NULL) {
		if (curbuffer->value == searchValue) {
			curbuffer->state = 1;
			break;
		}
		curbuffer = curbuffer->nextBuffer;
	}
}
void initializeBufferPool() {
	struct Buffer * initializeHashQueue(int[], int, int, int);
	int freeBufferIndex1, freeBufferIndex2;
	//initialize first hash queue
	int firstQueueContentArr[] = { 23, 28, 4, 64 };
	if (cameraPositionTracker == 53)
		freeBufferIndex1 = freeBufferIndex2 = -1;
	else {
		freeBufferIndex1 = 1;
		freeBufferIndex2 = 2;
	}
	firstHashQueueHead = initializeHashQueue(firstQueueContentArr, 6, freeBufferIndex1, freeBufferIndex2);

	//initialize second hash queue
	int secondQueueContentArr[] = { 68, 17, 5, 97 };
	if (cameraPositionTracker != 53) {
		freeBufferIndex1 = 2;
		freeBufferIndex2 = 3;
	}
	secondHashQueueHead = initializeHashQueue(secondQueueContentArr, 2, freeBufferIndex1, freeBufferIndex2);

	//initialize third hash queue
	int thirdQueueContentArr[] = { 56, 98, 50, 10 };
	if (cameraPositionTracker != 53)
		freeBufferIndex1 = 3;
	thirdHashQueueHead = initializeHashQueue(thirdQueueContentArr, -2, freeBufferIndex1, -1);

	//initialize fourth hash queue
	int fourthQueueContentArr[] = { 72, 3, 35, 99 };
	if (cameraPositionTracker != 53)
		freeBufferIndex1 = 1;
	fourthHashQueueHead = initializeHashQueue(fourthQueueContentArr, -6, freeBufferIndex1, -1);
}

struct Buffer * initializeHashQueue(int contentArr[], int yOffset, int freeBufferIndex1, int freeBufferIndex2) {
	int xOffset = -6;
	struct Buffer *head = NULL, *lastBuffer = NULL;

	for (int i = 0; i < 4; i++) {

		//check if the queue is empty if so initialize head buffer
		if (head == NULL) {
			head = (struct Buffer *)malloc(sizeof(Buffer));
			head->value = contentArr[i];
			head->nextBuffer = head->prevBuffer = NULL;
			head->translateX = (GLfloat)xOffset;
			head->translateY = (GLfloat)yOffset;
			head->translateZ = -25.0f;

			lastBuffer = head;
		}
		else {
			//create a buffer instance and allocate memory for it
			struct Buffer *buffer = (struct Buffer*)malloc(sizeof(Buffer));
			buffer->value = contentArr[i];
			buffer->nextBuffer = NULL;
			buffer->prevBuffer = NULL;
			buffer->translateX = (GLfloat)xOffset;
			buffer->translateY = (GLfloat)yOffset;
			buffer->translateZ = -25.0f;
			if (i == freeBufferIndex1 || i == freeBufferIndex2)
				buffer->state = 0;
			else
				buffer->state = 1;
			if (cameraPositionTracker == 40 && (contentArr[i] == 3 || contentArr[i] == 5))
				buffer->state = 2;

			//attach the buffer after the last buffer
			lastBuffer->nextBuffer = buffer;
			buffer->prevBuffer = lastBuffer;

			//make the new buffer as the last buffer
			lastBuffer = buffer;
		}
		xOffset += 4;
	}

	return head;
}

void drawHashQueue(struct Buffer *head) {
	struct Buffer *currentBuffer = head;

	while (currentBuffer != NULL) {

		gluLookAt(currentPosition->camera.x, currentPosition->camera.y, currentPosition->camera.z, currentPosition->center.x, currentPosition->center.y, currentPosition->center.z, currentPosition->upvector.x, currentPosition->upvector.y, currentPosition->upvector.z);
		//gluLookAt(-10.0f, 5.0f, -20.0f, 10.0f, 7.0f, -30.0f, 0.0f, 0.0f, 1.0f);
		glTranslatef(currentBuffer->translateX, currentBuffer->translateY, currentBuffer->translateZ);
		//glColor3f(1.0f, 1.0f, 1.0f);
		if (currentBuffer->state == 0)
		{
			glBindTexture(GL_TEXTURE_2D, freeTex[currentBuffer->value]);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			//use different color grading for hash queue header, free buffer and busy buffer
			//front


			glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
		}
		else if (currentBuffer->state == 1)
		{
			glBindTexture(GL_TEXTURE_2D, lockedTex[currentBuffer->value]);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			//use different color grading for hash queue header, free buffer and busy buffer
			//front


			glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
		}
		else if (currentBuffer->state == 2)
		{
			glBindTexture(GL_TEXTURE_2D, delayedWrite[currentBuffer->value]);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			//use different color grading for hash queue header, free buffer and busy buffer
			//front


			glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
		}

		if (currentBuffer->value == head->value)
		{
			glDisable(GL_TEXTURE_2D);
			if (currentBuffer->value == 23)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, queueHeaders[0]);
			}
			if (currentBuffer->value == 68)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, queueHeaders[1]);
			}
			if (currentBuffer->value == 56)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, queueHeaders[2]);
			}
			if (currentBuffer->value == 72)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, queueHeaders[3]);
			}
			glBegin(GL_QUADS);
			//use different color grading for hash queue header, free buffer and busy buffer
			/*if (currentBuffer->state == 0)
				glColor3f(0.0f, 0.35f, 0.0f);
			else if (currentBuffer->state == 1)
				glColor3f(0.35f, 0.0f, 0.0f);
			else if (currentBuffer->state == 2)
				glColor3f(0.35f, 0.35f, 0.0f);
			if (currentBuffer->value == head->value)
				glColor3f(0.35f, 0.35f, 0.35f);
				*/
			//front
			glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
		}


		//right face
		if (currentBuffer->state == 0)
			glColor3f(0.0f, 0.35f, 0.0f);
		else if (currentBuffer->state == 1)
			glColor3f(0.35f, 0.0f, 0.0f);
		else if (currentBuffer->state == 2)
			glColor3f(0.35f, 0.35f, 0.0f);
		if (currentBuffer->value == head->value)
			glColor3f(0.35f, 0.35f, 0.35f);
		glVertex3f(0.6f, 0.6f, -1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(0.6f, -0.6f, -1.0f);

		//back face
		if (currentBuffer->state == 0)
			glColor3f(0.0f, 0.35f, 0.0f);
		else if (currentBuffer->state == 1)
			glColor3f(0.35f, 0.0f, 0.0f);
		else if (currentBuffer->state == 2)
			glColor3f(0.35f, 0.35f, 0.0f);
		if (currentBuffer->value == head->value)
			glColor3f(0.35f, 0.35f, 0.35f);
		glVertex3f(0.6f, 0.6f, -1.0f);
		glVertex3f(-0.6f, 0.6f, -1.0f);
		glVertex3f(-0.6f, -0.6f, -1.0f);
		glVertex3f(0.6f, -0.6f, -1.0f);


		//left face
		if (currentBuffer->state == 0)
			glColor3f(0.0f, 0.35f, 0.0f);
		else if (currentBuffer->state == 1)
			glColor3f(0.35f, 0.0f, 0.0f);
		else if (currentBuffer->state == 2)
			glColor3f(0.35f, 0.35f, 0.0f);
		if (currentBuffer->value == head->value)
			glColor3f(0.35f, 0.35f, 0.35f);

		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-0.6f, 0.6f, -1.0f);
		glVertex3f(-0.6f, -0.6f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		//top face
		if (currentBuffer->state == 0)
			glColor3f(0.0f, 0.35f, 0.0f);
		else if (currentBuffer->state == 1)
			glColor3f(0.35f, 0.0f, 0.0f);
		else if (currentBuffer->state == 2)
			glColor3f(0.35f, 0.35f, 0.0f);
		if (currentBuffer->value == head->value)
			glColor3f(0.35f, 0.35f, 0.35f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.6f, 0.6f, -1.0f);
		glVertex3f(-0.6f, 0.6f, -1.0f);
		
		//bottom face
		if (currentBuffer->state == 0)
			glColor3f(0.0f, 0.35f, 0.0f);
		else if (currentBuffer->state == 1)
			glColor3f(0.35f, 0.0f, 0.0f);
		else if (currentBuffer->state == 2)
			glColor3f(0.35f, 0.35f, 0.0f);
		if (currentBuffer->value == head->value)
			glColor3f(0.35f, 0.35f, 0.35f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(-0.6f, -0.6f, -1.0f);
		glVertex3f(0.6f, -0.6f, -1.0f);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);


		//draw links between the current and next buffer using line
		if (currentBuffer->nextBuffer != NULL) {
			glColor3f(0.7f, 0.0f, 0.0f);
			glLineWidth(5.0f);
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(currentBuffer->nextBuffer->translateX - currentBuffer->translateX, currentBuffer->nextBuffer->translateY - currentBuffer->translateY, currentBuffer->nextBuffer->translateZ - currentBuffer->translateZ);
			glEnd();
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		currentBuffer = currentBuffer->nextBuffer;
	}

}

// Texture

void DrawSlides()
{
	glEnable(GL_TEXTURE_2D);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	gluLookAt(currentPosition->camera.x, currentPosition->camera.y, currentPosition->camera.z, currentPosition->center.x, currentPosition->center.y, currentPosition->center.z, currentPosition->upvector.x, currentPosition->upvector.y, currentPosition->upvector.z);
	for (int i = 0; i < sizeof(slides) / sizeof(GLuint); i++)
	{
		glBindTexture(GL_TEXTURE_2D, slides[i]);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.74f, 0.544f, -30.0f - i);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-0.74f, 0.544f, -30.0f - i);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.74f, -0.544f, -30.0f - i);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.74f, -0.544f, -30.0f - i);
		glEnd();

	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
}


void DrawContainer(GLfloat width, GLuint texture)
{
	width = width / 2.0f;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);
	//gluLookAt(-25.0f, 0.0f, -20.0f, 0.0f, 0.0f, -25.0f, 0.0f, 0.0f, 1.0f);
	gluLookAt(currentPosition->camera.x, currentPosition->camera.y, currentPosition->camera.z, currentPosition->center.x, currentPosition->center.y, currentPosition->center.z, currentPosition->upvector.x, currentPosition->upvector.y, currentPosition->upvector.z);
	glBegin(GL_QUADS);
	// Top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(width, width, -width);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-width, width, -width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-width, width, width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(width, width, width);
	// Bottom
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-width, -width, -width);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(width, -width, -width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(width, -width, width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-width, -width, width);

	// Left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-width, width, width);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-width, width, -width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-width, -width, -width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-width, -width, width);
	// Right
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(width, width, -width);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(width, width, width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(width, -width, width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(width, -width, -width);
	// Front
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(width, width, width);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-width, width, width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-width, -width, width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(width, -width, width);
	// Back
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-width, width, -width);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(width, width, -width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(width, -width, -width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-width, -width, -width);
	glEnd();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
}

int LoadGLTextures(GLuint * texture, TCHAR imageResourceId[])
{
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = -1;
	glGenTextures(1, texture);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		iStatus = 1;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		DeleteObject(hBitmap);
	}
	DWORD err = GetLastError();
	return(iStatus);
}