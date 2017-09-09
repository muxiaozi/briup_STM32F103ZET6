/**
 * 菜单场景
 * 主要用于显示菜单界面，菜单包括以下条目，使用五项按键进行切换，注意屏幕方向
 * 1.开始游戏
 * 2.选择关卡
 * 3.帮助
 * 4.排行榜
 */
#ifndef __BALLMENU_H__
#define __BALLMENU_H__

#include <stm32f10x.h>

/**
 * 进入场景时执行一次
 */
void onEnterMenu(void);

/**
 * 当有按键操作时执行
 * @param type 按键类型 JOY_S JOY_U JOY_D JOY_L JOY_R
 */
void onKeyDownMenu(u8 type);

/**
 * 每隔16ms以中断方式执行一次，帧率60FPS
 */
void onUpdateMenu(void);

/**
 * 传感器事件
 */
void onSensorMenu(short x, short y, short z);


#endif //__BALLMENU_H__