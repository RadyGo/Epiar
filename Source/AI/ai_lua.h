/**\file			ai_lua.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Monday, November 16 2009
 * \brief			Lua Bridge for AI objects
 * \details
 */

#ifndef __H_AI_LUA_
#define __H_AI_LUA_

#include "AI/ai.h"
#include "Utilities/lua.h"

#define EPIAR_SHIP_TABLE "Epiar.Ship"
#define EPIAR_SHIP "Ship"

class AI_Lua{
	public:
		// Functions to communicate with Lua
		static void RegisterAI(lua_State *L);
        static AI **pushShip(lua_State *L);
        static AI **checkShip(lua_State *L, int index);
		static int newShip(lua_State *L);

		// Actions
		static int ShipAccelerate(lua_State* L);
		static int ShipRotate(lua_State* L);
		static int ShipRadarColor(lua_State* L);
		static int ShipFire(lua_State* L);
		static int ShipDamage(lua_State* L);
		static int ShipExplode(lua_State* L);
		static int ShipAddWeapon(lua_State* L);
		static int ShipChangeWeapon(lua_State* L);
		static int ShipAddAmmo(lua_State* L);
		static int ShipSetModel(lua_State* L);

		// Current Ship State
		static int ShipGetID(lua_State* L);
		static int ShipGetAngle(lua_State* L);
		static int ShipGetPosition(lua_State* L);
		static int ShipGetMomentumAngle(lua_State* L);
		static int ShipGetMomentumSpeed(lua_State* L);
		static int ShipGetDirectionTowards(lua_State* L); // Accepts either Angles or Coordinates
		static int ShipGetCurrentWeapon(lua_State* L);
		static int ShipGetCurrentAmmo(lua_State* L);
		static int ShipGetWeapons(lua_State* L);

		// Ship Properties
		static int ShipGetModelName(lua_State* L);
		static int ShipGetHull(lua_State* L);
	private:
};


#endif /* __H_AI_LUA_ */