#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <fstream>
#include <string>

using namespace std;

ifstream cfg_file("config.rez");
float r_clr, g_clr, b_clr;

#pragma comment( lib, "opengl32.lib" )

typedef void ( APIENTRY *glColor3f_t)( GLfloat red, GLfloat green, GLfloat blue );
 
glColor3f_t pglColor3f = NULL;

/*void APIENTRY Hooked_glBegin( GLenum mode ) {
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
}*/

char msg[256];
BOOL bColorMode = FALSE;

void APIENTRY Hooked_glColor3f( GLfloat red, GLfloat green, GLfloat blue )
{    
	if(GetAsyncKeyState(VK_F1)){
		r_clr -= 0.001;
	}
	if(GetAsyncKeyState(VK_F2)){
		r_clr += 0.001;
	}
	if(GetAsyncKeyState(VK_F3)){
		g_clr -= 0.001;
	}
	if(GetAsyncKeyState(VK_F4)){
		g_clr += 0.001;
	}
	if(GetAsyncKeyState(VK_F5)){
		b_clr -= 0.001;
	}
	if(GetAsyncKeyState(VK_F6)){
		b_clr += 0.001;
	}
	(*pglColor3f)( red+r_clr, green-g_clr, blue-b_clr );
}

/*void APIENTRY Hooked_glVertex3f ( GLfloat x,  GLfloat y,  GLfloat z )
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
}*/

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
	pglColor3f = (glColor3f_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glColor3f" ), (LPBYTE)&Hooked_glColor3f, 6);
}

INT WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved ) {
    switch( fdwReason ) {
        case DLL_PROCESS_ATTACH:
			r_clr = 0.4;
			g_clr = 0.1;
			b_clr = 0.2;
			HookGL();
			//MessageBoxA(NULL, "RezFX v2.0 successfully initialized!", "RezFX", MB_ICONINFORMATION);
        break;
    }
    return TRUE;
}