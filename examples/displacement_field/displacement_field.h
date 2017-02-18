#ifndef DISPLACEMENT_FIELD_H
#define DISPLACEMENT_FIELD_H


typedef struct _displacement_field_params
{
  char *snap[3];
  int lowResX, lowResY, lowResZ;
  int lowResBoxLen;
  char *ascii[3];
} displacement_field_params;


#endif /* DISPLACEMENT_FIELD_H */
