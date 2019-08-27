#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#pragma comment( lib, "opengl32.lib" )

typedef void ( APIENTRY *glBegin_t )( GLenum );
typedef void ( APIENTRY *glEnd_t )( void );
typedef void ( APIENTRY *glClear_t )( GLbitfield );
typedef void ( APIENTRY *glVertex3fv_t )( const GLfloat *v );
typedef void ( APIENTRY *glVertex3f_t )( GLfloat x,  GLfloat y,  GLfloat z );
typedef void ( APIENTRY *glEnable_t )( GLenum );
typedef void ( APIENTRY *glColor3f_t)( GLfloat red, GLfloat green, GLfloat blue );
 
glBegin_t pglBegin = NULL;
glEnd_t pglEnd = NULL;
glClear_t pglClear = NULL;
glVertex3fv_t pglVertex3fv = NULL;
glVertex3f_t pglVertex3f = NULL;
glColor3f_t pglColor3f = NULL;
glEnable_t pglEnable = NULL;

void APIENTRY Hooked_glBegin( GLenum mode ) {
	//GLenum rMode;
	if(mode == GL_TRIANGLES || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN) {
		//rMode = mode;
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	}
    if(pglBegin)
		(*pglBegin)(mode);
}
 
void APIENTRY Hooked_glEnd( void )
{
    (*pglEnd)();
}
void APIENTRY Hooked_glVertex3fv( GLfloat *v )
{    
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    (*pglVertex3fv)( v );
}
 
void APIENTRY Hooked_glColor3f( GLfloat red, GLfloat green, GLfloat blue )
{    
	(*pglColor3f)( red+0.4, green-0.1, blue-0.3 );
}

void APIENTRY Hooked_glVertex3f ( GLfloat x,  GLfloat y,  GLfloat z )
{
    (*pglVertex3f)( x, y, z );
}
 
void APIENTRY Hooked_glClear( GLbitfield mask )
{
    (*pglClear)( mask );
}
 
void APIENTRY Hooked_glEnable (GLenum cap)
{ 
    (*pglEnable)(cap);
}

void* DetourFunc( BYTE *src, const BYTE *dst, const int len )
{
    BYTE *jmp = (BYTE*)malloc( len + 5 );
    DWORD dwback;
    VirtualProtect( src, len, PAGE_READWRITE, &dwback );
    memcpy( jmp, src, len );
    jmp += len;
    jmp[0] = 0xE9;
    *(DWORD*)( jmp + 1 ) = (DWORD)( src + len - jmp ) - 5;
    src[0] = 0xE9;
    *(DWORD*)( src + 1 ) = (DWORD)( dst - src ) - 5;
    VirtualProtect( src, len, dwback, &dwback );
    return ( jmp - len );
}

void HookGL( void ) {
    HMODULE hOpenGL = GetModuleHandleA("opengl32.dll");
    pglVertex3fv =   (glVertex3fv_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glVertex3fv" ), (LPBYTE)&Hooked_glVertex3fv, 6 );
    pglVertex3f  =   (glVertex3f_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glVertex3f" ), (LPBYTE)&Hooked_glVertex3f, 6);
	pglColor3f  =   (glColor3f_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glColor3f" ), (LPBYTE)&Hooked_glColor3f, 6);
	pglBegin     =   (glBegin_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glBegin"),(LPBYTE)&Hooked_glBegin,6);
    pglEnd       =   (glEnd_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glEnd" ), (LPBYTE)&Hooked_glEnd, 6 );
    pglClear     =   (glClear_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glClear" ), (LPBYTE)&Hooked_glClear, 7 );
    pglEnable    =   (glEnable_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glEnable"), (LPBYTE)&Hooked_glEnable, 6);
}

INT WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved ) {
    switch( fdwReason ) {
        case DLL_PROCESS_ATTACH:
			HookGL();
        break;
    }
    return TRUE;
}