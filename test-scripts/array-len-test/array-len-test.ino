//Testing code to calculate how many items are in an array
//For use in the main codebase for screen size selection

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  int screenOptions[] = {13, 15, 21, 24, 27, 32, 34, 36}; 
  int numOptions = sizeof(screenOptions) / sizeof(screenOptions[0]);
  Serial.print("numOptions: ");
  Serial.println(numOptions);
  Serial.print("sizeof(screenOptions): ");
  Serial.println(sizeof(screenOptions));
  Serial.print("sizeof(screenOptions[0]): ");
  Serial.println(sizeof(screenOptions[0]));
  delay(5000);
}
