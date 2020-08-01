#include "midifile.h"
#include <stdlib.h>

#define MAGIC_MThd 0x4d546864
#define MAGIC_MTrk 0x4d54726b
#define Midi_size(nTRK) ((2 * sizeof(u16)) + (nTRK) * sizeof(MTrk))

Midi *Midi_new(u8 *mem, size_t _len) {
    if (_len <= 14 || *(u32 *)mem != htobe32(MAGIC_MThd))
        return NULL;
    u16 trk = be16toh(*(u16 *)(mem + 10));
    Midi *m = (Midi *)malloc(Midi_size(trk));
    if (m == NULL)
        return NULL;
    u8 *ptr = mem + 14, *end = mem + _len;
    MTrk *t = m->trks;
    for (u32 len, i = 0; i < trk && ptr < end - 8; i++, t++) {
        if (*(u32 *)ptr != htobe32(MAGIC_MTrk))
            break;
        len = be32toh(*(u32 *)(ptr += 4)); // clang-format off
        t->ptr = \
        t->beg = ptr += 4;
        t->end = ptr += len;
        t->ne  = (len > 0);
        t->rs  = 0; // clang-format on
    }
    if (ptr == end) {
        m->trk = trk;
        m->div = be16toh(*(u16 *)(mem + 12));
        return m;
    } else {
        free(m);
        return NULL;
    }
}
void Midi_del(Midi *m) {
    free(m);
}

u32 MTrk_det(MTrk *t) {
    u8 *ptr = t->ptr, b;
    u32 det = (b = *(ptr++)) & 0x7F;
    for (u8 i = 0; i < 3; i++)
        if (b < 0x80)
            break;
        else {
            det <<= 7;
            det |= (b = *(ptr++)) & 0x7F;
        }
    t->ptr = ptr;
    return det;
}
bool MTrk_evt(MTrk *t, MEvt *e) {
    u8 *ptr = t->ptr; // clang-format off
    u8  cmd = *(ptr++), b = 0;
    u32 det;
    if(cmd < 0x80){
        e->_n.a1 = cmd;
        e->cmd =   cmd = t->rs;  // use rs
    } else {
        e->_n.a1 = b = *(ptr++); // also meta-type or det[0], a1 overlap type
        e->cmd = t->rs = cmd;
    }
    switch(cmd >> 4){
    case 0x8: case 0x9: case 0xA: case 0xB: case 0xE:
        e->_n.a2 = *(ptr++);
    case 0xC: case 0xD:
        return t->ne = ((t->ptr = ptr) < t->end);
    case 0xF: // clang-format on
        if (cmd == 0xFF)
            b = *(ptr++); // skip meta-type
        det = b & 0x7F;
        for (u8 i = 0; i < 3; i++)
            if (b < 0x80)
                break;
            else {
                det <<= 7;
                det |= (b = *(ptr++)) & 0x7F;
            }
        e->ms_data.len = det;
        e->ms_data.ptr = ptr;
        return t->ne = ((t->ptr = ptr + det) < t->end);
    }
    return false;
}