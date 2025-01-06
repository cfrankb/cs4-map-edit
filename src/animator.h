/*
    cs4-runtime-sdl
    Copyright (C) 2024  Francois Blanchette

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
#pragma once

#include <inttypes.h>
#include <unordered_map>

class CAnimator
{
public:
    CAnimator();
    ~CAnimator();
    void animate();
    uint8_t at(uint8_t tileID);
    int offset();
    struct animzSeq_t
    {
        const uint8_t tileID;
        const uint8_t animzID;
        const uint8_t count;
    };

protected:
    int m_offset = 0;
};
