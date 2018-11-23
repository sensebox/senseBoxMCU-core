#define LOG SerialUSB

void setup() {
  LOG.begin(115200);
  while(!LOG) {;}
}

void loop() {
  LOG.println("woop woop i am the app");
}
