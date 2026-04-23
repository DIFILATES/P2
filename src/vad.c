#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT", "MAYBE_SILENCE", "MAYBE_VOICE"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) {
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  feat.p = compute_power(x, N);
  feat.am = compute_am(x, N);
  feat.zcr = compute_zcr(x, N, 16000);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->llindar0 = 0.0F;
  vad_data->llindar_a = 0.0F;
  vad_data->llindar_b = 0.0F;
  vad_data->margin = 0.0F;
  vad_data->counter = 0.0F;
  vad_data->min_voice_frames = 0.0F;
  vad_data->min_silence_frames = 0.0F;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
    VAD_STATE state = vad_data->state;

    /* Si mor en un estat de dubte, tornem a l'estat estable anterior */
    if (state == ST_VOICE) {
        state = ST_VOICE;
    } else {
        state = ST_SILENCE;
    }

    free(vad_data);
    return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x, float alpha0) {
    Features f = compute_features(x, vad_data->frame_length);
    vad_data->last_feature = f.p; 

    switch (vad_data->state) {
        case ST_INIT:
            vad_data->llindar0 = f.p + alpha0;
            vad_data->state = ST_SILENCE;
            break;

        case ST_SILENCE:
            if (f.p > vad_data->llindar0) {
                vad_data->state = ST_MAYBE_VOICE;
                vad_data->counter = 1;
            }
            break;

        case ST_VOICE:
            if (f.p < vad_data->llindar0) {
                vad_data->state = ST_MAYBE_SILENCE;
                vad_data->counter = 1;
            }
            break;

        case ST_MAYBE_VOICE:
            if (f.p > vad_data->llindar0) {
                vad_data->counter++;
                if (vad_data->counter >= 5) {
                    vad_data->state = ST_VOICE;
                }
            } else {
                vad_data->state = ST_SILENCE;
            }
            break;

        case ST_MAYBE_SILENCE:
            if (f.p < vad_data->llindar0) {
                vad_data->counter++;
                if (vad_data->counter >= 10) {
                    vad_data->state = ST_SILENCE;
                }
            } else {
                vad_data->state = ST_VOICE;
            }
            break;

        default:
            break;
    }

    if (vad_data->state == ST_MAYBE_VOICE || vad_data->state == ST_INIT) {
        return ST_SILENCE;
    } else if (vad_data->state == ST_MAYBE_SILENCE) {
        return ST_VOICE;
    }

    return vad_data->state; 
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
