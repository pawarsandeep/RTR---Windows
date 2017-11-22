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
#include <SOIL/SOIL.h>
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
GLuint texStone, texKundali;
GLfloat anglePyramid = 0.0f;
GLfloat angleCube = 0.0f;

GLXContext gGLXContext;


int main(void)
{
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void Initialize(void);
	void display(void);
	void Resize(int, int);
	void Uninitialize(void);
	void Rotate(void);
	
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
		display();
		Rotate();
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
	
	XStoreName(gpDisplay, gWindow, "Texture: Rotating cube & pyramid");
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
	int LoadGLTextures(GLuint*, const char*);
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
	glEnable(GL_TEXTURE_2D);
	LoadGLTextures(&texStone, "Stone.bmp");
	LoadGLTextures(&texKundali,"Vijay_Kundali.bmp");
	Resize(giWindowWidth, giWindowHeight);
}

void display(void)
{
	// FUnction declarations
	// Variable declarations
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(anglePyramid, 0.0f, 1.0f,0.0f);
	glBindTexture(GL_TEXTURE_2D, texStone);
	glBegin(GL_TRIANGLES);
	//Front
	glTexCoord2f(0.5f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	//Right
	glTexCoord2f(0.5f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,1.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);

	// Back
	glTexCoord2f(0.5f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);

	// Left
	glTexCoord2f(0.5f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glEnd();

	// CUBE

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -6.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef(angleCube, 1.0f,1.0f,1.0f);

	glBindTexture(GL_TEXTURE_2D, texKundali);

	glBegin(GL_QUADS);

	// Front
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f);

	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,-1.0f,1.0f);

	// Right
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,1.0f,-1.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,1.0f,1.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,-1.0f,1.0f);

	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);

	// Back
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);

	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,1.0f,-1.0f);

	// Left
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);

	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);

	// Top
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,1.0f,-1.0f);

	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);

	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,1.0f,1.0f);

	// Bottom
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,1.0f);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);

	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);

	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	glEnd();
	glXSwapBuffers(gpDisplay, gWindow);
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width > height)
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
	else
		gluPerspective(45.0f, (GLfloat)height / (GLfloat)width, 1.0f, 1000.0f);
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

int LoadGLTextures(GLuint *texture, const char *path)
{
	int width, height;
	unsigned char *imageData = NULL;
	bool iStatus = false;
	glGenTextures(1, texture);
	imageData = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	if(imageData)
	{
		iStatus = true;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, (void *)imageData);
	}
	return (iStatus);
}

void Rotate(void)
{
	anglePyramid = anglePyramid + 0.5f;
	if(anglePyramid > 360.0f)
	{
		anglePyramid = anglePyramid - 360.0f;
	}
	angleCube = angleCube - 0.5f;
	if(angleCube < 0.0f)
	{
		angleCube = angleCube + 360.0f;
	}
}