#include <swilib.h>
#include <conf_loader.h>
#include <cfg_items.h>

/** Адаптация конфига под гсс
 * (с) Z.Vova
 */

// на случай если несколько конфигов
//__CFG_BEGIN(1)
//__CFG_BEGIN(2)
// и так далее

//void SetFTsettings (int load, int render, int res, int h)

__CFG_BEGIN(0)

 __root const CFG_HDR cfghdr0 = {CFG_UINT, "SIZE", 0, 100};
 __root const unsigned int CFG_FONT_SIZE = 12;

__root const CFG_HDR cfghdr1={CFG_CBOX,"LOAD MODE", 0, 3};
__root const int CFG_LOAD_MODE = 2;
__root const CFG_CBOX_ITEM cfgcbox1_1[3]={"FT_LOAD_DEFAULT", "FT_LOAD_TARGET_MONO", "FT_LOAD_TARGET_LIGHT"};

__root const CFG_HDR cfghdr2={CFG_CBOX,"RENDER MODE", 0, 3};
__root const int CFG_RENDER_MODE = 1;
__root const CFG_CBOX_ITEM cfgcbox2_1[3]={"FT_RENDER_MODE_NORMAL", "FT_RENDER_MODE_LIGHT", "FT_RENDER_MODE_MONO"};

 __root const CFG_HDR cfghdr3 = {CFG_UINT, "RESOLUTION H/V", 0, 300};
 __root const unsigned int CFG_RESOLUTION = 96;

__CFG_END(0)




/** Правая кнопка по config_data.c
  * Properties -> Advanced -> Use custom command to build this file
  * Ставим галочку
  * Вставляем туда $compiler $options -xc $includes -c $file -o $object -O0
*/
