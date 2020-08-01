#include "midifile.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE "/home/jerry/Documents/midi/mid/17.mid"

int main(int argc, char **argv) {
    u8 *map;
    size_t len;
    {
        int fd = open(FILE, O_RDONLY);
        struct stat buf;
        fstat(fd, &buf);
        map = mmap(NULL, len = buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
    }
    //
    Midi *m = Midi_new(map, len);

    printf("ntrks: %d division: %d\n", m->trk, m->div);

    MTrk *t;
    MEvt e;
    uint64_t nc = 0;
    for (u32 i = 0; i < m->trk; i++) {
        t = m->trks + i;
        while (t->ne) {
            MTrk_det(t);
            MTrk_evt(t, &e);
            if ((e.cmd & 0xF0) == 0x90)
                nc++;
        }
    }
    printf("NC: %ld\n", nc);

    Midi_del(m);
    munmap(map, len);
    return 0;
}