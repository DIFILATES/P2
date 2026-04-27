#ifndef VAD_H
#define VAD_H

#include <stdio.h>

typedef enum {ST_UNDEF=0, ST_SILENCE, ST_VOICE, ST_INIT, ST_MAYBE_SILENCE, ST_MAYBE_VOICE} VAD_STATE;

typedef struct {
    VAD_STATE state;
    float sampling_rate;
    unsigned int frame_length; 
    float last_feature;
    
    float llindar0;      
    float alpha0;        
    float zcr_threshold;    
    float framelength;   
    float lim_veu;
    float lim_sil;
    int counter;   
    float *window;      
} VAD_DATA;

VAD_DATA *vad_open(float rate, float alpha0, float zcr, int framelength, float lim_veu, float lim_sil);
VAD_STATE vad_close(VAD_DATA *vad_data);
unsigned int vad_frame_size(VAD_DATA *vad_data);
VAD_STATE vad(VAD_DATA *vad_data, float *x, float alpha1, float alpha2);

const char *state2str(VAD_STATE st);
void vad_show_state(const VAD_DATA *vad_data, FILE *out);

#endif