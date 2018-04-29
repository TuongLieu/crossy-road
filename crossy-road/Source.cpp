#include<iostream>
#include<conio.h>
#include<thread>
#include<Windows.h>
#include<time.h>
#include<vector>
#include<string>
#include<fstream>
using namespace std;
//Hằng số
#define MAX_CAR 17
#define MAX_CAR_LENGTH 20
#define MAX_SPEED 3
//Biến toàn cục
POINT** X; //Mảng chứa MAX_CAR xe
POINT Y; // Đại diện người qua đường
int cnt = 0;//Biến hỗ trợ trong quá trình tăng tốc độ xe di chuyển
int MOVING;//Biến xác định hướng di chuyển của người
int SPEED;// Tốc độ xe chạy (xem như level)
int HEIGH_CONSOLE = 20, WIDTH_CONSOLE = 110;// Độ rộng và độ cao của màn hình console
bool STATE; // Trạng thái sống/chết của người qua đường
vector<int> vitriYcu;
typedef struct  
{
	int x, y;
	string data;
}Menu;
typedef struct{
	int xPos;
	int yPos;
	int level;

}sPlayer;

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
void textcolor(int x)
{
	HANDLE mau;
	mau = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(mau, x);
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
void hieu_ung(int x, int y,string s,int color)
{
	GotoXY(x, y);
	textcolor(color);
	cout << s;
	textcolor(7);

}

void MoveCars() 
{
	
	for (int i = 1; i < MAX_CAR; i += 2)
	{
		cnt = 0;
		do {
			cnt++;
			for (int j = 0; j < MAX_CAR_LENGTH - 1; j++) 
			{
				X[i][j] = X[i][j + 1];

			}
			X[i][MAX_CAR_LENGTH - 1].x + 1 == WIDTH_CONSOLE ? X[i][MAX_CAR_LENGTH - 1].x = 1 : X[i][MAX_CAR_LENGTH - 1].x++; // Kiểm tra xem xe có đụng màn hình không

		} while (cnt < SPEED);
	}


	for (int i = 0; i < MAX_CAR; i += 2)
	{
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
void Stop_car()
{	
	for (int j = 0; j < MAX_CAR_LENGTH; j++)
	{
		X[2]
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

void Luu_toa_do_x( POINT &p, vector <int> &vitriYcu)
{
	if (p.y == 1)
	{
		vitriYcu.push_back(p.x);
	}
}
bool IsImpactWithY(POINT &p, vector<int> vitriYcu)
{
	
		for (int i = 0; i < vitriYcu.size(); i++)
		{
			if (p.x == vitriYcu[i])
			{
				return true;
			}
		}	
	return false;
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
			//Stop_car();
			MoveCars();
			DrawCars(".");
			
			if (IsImpact(Y, Y.y))
			{
				hieu_ung(Y.x, Y.y,"(@.@)    OUCH !!!",14);
				ProcessDead(); // Kiểm tra xe có đụng không
			}
			if (Y.y == 1)
			{
				Luu_toa_do_x(Y, vitriYcu);
				if (SPEED>1)
				{
					bool kt = IsImpactWithY(Y, vitriYcu);
					if (kt == 1)
					{
						ProcessDead();
					}
				}
				else{
					ProcessFinish(Y); // Kiểm tra xem về đích chưa
				}
				

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
void save_in_struct(POINT &p,sPlayer& save)
{
	save.xPos = p.x;
	save.yPos = p.y;
	save.level = SPEED;
}
void Save(sPlayer save,vector<int>viytiYcu)
{
	
	string filename;
	cout << "\nNhap ten tap tin muon luu: ";
	
	fflush(stdin);
	GotoXY(0, 26); getline(cin, filename);
	filename += ".txt";

	ofstream fo(filename, ios::out);
	if (fo.fail())
	{
		cout << "\n ERROR WITH FILE.";
	}

	for (int i = 0; i < vitriYcu.size(); i++)
	{
		fo << vitriYcu[i] << "\t\t";		
	}
	fo << "\n\n";
	fo << save.level << "\t\t" << save.xPos << "\t\t" << save.yPos;

	fo.close();
}
void Load_game(string filename,vector<int>vitriYcu)
{
	ifstream fi(filename, ios::in);
	if (fi.fail())
	{
		cout << "ERROR.";
	}
	int x;
	while (fi >> x)
	{
		if (x != '\n')
		{
			vitriYcu.push_back(x);
		}
		else
		{
			
		}
	}



}
 void main()
 {		
			 int temp;
			 string gamefile;
			 FixConsoleWindow();
			 AnConTro();
			 vector <Menu> list;
			 sPlayer save;
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
					 if (key == 'T' || key == 't')
					 {
						 hieu_ung(52, 18, ">>", 14);
						 Sleep(500);
						 cout << "\nNhap ten tap tin: ";
						 fflush(stdin);
						 getline(cin, gamefile);
						 gamefile += ".txt";

						 ifstream fi(gamefile, ios::in);


						

					 }
					 else if (key == 'E' || key == 'e')
					 {
						 hieu_ung(52, 20, ">>", 14);
						 Sleep(500);
						 exit(0);
					 }
					 else if (key == 'C' || key == 'c')
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
									
									 save_in_struct(Y,save);
									 GotoXY(0, 25);
									 Save(save, vitriYcu);

									 
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
	 
	 
 
	








