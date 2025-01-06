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
#include "animator.h"
#include "tilesdata.h"
#include <cstring>
#include <cstdio>

const CAnimator::animzSeq_t animzTileSeq[] = {
    {
        TILES_DUMMY,
        ANIMZ_DUMMY,
        2,
    },
    {
        TILES_GOBBLIN,
        ANIMZ_GOBBLIN,
        2,
    },
    {
        TILES_OCTOPUS,
        ANIMZ_OCTOPUS,
        2,
    },
    {
        TILES_TEDDY,
        ANIMZ_TEDDY,
        2,
    },
    {
        TILES_VAMPLANT,
        ANIMZ_VAMPLANT,
        2,
    },
    {
        TILES_CHANGE_LINK,
        ANIMZ_CHGLNK,
        2,
    },
    {
        TILES_PLATFORM_UP_DN,
        ANIMZ_PLATFORM,
        1,
    },
    {
        TILES_PLATFORM_LF_RG,
        ANIMZ_PLATFORM,
        1,
    },
    {
        TILES_FISH,
        ANIMZ_LFISH,
        1,
    },
    {
        TILES_DIAMOND,
        ANIMZ_DIAMOND,
        12,
    },
    {
        TILES_FORCE_FIELD,
        ANIMZ_FFIELD,
        8,
    },
    {
        TILES_TRIFORCE,
        ANIMZ_TRIFORCE,
        4,
    },
    {
        TILES_ORB,
        ANIMZ_ORB,
        4,
    },
    {
        TILES_TRANSPORTER,
        ANIMZ_TRANS,
        2,
    },
    {
        TILES_TRANSPORTER_DEST,
        ANIMZ_TRANS,
        2,
    },
};

CAnimator::CAnimator()
{
}

CAnimator::~CAnimator()
{
}

void CAnimator::animate()
{
    ++m_offset;
}

uint8_t CAnimator::at(uint8_t tileID)
{
    constexpr size_t seqCount = sizeof(animzTileSeq) / sizeof(decltype(animzTileSeq[0]));
    for (size_t i = 0; i < seqCount; ++i)
    {
        const auto &seq = animzTileSeq[i];
        if (seq.tileID == tileID)
        {
            return seq.animzID + (m_offset % seq.count);
        }
    }
    printf("unhandled tileID: %.2x\n", tileID);
    return 0;
}

int CAnimator::offset()
{
    return m_offset;
}
