#include <FastLED.h>
#include <MIDI.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <ArduinoJson.h>

#define LED_PIN     17
#define NUM_LEDS    10
#define BRIGHTNESS  3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
StaticJsonDocument<768> doc;
std::vector<std::vector<float>> song_vec;

#define UPDATES_PER_SECOND 100
// convert midi file to bytestream
// #define std::ifstream jsonFile("D:\Documents\Audiveris\maple-a4\usersGuide_04_stream1_47_0.json");
// #define StaticJsonBuffer<300> buffer;
MIDI_CREATE_DEFAULT_INSTANCE();

class Chord {
  public:
    int duration;
    int pitches[3];
};

void setup() {
    Serial.begin(9600);
    while (!Serial);
    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI.setHandleNoteOn(turnOnLight);
    MIDI.setHandleNoteOff(turnOffLight);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
    const int capacity = JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(capacity);
    // DeserializationError err = deserializeJson(doc, input);
    const char* json = "{\"tempo\":0,\"num_events\":20,\"song\":[[0,0,1],[0.25,0,0],[0.25,1,1],[0.5,1,0],[0.5,3,1],[0.75,2,1],[1,2,0],[1,3,0],[1,5,1],[1.25,3,1],[1.25,5,0],[1.5,1,1],[1.5,2,1],[1.5,3,0],[1.5,4,1],[1.75,1,0],[1.75,2,0],[1.75,4,0],[2,6,1],[2.25,6,0]]}";
    deserializeJson(doc, json);
    // std::vector<std::vector<float>> song;
    
    // auto song = doc["song"].as<std::vector<std::vector<float>>>();
    // if (error) {
    //   Serial.print(F("deserializeJson() failed: "));
    //   Serial.println(error.f_str());
    //   return;
    // }

    int tempo = doc["tempo"]; // 0
    Serial.println("tempo: ");
    Serial.println(tempo);
    int num_events = doc["num_events"]; // 20
    Serial.println("num events: ");
    Serial.println(num_events);
    

    JsonArray song = doc["song"];
    // std::vector<std::vector<float>> song_vec;
    for (int i=0;i<num_events;i++){
      JsonArray event = song[i];
      std::vector<float> event_vec;
      for (int j=0;j<3;j++) {
        Serial.println("event:: ");
        Serial.println(float(event[j]));
        event_vec.push_back(float(event[j]));
      }
      song_vec.push_back(event_vec);
    }
}


void loop() {
    // Serial.println("in loop");
    // MIDI.read()
    // change implementation into array, and send the song array size as first byte when sending to the arduino
    std::vector<float> li1{0.0, 0.0, 1.0};
    std::vector<float> li2{0.25, 0.0, 0.0};
    std::vector<float> li3{0.25, 1.0, 1.0};
    std::vector<float> li4{0.5, 1.0, 0.0};
    std::vector<float> li5{0.5, 3.0, 1.0};
    std::vector<float> li6{0.75, 2.0, 1.0};
    std::vector<float> li7{1.0, 2.0, 0.0};
    std::vector<float> li8{1.0, 3.0, 0.0};
    std::vector<float> li9{1.0, 5.0, 1.0};
    std::vector<float> li10{1.25, 3.0, 1.0};
    std::vector<float> li11{1.25, 5.0, 0.0};
    std::vector<float> li12{1.5, 1.0, 1.0};
    std::vector<float> li13{1.5, 2.0, 1.0};
    std::vector<float> li14{1.5, 3.0, 0.0};
    std::vector<float> li15{1.5, 4.0, 1.0};
    std::vector<float> li16{1.75, 1.0, 0.0};
    std::vector<float> li17{1.75, 2.0, 0.0};
    std::vector<float> li18{1.75, 4.0, 0.0};
    std::vector<float> li19{2.0, 6.0, 1.0};
    std::vector<float> li20{2.25, 6.0, 0.0};
    // std::vector<std::vector<float>> song{li1, li2, li3, li4, li5, li6, li7, li8, li9, li10, li11, li12, li13, li14, li15, li16, li17, li18, li19, li20};


    // float prev_stamp = 0;
    // int duration = 0;
    // calculate duration by subtracting curr timestamp from prev one, then keep looping through notes while the timestamps are the same and set the LEDs and set a delay eq to duraton
    int note_window_start = 0;
    int note_window_end = 1;
    // Serial.println("song size: ");
    // Serial.println(song_vec.size());
    while (note_window_end < song_vec.size()) {
      float curr_note_time = song_vec[note_window_start][0];
      // get window size of all notes at the same time stamp
      while (curr_note_time == song_vec[note_window_end][0]) {
        note_window_end++;
      }
      // iterate through all notes of same time stamp and set LED
      for (int i=note_window_start; i<note_window_end; i++) {
        // Serial.println("curr note");
        // Serial.println(i);
        std::vector<float> curr_note = song_vec[i];
        int pitch = (int)curr_note[1];
        leds[pitch] = (curr_note[2] == 0.0) ? CRGB::Black : CRGB::Purple;
      }
      // set delay, subtract timestamp of current window from timestamp of next window
      // error: if last note is at the same time as prev note
      float delay = song_vec[note_window_end][0] - song_vec[note_window_end-1][0];
      FastLED.show();
      // 1 tick every 2 seconds
      // Serial.println("delay: ");
      // Serial.println(delay*2000);
      FastLED.delay(delay*2000);
      // slide window down past processed lights. not sure if subscripting error might happen with moving window past end of song
      if (note_window_end < song_vec.size()) {
        // Serial.println("move window");
        note_window_start = note_window_end;
        note_window_end = note_window_start + 1;
      } 

















    // int note_window_start = 0;
    // int note_window_end = 1;
    // while (note_window_end < song.size()) {
    //   float curr_note_time = song[note_window_start][0];
    //   // get window size of all notes at the same time stamp
    //   while (curr_note_time == song[note_window_end][0]) {
    //     note_window_end++;
    //   }
    //   // iterate through all notes of same time stamp and set LED
    //   for (int i=note_window_start; i<note_window_end; i++) {
    //     Serial.println("curr note");
    //     Serial.println(i);
    //     std::vector<float> curr_note = song[i];
    //     int pitch = (int)curr_note[1];
    //     leds[pitch] = (curr_note[2] == 0.0) ? CRGB::Black : CRGB::Purple;
    //   }
    //   // set delay, subtract timestamp of current window from timestamp of next window
    //   // error: if last note is at the same time as prev note
    //   float delay = song[note_window_end][0] - song[note_window_end-1][0];
    //   FastLED.show();
    //   // 1 tick every 2 seconds
    //   FastLED.delay(delay*2000);
    //   // slide window down past processed lights. not sure if subscripting error might happen with moving window past end of song
    //   if (note_window_end < song.size()) {
    //     note_window_start = note_window_end;
    //     note_window_end = note_window_start + 1;
    //   } 
      // else {
      //   // check if last note is diff time or if i should set with prev notes
      //   // int pitch = (int)song[song.size()-1][1];
      //   // leds[pitch] = (song[song.size()-1][2] == 0.0) ? CRGB::Black : CRGB::Purple;
      //   //last event is basically always the last LED clearing, so just call .clear()
      //   FastLED.clear();
        
      // }
      Serial.println("window start");
      Serial.println(note_window_start);
      Serial.println("window end");
      Serial.println(note_window_end);
      if (note_window_end == song_vec.size()) {
        FastLED.clear();
      }
    }
    




    // float curr_time = 0.0;
    
    // while (i<20) {
    //   list<float> curr_note = song[i];
    //   float curr_time = curr_note[0];
    //   int pitch = (int) curr_note[1];
    //   int power = (int) curr_note[2];

    // }

    // Chord first;
    // Chord second;
    // first.duration = 2;
    // second.duration = 3;
    // int first_pitch[3] = {0, 2, 5};
    // int second_pitch[3] = {1, 4, 3};
    // for (int i=0;i<3;i++) {
    //     first.pitches[i] = first_pitch[i];
    //     second.pitches[i] = second_pitch[i];
    // }

    // Chord song[2] = {first, second};
    // for (int i=0; i<2; i++) {
    //     Chord curr_chord = song[i];
    //     for (int j=0; j<3; j++) {
    //         int curr_led = curr_chord.pitches[j];
    //         leds[curr_led] = CRGB::Purple;
    //     }
    //     FastLED.show();
    //     FastLED.delay(curr_chord.duration*1000);
    //     FastLED.clear();
    // }
}

void changeLight(int pitchLED, int event) {
    if (event == 0) {
      leds[pitchLED] = CRGB::Black;
    }
    leds[pitchLED] = CRGB::Purple;
}

void turnOnLight(byte channel, byte note, byte velocity) {
    Serial.println("turn on");
    // ensure velocity isn't 0 or else that means the note is off
    // lowest piano note starts at 21
    if (velocity == 0) {
      turnOffLight(channel, note, velocity);
    } else {
      leds[note-21] = CRGB::Purple;
      FastLED.show();
    }

}

void turnOffLight(byte channel, byte note, byte velocity) {
  Serial.println("turn off");
  leds[note-21] = CRGB::Black;
  FastLED.show();
}
