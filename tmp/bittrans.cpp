#include <stdio.h>
#include <windows.h>
#include <gl/glut.h>
#include <math.h>

#define NUMBER_OF_POINT		100								// 저장가능한 최대 버텍스 개수
#define WindowWidth			512
#define WindowHeight		512

float vertex[NUMBER_OF_POINT][2];								// 버텍스 좌표를 저장하는 배열

int iDrawMode;												// 프리미티브 종류 (line, line-strip, triangle, quad 등 선택) 
int iNumPoints;												// 현재까지 입력된 버텍스의 개수
int index;

int iTransMode;
float trans_x,trans_y=0;


float glRed = 1.0f, glGreen = 1.0f, glBlue = 1.0f;

/* -------------------------------------------------

@ Display Callback Functions

   -------------------------------------------------*/
void ScanBitmapFile(FILE *fp,unsigned char *arr, int width, int height)
{										
	BITMAPFILEHEADER bfh;										// 비트맵 파일 처리를 위한 헤더 구조체
	BITMAPINFOHEADER bih;										// 비트맵 인포 헤더 구조체

	bih.biClrUsed  = 0;											// 색상의 수
	bih.biBitCount = 24;										// 1픽셀에 사용된 비트수
	bih.biWidth = width;										// 폭
	bih.biHeight = height;										// 높이
	bih.biXPelsPerMeter = 0;									// 가로 해상도
	bih.biYPelsPerMeter = 0;									// 새로 해상도
	bih.biPlanes = 1;											// 비트 플레인 수(항상 1)
	bih.biCompression = BI_RGB;									// 압축 유무
	bih.biSize = sizeof(BITMAPINFOHEADER);						// 구조체의 크기
	bih.biSizeImage = 3 * bih.biWidth * bih.biHeight;			// 이미지 픽셀의 크기
	bih.biClrImportant = 0;										// 중요한 색상 인덱스


	bfh.bfType = 0x4d42;										// 16bit 이미지 타입
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfSize = sizeof(bfh) + sizeof(bih) + bih.biSizeImage;	// 파일전체의 크기
	bfh.bfOffBits = sizeof(bfh) + sizeof(bih);					// 픽셀 시작위치

	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);				// 파일 헤더 쓰기
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);				// 인포 헤더 쓰기
	fwrite(arr, 1, bih.biSizeImage, fp);									
}



void display()
{
	GLint index;
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(glRed, glGreen, glBlue);
	glPointSize(4);
	
	glBegin(GL_POINTS);
		for(index = 0; index < iNumPoints; index++)
		{
			glVertex2f(vertex[index][0], vertex[index][1]);
		}
	glEnd();

	glBegin(iDrawMode);
		for(index = 0; index < iNumPoints; index++)
		{
			glVertex2f(vertex[index][0], vertex[index][1]);
		}
	glEnd();
	glFlush();
}

/* -------------------------------------------------

@ Reshape Callback Functions

   -------------------------------------------------*/
void reshape(int _w, int _h)
{
	glViewport(0, 0, _w, _h);	
	
	// used perspective projection
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluOrtho2D(0.0, 500.0, 0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* -------------------------------------------------

@ Mouse Callback Functions

   -------------------------------------------------*/
void mouse(int _button, int _state, int _x, int _y)
{
	printf("Mouse Is Pressed! (%d, %d, %d, %d)\n",_button, _state, _x, _y);
	printf("%d \n",vertex[0][0]);
	if(_button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN)
	{
		vertex[iNumPoints][0] = _x;
		vertex[iNumPoints][1] = 512 - _y;
		iNumPoints++;
		glutPostRedisplay();
	}
}

/* -------------------------------------------------

@ Keyboard Callback Functions

   -------------------------------------------------*/
void keyboard(unsigned char _key, int _x, int _y)
{
	switch(_key)
	{
		// exit
		case 27:		//Esc
		case 'q':
		case 'Q':
			exit(0);
		break;
		
		// model 선택
			
		case '1':
			iDrawMode = GL_POINTS;
			printf("You selected DrawMode 1 : POINT. \n");
			break;
		case '2':
			iDrawMode = GL_LINES;
			printf("You selected DrawMode 2 : LINE. \n");
			break;	
		case '3':
			iDrawMode = GL_LINE_STRIP;
			printf("You selected DrawMode 3 : LINE_STRIP. \n");
			break;
		case '4':
			iDrawMode = GL_LINE_LOOP;
			printf("You selected DrawMode 4 : LINE_LOOP. \n");
			break;
		case '5':
			iDrawMode = GL_POLYGON;
			printf("You selected DrawMode 5 : POLYGON. \n");
			break;
		case '6':
			iDrawMode = GL_TRIANGLES;
			printf("You selected DrawMode 6 : TRIANGLE. \n");
			break;
		case '7':
			iDrawMode = GL_TRIANGLE_STRIP;
			printf("You selected DrawMode 7 : TRIANGLE_STRIP. \n");
			break;
		case '8':
			iDrawMode = GL_TRIANGLE_FAN;
			printf("You selected DrawMode 8 : TRIANGLE_FAN. \n");
			break;
		case '9':
			iDrawMode = GL_QUADS;
			printf("You selected DrawMode 9 : QUAD. \n");
			break;
		case '0':
			iDrawMode = GL_QUAD_STRIP;
			printf("You selected DrawMode 0 : QUAD_STRIP. \n");
			break;

	// color 선택

		case 'R':
		case 'r':
			glRed = 1.0f;
			glGreen = 0.0f;
			glBlue = 0.0f;
			printf("You selected Red Color.\n");
			break;

		case 'G':
		case 'g':
			glRed = 0.0f;
			glGreen = 1.0f;
			glBlue = 0.0f;
			printf("You selected Green Color.\n");
			break;

		case 'B':
		case 'b':
			glRed = 0.0f;
			glGreen = 0.0f;
			glBlue = 1.0f;
			printf("You selected Blue Color.\n");
			break;


		
	// 이동, 확대, 회전
		case 'T':
		case 't':
			printf("Trans mode\n");
			iTransMode=20;
			break;

		case'S':
		case's':
			printf("Scale mode\n");
			iTransMode=21;
			break;
		case'O':
		case'o':
			printf("rotate mode\n");
			iTransMode=22;
			break;

	// 화면 클리어
		case 'C':
		case 'c':
			iNumPoints=0;
			break;	

	// 비트맵 저장

		case'P':
		case'p':
			FILE *fp;
			fp=fopen("bit.bmp","wb");

			for(int i=0;i<iNumPoints;i++){
			}


			ScanBitmapFile(fp, (unsigned char *)vertex , WindowWidth, WindowHeight);
			fclose(fp);
			break;

		default:
			printf("You pressed %c\n", _key);
			break;
	}
	glutPostRedisplay();
}

void specialKey(int _key, int _x, int _y){
	float vert;
	switch(_key){
		case GLUT_KEY_LEFT :
			if(iTransMode==20){
				trans_x = -0.5;
				trans_y = 0;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x=0.5;
				trans_y=1;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}
			else if(iTransMode==22){
				for(int i=0;i<iNumPoints; i++){
					vert=vertex[i][0];
					vertex[i][0]= vertex[i][0]*cos(0.1)- (vertex[i][1]*sin(0.1)) ;
					vertex[i][1]= vert*sin(0.1)+vertex[i][1]*cos(0.1);
				}
			}
			glutPostRedisplay();
			break;

		case GLUT_KEY_RIGHT:
			if(iTransMode==20){
				trans_x = 0.5;
				trans_y = 0;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x= 1.5;
				trans_y= 1;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}

			else if(iTransMode==22){
				for(int i=0;i<iNumPoints; i++){
					vert=vertex[i][0];
					vertex[i][0]= vertex[i][0]*cos(0.1)- (vertex[i][1]* -sin(0.1));
					vertex[i][1]= vert* -sin(0.1)+vertex[i][1]*cos(0.1);
				}
			}
			glutPostRedisplay();
			break;

		case GLUT_KEY_UP:
			if(iTransMode==20){
				trans_x = 0;
				trans_y = 0.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x= 1;
				trans_y= 1.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}
			glutPostRedisplay();
			break;

		case GLUT_KEY_DOWN:
			if(iTransMode==20){
				trans_x = 0;
				trans_y = -0.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x= 1;
				trans_y= 0.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}
			glutPostRedisplay();
			break;
	}	
}

/* -------------------------------------------------

@ Main Functions

   -------------------------------------------------*/

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);// used double buffers, RGBA and depth buffer
	glutInitWindowSize( WindowWidth, WindowHeight );				// default window size
	glutInitWindowPosition(512,512);			// default window position

	glutCreateWindow("Computer Graphics assignment_2 : Event Handler");
	
	// set callback functions
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutDisplayFunc(display);

	glutMainLoop();
	return 0;
}