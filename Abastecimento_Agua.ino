/* PINOS */
const int pin_sensor_cima       = 22; 
const int pin_sensor_baixo      = 23; 
const int pin_rele_bomba_dagua  = 17; 
const int pin_solenoide         = 16;
const int pin_water_flow        = 21;

/* STATES */
int state_pin_sensor_cima      = LOW; 
int state_pin_sensor_baixo     = LOW; 
int state_pin_rele_bomba_dagua = HIGH; 
int state_pin_solenoide        = LOW;
volatile int water_flow_count  = 0;  

float calibrationFactor        = 4.5;
unsigned int flowRate          = 0; // Taxa de fluxo em litros por minuto
unsigned int totalMilliliters  = 0; // Total de mililitros medidos
unsigned int previousMillis    = 0; // Armazena o valor dos millis() anterior
const unsigned long interval   = 1000; // Intervalo de tempo em milissegundos 
//para calcular a taxa de fluxo
 
void setup(){
  Serial.begin(9600);
  initialize();
  attachInterrupt(digitalPinToInterrupt(pin_water_flow), countPulse, RISING);
}
 
void loop() {
 
  state_pin_sensor_cima   = digitalRead(pin_sensor_cima); 
  state_pin_sensor_baixo  = digitalRead(pin_sensor_baixo);

  on_off_bomba(state_pin_sensor_cima);
  on_off_solenoide(state_pin_sensor_baixo, state_pin_sensor_cima);

  function_water_flow_count();
  
}

void initialize(){
  pinMode(pin_sensor_cima, INPUT); 
  pinMode(pin_rele_bomba_dagua, OUTPUT); 

  pinMode(pin_sensor_baixo, INPUT); 
  pinMode(pin_solenoide, OUTPUT);

  pinMode(pin_water_flow, INPUT);

  digitalWrite(pin_rele_bomba_dagua, HIGH); 
  digitalWrite(pin_solenoide, LOW); 
}

void on_off_bomba(int state_pin_sensor_cima){
   if( state_pin_sensor_cima == 0 && state_pin_sensor_baixo == 1){
     state_pin_rele_bomba_dagua = LOW;
   } else if( state_pin_sensor_cima == 1 ){
      state_pin_rele_bomba_dagua = HIGH;
   }

   digitalWrite(pin_rele_bomba_dagua, state_pin_rele_bomba_dagua);
}

void on_off_solenoide(int state_pin_sensor_baixo, int state_pin_sensor_cima){
   if(state_pin_sensor_cima == 1 && state_pin_sensor_baixo == 0){
     state_pin_solenoide = LOW;
   } else if( state_pin_sensor_baixo == 1 ){
      state_pin_solenoide = HIGH;
   }

   digitalWrite(pin_solenoide, state_pin_solenoide);
}

void function_water_flow_count(){
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval){
    detachInterrupt(digitalPinToInterrupt(pin_water_flow));
    //Calcula a taxa de fluxo
    flowRate = (water_flow_count * calibrationFactor) / (interval / 1000.0);
    //Calcula o total de mililitros medidos
    totalMilliliters += (flowRate / 60.0) * (interval / 1000.0 ) * 1000;
    //Reinicia a contagem do fluxo de água
    water_flow_count = 0;
    //Armazena o valor do millis() atual
    previousMillis = currentMillis;
    //Imprime os resultados
    Serial.println("Taxa de fluxo: ");
    Serial.println(flowRate);
    Serial.println("L/min\tTotal de mililitros: ");
    Serial.println(totalMilliliters);
    Serial.println(" ml");

    attachInterrupt(digitalPinToInterrupt(pin_water_flow), countPulse, RISING);
  }
}

void countPulse(){
  water_flow_count++;
  }