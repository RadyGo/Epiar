/**\file			ui_lua.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Lua bridge for UI objects
 * \details
 */


#ifndef __H_UI_LUA
#define __H_UI_LUA

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#include "UI/ui.h"
#include "ui_label.h"
#include "ui_window.h"
#include "ui_button.h"

#define EPIAR_UI_TABLE "Epiar.UI"
#define EPIAR_UI "UI"

class UI_Lua {
	public:
		// Functions to communicate with Lua
		static void RegisterUI(lua_State *L);

		// Functions to create new Widgets
		static int newWindow(lua_State *L);
		static int newButton(lua_State *L);
		static int newTextbox(lua_State *L);
		static int newLabel(lua_State *L);
		static int newPicture(lua_State *L);
		static int newCheckbox(lua_State *L);

		// Fuctions to get Widget information
		static int IsChecked(lua_State *L);
		static int GetText(lua_State *L);

		// Functions to manipulate Widgets
		static int add(lua_State *L);
		static int close(lua_State *L);
		static int rotatePicture(lua_State *L);
		static int setText(lua_State *L);
		static int setLabel(lua_State *L);
		static int setPicture(lua_State *L);
		static int setChecked(lua_State *L);
	private:
};
#endif /*__H_UI_LUA*/