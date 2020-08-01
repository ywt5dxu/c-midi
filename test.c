#include "midifile.h"
#include <stdio.h>

int main(int argc, char **argv) {
    // clang-format off
    u8 data[] = {
        0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06,
        0x00, 0x01, // type 1
        0x00, 0x04, // nTrk 4
        0x01, 0xe0, // divs 480
        0x4d, 0x54, 0x72, 0x6b, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04,
        0x4d, 0x54, 0x72, 0x6b, 0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03,
        0x4d, 0x54, 0x72, 0x6b, 0x00, 0x00, 0x00, 0x02, 0x01, 0x02,
        0x4d, 0x54, 0x72, 0x6b, 0x00, 0x00, 0x00, 0x01, 0x01
    };
    // clang-format on
    Midi *m = Midi_new(data, sizeof(data));
    if (m == NULL)
        return 1;
    printf("Trks: %d Division: %d\n", m->trk, m->div);
    for (u32 i = 0; i < m->trk; i++) {
        printf("%2d ", i);
        fwrite(m->trks[i].ptr - 8, 1, 4, stdout);
        printf(" len: %ld\n", m->trks[i].end - m->trks[i].beg);
    }
    Midi_del(m);
    u8 datb[4] = {0b10000001, 0b10000001, 0b10000001, 0b00000001};
    MTrk t = {NULL, datb + sizeof(datb), NULL, 0x00, true}, *pt = &t;
    // 4
    t.ptr = t.beg = datb;
    printf("4: %d,\t", MTrk_det(pt));
    printf("%d ?= %d,\t1\n", t.ptr == t.end, 0b0000001000000100000010000001);
    // 3
    t.ptr = t.beg = datb + 1;
    printf("3: %d,\t", MTrk_det(pt));
    printf("%d ?= %d,\t1\n", t.ptr == t.end, 0b000000100000010000001);
    // 2
    t.ptr = t.beg = datb + 2;
    printf("2: %d,\t\t", MTrk_det(pt));
    printf("%d ?= %d,\t1\n", t.ptr == t.end, 0b00000010000001);
    // 1
    t.ptr = t.beg = datb + 3;
    printf("1: %d,\t\t", MTrk_det(pt));
    printf("%d ?= %d,\t\t1\n", t.ptr == t.end, 0b0000001);
    //
    u8 datc[] = {
        // clang-format off
        0x00, 0xC0, 0x40,       // dt 0 prg-chg 0x40
        0x00, 0x80, 0x40, 0x40, // dt 0 note-on 0x40 vel 0x40
        0x00, 0xF0, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05,
            // dt 0 sys-ex len 5 dat[5]
        0x00, 0xFF, 0x2F, 0x00  // dt 0 meta type 2F len 0
    }; // clang-format on
    MEvt e;
    /**/ t = (MTrk){datc, datc + sizeof(datc), datc, 0x00, true};
    printf("0 == %d\n", MTrk_det(&t));
    printf("1 == %d\n", MTrk_evt(&t, &e));
    printf("C0 40 == %X %X\n", e._n.cmd, e._n.a1);

    printf("0 == %d\n", MTrk_det(&t));
    printf("1 == %d\n", MTrk_evt(&t, &e));
    printf("80 40 40 == %X %X %X\n", e._n.cmd, e._n.a1, e._n.a2);

    printf("0 == %d\n", MTrk_det(&t));
    printf("1 == %d\n", MTrk_evt(&t, &e));
    printf("5 == %d\n", e.ms_data.len);
    printf("e.ptr + len == ptr: %d\n", (e.ms_data.ptr + e.ms_data.len) == t.ptr);
    printf("dat == 1 2 3 4 5?\ndat :: ");
    for (u32 i = 0; i < e.ms_data.len; i++)
        printf("%X ", e.ms_data.ptr[i]);
    printf("\n");

    printf("0 == %d\n", MTrk_det(&t));
    printf("0 == %d\n", MTrk_evt(&t, &e));
    printf("2F == %X\n", e.meta.type);
    printf("0 == %d\n", e.ms_data.len);

    printf("ptr == end? %d\n", t.ptr == t.end);

    return 0;
}