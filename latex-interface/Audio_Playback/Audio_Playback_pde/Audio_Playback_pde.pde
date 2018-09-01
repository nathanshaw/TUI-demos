/**
 * Simple Read for "Balls" TUI
 * 
 */
import ddf.minim.*;

Minim minim;
AudioOutput out;
AudioSample [] samples = new AudioSample[11];
// 9-DOF
int [] values = new int[5];
int [] lvalues = new int[5];

int FSRA = 0;
int FSRB = 1;

void setup() 
{
  size(512, 400);
  minim = new Minim(this);
  out = minim.getLineOut();
  String name = "";
  for (int i = 1; i < 5; i++) {
    name = "sample" + str(i) + ".wav";
    println(name);
    samples[i-1] = minim.loadSample(name, 512);
  }

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
        if (sChunks[0] != "") {
        if (Integer.parseInt(sChunks[0]) == 255) {  // If data is available,
          //println(incString.split(", "));
          for (int i = 0; i < incString.split(",").length - 1; i++) {
            lvalues[i] = values[i];
            values[i] = Integer.parseInt(sChunks[i]);
            print(values[i], " - ");
          }
          println();         // read it and store it in val
        }
      }
    }
    }
  }
}


void drawWaveForms() {
  background(0);
  stroke(255);
  // use the mix buffer to draw the waveforms.
  for (int i = 0; i < samples[0].bufferSize() - 1; i++)
  {
    float x1 = map(i, 0, samples[0].bufferSize(), 0, width);
    float x2 = map(i+1, 0, samples[1].bufferSize(), 0, width);
    float x3 = map(i+1, 0, samples[2].bufferSize(), 0, width);
    float x4 = map(i+1, 0, samples[3].bufferSize(), 0, width);

    line(x1, 50 - samples[0].mix.get(i)*50, x2, 50 - samples[0].mix.get(i+1)*50);
    line(x1, 150 - samples[1].mix.get(i)*50, x2, 150 - samples[1].mix.get(i+1)*50);
    line(x3, 250 - samples[2].mix.get(i)*50, x4, 250 - samples[2].mix.get(i+1)*50);
    line(x3, 350 - samples[3].mix.get(i)*50, x4, 350 - samples[3].mix.get(i+1)*50);
  }
}

void scrub(int loc, int samNum) {
  /*int pos = samples[samNum].position();
   if (pos < samples[samNum].length() - 40) {
   samples[samNum].skip(40); 
   }*/
}


void risingE(int valNum, int thresh, int sampNum) {
  if (values[valNum] > thresh && lvalues[valNum] < thresh) {
    samples[sampNum].trigger();
  }
}

void fallingE(int valNum, int thresh, int sampNum) {
  if (values[valNum] <= thresh && lvalues[valNum] > thresh) {
    samples[sampNum].trigger();
  }
}

void draw()
{
  // for the "touch sensors"
  risingE(FSRA, 90, 0);
  falling(FSRA, 60, 1);
  risingE(FSRB, 120, 2);
  fallingE(FSRB, 60, 3);
  // drawWaveForms();
  readArduino();
}
