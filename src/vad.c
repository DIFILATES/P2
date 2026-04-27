#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "vad.h"
#include "pav_analysis.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float zcr;
    float p;
    float am;
} Features;

Features compute_features(const float *x, int N) {
    Features feat;
    float win[N]; 

    for (int n = 0; n < N; n++) {
        win[n] = 0.54 - 0.46 * cos(2.0 * M_PI * n / (N - 1));
    }

    feat.p = compute_power_win(x, win, N);
    feat.am = compute_am(x, N);
    feat.zcr = compute_zcr(x, N, 16000); 
    return feat;
}

VAD_DATA * vad_open(float rate, float alpha0, float zcr, int framelength, float lim_veu, float lim_sil) {
    VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
    vad_data->state = ST_INIT;
    vad_data->sampling_rate = rate;
    vad_data->alpha0 = alpha0;
    vad_data->framelength = (float)framelength;
    vad_data->frame_length = rate * (float)framelength * 1e-3;
    
    vad_data->zcr_threshold = zcr;
    vad_data->lim_veu = lim_veu;
    vad_data->lim_sil = lim_sil;
    
    vad_data->init_p = 0.0;
    vad_data->init_N = 0;
    vad_data->noise_power = 0.0; 

    return vad_data;
}

VAD_STATE vad(VAD_DATA *vad_data, float *x, float alpha1, float alpha2) {
    Features f = compute_features(x, vad_data->frame_length);
    
    float p_db = f.p; 
    vad_data->last_feature = p_db;

    float effective_a1 = alpha1;
    float effective_a2 = alpha2;

    if (vad_data->zcr_threshold > 0 && f.zcr > vad_data->zcr_threshold) {
        effective_a1 *= 0.7f; 
        effective_a2 *= 0.7f; 
    }

    float l_act = vad_data->llindar0 + effective_a1; 
    float l_maint = l_act - effective_a2;         
    
    int is_active = (p_db > l_act);
    int is_maintain = (p_db > l_maint);

    switch (vad_data->state) {
        case ST_INIT:
            vad_data->init_p += p_db;
            vad_data->init_N++;
            
            if ((vad_data->init_N * vad_data->framelength) >= 100) {
                vad_data->noise_power = vad_data->init_p / vad_data->init_N;
                vad_data->llindar0 = vad_data->noise_power + vad_data->alpha0; 
                vad_data->state = ST_SILENCE;
            }
            break;

        case ST_SILENCE:
            if (is_active) { 
                vad_data->state = ST_MAYBE_VOICE;
                vad_data->counter = 1;
            } else {
                vad_data->noise_power = 0.98f * vad_data->noise_power + 0.02f * p_db;
                vad_data->llindar0 = vad_data->noise_power + vad_data->alpha0;
            }
            break;

        case ST_MAYBE_VOICE:
            if (is_maintain) {
                vad_data->counter++;
                if ((vad_data->counter * vad_data->framelength) >= vad_data->lim_veu) {
                    vad_data->state = ST_VOICE;
                }
            } else {
                vad_data->state = ST_SILENCE;
            }
            break;

        case ST_VOICE:
            if (!is_maintain) {
                vad_data->state = ST_MAYBE_SILENCE;
                vad_data->counter = 1;
            }
            break;

        case ST_MAYBE_SILENCE:
            if (!is_maintain) {
                vad_data->counter++;
                if ((vad_data->counter * vad_data->framelength) >= vad_data->lim_sil) {
                    vad_data->state = ST_SILENCE;
                }
            } else {
                vad_data->state = ST_VOICE;
            }
            break;
            
        default:
            break;
    }

    if (vad_data->state == ST_VOICE || vad_data->state == ST_MAYBE_SILENCE) {
        return ST_VOICE;
    }
    return ST_SILENCE;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
    VAD_STATE state = (vad_data->state == ST_VOICE) ? ST_VOICE : ST_SILENCE;
    free(vad_data);
    return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
    return vad_data->frame_length;
}

const char *state2str(VAD_STATE st) {
    if (st == ST_VOICE) return "V";
    return "S"; 
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
    fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}