typedef struct _triplematches_params
{
  char *matches_256_512, *matches_512_1024;
  char *halos_256, *halos_512, *halos_1024;
  char *ic_256, *ic_512, *ic_1024;
  char *snap_256, *snap_512, *snap_1024;
  char *output_dir;
} triplematches_params;


typedef struct _head
{
  int id;
  int *traces;
} head;


static triplematches_params* load_params(const char*);
static void dispose_params(triplematches_params *);
static snapshot* safe_snapshot_load(char*);
