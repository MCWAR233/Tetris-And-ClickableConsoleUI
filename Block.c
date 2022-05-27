#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include"ScreenMap.h"

// 方块移动区 12 * 14  起始坐标(1,1)
// 方块提示区 6 * 6  起始坐标(14,1)
// 分数显示区 6 * 6  起始坐标(14,11) 
#define MAP_WIDTH  (21+1)  // 还有'\n'
#define MAP_HIGHT  16
#define BMAP_WIDTH 12  // 还有'\n'
#define BMAP_HIGHT 14

typedef struct tagButton
{
    int left;
    int top;
    int right;
    int bottom;
}Button;

Button b1 = { 55,142,170,156 };
Button b2 = { 55,175,170,199 };
Button b3 = { 55,211,170,236 };
Button b4 = { 144,290,196,305 };
Button b5 = { 55,122,170,150 };
Button b6 = { 55,196,170,223 };

char map[MAP_WIDTH * MAP_HIGHT] = { 0 };
char mapBuf[MAP_WIDTH * MAP_HIGHT] = { 0 };

int screenID = 0;  // 场景ID
int cmdID = 0;     // 记录键盘控制的返回值
int styleID = 0;       // 第种个图形
int newStyleID = 0;    // 变化后的图形,用于绘制方块提示区
// int isCanNewBlock = 0; // 判断是否可以生成新方块，给用户反应时间 

POINT moveblock = { 5, 1 }; // 保存正在移动的方块
POINT moveBlockArr[4] = { 0 }; // 保存正在移动的方块
int isDraw = 0;  // 让第一个场景只绘制一次

HWND hwnd;  // 获得控制台的句柄
HANDLE conHanle;
CONSOLE_CURSOR_INFO conInfo = { 1, 0 };
COORD coord = { 0, 0 }; // 光标坐标位置
RECT rect;    // 屏幕坐标(屏幕)
POINT point;  // 鼠标坐标(屏幕)
POINT wPoint; // 鼠标坐标(窗口)
int nums = 0; // 分数
unsigned int frame = 1;
unsigned long long totalframe = 0;  // 总帧数


// 各个方块的位置映射表（注意用unsigned int）
unsigned int style[] = {
0x00010203, 0x00102030, 0x00010203, 0x00102030,
0x00101121, 0x10011102, 0x00101121, 0x10011102,
0x00011112, 0x01101120, 0x00011112, 0x01101120,
0x00011121, 0x00100102, 0x00102021, 0x02101112,
0x00011020, 0x00101112, 0x01112021, 0x00010212,
0x00011102, 0x00101120, 0x01101112, 0x01101121,
0x00011011, 0x00011011, 0x00011011, 0x00011011
};


void SaveGameData()
{
    FILE* fp;
    if ((fp = fopen("GameData.txt", "wb")) == NULL)
        printf("打开文件失败\n");
    fwrite(map, MAP_WIDTH * MAP_HIGHT, 1, fp);
    fwrite(&styleID, sizeof(int), 1, fp);
    fwrite(&newStyleID, sizeof(int), 1, fp);
    fwrite(&nums, sizeof(int), 1, fp);
    fwrite(&moveblock, sizeof(POINT), 1, fp);
    fwrite(moveBlockArr, sizeof(POINT) * 4, 1, fp);
    fclose(fp);
}

void LoadGameData()
{
    FILE* fp;
    if ((fp = fopen("GameData.txt", "rb")) == NULL)
        printf("打开文件失败\n");
    fread(map, MAP_WIDTH * MAP_HIGHT, 1, fp);
    fread(&styleID, sizeof(int), 1, fp);
    fread(&newStyleID, sizeof(int), 1, fp);
    fread(&nums, sizeof(int), 1, fp);
    fread(&moveblock, sizeof(POINT), 1, fp);
    fread(moveBlockArr, sizeof(POINT) * 4, 1, fp);
    fclose(fp);
}

int GetCmdID()
{
    if (GetAsyncKeyState(0x41) & 0x01)   // 按下是1，抬起是0，一次发一条消息）
        return 1;   // A 1
    else if (GetAsyncKeyState(0x44) & 0x01)
        return 2;   // D 2
    else if (GetAsyncKeyState(0x53) & 0x01)
        return 3;   // S 3
    else if (GetAsyncKeyState(0x20) & 0x01)
        return 4;   // 空格 4
    else if (GetAsyncKeyState(VK_LBUTTON) & 0x01)
        return 6;   // 左键按下 4
    else
        return 0;
}

int DetectBotton(POINT wPoint)
{
    if (cmdID != 6)   // 如果不是鼠标消息。就直接退出
        return 900;
    if (screenID == 1 ) {
        if (wPoint.x >= b1.left && wPoint.x <= b1.right && wPoint.y >= b1.top && wPoint.y <= b1.bottom) {
            strcpy(map, PlayGameScreen);
            screenID = 2;
            moveblock = (POINT){ 5 ,1 };
            styleID = rand() % 28;
        }
        else if (wPoint.x >= b2.left && wPoint.x <= b2.right && wPoint.y >= b2.top && wPoint.y <= b2.bottom) {
            LoadGameData();
            screenID = 2;
        }
        else if (wPoint.x >= b3.left && wPoint.x <= b3.right && wPoint.y >= b3.top && wPoint.y <= b3.bottom) {
            system("cls");
            return 0;  // 关闭程序
        }
    }
    else if(screenID == 2){
        if (wPoint.x >= b4.left && wPoint.x <= b4.right && wPoint.y >= b4.top && wPoint.y <= b4.bottom) {
            OutputDebugString(L"123\n");
            SaveGameData();
            strcpy(map, StartScreen);
            memset(mapBuf, 0, MAP_WIDTH * MAP_HIGHT);  // 清空mapBuf
            screenID = 1;
            isDraw = 0;
        }
    }
    else if (screenID == 3){
        if (wPoint.x >= b5.left && wPoint.x <= b5.right && wPoint.y >= b5.top && wPoint.y <= b5.bottom) {
            strcpy(map, PlayGameScreen);
            memset(mapBuf, 0, MAP_WIDTH * MAP_HIGHT);
            screenID = 2;
            moveblock = (POINT){ 5 ,1 };
            styleID = rand() % 28;
        }
        else if (wPoint.x >= b6.left && wPoint.x <= b6.right && wPoint.y >= b6.top && wPoint.y <= b6.bottom) {
            system("cls");
            return 0;  // 关闭程序
        }
    }
    return 1;  // 程序正常执行
}

int IsDeteleMap()
{
    int isCanDelete = 0;
    int dnum = 0;
    int k = 14;
    for (int i = 14; i >= 1; i--) {
        for (int j = 12; j >= 1; j--) {
            if (map[j + i * MAP_WIDTH] == ' '){
                isCanDelete = 1; break;
            }    
        }
        if (isCanDelete == 1) {
            for (int j = 12; j >= 1; j--) {
                map[j + k * MAP_WIDTH] = map[j + i * MAP_WIDTH];
            }
            isCanDelete = 0;
            k--;
        }
        else 
            dnum += 1; 
    }
    if (dnum == 0)
        return 0;
    else {
        nums += dnum * 12;
        return 1;
    }
}

int IsCanDraw(POINT* BlockArr)
{
    for (int i = 0; i < 4; i++)   // 判断是否有覆盖
        if (map[BlockArr[i].x + BlockArr[i].y * MAP_WIDTH] != ' ')
            return 1;         
    for (int i = 0; i < 4; i++)   // 判断是否到顶
        if (BlockArr[i].y <= 1)
            return 2;   // 游戏结束  
    for (int i = 0; i < 4; i++)
        if (BlockArr[i].y + 1 == 15)  // 判断是否到底
            return 3;
    for (int i = 0; i < 4; i++)
        if (map[BlockArr[i].x + (BlockArr[i].y + 1) * MAP_WIDTH] == '#')  // 判断是否底下有方块
            return 4;
    return 5;
}


void ChangeMap(POINT* newblockarr)
{
    for (int i = 0; i < 4; i++) {  // 改新位置
        map[newblockarr[i].x + newblockarr[i].y * MAP_WIDTH] = '#';
    }
}

void ChangeStylePosi(POINT* blockArr ,POINT *block,int bx,int by)
{
    unsigned int stylePosi = style[styleID];   
    for (int j = 24; j >= 0; j -= 8) {
        unsigned int posi = stylePosi >> j;
        stylePosi <<= (32 - j);
        stylePosi >>= (32 - j);
        blockArr[(32 - j) / 8 - 1].x = posi / 16;
        blockArr[(32 - j) / 8 - 1].y = posi % 16;
    }
    block->x += bx;
    block->y += by;
    for (int i = 0; i < 4; i++) {
        blockArr[i].x += block->x;
        blockArr[i].y += block->y;
    }
}


void ChangeSmallWnd(POINT* oldblockArr)
{
    unsigned int stylePosi = style[newStyleID];
    for (int j = 24; j >= 0; j -= 8) {
        unsigned int posi = stylePosi >> j;
        stylePosi <<= (32 - j);
        stylePosi >>= (32 - j);
        oldblockArr[(32 - j) / 8 - 1].x = posi / 16;
        oldblockArr[(32 - j) / 8 - 1].y = posi % 16;
    }
    for (int i = 0; i < 4; i++) {
        oldblockArr[i].x += 15;
        oldblockArr[i].y += 2;
    }
    for (int i = 0; i < 4; i++)
        map[oldblockArr[i].x + oldblockArr[i].y * MAP_WIDTH] = '#';
}

int MoveBlock()
{
    if (IsDeteleMap() == 1)
        return 10;  

    POINT oldblock = { 0 };    // 保存存变化前的坐标数据
    POINT oldblockArr[4] = { 0 }; // 保存存变化前的坐标数据

    oldblock = moveblock;
    ChangeStylePosi(moveBlockArr, &moveblock, 0, 0);  // 给moveBlockArr赋值
    ChangeStylePosi(oldblockArr, &oldblock, 0, 0);  // 给oldblockArr赋值

    for (int i = 0; i < 4; i++)  // 清空原位置，以免影响后续的判断
        map[oldblockArr[i].x + oldblockArr[i].y * MAP_WIDTH] = ' '; 

    for (int i = 2; i <= 5; i++)  // 清空方块提示区
        for (int j = 14; j <= 18; j++)  
        map[j + i * MAP_WIDTH] = ' ';

    // 按键判断
    if (cmdID == 1 && moveblock.y > 1) {  // A
        ChangeStylePosi(moveBlockArr, &moveblock, -1, 0);
    }
    else if (cmdID == 2 && moveblock.y > 1) {  // D
        ChangeStylePosi(moveBlockArr, &moveblock, 1, 0);
    }
    else if (cmdID == 4 && moveblock.y > 1) { // 空格
        int oldstyleID = styleID;  ////
        if (styleID / 4 == (styleID + 1) / 4) {
            styleID += 1;
            ChangeStylePosi(moveBlockArr, &moveblock, 0, 0);
        }
        else{
            styleID -= 3;
            ChangeStylePosi(moveBlockArr, &moveblock, 0, 0);
        }
    }
    else
        ChangeStylePosi(moveBlockArr, &moveblock, 0, 1);
    totalframe = 0;  // 防止按键检测导致帧数混乱
    
    // 输入方块提示区
    POINT smallblockArr[4] = { 0 }; // 保存存变化前的坐标数据
    ChangeSmallWnd(smallblockArr);

    // 判断新位置是否可行
    int res = IsCanDraw(moveBlockArr);
    if (res == 1) {
        if (IsCanDraw(oldblockArr) == 2) {
            ChangeMap(oldblockArr);
            return 0;   // 游戏结束
        }  
        else {
            moveblock = oldblock; // 不改变原值
            ChangeMap(oldblockArr);
            return 10;
        }
    }
    else if (res == 2){
        ChangeMap(moveBlockArr);
        return 0;
    }
    else if (res == 3 || res == 4) {
            ChangeMap(moveBlockArr);
            return 2;  // 生成新方块
    }
    else if (res == 5) {
        ChangeMap(moveBlockArr);
    }
}


void ShowScreen(char* map, char* mapbuf)
{
    for (int i = 0; i < MAP_WIDTH * MAP_HIGHT; i++)
    {
        if (mapbuf[i] != map[i])
        {
            coord.X = i % MAP_WIDTH;
            coord.Y = i / MAP_WIDTH;
            SetConsoleCursorPosition(conHanle, coord);
            printf("%c", map[i]);
            mapbuf[i] = map[i];
        }
    }
    // 输出分数
    coord.X = 14;
    coord.Y = 11;
    SetConsoleCursorPosition(conHanle, coord);
    printf("%4d\n", nums);
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(conHanle, coord);
}


int main()
{
    srand(time(NULL));  // 加入随机数种子
    hwnd = GetConsoleWindow();  // 获得控制台的句柄
    conHanle = GetStdHandle(STD_OUTPUT_HANDLE);
    // system("color F4");  // 改变建面颜色
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE/*注意这个参数*/), ENABLE_EXTENDED_FLAGS);// 禁用编辑模式，光标无法选中文本
    // 改变窗口的大小
    SetWindowPos(hwnd, HWND_TOP, 200, 10, 400, 500, SWP_SHOWWINDOW);
    // 改变窗口的样式，禁止修改窗口的大小(修改窗口大小会使光标影藏失效)
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
    SetConsoleCursorInfo(conHanle, &conInfo);  // 隐藏光标，防止打印时闪屏
    SetConsoleCursorPosition(conHanle, coord); //改变光标位置

    strcpy(map, StartScreen);   // 加载第一个场景
    screenID = 1;
    styleID = rand() % 28;
    newStyleID = rand() % 28;
    while (1)
    {
        if (screenID == 2) // 特殊场景不受帧数控制
            frame = 30;
        else
            frame = 1;

        // 按键检测不受帧数控制
        if(cmdID == 0)  // 确保每次输入都有回应
            cmdID = GetCmdID();
        GetWindowRect(hwnd, &rect);
        GetCursorPos(&point);
        wPoint.x = point.x - rect.left;
        wPoint.y = point.y - rect.top;
        if (DetectBotton(wPoint) == 0)  // 退出游戏 // 按钮控制不受帧数控制
            break;
        
        if (totalframe % frame == 0 || cmdID == 3 || cmdID == 4 || cmdID == 1 || cmdID == 2)// 特殊按键不受帧数控制
        {
            // Update
            if (screenID == 2) {
                int result = MoveBlock();
                if (result == 0) {   
                    system("cls");
                    memset(mapBuf, 0, MAP_WIDTH * MAP_HIGHT);
                    ShowScreen(map, mapBuf);
                    Sleep(300);
                    system("cls");
                    strcpy(map, failScreen);
                    screenID = 3;
                    isDraw = 0;
                }
                else if (result == 2) {
                    styleID = newStyleID;
                    newStyleID = rand() % 28;
                    moveblock = (POINT){ 5 ,1 };
                }
            }

            // Update release
            cmdID = 0;

            // Rander
            if (isDraw == 0) {
                isDraw = 1;  // 只绘制一次
                printf("%s", map);
            }
            else if (screenID == 2)
                ShowScreen(map, mapBuf);

            // 显示鼠标在窗口中的坐标
            SetConsoleCursorPosition(conHanle, (COORD) { 0, 16 });
            printf("                   ");
            SetConsoleCursorPosition(conHanle, (COORD) { 0, 16 });
            printf("%d %d\n", wPoint.x, wPoint.y);
            SetConsoleCursorPosition(conHanle, (COORD) { 0, 0 });
        }
        totalframe++;
        Sleep(10);
    }
    printf("游戏结束！\n");
    return 0;
}

