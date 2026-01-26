///////////////////////////////////////////////////////////////
// medicine.h - все классы медицины
// CMedkit - аптечка, повышающая здоровье
// CBandage - Бинт
// CAntirad - таблетки выводящие радиацию
// CStimulator - стимуляторы
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "medicine.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s", on)
void CMedkit::script_register(lua_State *L)
{
    module(L)[class_<CMedkit, CGameObject>("CMedkit").def(constructor<>())];
}
void CBandage::script_register(lua_State *L)
{
    module(L)[class_<CBandage, CGameObject>("CBandage").def(constructor<>())];
}
void CAntirad::script_register(lua_State *L)
{
    module(L)[class_<CAntirad, CGameObject>("CAntirad").def(constructor<>())];
}
void CStimulator::script_register(lua_State *L)
{
    module(L)[class_<CStimulator, CGameObject>("CStimulator").def(constructor<>())];
}
