#ifndef SNAPSHOT_H
#define SNAPSHOT_H


#define PARTICLENOTSET -1


#include <inttypes.h>


typedef struct _snapshotheader {
  int npart[6]; // enum('Gas', 'Halo', 'Disk', 'Bulge', 'Stars', 'Bndry')
  int tot_nparticles;
  double mass[6]; // enum('Gas', 'Halo', 'Disk', 'Bulge', 'Stars', 'Bndry')
  double time;
  double redshift;
  double boxsize;
  double Om, Ol, h0;
} snapshotheader;


typedef struct _snapshotparticle {
  int64_t id;
  float pos[3];
  float vel[3];
  double mass;
  int type; // enum('Gas', 'Halo', 'Disk', 'Bulge', 'Stars', 'Bndry')
} snapshotparticle;


typedef struct _snapshot {
  snapshotheader *header;
  snapshotparticle *particles;
  void (*dispose)(struct _snapshot*);
} snapshot;


snapshot* new_snapshot(int);
void dispose_snapshot(snapshot*);


#endif /* SNAPSHOT_H */
