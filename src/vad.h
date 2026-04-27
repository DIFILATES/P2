#ifndef VAD_H
#define VAD_H

#include <stdio.h>

typedef enum {
    ST_UNDEF = 0,
    ST_INIT,
    ST_SILENCE,
    ST_MAYBE_VOICE,
    ST_VOICE,
    ST_MAYBE_SILENCE
} VAD_STATE;

typedef struct {
    VAD_STATE state;
    float sampling_rate;
    float frame_length;
    float last_feature;
    float alpha0;
    float framelength;
    float zcr_threshold;
    float lim_veu;
    float lim_sil;
    float llindar0;
    float noise_power; 
    int counter;
    
    float init_p;
    int init_N;
} VAD_DATA;


VAD_DATA * vad_open(float rate, float alpha0, float zcr, int framelength, float lim_veu, float lim_sil);
VAD_STATE vad(VAD_DATA *vad_data, float *x, float alpha1, float alpha2);
VAD_STATE vad_close(VAD_DATA *vad_data);
unsigned int vad_frame_size(VAD_DATA *vad_data);
const char *state2str(VAD_STATE st);
void vad_show_state(const VAD_DATA *vad_data, FILE *out);

#endif