#include <FastLED.h>
#include <list>
#include <vector>
#include <ArduinoJson.h>

#define LED_PIN     17
#define NUM_LEDS    10
#define BRIGHTNESS  3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
std::vector<std::vector<float>> song_vec;


void setup() {
    Serial.begin(9600);
    while (!Serial);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
    StaticJsonDocument<2048> doc;
    char json[] = "{\"tempo\":0.0,\"num_events\":24,\"song\":[[0.0,2.0,1.0],[0.25,2.0,0.0],[0.25,3.0,1.0],[0.25,4.0,1.0],[0.5,2.0,1.0],[0.5,3.0,0.0],[0.75,1.0,1.0],[0.75,4.0,0.0],[1.0,0.0,1.0],[1.0,1.0,0.0],[1.0,2.0,0.0],[1.0,3.0,1.0],[1.25,-2.0,1.0],[1.25,0.0,0.0],[1.25,3.0,0.0],[1.5,-2.0,0.0],[1.5,0.0,1.0],[2.0,-3.0,1.0],[2.0,-1.0,1.0],[2.0,0.0,0.0],[2.0,1.0,1.0],[2.5,-3.0,0.0],[2.5,-1.0,0.0],[2.5,1.0,0.0]]}";
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(err.f_str());
      return;
    }

    int tempo = doc["tempo"]; // TODO factor this into the delay calculation
    Serial.println("tempo: ");
    Serial.println(tempo);
    int num_events = doc["num_events"];
    Serial.println("num events: ");
    Serial.println(num_events);
    

    JsonArray song = doc["song"];
    for (int i=0;i<num_events;i++){
      auto event = song[i];
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
    // change implementation into array, and send the song array size as first byte when sending to the arduino
    // calculate duration by subtracting curr timestamp from prev one, then keep looping through notes while the timestamps are the same and set the LEDs and set a delay eq to duration
    // TODO: figure out how to display upcoming notes. Either a second strip on top of the first, or a gradually brightening light, or both
    int note_window_start = 0;
    int note_window_end = 1;
    while (note_window_end < song_vec.size()) {
      float curr_note_time = song_vec[note_window_start][0];
      // get window size of all notes at the same time stamp
      while (curr_note_time == song_vec[note_window_end][0]) {
        note_window_end++;
      }
      // iterate through all notes of same time stamp and set LED
      for (int i=note_window_start; i<note_window_end; i++) {
        std::vector<float> curr_note = song_vec[i];
        int pitch = (int)curr_note[1];
        // Black == LED off
        leds[pitch] = (curr_note[2] == 0.0) ? CRGB::Black : CRGB::Purple;
      }
      // set delay, subtract timestamp of current window from timestamp of next window
      // error: if last note is at the same time as prev note, we might not clear the LEDs at the same time. potentially doesn't matter
      float delay = song_vec[note_window_end][0] - song_vec[note_window_end-1][0];
      FastLED.show();
      // 1 tick every 2 seconds (1/8th note per second, approx 30bpm?) = magnitude of 2000
      FastLED.delay(delay*2000);
      // slide window down past processed lights. not sure if subscripting error might happen with moving window past end of song
      if (note_window_end < song_vec.size()) {
        // Serial.println("move window");
        note_window_start = note_window_end;
        note_window_end = note_window_start + 1;
      } else {
        //last event should always be the last LED clearing, so just call .clear()
        FastLED.clear();
      }
      // one final clear just in case
      FastLED.clear();
    }
}