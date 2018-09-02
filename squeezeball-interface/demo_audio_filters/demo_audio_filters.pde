/**
 * For use with the FSR "Stress Ball" interface
 * Expects two values from the range of 0 to 100
 */

import ddf.minim.*;
import ddf.minim.effects.*;
import ddf.minim.ugens.*;

// 9-DOF
int [] values = new int[5];
int [] lvalues = new int[5];
int isPlaying = 0;

Minim minim;
AudioOutput output;
FilePlayer  groove;
FilePlayer groove2;
LowPassSP lpf;
HighPassSP hpf;

void setup()
{
  size(512, 200, P3D);
  minim = new Minim(this);
  output = minim.getLineOut();
  groove = new FilePlayer( minim.loadFileStream("sample.mp3") );
  // make a low pass filter with a cutoff frequency of 100 Hz
  // the second argument is the sample rate of the audio that will be filtered
  // it is required to correctly compute values used by the filter
  lpf = new LowPassSP(10, output.sampleRate());
  hpf = new HighPassSP(1000, output.sampleRate());
  groove.patch( lpf ).patch( output );
  groove.patch( hpf ).patch( output );
  connectToArduino(4);
  myPort.clear();
}


void readArduino() {
  while (myPort.available() > 0) {
    String incString = myPort.readStringUntil(10);
    //println(incString);
    if (incString != null && incString.length() < 31) {
      String [] sChunks = incString.split(",");
      if ( incString != null && incString.split(",").length > 1) { 
        //println(incString);
        if (sChunks[0] != "" && sChunks != null) {
          if (Integer.parseInt(sChunks[0]) == 255) {  // If data is available,
            //println(incString.split(", "));
            for (int i = 0; i < incString.split(",").length - 1; i++) {
              if (i < values.length) {
                lvalues[i] = values[i];
                values[i] = Integer.parseInt(sChunks[i]);
                print(values[i], " - ");
              }
            }
            println();         // read it and store it in val
          }
        }
      }
    }
  }
}

void adjustLPFilter(int cut) {
  cut = cut*4;
  print("lpf : ", cut);
  lpf.setFreq(cut);
}

void adjustHPFilter(int cut) {
  if (cut > 20) {
    cut = ((100 - cut) * 20) + 400;
  } else {
    cut = ((100 - cut) * 200) + 400;
  }
  println(" - hpf - ", cut);
  hpf.setFreq(cut);
}

void draw()
{
  background(0);
  stroke(255);
  // we multiply the values returned by get by 50 so we can see the waveform
  for ( int i = 0; i < output.bufferSize() - 1; i++ )
  {
    float x1 = map(i, 0, output.bufferSize(), 0, width);
    float x2 = map(i+1, 0, output.bufferSize(), 0, width);
    line(x1, height/4 - output.left.get(i)*50, x2, height/4 - output.left.get(i+1)*50);
    line(x1, 3*height/4 - output.right.get(i)*50, x2, 3*height/4 - output.right.get(i+1)*50);
  }
  if (values[1] + values[2] > 4) {
    if (isPlaying == 0) {
      groove.play(groove.position());
      groove.loop();
      isPlaying = 1;
    }

    adjustLPFilter(values[1]);
    adjustHPFilter(values[2]);
  } else {   
    if (isPlaying == 1) {
      groove.pause();
      isPlaying = 0;
    }
  }
  readArduino();
}
