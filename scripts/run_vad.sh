#!/bin/bash

# Be sure that this file has execution permissions:
# Use the nautilus explorer or chmod +x run_vad.sh

# Establecemos que el código de retorno de un pipeline sea el del último programa con código de retorno
# distinto de cero, o cero si todos devuelven cero.
set -o pipefail

# Write here the name and path of your program and database
DIR_P2=$HOME/PAV/P2
DB=$DIR_P2/db.v4

# CLAVE: '$@' recoge todos los parámetros que le mande optimizar.sh
CMD="$DIR_P2/bin/vad $@"

for filewav in $DB/*/*wav; do
#    echo
    echo "**************** $filewav ****************"
    if [[ ! -f $filewav ]]; then 
        echo "Wav file not found: $filewav" >&2
        exit 1
    fi

    filevad=${filewav/.wav/.vad}

    # Hemos comentado la llamada antigua y activado la que genera el .wav de salida
    # NOTA: Asegúrate de que en tu vad.docopt la salida wav se asigne con -w
    filewavOut=${filewav/.wav/.vad.wav}
    $CMD -i $filewav -o $filevad -w $filewavOut || exit 1

done

scripts/vad_evaluation.pl $DB/*/*lab

exit 0