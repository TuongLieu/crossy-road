#include<iostream>
#include<thread>
#include<Windows.h>
#include<time.h>
#include<string>
#include<fstream>
#include<vector>
#include<conio.h>
using namespace std;
//Hằng số
#define MAX_CAR 17
#define MAX_CAR_LENGTH 10
#define MAX_SPEED 3
//Biến toàn cục
POINT** X; //Mảng chứa MAX_CAR xe
POINT Y; // Đại diện người qua đường
int cnt = 0;//Biến hỗ trợ trong quá trình tăng tốc độ xe di chuyển
int MOVING;//Biến xác định hướng di chuyển của người
int SPEED;// Tốc độ xe chạy (xem như level)
int HEIGH_CONSOLE = 20, WIDTH_CONSOLE = 110;// Độ rộng và độ cao của màn hình console
bool STATE; // Trạng thái sống/chết của người qua đường
int *vitriYcu = new int[WIDTH_CONSOLE];
typedef struct
{
	int x, y;
	string data;
}Menu;
void FixConsoleWindow()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX)& ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);

}
void GotoXY(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void clrscr()
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE hConsoleOut;
	COORD Home = { 0, 0 };
	DWORD dummy;

	hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsoleOut, &csbiInfo);

	FillConsoleOutputCharacter(hConsoleOut, ' ', csbiInfo.dwSize.X * csbiInfo.dwSize.Y, Home, &dummy);
	csbiInfo.dwCursorPosition.X = 0;
	csbiInfo.dwCursorPosition.Y = 0;
	SetConsoleCursorPosition(hConsoleOut, csbiInfo.dwCursorPosition);
}
void ResetData()
{
	MOVING = 'D'; // Ban đầu cho người di chuyển sang phải
	SPEED = 1; // Tốc độ lúc đầu
	Y = { 18, 19 }; // Vị trí lúc đầu của người
	// Tạo mảng xe chạy
	if (X == NULL)
	{
		X = new POINT*[MAX_CAR];
		for (int i = 0; i < MAX_CAR; i++)
			X[i] = new POINT[MAX_CAR_LENGTH];
	}
	for (int i = 0; i < MAX_CAR; i++)
	{
		int temp = (rand() % (WIDTH_CONSOLE - MAX_CAR_LENGTH)) + 1;
		for (int j = 0; j < MAX_CAR_LENGTH; j++)
		{
			X[i][j].x = temp + j;
			X[i][j].y = 2 + i;
		}
	}
}
void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
	GotoXY(x, y); cout << 'X';
	for (int i = 1; i < width; i++)cout << 'X';
	cout << 'X';
	GotoXY(x, height + y); cout << 'X';
	for (int i = 1; i < width; i++)cout << 'X';
	cout << 'X';
	for (int i = y + 1; i < height + y; i++)
	{
		GotoXY(x, i); cout << 'X';
		GotoXY(x + width, i); cout << 'X';
	}
	GotoXY(curPosX, curPosY);
}
void StartGame()
{
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE); // Vẽ màn hình game
	STATE = true;//Bắt đầu cho Thread chạy

}
//Hàm dọn dẹp tài nguyên
void GabageCollect()
{
	for (int i = 0; i < MAX_CAR; i++)
	{
		delete[] X[i];
	}
	delete[] X;
}
//Hàm thoát game
void ExitGame(HANDLE t)
{
	system("cls");
	TerminateThread(t, 0);
	GabageCollect();
}
//Hàm dừng game
void PauseGame(HANDLE t)
{
	SuspendThread(t);
}
//Hàm xử lý khi người đụng xe
void ProcessDead()
{
	STATE = 0;
	GotoXY(0, HEIGH_CONSOLE + 5);
	cout << "Dead, type y to continue or anykey to exit";
}
//Hàm xử lý khi người băng qua đường thành công
void ProcessFinish(POINT& p)
{
	SPEED == MAX_SPEED ? SPEED = 1 : SPEED++;
	p = { 18, 19 }; // Vị trí lúc đầu của người
	MOVING = 'D'; // Ban đầu cho người di chuyển sang phải

}
//Hàm vẽ các toa xe
void DrawCars(char* s)
{
	for (int i = 0; i < MAX_CAR; i++)
	{
		for (int j = 0; j < MAX_CAR_LENGTH; j++)
		{
			GotoXY(X[i][j].x, X[i][j].y);
			printf(".");
		}

	}
}
//Hàm vẽ người qua đường
void DrawSticker(const POINT& p, char* s)
{
	GotoXY(p.x, p.y);
	printf(s);

}

//Hàm kiểm tra xem người qua đường có đụng xe không
bool IsImpact(const POINT& p, int d)
{
	
	if (d == 1 || d == 19)return false;
	for (int i = 0; i < MAX_CAR_LENGTH; i++)
	{
		if (p.x == X[d - 2][i].x && p.y == X[d - 2][i].y) return true;
	}
	return false;
}
void IsImpactwith_Y(POINT Y,int *vitriYcu)
{
	if (Y.y == 1)
	{
		if (vitriYcu[Y.x] == 1)
		{
			if (*(vitriYcu + Y.x) == 1)
			{
				int i = 0;
				while (i < WIDTH_CONSOLE + 1)
				{
					if (*(vitriYcu + i) == 1)
					{
						*(vitriYcu + i) = { NULL };
					}
					i++;
				}
				
				ProcessDead();
			}
		}
		else
		{
			vitriYcu[Y.x] = 1;

		}
	}
}
void MoveCars() {
	srand(time(NULL));
	int dong = rand() % MAX_CAR;

	for (int i = 1; i < MAX_CAR; i += 2)
	{
		if (i == dong)break;
		cnt = 0;
		do {
			cnt++;
			for (int j = 0; j < MAX_CAR_LENGTH - 1; j++) {
				X[i][j] = X[i][j + 1];

			}
			X[i][MAX_CAR_LENGTH - 1].x + 1 == WIDTH_CONSOLE ? X[i][MAX_CAR_LENGTH - 1].x = 1 : X[i][MAX_CAR_LENGTH - 1].x++; // Kiểm tra xem xe có đụng màn hình không

		} while (cnt < SPEED);
	}
	for (int i = 0; i < MAX_CAR; i += 2)
	{
		if (i == dong)break;
		cnt = 0;
		do {
			cnt++;
			for (int j = MAX_CAR_LENGTH - 1; j > 0; j--)
			{
				X[i][j] = X[i][j - 1];
			}
			X[i][0].x - 1 == 0 ? X[i][0].x = WIDTH_CONSOLE - 1 : X[i][0].x--;// Kiểm tra xem xe có đụng màn hình không
		} while (cnt < SPEED);

	}

}
// Hàm xóa xe (xóa có nghĩa là không vẽ)
void EraseCars()
{
	for (int i = 0; i < MAX_CAR; i += 2)
	{
		cnt = 0;
		do
		{
			GotoXY(X[i][MAX_CAR_LENGTH - 1 - cnt].x, X[i][MAX_CAR_LENGTH - 1 - cnt].y);
			printf(" ");
			cnt++;

		} while (cnt < SPEED);

	}
	for (int i = 1; i < MAX_CAR; i += 2)
	{
		cnt = 0;
		do
		{
			GotoXY(X[i][0 + cnt].x, X[i][0 + cnt].y);
			printf(" ");
			cnt++;

		} while (cnt < SPEED);

	}
}
void MoveRight()
{
	if (Y.x < WIDTH_CONSOLE - 1)
	{
		DrawSticker(Y, " ");
		Y.x++;
		DrawSticker(Y, "Y");
	}

}
void MoveLeft()
{
	if (Y.x > 1)
	{
		DrawSticker(Y, " ");
		Y.x--;
		DrawSticker(Y, "Y");

	}

}
void MoveDown()
{
	if (Y.y < HEIGH_CONSOLE - 1)
	{
		DrawSticker(Y, " ");
		Y.y++;
		DrawSticker(Y, "Y");
	}

}
void MoveUp()
{
	if (Y.y > 1)
	{
		DrawSticker(Y, " ");
		Y.y--;
		DrawSticker(Y, "Y");
	}
}
void textcolor(int x)
{
	HANDLE mau;
	mau = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(mau, x);
}
void hieu_ung(int x, int y, string s, int color)
{
	GotoXY(x, y);
	textcolor(color);
	cout << s;
	textcolor(7);

}


void SubThread()
{
	while (1)
	{
		if (STATE) //Nếu người vẫn còn sống
		{
			switch (MOVING) //Kiểm tra biến moving
			{
			case 'A':
				MoveLeft();
				break;
			case 'D':
				MoveRight();
				break;
			case 'W':
				MoveUp();
				break;
			case 'S':
				MoveDown();
				break;
			}
			MOVING = ' ';// Tạm khóa không cho di chuyển, chờ nhận phím từ hàm main
			EraseCars();
			MoveCars();
			DrawCars(".");
			if (IsImpact(Y, Y.y))
			{
				hieu_ung(Y.x, Y.y, "(T.T)    OUCH !!!", 14);
				ProcessDead(); // Kiểm tra xe có đụng không
			}
			if (Y.y == 1)
			{
				
				IsImpactwith_Y(Y, vitriYcu);
				ProcessFinish(Y); // Kiểm tra xem về đích chưa

			}


			Sleep(50);//Hàm ngủ theo tốc độ SPEED
		}

	}
}

void menu(vector<Menu> &list)
{
	Menu m;
	m.x = 55;
	m.y = 16;
	m.data = "NEW GAME (C)";
	list.push_back(m);

	m.x = 55;
	m.y = 18;
	m.data = "CONTINUE (T)";
	list.push_back(m);

	m.x = 55;
	m.y = 20;
	m.data = "EXIT (E)";
	list.push_back(m);
}
void In_menu(vector<Menu> list)
{
	for (int i = 0; i < list.size(); i++)
	{
		GotoXY(list[i].x, list[i].y);
		cout << list[i].data;
	}
}
void Draw_Text(string filename)
{
	ifstream FileIn(filename, ios::in);
	if (FileIn.fail())
	{
		cout << "ERROR file text";
		Sleep(5000);
		exit(0);
	}

	int line = 1;
	while (!FileIn.eof())
	{
		string text;
		getline(FileIn, text);

		if (text == "**")
		{
			break;
		}
		else
		{
			GotoXY(2, line++); cout << text;
		}
	}

	FileIn.close();
}
int Save_game(string link)
{
	ofstream fo;
	fo.open(link, ios::out);
	
	if (!fo.fail())
	{
		fo << SPEED << endl;
		for (int i = 1; i < MAX_CAR_LENGTH; i++)
		{
			if (vitriYcu[i] == 1)
			{
				fo << i;
			}
		}
		
		fo.close();
		return 1;
	}
	return 0;
}
int Load_game(string link)
{
	ifstream fi;
	fi.open(link, ios::in);
	if (fi.fail())
	{
		cout << "\nCAN'T OPEN FILE";
		return 0;
	}
	else{
		GotoXY(Y.x, Y.y); cout << " ";
		fi >> SPEED >> Y.x >> Y.y;
		fi.close();
		return 1;
	}
}


void AnConTro()
{
	HANDLE hOut;
	CONSOLE_CURSOR_INFO ConCurInf;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &ConCurInf);
}
void HienConTro()
{
	HANDLE hOut;
	CONSOLE_CURSOR_INFO ConCurInf;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = TRUE;
	SetConsoleCursorInfo(hOut, &ConCurInf);
}


void main()
{
	int temp;
	FixConsoleWindow();
	AnConTro();
	vector <Menu> list;
	menu(list);
	while (1)
	{

		textcolor(14);
		Draw_Text("ascii_generator.txt");
		In_menu(list);
		textcolor(7);
		if (_kbhit())
		{
			char key = _getch();
			if (key == 't')//load game
			{
				hieu_ung(52, 18, ">>", 14);
				Sleep(500);
				string filename;
				GotoXY(52, 22); cout << "\nEnter file name: ";
				fflush(stdin);
				getline(cin, filename);
				filename += ".txt";

				Load_game(filename);





			}
			else if (key == 'e')
			{
				hieu_ung(52, 20, ">>", 14);
				Sleep(500);
				exit(0);
			}
			else if (key == 'c')
			{
				hieu_ung(52, 16, ">>", 14);
				Sleep(500);
				srand(time(NULL));
				StartGame();
				thread t1(SubThread);

				while (1)
				{

					temp = toupper(_getch());
					if (STATE == 1)
					{
						if (temp == 27) {
							ExitGame(t1.native_handle());
							return;

						}
						else if (temp == 'P')
						{
							PauseGame(t1.native_handle());

						}
						else if (temp == 'L')
						{
							PauseGame(t1.native_handle());
							string filename;
							GotoXY(0, 25); cout << "Enter file name to save: ";
							fflush(stdin);
							GotoXY(30, 25); getline(cin, filename);
							filename += ".txt";

							Save_game(filename);

							GotoXY(0, 26); cout << "Saved.";

							Sleep(400);
							for (int i = 0; i < 60; i++)
							{
								GotoXY(i, 25); cout << " ";
								GotoXY(i, 26); cout << " ";
							}

						}
						else
						{
							ResumeThread((HANDLE)t1.native_handle());
							if (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S')
							{
								MOVING = temp;
							}

						}
					}



					else//khi Y chet
					{
						if (temp == 'Y') StartGame();
						else {
							ExitGame(t1.native_handle());
							return;

						}
					}

				}
			}
		}

	}

}

/*srand(time(NULL));
StartGame();
thread t1(SubThread);
while (1)
{
temp = toupper(_getch());
if (STATE == 1)
{
if (temp == 27) {
ExitGame(t1.native_handle());
return;

}
else if (temp == 'P') {
PauseGame(t1.native_handle());

}
else {
ResumeThread((HANDLE)t1.native_handle());
if (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S')
{
MOVING = temp;
}

}
}
else
{
if (temp == 'Y') StartGame();
else {
ExitGame(t1.native_handle());
return;

}
}
}
}*/
