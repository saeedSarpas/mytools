#ifndef SNAPSHOT_H
#define SNAPSHOT_H


#include <inttypes.h>


typedef struct _snapshotheader {
  int ndarkpart, ngaspart, nstarpart;
  double mdarkpart, mgaspart, mstarpart;
  double time;
  double redshift;
  double boxsize;
  double Om, Ol, h0;
} snapshotheader;


typedef struct _snapshotparticle {
  int64_t id;
  float Pos[3];
  float Vel[3];
} snapshotparticle;


typedef struct _snapshot {
  snapshotheader *header;
  snapshotparticle *darkparts;
  snapshotparticle *gasparts;
  snapshotparticle *starparts;
  void (*dispose)(struct _snapshot*);
} snapshot;


snapshot* new_snapshot(int, int, int);


#endif /* SNAPSHOT_H */
