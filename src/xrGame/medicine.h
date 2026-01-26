///////////////////////////////////////////////////////////////
// medicine.h - все классы медицины
// CMedkit - аптечка, повышающая здоровье
// CBandage - Бинт
// CAntirad - таблетки выводящие радиацию
// CStimulator - стимуляторы
///////////////////////////////////////////////////////////////

#pragma once

#include "eatable_item_object.h"
#include "../xrScripts/script_export_space.h"

class CMedicineItem : public CEatableItemObject
{
public:
    virtual ~CMedicineItem() = 0;
};
inline CMedicineItem::~CMedicineItem() {}


class CMedkit final: public CMedicineItem
{
public:
    CMedkit() = default;
    virtual	~CMedkit() = default;
    DECLARE_SCRIPT_REGISTER_FUNCTION
};

class CBandage : public CMedicineItem
{
public:
    CBandage() = default;
    virtual	~CBandage() = default;
    DECLARE_SCRIPT_REGISTER_FUNCTION
};

class CAntirad final: public CMedicineItem
{
public:
    CAntirad() = default;
    virtual ~CAntirad() = default;
    DECLARE_SCRIPT_REGISTER_FUNCTION
};

class CStimulator : public CMedicineItem
{
public:
    CStimulator() = default;
    virtual	~CStimulator() = default;
    DECLARE_SCRIPT_REGISTER_FUNCTION
};
