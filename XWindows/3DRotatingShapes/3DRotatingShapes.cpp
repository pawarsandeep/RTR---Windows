#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

// Namespace
using namespace std;

// Variable declarations
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;

GLXContext gGLXContext;

struct myColor
{
	GLfloat R;
	GLfloat G;
	GLfloat B;
	GLfloat A;
};

struct myVertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
float triAngle = 0.0f;
float cubeAngle = 360.0f;
myColor myColorRed = { 1.0f,0.0f,0.0f,0.0f };
myColor myColorGreen = { 0.0f,1.0f,0.0f,0.0f };
myColor myColorBlue = { 0.0f, 0.0f, 1.0f, 0.0f };
myColor myColorWhite = { 1.0f,1.0f,1.0f,0.0f };
myColor myColorCyan = { 0.0f,1.0f,1.0f,0.0f };
myColor myColorMagenta = { 1.0f,0.0f,1.0f,0.0f };
myColor myColorYellow = { 1.0f,1.0f,0.0f,0.0f };
myColor myColorOrange = { 1.0f,0.64f,0.0f,0.0f };

int main(void)
{
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void Initialize(void);
	void display(void);
	void Resize(int, int);
	void Uninitialize(void);
	void RotateTriangleAndSquare(void);
	
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;

	bool bDone = false;

	CreateWindow();

	Initialize();
	
	XEvent event;
	KeySym keysym;
	
	while(bDone == false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);
			switch(event.type)
			{
				case MapNotify:
					break;
				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
					switch(keysym)
					{
						case XK_Escape:
							bDone = true;
						case XK_F:
						case XK_f:
							if(bFullscreen == false)
							{
								ToggleFullscreen();
								bFullscreen = true;
							}
							else
							{
								ToggleFullscreen();
								bFullscreen = false;
							}
							break;
						default:
							break;
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
						default:
							break;
					}
					break;
				case MotionNotify:
					break;
				case ConfigureNotify:
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					Resize(winWidth, winHeight);
					break;
				case Expose:
					break;
				case DestroyNotify:
					break;
				case 33:
					bDone = true;
					break;
				default:
					break;
			}
		}
		RotateTriangleAndSquare();
		display();
	}
	return(0);
}

void CreateWindow(void)
{
	// Function Declarations
	void Uninitialize(void);
	
	// Variable Declarations
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;
	
	static int frameBufferAttributes[] =
	{
		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_DOUBLEBUFFER, True,
		None
	};
	
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL)
	{
		printf("ERROR: Unable to Open X Display.\n Exitting Now...");
		Uninitialize();
		exit(0);
	}
	defaultScreen = XDefaultScreen(gpDisplay);
	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone);
	gColormap = winAttribs.colormap;
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	gWindow = XCreateWindow(gpDisplay, 
				RootWindow(gpDisplay, gpXVisualInfo->screen), 
				0, 
				0,
				giWindowWidth,
				giWindowHeight,
				0,
				gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs);
	if(!gWindow)
	{
		printf("ERROR: Failed to create main window. \n Exitting now...");
		Uninitialize();
		exit(0);
	}
	
	XStoreName(gpDisplay, gWindow, "First OpenGL Window");
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
	XMapWindow(gpDisplay, gWindow);
}

void ToggleFullscreen(void)
{
	// Variable declarations
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};
	
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	memset(&xev, 0, sizeof(xev));
	
	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bFullscreen ? 0:1;
	
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] = fullscreen;
	XSendEvent(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), False, StructureNotifyMask, &xev);
}

void Initialize(void)
{
	// Function declarations
	void Resize(int, int);
	
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Resize(giWindowWidth, giWindowHeight);
}

void display(void)
{
	// FUnction declarations
	void DrawPyramid(float, myColor, myColor, myColor);
	void DrawCube(float, myColor, myColor, myColor, myColor, myColor, myColor);
	// Variable declarations
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(triAngle, 0.0f, 1.0f, 0.0f);
	DrawPyramid(2.0f, myColorRed, myColorGreen, myColorBlue);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -6.0f);
	glRotatef(cubeAngle, 1.0f, 1.0f, 1.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	DrawCube(2.0f, myColorBlue, myColorCyan, myColorGreen, myColorMagenta, myColorRed, myColorOrange);
	glXSwapBuffers(gpDisplay, gWindow);
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLsizei)width/(GLsizei)height, 0.1f,100.0f);
}

void Uninitialize(void)
{
	GLXContext currentGLXContext;
	currentGLXContext = glXGetCurrentContext();
	if(currentGLXContext != NULL &&currentGLXContext== gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0,0);
	}
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}
	if(gWindow)
	{
		XDestroyWindow(gpDisplay, gWindow);
	}
	if(gColormap)
	{
		XFreeColormap(gpDisplay, gColormap);
	}
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo == NULL;
	}
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay == NULL;
	}
}

void DrawPyramid(float size, myColor top, myColor right, myColor left)
{
	// Function Declaration
	void DrawMultiColorTriangle(myVertex, myVertex, myVertex, myColor, myColor, myColor);

	// Variable declarations
	float sizeFromCenter = size / 2;

	// Front face
	DrawMultiColorTriangle({ 0.0f,sizeFromCenter,0.0f }, { -sizeFromCenter,-sizeFromCenter,sizeFromCenter }, { sizeFromCenter,-sizeFromCenter,sizeFromCenter }, top, right, left);

	// Right face
	DrawMultiColorTriangle({ 0.0f,sizeFromCenter,0.0f }, { sizeFromCenter,-sizeFromCenter,sizeFromCenter }, { sizeFromCenter,-sizeFromCenter,-sizeFromCenter }, top, left, right);

	// Back face
	DrawMultiColorTriangle({ 0.0f,sizeFromCenter,0.0f }, { -sizeFromCenter,-sizeFromCenter,-sizeFromCenter }, { sizeFromCenter,-sizeFromCenter,-sizeFromCenter }, top, left, right);

	//Left face
	DrawMultiColorTriangle({ 0.0f,sizeFromCenter,0.0f }, { -sizeFromCenter,-sizeFromCenter,-sizeFromCenter }, { -sizeFromCenter,-sizeFromCenter,sizeFromCenter }, top, left, right);
}

void DrawMultiColorTriangle(myVertex v1, myVertex v2, myVertex v3, myColor c1, myColor c2, myColor c3)
{
	glBegin(GL_TRIANGLES);
	glColor4f(c1.R, c1.G, c1.B, c1.A);
	glVertex3f(v1.x, v1.y, v1.z);
	glColor4f(c2.R, c2.G, c2.B, c2.A);
	glVertex3f(v2.x, v2.y, v2.z);
	glColor4f(c3.R, c3.G, c3.B, c3.A);
	glVertex3f(v3.x, v3.y, v3.z);
	glEnd();
}


void DrawCube(float size, myColor colorFront, myColor colorRight, myColor colorBack, myColor colorLeft, myColor colorTop, myColor colorBottom)
{
	// Function declaration
	void DrawRectangle(myVertex, myVertex, myVertex, myVertex, myColor);
	// Variable declaration
	float s = size / 2;
	// Code

	// Front face
	DrawRectangle({ s,s,s }, { -s,s,s }, { -s,-s,s }, { s,-s,s }, colorFront);

	// Right face
	DrawRectangle({ s,s,-s }, { s,s,s }, { s,-s,s }, { s,-s,-s }, colorRight);

	// Back face
	DrawRectangle({ s,s,-s }, { -s,s,-s }, { -s,-s,-s }, { s,-s,-s }, colorBack);

	// Left face
	DrawRectangle({ -s,s,-s }, { -s,s,s }, { -s,-s,s }, { -s,-s,-s }, colorRight);

	// Top face
	DrawRectangle({ s,s,-s }, { -s,s,-s }, { -s,s,s }, { s,s,s }, colorTop);

	// Bottom face
	DrawRectangle({ s,-s,-s }, { -s,-s,-s }, { -s,-s,s }, { s,-s,s }, colorBottom);
}

void DrawRectangle(myVertex rightTop, myVertex leftTop, myVertex leftBottom, myVertex rightBottom, myColor color)
{
	glBegin(GL_QUADS);
	glColor4f(color.R, color.G, color.B, color.A);
	glVertex3f(rightTop.x, rightTop.y, rightTop.z);
	glVertex3f(leftTop.x, leftTop.y, leftTop.z);
	glVertex3f(leftBottom.x, leftBottom.y, leftBottom.z);
	glVertex3f(rightBottom.x, rightBottom.y, rightBottom.z);
	glEnd();
}

void RotateTriangleAndSquare()
{
	triAngle = triAngle + 0.5f;
	if (triAngle > 360.0f)
		triAngle = 0.0f;
	cubeAngle = cubeAngle - 0.5f;
	if (cubeAngle < 0.0f)
		cubeAngle = 360.0f;
}