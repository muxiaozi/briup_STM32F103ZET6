/**
 * 游戏场景
 */
#include "ballGame.h"
#include "ballCore.h"
#include "briupLCD.h"
#include "image.h"

//全局地图
u8 g_map1[64] = {
	0x18, 0x18, 0x10, 0x10, 0xf0, 0x80, 0xfe, 0x02,
	0x0e, 0x18, 0x10, 0x18, 0x08, 0x18, 0x10, 0x18,
	0x08, 0x18, 0x10, 0x18, 0x08, 0xf0, 0xf0, 0xf0,
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0
};

u8 g_map[144] = {
	0xFF,0x04,0x04,0x06,0x1E,0x02,0x02,0x0F,0x04,//9
	0x0C,0x08,0x08,0x18,0x30,0x20,0x30,0x18,0x10,
	0x3F,0x0C,0x38,0x28,0x28,0x28,0x38,0x10,0x10,
	0x30,0x60,0x60,0x30,0x60,0x60,0x30,0x1F,0x18,
	0x0C,0x0C,0x07,0x01,0x01,0x03,0x07,0x0F,0x08,
	0x08,0xFF,0xC3,0x42,0x66,0x3C,0x38,0x38,0x20,
	0x30,0x18,0x08,0x08,0x08,0x1E,0x05,0x04,0x0C,
	0x0C,0x08,0x08,0x0C,0x18,0x1E,0x30,0x38,0x08,
	0x38,0x20,0x3C,0x2C,0x3C,0x10,0x18,0x3C,0x24,//9
	0x24,0x66,0x66,0xE7,0x3C,0x04,0x04,0x06,0x0C,
	0x08,0x18,0xED,0xCE,0x04,0x0C,0x18,0x36,0x10,
	0x10,0x30,0x1E,0x03,0x01,0x01,0x01,0x01,0x03,
	0x02,0x06,0x04,0x0E,0x0C,0x08,0x18,0x30,0x30,
	0x18,0x18,0x1E,0x07,0x03,0x02,0x02,0x06,0x0E,
	0x1E,0x3E,0x7C,0x3E,0x1C,0x3E,0x7C,0x3E,0x1F,
	0x07,0x06,0x0E,0x02,0x06,0x0E,0x02,0x0C,0x0C
};

//球和地板
Image_TypeDef g_ballImg, g_boardImg;
//缓冲图
Image_TypeDef g_cacheImg;

//地图最下面贴着屏幕顶端时为0，地图往下移动值增加，单位px
u32 g_offset;

//小球的x坐标
u16 g_ballX; 

static void draw(void);
static void drawSky(u8 row);
static void drawBoard(u8 row);
static void drawBall(u8 *gameover);

/**
 * 进入场景时执行一次
 */
void onEnterGame(void)
{
	briupLcdClear(WHITE);
	briupLcdImageInit(&g_boardImg, 40, 40, (u16*)gImage_board);
	briupLcdImageInit(&g_ballImg, 40, 40, (u16*)gImage_ball);
	briupLcdImageInit(&g_cacheImg, 320, 40, gImage_cache);
	
	//初始化地图位置
	g_offset = 7*40;
	//初始化球位置
	g_ballX = (320 - 40) / 2;
	
	draw();
}

/**
 * 当有按键操作时执行
 * @param type 按键类型 JOY_S JOY_U JOY_D JOY_L JOY_R
 */
void onKeyDownGame(u8 type)
{
	switch(type)
	{
		case JOY_L:
			g_offset++;
		break;
		case JOY_R:
			g_offset--;
		break;
		case JOY_U:
			g_offset = 41;
		break;
		case JOY_D:
			g_offset = 42;
		break;
	}

	draw();
}

/**
 * 每隔16ms以中断方式执行一次，帧率60FPS
 */
void onUpdateGame(void)
{
	
}

/**
 * 传感器事件
 */
void onSensorGame(short x, short y, short z)
{
	s16 tb = g_ballX + x;
	s32 to = g_offset - y;
	
	if(tb < 0) tb = 0;
	if(tb > 280) tb = 280;
	g_ballX = tb;
	
	if(to < 40) to = 40;
	if(to > 40 * 150) to = 40*150;
	g_offset = to;
	
	draw();
}

/**
 * 绘制地图
 */
static void draw(void)
{
	u8 r; //当前渲染行
	u8 gameover = 0;
	
	for(r = 0; r < 10; r++) //屏幕高度480px / 每行渲染高度40px = 12行
	{
		//绘制天空
		drawSky(r);
		
		//绘制地板
		drawBoard(r);
		
		//绘制小球
		if(r == 6)
			drawBall(&gameover);
		
		if(gameover) 
		{
			changeScene(SCENE_GAMEOVER);
			return;
		}
		
		//绘制内存数据到屏幕
		briupLcdImageDraw(&g_cacheImg, 0, r * 40);
	}
}

/**
 * 绘制天空
 * @param row 行
 */
static void drawSky(u8 row)
{
	u32 i;
	for(i = 0; i < 320 * 40; i++)
	{
		gImage_cache[i] = 0;
	}
}

/**
 * 绘制地板
 * @param row 行
 */
static void drawBoard(u8 row)
{
	s32 offset;	//当前偏移量
	u8 cr, cy;	//当前行,分割y
	u8 i, j, c;	//临时变量
	
	//计算当前缓存区中地图偏移量
	offset = g_offset - row * 40;
	
	//如果偏移量小于0，说明地图还没滚动到这一行
	if(offset <= 0) 
		return;
	
	//cr：上半部分 cr-1：下半部分
	cr = (offset - 1) / 40;
	cy = (offset - 1) % 40 + 1;
	
	for(c = 0; c < 8; c++) //每行8个小块
	{
		if(g_map[cr] & 1 << (7 - c)) //上半部分有方块
		{
			for(i = 40 - cy; i < 40; i++)
			{
				for(j = 0; j < 40; j++)
				{
					//40 * c 偏移缓冲区别的方块位置
					gImage_cache[(i - (40 - cy)) * 320 + j + 40 * c] = g_boardImg.imgArr[i * 40 + j];
				}
			}
		}

		if(cr > 0 && (g_map[cr - 1] & 1 << (7 - c))) //下半部分有方块
		{
			for(i = 0; i < 40 - cy; i++)
			{
				for(j = 0; j < 40; j++)
				{
					gImage_cache[(i + cy) * 320 + j + 40 * c] = g_boardImg.imgArr[i * 40 + j];
				}
			}
		}
	}
}

/**
 * 绘制小球
 */
static void drawBall(u8 *gameover)
{
	u16 i, j;
	if(g_cacheImg.imgArr[20 * 320 + g_ballX + 20] == 0)
	{
		*gameover = 1;
	}else
	{
		for(i = 0; i < g_ballImg.imgH; i++)
		{
			for(j = 0; j < g_ballImg.imgW; j++)
			{
				if(g_ballImg.imgArr[i * g_ballImg.imgW + j] != 0)
				{
					g_cacheImg.imgArr[i * 320 + g_ballX + j] = g_ballImg.imgArr[i * g_ballImg.imgW + j];
				}
			}
		}
	}
}
