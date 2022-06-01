#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <assert.h>
#include "assignment_main.h"

typedef unsigned char    byte;
typedef unsigned short   word;
typedef unsigned int     uint;

#include <windows.h>
#include <gl/glut.h>
#include <math.h>

#define WindowWidth			1600
#define WindowHeight		900
#define NUMBER_OF_POINT		WindowWidth*WindowHeight			// 저장가능한 최대 버텍스 개수
float vertex[NUMBER_OF_POINT][3]={0};							// 버텍스 좌표를 저장하는 배열
float back_vert[NUMBER_OF_POINT][3]={0};						// 예비 좌표
float Shadow[NUMBER_OF_POINT]={0};								//그림자부분 배열

char* information;												//안내용 문자열

int iDrawMode;													// 프리미티브 종류 (line, line-strip, triangle, quad 등 선택) 
int iNumPoints;													// 현재까지 입력된 버텍스의 개수
int ShadowPoints=0;												// 그림자진 픽셀의 위치수(중복 가능)
int index;

GLfloat a[3];
GLfloat b[3];


ObjLoader::ObjLoader()
{
	m_Vertices.clear();
	m_Normals.clear();
	m_Faces.clear();
	m_NormalIDX.clear();
	m_TexCoord.clear();
}
ObjLoader::~ObjLoader()
{
	m_Vertices.clear();
	m_Normals.clear();
	m_Faces.clear();
	m_NormalIDX.clear();
	m_TexCoord.clear();
}



#pragma pack(1) // 1바이트 단위로 저장
struct BITHEADER { //FILE HEADER를 BITHEADER 이름의 구조체로 정의
    word bfType;
    uint bfSize;
    word bfReserved1;
    word bfReserved2;
    uint bfOffBits; //데이터 파일 시작 위치
};
struct BITINFO { //Image Header를 BITINFO 이름의 구조체로 정의
    uint biSize;
    int biWidth;
    int biHeight;
    word biPlanes;
    word biBitCount;
    uint biCompression;
    uint biSizeImage;
    int biXPelsPerMeter;
    int biYpelsperMeter;
    uint biClrUsed;
    uint biClrImortant;
};
struct RGB_Q {
    byte rgbBlue;
    byte rgbGreen;
    byte rgbRed;
    byte rgbReserved;
};
#pragma pack()

struct BITHEADER bf_header; // BITHEADER형 변수 bf_header선언
struct BITINFO bi_header; // BITINFO형 변수 bi_header선언
struct RGB_Q bmp_pal[256];       // 비트맵 팔레트
int Width = 1600;
int Height = 900;

float MaxGrey[7]={0};		// 7장의 이미지
float minGrey[7]={0};

float histogram[256]={0};	// 0~255


const char *bmp_name = "air.bmp";
const char *grey_name = "T_grey.bmp";

void Load_grey(char *bmpnam,int imageNumber)
{
    FILE *fbmp, *fgrey;
    size_t bmp_wbytes;  // 24 비트 이미지의 1 라인 바이트 수
    size_t grey_wbytes; //  8 비트 이미지의 1 라인 바이트 수
	byte *bmp, *grey;
    byte  red, green, blue;
    int i, j;                

    // 원본 파일 열기
    fbmp = fopen(bmpnam, "rb");
    assert(fbmp != NULL);

    // 비트맵 파일 정보와 비트맵 정보 읽기
    fread(&bf_header, sizeof(bf_header), 1, fbmp);
    fread(&bi_header, sizeof(bi_header), 1, fbmp);

    // 비트맵 크기
    Width = bi_header.biWidth;
    Height = bi_header.biHeight;
	
    // 24비트 비트맵의 1 라인의 바이트 수가 4 의 배수가 되도록 수정
	if((Width%4) == 0) Width-=1;
	bmp_wbytes = Width * 3;
	bmp_wbytes += 4 - bmp_wbytes % 4;		//bmp_wbytes = Width * 3 + (4 - bmp_wbytes % 4);
    bmp = (byte *)malloc(bmp_wbytes*Height);

    // 8비트 비트맵의 1 라인의 바이트 수가 4 의 배수가 되도록 수정
    grey_wbytes = Width;
    grey_wbytes += 4 - grey_wbytes % 4;
    grey = (byte *)malloc(grey_wbytes*Height);

    // 비트맵 읽기
    fread(bmp, sizeof(byte), bmp_wbytes*Height, fbmp);
    fclose(fbmp);

    // 그레이 변환
	int multPoints = 0;
	iNumPoints=0;
    for (i=0; i < Height; i++) {
        for (j=0; j < Width; j++) {
            // 그레이 변환
            blue  = bmp[bmp_wbytes*i + 3*j + 0];
            green = bmp[bmp_wbytes*i + 3*j + 1];
            red   = bmp[bmp_wbytes*i + 3*j + 2];
            grey[grey_wbytes*i + j] = ((114 * red + 587 * green + 299 * blue) / 1000);
			
			if(grey[grey_wbytes*i + j] > MaxGrey[imageNumber]){
				MaxGrey[imageNumber]= grey[grey_wbytes*i + j];
			}
			else if(grey[grey_wbytes*i + j] < minGrey[imageNumber]){
				minGrey[imageNumber]= grey[grey_wbytes*i + j];
			}

        }
        // 패딩( 1라인의 Width 넘는 부분 0 으로 채움)
        for(; j < grey_wbytes; j++) 
            grey[grey_wbytes*i + j] = 0; 
    }

	for (i=0; i < Height; i++) {
        for (j=0; j < Width; j++) {
            // 그레이 변환
            blue  = bmp[bmp_wbytes*i + 3*j + 0];
            green = bmp[bmp_wbytes*i + 3*j + 1];
            red   = bmp[bmp_wbytes*i + 3*j + 2];
            grey[grey_wbytes*i + j] = ((114 * red + 587 * green + 299 * blue) / 1000);
			
			if(grey[grey_wbytes*i + j] ==0){
				Shadow[ShadowPoints]= iNumPoints;
				ShadowPoints++;
			}

			if(MaxGrey[imageNumber]- minGrey[imageNumber] > 0){
			vertex[multPoints][0] = j;
			vertex[multPoints][1] = i;
			//히스토그램 스트레칭
			vertex[multPoints][2] += (grey[grey_wbytes*i + j]-minGrey[imageNumber])*255 / (MaxGrey[imageNumber]- minGrey[imageNumber]);

			multPoints++;
			iNumPoints++;
			grey[grey_wbytes*i + j] = (grey[grey_wbytes*i + j] - minGrey[imageNumber]) * 255 / (MaxGrey[imageNumber] - minGrey[imageNumber]);
			}

			

        }

        // 패딩( 1라인의 Width 넘는 부분 0 으로 채움)
        for(; j < grey_wbytes; j++) 
            grey[grey_wbytes*i + j] = 0;
    }
    // 위에서의 헤더는 24비트 비트맵의 헤더입니다.
    // 그레이비트맵은 8비트 비트맵이므로 정보를 바꾸어주어야 합니다.
    // 8비트 이하의 비트맵은 팔레트라고 해서 색상값을 따로 저장하고
    // 있습니다. 그 팔레트를 만들어 주셔야 합니다.

    // 인포헤더 아래에 팔레트가 오므로 1024만큼 뒤에 비트맵 데이터 시작
    bf_header.bfOffBits += 1024;
    // 8비트 비트맵이므로 8로 수정
    bi_header.biBitCount = 8;

    // 팔레트 만들기
    for (i = 0; i < 256; i++) {
        bmp_pal[i].rgbBlue = i;
        bmp_pal[i].rgbGreen = i;
        bmp_pal[i].rgbRed = i;
        bmp_pal[i].rgbReserved = 0;
    }

    // 파일 저장
 //   fgrey = fopen(grey_name, "wb");
 //   fwrite(&bf_header, sizeof(bf_header), 1, fgrey);            // BMPFILE헤더 저장
 //   fwrite(&bi_header, sizeof(bi_header), 1, fgrey);            // BMPINFO헤더 저장
 //   fwrite(bmp_pal, sizeof(bmp_pal), 1, fgrey);                 // 팔레트를 저장
 //   fwrite(grey, sizeof(byte), grey_wbytes * Height, fgrey);    // 비트맵 저장
 //   fclose(fgrey);
}

/*
void normalize(float v[3])
{
	float r;

	r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	if (r == 0.0) return;

	v[0] /= r;
	v[1] /= r;
	v[2] /= r;
}

void cross(float v1[3], float v2[3], float result[3])
{
	result[0] = v1[1]*v2[2] - v1[2]*v2[1];
	result[1] = v1[2]*v2[0] - v1[0]*v2[2];
	result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}


*/
void Draw_Axis()// XYZ축 그리기
{

	//밑바닥 그리기
	
	glBegin(GL_QUADS);
		glVertex3f(0,0,0);
		glVertex3f(0,Height,0);
		glVertex3f(Width,Height,0);
		glVertex3f(Width,0,0);
	glEnd();
	
	glBegin(GL_TRIANGLES);
		for(index = 0; index < iNumPoints; index++)
		{
			if( (vertex[index][0] < Width-1 ) && (index+Width <iNumPoints && index+Width+1 <iNumPoints) ) {
				
				glNormal3f(vertex[index+Width][0],vertex[index+Width][1],vertex[index+1][2]);	
				glVertex3f(vertex[index][0], vertex[index][1],(vertex[index][2]/7));
				glVertex3f(vertex[index+1][0], vertex[index+1][1],(vertex[index+1][2]/7));
				glVertex3f(vertex[index+Width][0], vertex[index+Width][1],(vertex[index+Width][2]/7));

				glVertex3f(vertex[index+Width][0], vertex[index+Width][1],(vertex[index+Width][2]/7));
				glVertex3f(vertex[index+1][0], vertex[index+1][1],(vertex[index+1][2]/7));
				glVertex3f(vertex[index+Width+1][0], vertex[index+Width+1][1],(vertex[index+Width+1][2]/7));

				
				//왼쪽 X면
				if( vertex[index][0] ==0) {
					glVertex3f(0,(index/Width),0);
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f(0,(index/Width)+1,0);
	
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f(vertex[index+Width][0],vertex[index+Width][1],vertex[index+Width][2]/7);
					glVertex3f(0,(index/Width)+1,0);
				}

				//아래 면
				if( vertex[index][1] ==0) {
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f((index%Width),0,0);
					glVertex3f(vertex[index+1][0],vertex[index+1][1],vertex[index+1][2]/7);
	
					glVertex3f(vertex[index+1][0],vertex[index+1][1],vertex[index+1][2]/7);
					glVertex3f((index%Width),0,0);
					glVertex3f((index%Width)+1,0,0);
				}

				//오른쪽 X면
				if( vertex[index][0]+2 == Width) {
					glVertex3f(Width,(index/Width),0);
					glVertex3f(Width,(index/Width)+1,0);
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
	
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f(Width,(index/Width)+1,0);
					glVertex3f(vertex[index+Width][0],vertex[index+Width][1],vertex[index+Width][2]/7);
				}

				//위쪽 면
				if( vertex[index][1]+2== Height) {
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f(vertex[index+1][0],vertex[index+1][1],vertex[index+1][2]/7);
					glVertex3f((index%Width),Height,0);
	
					glVertex3f(vertex[index+1][0],vertex[index+1][1],vertex[index+1][2]/7);
					glVertex3f((index%Width)+1,Height,0);
					glVertex3f((index%Width),Height,0);
				}
			}
		}
	glEnd();
}


/* -------------------------------------------------

@ Display Callback Functions

   -------------------------------------------------*/
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	// set View transformation
	gluLookAt(0.0, 0.0, 1000.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glTranslatef(g_TranslateFactor[0],g_TranslateFactor[1], g_TranslateFactor[2]);
	glScalef(g_ScaleFactor[0],g_ScaleFactor[1],g_ScaleFactor[2]);
	glRotatef(g_RotateFactor[0], 1.0, 0.0, 0.0);
	glRotatef(g_RotateFactor[1], 0.0, 1.0, 0.0);
	glRotatef(g_RotateFactor[2], 0.0, 0.0, 1.0);

	glCallList(g_LoadedObj);

	Draw_Axis();// XYZ축 그리기

	glutSwapBuffers();
	glutPostRedisplay();
}

/* -------------------------------------------------

@ Reshape Callback Functions

   -------------------------------------------------*/
void reshape(int _w, int _h)
{

	glViewport(0, 0, _w, _h);	

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(45.0f, 1.0f * _w / _h, 0.1f, 1600.0f);		//보여지는 화면 설정(fovy,aspect,near,far)
																//fov는 시야각, aspect는 가로세로 종횡비, near, far는 화면에 보여지는 최소, 최대 거리
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	g_WindowSize[0] = _w;
	g_WindowSize[1] = _h;
}

/* -------------------------------------------------

@ Keyboard Callback Functions

   -------------------------------------------------*/
void keyboard(unsigned char _key, int _x, int _y)
{
	switch(_key) {
		case 27:
		case 'q':
		case 'Q':		
			exit(0);
			break;

		case 't':
		case 'T':
			information= "You selected 't' or 'T' : Translate Mode";
			g_TranslateState = true;
			g_ScaleState = false;
			g_RotateState = false;
			printinterface();
			break;

		case 's':
		case 'S':
			information= "You selected 's' or 'S' : Scale Mode";
			g_TranslateState = false;
			g_ScaleState = true;
			g_RotateState = false;
			printinterface();
			break;

		case 'r':
		case 'R':
			information= "You selected 'r' or 'R' : Rotate Mode";
			g_TranslateState = false;
			g_ScaleState = false;
			g_RotateState = true;
			printinterface();
			break;

		case 'z':
			if(g_TranslateState){
				g_TranslateFactor[2] -= 0.1*1000;
			}
			if(g_ScaleState) {
				g_ScaleFactor[2] -= 0.1;
			}
			if(g_RotateState) {
				g_RotateFactor[2] -= 10.0;
			}
			break;

		case 'Z':
			if(g_TranslateState){
				g_TranslateFactor[2] += 0.1*1000;
			}
			if(g_ScaleState) {
				g_ScaleFactor[2] += 0.1;
			}
			if(g_RotateState) {
				g_RotateFactor[2] += 10.0;
			}
			break;
	}
}
void specialKey(int key, int _x, int _y)
{
	switch(key){
		case GLUT_KEY_LEFT :
			if(g_TranslateState) {
				g_TranslateFactor[0] -= 0.1;
			}
			if(g_ScaleState) {
				g_ScaleFactor[0] -= 0.1;
			}
			if(g_RotateState) {
				g_RotateFactor[1] -= 1.0;
			}
			break;

		case GLUT_KEY_RIGHT :
			if(g_TranslateState) {
				g_TranslateFactor[0] += 0.1;
			}
			if(g_ScaleState) {
				g_ScaleFactor[0] += 0.1;
			}
			if(g_RotateState) {
				g_RotateFactor[1] += 1.0;
			}
			break;

		case GLUT_KEY_UP :
			if(g_TranslateState) {
				g_TranslateFactor[1] += 0.1;
			}
			if(g_ScaleState) {
				g_ScaleFactor[1] += 0.1;
			}
			if(g_RotateState) {
				g_RotateFactor[0] -= 1.0;
			}
			break;

		case GLUT_KEY_DOWN :
			if(g_TranslateState) {
				g_TranslateFactor[1] -= 0.1;
			}
			if(g_ScaleState) {
				g_ScaleFactor[1] -= 0.1;
			}
			if(g_RotateState) {
				g_RotateFactor[0] += 1.0;
			}
			break;
	}	
}

/* -------------------------------------------------

@ Mouse Callback Functions

   -------------------------------------------------*/
void buttonPress(int _button, int _state, int _x, int _y)
{
	switch(_button) {
		case GLUT_LEFT_BUTTON:
			if(_state == GLUT_UP) {

			} else if(_state == GLUT_DOWN) {
				if(g_TranslateState) {
					g_TransPrev[0] = _x;
					g_TransPrev[1] = _y;
				}
				if(g_ScaleState) {
					g_ScalePrev[0] = _x;
					g_ScalePrev[1] = _y;
				}
				if(g_RotateState){
					g_RotatePrev[0] = _x;
					g_RotatePrev[1] = _y;
				}
			}
			break;	
		case GLUT_RIGHT_BUTTON:
			if(_state == GLUT_UP) {

			} else if(_state == GLUT_DOWN) {
				if(g_TranslateState) {
					g_TransPrev[0] = _x;
					g_TransPrev[1] = _y;
				}
				if(g_ScaleState) {
					g_ScalePrev[0] = _x;
					g_ScalePrev[1] = _y;
				}
				if(g_RotateState){
					g_RotatePrev[0] = _x;
					g_RotatePrev[1] = _y;
				}
			}
			break;
	}
}

void mouseMove(int _x, int _y)
{

	if(g_TranslateState)  {
		g_TranslateFactor[0] += (_x - g_TransPrev[0])*Width / 100.0;
		g_TranslateFactor[1] += (g_TransPrev[1] - _y)*Height / 100.0;

		g_TransPrev[0] = _x;
		g_TransPrev[1] = _y;
	}
	if(g_ScaleState) {
		float dx = (_x - g_ScalePrev[0]) / 400.0;
		float dy = (g_ScalePrev[1] - _y) / 400.0;
		float scaleV = 0.0;
		scaleV =  dx + dy;

		g_ScaleFactor[0] += scaleV;
		g_ScaleFactor[1] += scaleV;
		g_ScaleFactor[2] += scaleV;
		g_ScalePrev[0] = _x;
		g_ScalePrev[1] = _y;
	}
	if(g_RotateState){
		g_RotateFactor[1] += _x - g_RotatePrev[0];
		g_RotateFactor[0] += _y - g_RotatePrev[1];

		g_RotatePrev[0] = _x;
		g_RotatePrev[1] = _y;
	}
}

void lightModelSetting()
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);	
	glDisable(GL_LIGHTING);	

	//Light Properties
	//Light 속성 선택 여기에..


}

void InitializeGL()
{
	// Light 속성들 여기에
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_NORMALIZE);
	
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

}

void InitializeParam()
{
	g_WindowSize[0] = Width;
	g_WindowSize[1] = Height;
	g_RotateState		= false;
	g_TranslateState	= false;
	g_ScaleState		= false;

	g_TranslateFactor[0] = 0.0- Width/10;
	g_TranslateFactor[1] = 0.0- Height/10;
	g_TranslateFactor[2] = 0.0;

	g_ScaleFactor[0] = 1.0;
	g_ScaleFactor[1] = 1.0;
	g_ScaleFactor[2] = 1.0;
	//lignt 속성 default 세팅 여기에....
}

/* -------------------------------------------------

@ Idle Callback Functions

   -------------------------------------------------*/
void idle()
{
	glutPostRedisplay();
}

/* -------------------------------------------------

@ Main Functions

   -------------------------------------------------*/

void greyPlay(){

	Load_grey("1.bmp",1);
	Load_grey("2.bmp",2);
	Load_grey("3.bmp",3);
	Load_grey("4.bmp",4);
	Load_grey("5.bmp",5);
	Load_grey("6.bmp",6);
	Load_grey("7.bmp",7);

	for(int i=0;i<ShadowPoints;i++){
		int j=Shadow[i];
		vertex[j][2]= vertex[j-1][2];
	}
	

}

void printinterface(){
	system("cls");
	printf("///////////////////////////////////////////////////////\n");
	printf("안녕하세요 3D 이미지 뷰어입니다.\n");
	printf("사용 방법에 대해 설명해 드리겠습니다.\n");
	printf("T,t : 화면 이동(Translate)\n");
	printf("R,r : 화면 회전(Rotate)\n");
	printf("S,s : 화면 확대, 축소(Scale)\n\n");
	printf("마우스와 방향키로 이용하실수 있으며 Z축 번경은 Z,z 키를 이용하시면 됩니다.\n\n\n");
	printf("**주의사항**\n");
	printf("사진의 해상도는 1600*900 이 한계이며\n");
	printf("해상도 이상의 픽셀수를 가진 사진은 인식할 수 없습니다.\n\n\n");
	printf("");
	printf("현재상태: %s\n",information);
	printf("///////////////////////////////////////////////////////\n");
}


int main(int argc, char* argv[])
{
	InitializeParam();
	greyPlay();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// used double buffers, RGBA and depth buffer
	glutInitWindowSize(1024, 768);		// default window size
	glutInitWindowPosition(100, 100);				// default window position
	
	glutCreateWindow("");

	InitializeGL();	// initialize opengl setting

	// set callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(buttonPress);
	glutSpecialFunc(specialKey);
	glutMotionFunc(mouseMove);
	printinterface();

	glutMainLoop();

	return 0;
}