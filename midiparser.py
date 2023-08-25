import os
import sys

import pretty_midi
import serial
import struct
import json

sys.path.append('..')

def midi_to_list(midi):
    """Convert a midi file to a list of note events

    Args:
        midi (path)

    Returns:
        song (list): A list of note events sorted by timestamp, where each note
        event is specified as
            ``[timestamp, pitch, power]``
        with power represented as 0 (off) and 1 (on)
    """

    if isinstance(midi, str):
        midi_data = pretty_midi.pretty_midi.PrettyMIDI(midi)
    elif isinstance(midi, pretty_midi.pretty_midi.PrettyMIDI):
        midi_data = midi
    else:
        raise RuntimeError('midi must be a path to a midi file or pretty_midi.PrettyMIDI')

    score = []
    OFF = 0.0
    ON = 1.0
    # keep sorting in mind for larger scores, since appending an ON and OFF
    # event will require sorting to get things in chronological order.
    # consider using merge sort while adding to score. so: process note, append
    # to score_on and score_off, then merge sort score_on and score_off.
    # score_off is essentially a reserve of notes we know are later in time
    # and need to be sorted in.
    for instrument in midi_data.instruments:
        for note in instrument.notes:
            start_time = note.start
            end_time = note.end
            pitch = note.pitch-9 #piano notes start at midi note 21, for some reason https://signal.vercel.app/edit starts at -12
            score.append([start_time, float(pitch), ON])
            score.append([end_time, float(pitch), OFF])
    # possibly return score with list of notes grouped by time stamp
    score.sort()
    return score

if __name__ == "__main__":
    output = midi_to_list("no name.mid")
    dict_output = {"tempo": 0.0, "num_events": 20, "song":output}
    json_object = json.dumps(dict_output, indent=4)
    with open("no name2.json", "w") as outfile:
        outfile.write(json_object)
