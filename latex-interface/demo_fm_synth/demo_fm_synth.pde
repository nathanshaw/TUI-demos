/* frequencyModulation
 <p>
 A simple example for doing FM (frequency modulation) using two Oscils.
 <p>
 For more information about Minim and additional features, 
 visit http://code.compartmental.net/minim/
 <p>  
 Author: Damien Di Fede
 */

// import everything necessary to make sound.
import ddf.minim.*;
import ddf.minim.ugens.*;
import processing.serial.*;
Serial myPort;  // Create object from Serial class


// create all of the variables that will need to be accessed in
// more than one methods (setup(), draw(), stop()).
Minim minim;
AudioOutput out;

// the Oscil we use for modulating frequency.
Oscil fm;

int [] values = new int[5];
int [] lvalues = new int[5];
int isPlaying = 0;
float modA  = 1.0;
float modF = 1.0;

// setup is run once at the beginning
void setup()
{
  // initialize the drawing window
  size( 512, 200, P3D );

  // initialize the minim and out objects
  minim = new Minim( this );
  out   = minim.getLineOut();

  // make the Oscil we will hear.
  // arguments are frequency, amplitude, and waveform
  Oscil wave = new Oscil( 200, 0.8, Waves.TRIANGLE );
  // make the Oscil we will use to modulate the frequency of wave.
  // the frequency of this Oscil will determine how quickly the
  // frequency of wave changes and the amplitude determines how much.
  // since we are using the output of fm directly to set the frequency 
  // of wave, you can think of the amplitude as being expressed in Hz.
  fm   = new Oscil( 10, 2, Waves.SINE );
  // set the offset of fm so that it generates values centered around 200 Hz
  fm.offset.setLastValue( 200 );
  // patch it to the frequency of wave so it controls it
  fm.patch( wave.frequency );
  // and patch wave to the output
  wave.patch( out );
  connectToArduino(4);
  myPort.clear();
}

void connectToArduino(int port) {
  for (int i = 0; i < Serial.list().length; i++) {
    println(i, ":", Serial.list()[i]);
  }
  String portName = Serial.list()[port];
  myPort = new Serial(this, portName, 115200);
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
          try {
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
          catch (NumberFormatException npe) {
            // just forget it not an int
          }
        }
      }
    }
  }
}


// draw is run many times
void draw()
{
  // erase the window to black
  background( 0 );
  // draw using a white stroke
  stroke( 255 );
  // draw the waveforms

  for ( int i = 0; i < out.bufferSize() - 1; i++ )
  {
    // find the x position of each buffer value
    float x1  =  map( i, 0, out.bufferSize(), 0, width );
    float x2  =  map( i+1, 0, out.bufferSize(), 0, width );
    // draw a line from one buffer position to the next for both channels
    line( x1, 50 + out.left.get(i)*50, x2, 50 + out.left.get(i+1)*50);
    line( x1, 150 + out.right.get(i)*50, x2, 150 + out.right.get(i+1)*50);
  }

  updateModAndFreq();
  readArduino();
  text( "Modulation frequency: " + fm.frequency.getLastValue(), 5, 15 );
  text( "Modulation amplitude: " + fm.amplitude.getLastValue(), 5, 30 );
}

void updateModAndFreq() {

  float modA = map( values[1], 0, 100, 420, 1 );
  float modF = map( values[2], 0, 100, 0.1, 100 );
  // println(modA, " ", modF);
  fm.setFrequency( modF );
  fm.setAmplitude( modA );
}
