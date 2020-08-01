// clang-format off
#include <endian.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct MTrk {
    u8  *ptr, *end,
        *beg,   rs;
    bool        ne;
} MTrk;
typedef struct Midi {
    u16 trk, div;
    MTrk trks[0];
} Midi;
typedef union  MEvt {
    u8  cmd;
    u32 normal;
    struct {u8 cmd, a1,   a2, _a;} _n;
    struct {u8 cmd, type, _a, _b;}      meta;
    struct {u32 _u; u32 len; u8* ptr;}  ms_data;
} MEvt;

Midi *Midi_new(u8 *mem, size_t len);
void  Midi_del(Midi *m);

u32   MTrk_det(MTrk *t);
bool  MTrk_evt(MTrk *t, MEvt *e);