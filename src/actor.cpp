/*
    cs4-runtime-sdl
    Copyright (C) 2025 Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "actor.h"
#include "game.h"
#include "tilesdata.h"

enum
{
    AIM_UP,
    AIM_DOWN,
    AIM_LEFT,
    AIM_RIGHT
};

uint8_t AIMS[] = {
    AIM_DOWN, AIM_RIGHT, AIM_UP, AIM_LEFT,
    AIM_UP, AIM_LEFT, AIM_DOWN, AIM_RIGHT,
    AIM_RIGHT, AIM_UP, AIM_LEFT, AIM_DOWN,
    AIM_LEFT, AIM_DOWN, AIM_RIGHT, AIM_UP};

CActor::CActor(const uint8_t x, const uint8_t y, const uint8_t type, const uint8_t tileID, const uint8_t aim)
{
    m_x = x;
    m_y = y;
    m_aim = aim;
    m_type = type;
    m_tileID = tileID;
}

CActor::~CActor()
{
}

uint8_t CActor::x() const
{
    return m_x;
}

void CActor::setX(const uint8_t _x)
{
    m_x = _x;
}

uint8_t CActor::y() const
{
    return m_y;
}

void CActor::setY(const uint8_t _y)
{
    m_y = _y;
}

uint8_t CActor::aim() const
{
    return m_aim;
}

uint8_t CActor::type() const
{
    return m_type;
}

uint8_t CActor::tileID() const
{
    return m_tileID;
}

void CActor::setAim(const uint8_t _aim)
{
    m_aim = _aim;
}

bool CActor::canFall() const
{
    auto &game = *CGame::getGame();
    CMap &map = game.map();
    const uint8_t tileID = map.at(m_x, m_y);
    const auto &def = getTileDef(tileID);
    const uint8_t rawData = map.getAttr(m_x, m_y);
    if ((rawData & FILTER_HAZARD) == ENV_WATER)
    {
        return false;
    }
    if (!(rawData & FLAG_HIDDEN) &&
        (def.type == TYPE_LADDER ||
         def.type == TYPE_BRIDGE ||
         def.type == TYPE_ROOT ||
         def.type == TYPE_PULLEY ||
         def.type == TYPE_AUTO_ROPE ||
         def.type == TYPE_SOCKET ||
         def.type == TYPE_DOOR))
    {
        return false;
    }

    const Pos posU = game.translate(Pos{m_x, m_y}, Down);
    const auto tileIDu = map.at(posU.x, posU.y);
    const auto &defU = getTileDef(tileIDu);
    const uint8_t rawDataU = map.getAttr(posU.x, posU.y);
    if ((rawDataU & ENV_FILTER) == (ENV_WATER | BOTTOM_FILTER))
    {
        return false;
    }
    if (!(rawDataU & FLAG_HIDDEN) &&
        (defU.type == TYPE_DIAMOND ||
         defU.type == TYPE_LADDER ||
         defU.type == TYPE_WALLS ||
         defU.type == TYPE_DOOR ||
         defU.type == TYPE_SWITCH))
    {
        return false;
    }
    else if (m_type == TYPE_PLAYER && isMonsterThere(Down))
    {
        return false;
    }
    return true;
}

bool CActor::canMove(const int aim) const
{
    auto &game = *CGame::getGame();
    CMap &map = game.map();
    const Pos &pos = Pos{m_x, m_y};
    const Pos &newPos = game.translate(pos, aim == Fall ? Down : aim);
    if (pos.x == newPos.x && pos.y == newPos.y)
    {
        // translation failed assumes
        // invalid position outside of map
        return false;
    }

    const uint8_t tileID = map.at(newPos.x, newPos.y);
    const auto &def = getTileDef(tileID);
    const uint8_t rawData = map.getAttr(newPos.x, newPos.y);
    const uint8_t attr = rawData & FILTER_ATTR;
    if (m_type == TYPE_MONSTER && aim != Fall)
    {
        CActor tmp = *this;
        tmp.move(aim);
        if (tmp.canFall())
        {
            return false;
        }
    }
    if (m_type == TYPE_PLAYER && isMonsterThere(aim))
    {
        return false;
    }
    else if ((m_type != TYPE_PLAYER) &&
             (aim != Fall) && (attr == ATTR_STOP))
    {
        return false;
    }
    else if (rawData & FLAG_HIDDEN)
    {
        // ignore hidden tiles
        return true;
    }
    else if (def.type == TYPE_BACKGROUND ||
             def.type == TYPE_LADDER ||
             def.type == TYPE_BRIDGE ||
             def.type == TYPE_ROOT ||
             def.type == TYPE_SPECIAL ||
             def.type == TYPE_SOCKET ||
             def.type == TYPE_PULLEY ||
             def.type == TYPE_AUTO_ROPE ||
             def.type == TYPE_TRAP)
    {
        return true;
    }
    else if (m_type == TYPE_PLAYER)
    {
        if (def.type == TYPE_SWAMP ||
            def.type == TYPE_PICKUP ||
            def.type == TYPE_DIAMOND ||
            def.type == TYPE_STOP ||
            def.type == TYPE_SWITCH ||
            def.type == TYPE_KEY ||
            def.type == TYPE_TRANS_SOURCE ||
            def.type == TYPE_TRANS_DEST)
        {
            return true;
        }
        else if (def.type == TYPE_DOOR)
        {
            return game.hasKey(tileID - 1);
        }
        else if (tileID >= TILES_MAX)
        {
            // TODO: Revisit later
            printf("undefined tile=%.2x type=%.2x\n", tileID, def.type);
            return true;
        }
    }
    return false;
}

void CActor::move(const int aim, const bool saveAim)
{
    auto &game = *CGame::getGame();
    Pos pos{m_x, m_y};
    pos = game.translate(pos, aim);
    m_x = pos.x;
    m_y = pos.y;
    if (saveAim)
    {
        m_aim = aim;
    }
}

uint8_t CActor::findNextDir() const
{
    int i = AimCount - 1;
    while (i >= 0)
    {
        int aim = AIMS[m_aim * AimCount + i];
        if (canMove(aim))
        {
            return aim;
        }
        --i;
    }
    return None;
}

bool CActor::isPlayerThere(const uint8_t aim) const
{
    auto &game = *CGame::getGame();
    const auto &player = game.player();
    if (aim == Here)
    {
        return player.x() == m_x && player.y() == m_y;
    }
    const auto newPos = game.translate(Pos{m_x, m_y}, aim);
    return newPos.x == player.x() && newPos.y == player.y();
}

bool CActor::isMonsterThere(const uint8_t aim) const
{
    auto &game = *CGame::getGame();
    auto &monsters = game.monsters();
    const auto newPos = game.translate(Pos{m_x, m_y}, aim);
    for (size_t i = 0; i < monsters.size(); ++i)
    {
        const CActor &actor = monsters[i];
        if (newPos.x == actor.x() && newPos.y == actor.y())
            return true;
    }
    return false;
}

const Pos CActor::pos() const
{
    return Pos{m_x, m_y};
}
