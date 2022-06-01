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
#define NUMBER_OF_POINT		WindowWidth*WindowHeight			// ���尡���� �ִ� ���ؽ� ����
float vertex[NUMBER_OF_POINT][3]={0};							// ���ؽ� ��ǥ�� �����ϴ� �迭
float back_vert[NUMBER_OF_POINT][3]={0};						// ���� ��ǥ
float Shadow[NUMBER_OF_POINT]={0};								//�׸��ںκ� �迭

char* information;												//�ȳ��� ���ڿ�

int iDrawMode;													// ������Ƽ�� ���� (line, line-strip, triangle, quad �� ����) 
int iNumPoints;													// ������� �Էµ� ���ؽ��� ����
int ShadowPoints=0;												// �׸����� �ȼ��� ��ġ��(�ߺ� ����)
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



#pragma pack(1) // 1����Ʈ ������ ����
struct BITHEADER { //FILE HEADER�� BITHEADER �̸��� ����ü�� ����
    word bfType;
    uint bfSize;
    word bfReserved1;
    word bfReserved2;
    uint bfOffBits; //������ ���� ���� ��ġ
};
struct BITINFO { //Image Header�� BITINFO �̸��� ����ü�� ����
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

struct BITHEADER bf_header; // BITHEADER�� ���� bf_header����
struct BITINFO bi_header; // BITINFO�� ���� bi_header����
struct RGB_Q bmp_pal[256];       // ��Ʈ�� �ȷ�Ʈ
int Width = 1600;
int Height = 900;

float MaxGrey[7]={0};		// 7���� �̹���
float minGrey[7]={0};

float histogram[256]={0};	// 0~255


const char *bmp_name = "air.bmp";
const char *grey_name = "T_grey.bmp";

void Load_grey(char *bmpnam,int imageNumber)
{
    FILE *fbmp, *fgrey;
    size_t bmp_wbytes;  // 24 ��Ʈ �̹����� 1 ���� ����Ʈ ��
    size_t grey_wbytes; //  8 ��Ʈ �̹����� 1 ���� ����Ʈ ��
	byte *bmp, *grey;
    byte  red, green, blue;
    int i, j;                

    // ���� ���� ����
    fbmp = fopen(bmpnam, "rb");
    assert(fbmp != NULL);

    // ��Ʈ�� ���� ������ ��Ʈ�� ���� �б�
    fread(&bf_header, sizeof(bf_header), 1, fbmp);
    fread(&bi_header, sizeof(bi_header), 1, fbmp);

    // ��Ʈ�� ũ��
    Width = bi_header.biWidth;
    Height = bi_header.biHeight;
	
    // 24��Ʈ ��Ʈ���� 1 ������ ����Ʈ ���� 4 �� ����� �ǵ��� ����
	if((Width%4) == 0) Width-=1;
	bmp_wbytes = Width * 3;
	bmp_wbytes += 4 - bmp_wbytes % 4;		//bmp_wbytes = Width * 3 + (4 - bmp_wbytes % 4);
    bmp = (byte *)malloc(bmp_wbytes*Height);

    // 8��Ʈ ��Ʈ���� 1 ������ ����Ʈ ���� 4 �� ����� �ǵ��� ����
    grey_wbytes = Width;
    grey_wbytes += 4 - grey_wbytes % 4;
    grey = (byte *)malloc(grey_wbytes*Height);

    // ��Ʈ�� �б�
    fread(bmp, sizeof(byte), bmp_wbytes*Height, fbmp);
    fclose(fbmp);

    // �׷��� ��ȯ
	int multPoints = 0;
	iNumPoints=0;
    for (i=0; i < Height; i++) {
        for (j=0; j < Width; j++) {
            // �׷��� ��ȯ
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
        // �е�( 1������ Width �Ѵ� �κ� 0 ���� ä��)
        for(; j < grey_wbytes; j++) 
            grey[grey_wbytes*i + j] = 0; 
    }

	for (i=0; i < Height; i++) {
        for (j=0; j < Width; j++) {
            // �׷��� ��ȯ
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
			//������׷� ��Ʈ��Ī
			vertex[multPoints][2] += (grey[grey_wbytes*i + j]-minGrey[imageNumber])*255 / (MaxGrey[imageNumber]- minGrey[imageNumber]);

			multPoints++;
			iNumPoints++;
			grey[grey_wbytes*i + j] = (grey[grey_wbytes*i + j] - minGrey[imageNumber]) * 255 / (MaxGrey[imageNumber] - minGrey[imageNumber]);
			}

			

        }

        // �е�( 1������ Width �Ѵ� �κ� 0 ���� ä��)
        for(; j < grey_wbytes; j++) 
            grey[grey_wbytes*i + j] = 0;
    }
    // �������� ����� 24��Ʈ ��Ʈ���� ����Դϴ�.
    // �׷��̺�Ʈ���� 8��Ʈ ��Ʈ���̹Ƿ� ������ �ٲپ��־�� �մϴ�.
    // 8��Ʈ ������ ��Ʈ���� �ȷ�Ʈ��� �ؼ� ������ ���� �����ϰ�
    // �ֽ��ϴ�. �� �ȷ�Ʈ�� ����� �ּž� �մϴ�.

    // ������� �Ʒ��� �ȷ�Ʈ�� ���Ƿ� 1024��ŭ �ڿ� ��Ʈ�� ������ ����
    bf_header.bfOffBits += 1024;
    // 8��Ʈ ��Ʈ���̹Ƿ� 8�� ����
    bi_header.biBitCount = 8;

    // �ȷ�Ʈ �����
    for (i = 0; i < 256; i++) {
        bmp_pal[i].rgbBlue = i;
        bmp_pal[i].rgbGreen = i;
        bmp_pal[i].rgbRed = i;
        bmp_pal[i].rgbReserved = 0;
    }

    // ���� ����
 //   fgrey = fopen(grey_name, "wb");
 //   fwrite(&bf_header, sizeof(bf_header), 1, fgrey);            // BMPFILE��� ����
 //   fwrite(&bi_header, sizeof(bi_header), 1, fgrey);            // BMPINFO��� ����
 //   fwrite(bmp_pal, sizeof(bmp_pal), 1, fgrey);                 // �ȷ�Ʈ�� ����
 //   fwrite(grey, sizeof(byte), grey_wbytes * Height, fgrey);    // ��Ʈ�� ����
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
void Draw_Axis()// XYZ�� �׸���
{

	//�عٴ� �׸���
	
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

				
				//���� X��
				if( vertex[index][0] ==0) {
					glVertex3f(0,(index/Width),0);
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f(0,(index/Width)+1,0);
	
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f(vertex[index+Width][0],vertex[index+Width][1],vertex[index+Width][2]/7);
					glVertex3f(0,(index/Width)+1,0);
				}

				//�Ʒ� ��
				if( vertex[index][1] ==0) {
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f((index%Width),0,0);
					glVertex3f(vertex[index+1][0],vertex[index+1][1],vertex[index+1][2]/7);
	
					glVertex3f(vertex[index+1][0],vertex[index+1][1],vertex[index+1][2]/7);
					glVertex3f((index%Width),0,0);
					glVertex3f((index%Width)+1,0,0);
				}

				//������ X��
				if( vertex[index][0]+2 == Width) {
					glVertex3f(Width,(index/Width),0);
					glVertex3f(Width,(index/Width)+1,0);
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
	
					glVertex3f(vertex[index][0],vertex[index][1],vertex[index][2]/7);
					glVertex3f(Width,(index/Width)+1,0);
					glVertex3f(vertex[index+Width][0],vertex[index+Width][1],vertex[index+Width][2]/7);
				}

				//���� ��
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

	Draw_Axis();// XYZ�� �׸���

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
	gluPerspective(45.0f, 1.0f * _w / _h, 0.1f, 1600.0f);		//�������� ȭ�� ����(fovy,aspect,near,far)
																//fov�� �þ߰�, aspect�� ���μ��� ��Ⱦ��, near, far�� ȭ�鿡 �������� �ּ�, �ִ� �Ÿ�
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
	//Light �Ӽ� ���� ���⿡..


}

void InitializeGL()
{
	// Light �Ӽ��� ���⿡
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
	//lignt �Ӽ� default ���� ���⿡....
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
	printf("�ȳ��ϼ��� 3D �̹��� ����Դϴ�.\n");
	printf("��� ����� ���� ������ �帮�ڽ��ϴ�.\n");
	printf("T,t : ȭ�� �̵�(Translate)\n");
	printf("R,r : ȭ�� ȸ��(Rotate)\n");
	printf("S,s : ȭ�� Ȯ��, ���(Scale)\n\n");
	printf("���콺�� ����Ű�� �̿��ϽǼ� ������ Z�� ������ Z,z Ű�� �̿��Ͻø� �˴ϴ�.\n\n\n");
	printf("**���ǻ���**\n");
	printf("������ �ػ󵵴� 1600*900 �� �Ѱ��̸�\n");
	printf("�ػ� �̻��� �ȼ����� ���� ������ �ν��� �� �����ϴ�.\n\n\n");
	printf("");
	printf("�������: %s\n",information);
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