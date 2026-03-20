#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    float resultat = 1e-12;
    for (int i = 0; i < N; i++) {
        resultat += x[i] * x[i];
    }
    return 10*log10(resultat / N);
}

float compute_power_win(const float *x, const float *win, unsigned int N) {
    float numerador = 0.0;
    float denominador = 0.0;

    for (int i = 0; i < N; i++) {
        // Apliquem la finestra a la mostra i elevem al quadrat: (x[i] * w[i])^2
        float x_enventanada = x[i] * win[i];
        numerador += x_enventanada * x_enventanada;

        // Sumatori de l'energia de la finestra: w[i]^2
        denominador += win[i] * win[i];
    }

    // P_dB = 10 * log10 ( Energia_senyal_enventanada / Energia_finestra )
    float ratio = numerador / denominador;
    if (ratio < 1e-12) ratio = 1e-12; // Protecció per evitar log10(0)
    return 10.0 * log10(ratio);
}

float compute_am(const float *x, unsigned int N) {
    float resultat = 0;
    for(int i = 0; i < N; i++) {
        resultat += fabs(x[i]);
    }
    resultat /= N;
    return resultat;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    int resultat = 0;
    for(int n = 1; n < N; n++)
        if ((x[n-1] >= 0 && x[n] < 0) || (x[n-1] < 0 && x[n] >= 0))
            resultat += 1.0;
    resultat *= fm / (2.0 * N);
    return resultat;

}
