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

#include <stdint.h>
#include <cstdio>
#include <unordered_map>
#include <string>

typedef std::unordered_map<uint16_t, uint8_t> AttrMap;
typedef struct
{
    uint16_t x;
    uint16_t y;
} Pos;

class IFile;

class CMap
{
public:
    CMap(uint16_t len = 0, uint16_t hei = 0, uint8_t t = 0);
    ~CMap();
    uint8_t &at(int x, int y);
    uint8_t *row(int y);
    void set(int x, int y, uint8_t t);
    bool read(const char *fname);
    bool write(const char *fname);
    bool read(FILE *sfile);
    bool read(IFile &file);
    bool write(FILE *tfile);
    void forget();
    int len() const;
    int hei() const;
    bool resize(uint16_t len, uint16_t hei, bool fast);
    const Pos findFirst(uint8_t tileId);
    int count(uint8_t tileId);
    void clear(uint8_t ch = 0);
    uint8_t getAttr(const int x, const int y);
    void setAttr(const int x, const int y, const uint8_t a);
    int size();
    const char *lastError();
    CMap &operator=(const CMap &map);
    bool fromMemory(uint8_t *mem);
    const char *title();
    void setTitle(const char *title);
    uint8_t *data()
    {
        return m_map;
    }
    const AttrMap &attrs()
    {
        return m_attrs;
    }

    enum : uint16_t
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        MAX = RIGHT,
        NOT_FOUND = 0xffff
    };
    void shift(int aim);
    void debug();

    static uint16_t toKey(const int x, const int y);
    static Pos toPos(const uint16_t key);

protected:
    uint16_t m_len;
    uint16_t m_hei;
    uint8_t *m_map;
    int m_size;
    AttrMap m_attrs;
    std::string m_lastError;
    std::string m_title;
};
