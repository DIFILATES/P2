#ifndef _VAD_H
#define _VAD_H
#include <stdio.h>

/* TODO: add the needed states */
typedef enum {ST_UNDEF=0, ST_SILENCE, ST_VOICE, ST_INIT, ST_MAYBE_SILENCE, ST_MAYBE_VOICE} VAD_STATE;

/* Return a string label associated to each state */
const char *state2str(VAD_STATE st);

/* TODO: add the variables needed to control the VAD 
   (counts, thresholds, etc.) */

typedef struct {
  VAD_STATE state;
  float sampling_rate;
  unsigned int frame_length;
  float last_feature; /* for debuggin purposes */
  float margin; /* margin between silence and voice */
  float llindar_a; /* threshold for maybe silence -> voice */
  float llindar_b; /* threshold for maybe voice -> silence */
  float llindar0; /* threshold for voice activity */
  float counter; /* to count the number of frames in the current state, if needed */
  float min_voice_frames; /* minimum number of frames to consider a segment as voice, if needed */
  float min_silence_frames; /* minimum number of frames to consider a segment as silence, if needed */
  } VAD_DATA;

/* Call this function before using VAD: 
   It should return allocated and initialized values of vad_data

   sampling_rate: ... the sampling rate */
VAD_DATA *vad_open(float sampling_rate);

/* vad works frame by frame.
   This function returns the frame size so that the program knows how
   many samples have to be provided */
unsigned int vad_frame_size(VAD_DATA *);

/* Main function. For each 'time', compute the new state 
   It returns:
    ST_UNDEF   (0) : undefined; it needs more frames to take decission
    ST_SILENCE (1) : silence
    ST_VOICE   (2) : voice

    x: input frame
       It is assumed the length is frame_length */
VAD_STATE vad(VAD_DATA *vad_data, float *x, float alpha0);

/* Free memory
   Returns the state of the last (undecided) states. */
VAD_STATE vad_close(VAD_DATA *vad_data);

/* Print actual state of vad, for debug purposes */
void vad_show_state(const VAD_DATA *, FILE *);

#endif
