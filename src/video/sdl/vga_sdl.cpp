/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 1997,1998 Enlight Software Ltd.
 * Copyright 2010 Jesse Allen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Filename    : vga_sdl.cpp
//Description : VGA manipulation functions (SDL version)

#include <OVGA.h>
#include <OMOUSE.h>
#include <OSYS.h>
#include <surface.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(Vga);


//------ Define static class member vars ---------//

char    VgaBase::use_back_buf = 0;
char    VgaBase::opaque_flag  = 0;
VgaBuf* VgaBase::active_buf   = &vga_front;      // default: front buffer

//-------- Begin of function VgaSDL::VgaSDL ----------//

VgaSDL::VgaSDL()
{
   front = NULL;
   memset(game_pal, 0, sizeof(SDL_Color)*VGA_PALETTE_SIZE);
   custom_pal = NULL;
   vga_color_table = NULL;
}
//-------- End of function VgaSDL::VgaSDL ----------//


//-------- Begin of function VgaSDL::~VgaSDL ----------//

VgaSDL::~VgaSDL()
{
   deinit();
}
//-------- End of function VgaSDL::~VgaSDL ----------//


//-------- Begin of function VgaSDL::init ----------//

int VgaSDL::init()
{
   if (SDL_Init(SDL_INIT_VIDEO))
      return 0;

   front = SDL_SetVideoMode(VGA_WIDTH, VGA_HEIGHT, VGA_BPP, SDL_HWSURFACE|SDL_HWPALETTE|SDL_FULLSCREEN);
   if (!front)
   {
      SDL_Quit();
      return 0;
   }

   SDL_WM_SetCaption(WIN_TITLE, WIN_TITLE);

   sys.active_flag = 1;

   return 1;
}
//-------- End of function VgaSDL::init ----------//


//-------- Begin of function VgaBuf::init_front ----------//
//
// Create a direct draw front buffer.
//
int VgaSDL::init_front(VgaBuf *b)
{
   b->init(new SurfaceSDL(front), 1);
   SDL_SetColors(front, game_pal, 0, VGA_PALETTE_SIZE);
   return 1;
}
//-------- End of function VgaBuf::init_front ----------//


//-------- Begin of function VgaBuf::init_back ----------//
//
// Create a direct draw back buffer.
//
// [DWORD] w      : width of the surface [default 0 : VGA_WIDTH]
// [DWORD] h      : height of the surface [default 0 : VGA_HEIGHT]
//
int VgaSDL::init_back( VgaBuf *b, unsigned long w, unsigned long h )
{
   SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                               VGA_WIDTH,
                                               VGA_HEIGHT,
                                               VGA_BPP,
                                               0, 0, 0, 0);
   if (!surface)
   {
      ERR("Surface not created!\n");
      return 0;
   }

   SurfaceSDL *wrapper = new SurfaceSDL(surface);
   SDL_SetPalette(surface, SDL_LOGPAL, game_pal, 0, VGA_PALETTE_SIZE);
   b->init(wrapper, 0);
   return 1;
}
//-------- End of function VgaBuf::init_back ----------//


//-------- Begin of function VgaSDL::deinit ----------//

void VgaSDL::deinit()
{
   if (vga_color_table) delete vga_color_table;
   SDL_Quit();
}
//-------- End of function VgaSDL::deinit ----------//


//--------- Start of function VgaSDL::init_pal ----------//
//
// Loads the default game palette specified by fileName. Creates the ddraw
// palette.
//
int VgaSDL::init_pal(const char* fileName)
{
   char palBuf[VGA_PALETTE_SIZE][3];
   File palFile;

   palFile.file_open(fileName);
   palFile.file_seek(8);               // bypass the header info
   palFile.file_read(palBuf, VGA_PALETTE_SIZE*3);
   palFile.file_close();

   for (int i = 0; i < VGA_PALETTE_SIZE; i++)
   {
      game_pal[i].r = palBuf[i][0];
      game_pal[i].g = palBuf[i][1];
      game_pal[i].b = palBuf[i][2];
   }

   //----- initialize interface color table -----//

   PalDesc palDesc( (unsigned char*) game_pal, sizeof(SDL_Color), VGA_PALETTE_SIZE, 8);
   vga_color_table = new ColorTable;
   vga_color_table->generate_table( MAX_BRIGHTNESS_ADJUST_DEGREE, palDesc, ColorTable::bright_func );

   return 1;
}
//----------- End of function VgaSDL::init_pal ----------//

//--------- Start of function VgaSDL::refresh_palette ----------//
//
// When the system changes the palette, this function will set
// the palette back to the correct entries.
//
void VgaSDL::refresh_palette()
{
   if (custom_pal)
      SDL_SetColors(front, custom_pal, 0, VGA_PALETTE_SIZE);
   else
      SDL_SetColors(front, game_pal, 0, VGA_PALETTE_SIZE);
}
//----------- End of function VgaSDL::refresh_palette ----------//


//-------- Begin of function VgaSDL::activate_pal ----------//
//
// we are getting the palette focus, select our palette
//
void VgaSDL::activate_pal(VgaBuf* vgaBufPtr)
{
   SurfaceSDL *surface = vgaBufPtr->get_buf();
   surface->activate_pal(game_pal, 0, VGA_PALETTE_SIZE);
}
//--------- End of function VgaSDL::activate_pal ----------//


//-------- Begin of function VgaSDL::set_custom_palette ----------//
//
// Read the custom palette specified by fileName and set to display.
//
int VgaSDL::set_custom_palette(char *fileName)
{
   if (!custom_pal)
      custom_pal = (SDL_Color*)mem_add(sizeof(SDL_Color)*VGA_PALETTE_SIZE);

   char palBuf[VGA_PALETTE_SIZE][3];
   File palFile;

   palFile.file_open(fileName);
   palFile.file_seek(8);     				// bypass the header info
   palFile.file_read(palBuf, VGA_PALETTE_SIZE*3);
   palFile.file_close();

   for(int i=0; i<VGA_PALETTE_SIZE; i++)
   {
      custom_pal[i].r = palBuf[i][0];
      custom_pal[i].g = palBuf[i][1];
      custom_pal[i].b = palBuf[i][2];
   }

   refresh_palette();

   return 1;
}
//--------- End of function VgaSDL::set_custom_palette ----------//


//--------- Begin of function VgaSDL::free_custom_palette ----------//
//
// Frees the custom palette and restores the game palette.
//
void VgaSDL::free_custom_palette()
{
   if (custom_pal)
   {
      mem_del(custom_pal);
      custom_pal = NULL; 
   }
   refresh_palette();
}
//--------- End of function VgaSDL::free_custom_palette ----------//


//-------- Begin of function VgaSDL::adjust_brightness ----------//
//
// <int> changeValue - the value to add to the RGB values of
//                     all the colors in the palette.
//                     the value can be from -255 to 255.
//
// <int> preserveContrast - whether preserve the constrast or not
//
void VgaSDL::adjust_brightness(int changeValue)
{
   //---- find out the maximum rgb value can change without affecting the contrast ---//

   int          i;
   int          newRed, newGreen, newBlue;
   SDL_Color palBuf[VGA_PALETTE_SIZE];

   //------------ change palette now -------------//

   for( i=0 ; i<VGA_PALETTE_SIZE ; i++ )
   {
      newRed   = (int)game_pal[i].r + changeValue;
      newGreen = (int)game_pal[i].g + changeValue;
      newBlue  = (int)game_pal[i].b + changeValue;

      palBuf[i].r = MIN(255, MAX(newRed,0));
      palBuf[i].g = MIN(255, MAX(newGreen,0));
      palBuf[i].b = MIN(255, MAX(newBlue,0));
   }

   //------------ set palette ------------//

   vga_front.temp_unlock();

   SDL_SetColors(front, palBuf, 0, VGA_PALETTE_SIZE);

   vga_front.temp_restore_lock();
}
//--------- End of function VgaSDL::adjust_brightness ----------//


//-------- Begin of function VgaSDL::handle_messages --------//
void VgaSDL::handle_messages()
{
   SDL_Event event;

   SDL_PumpEvents();

   while (SDL_PeepEvents(&event,
                         1,
                         SDL_GETEVENT,
                         SDL_ALLEVENTS ^
                         SDL_KEYEVENTMASK ^
                         SDL_MOUSEEVENTMASK ^
                         SDL_JOYEVENTMASK) > 0) {

      switch (event.type) {
      case SDL_ACTIVEEVENT:
         sys.active_flag = event.active.gain;
         if (sys.active_flag) 
         {
            sys.need_redraw_flag = 1;
            sys.unpause();
         } else {
            sys.pause();
         }
         break;
      case SDL_QUIT:
         sys.signal_exit_flag = 1;
         break;
      default:
         ERR("unhandled event %d\n", event.type);
         break;
      }
   }
}
//-------- End of function VgaSDL::handle_messages --------//

//-------- Begin of function VgaSDL::flag_redraw --------//
void VgaSDL::flag_redraw()
{
}
//-------- End of function VgaSDL::flag_redraw ----------//


//-------- Begin of function VgaSDL::toggle_full_screen --------//
void VgaSDL::toggle_full_screen()
{
   SDL_WM_ToggleFullScreen(front);
}
//-------- End of function VgaSDL::toggle_full_screen ----------//

