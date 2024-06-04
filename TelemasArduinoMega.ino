// ================== VARIÁVEIS PITOT
#define Pitot1 A0
#define Pitot2 A1
float V_0 = 5;  // Tensão de operação do Sensor (Fator de calibração, para o Mega deve ser 4.89)
float rho = 1.1;  // Densidade do ar
// Calibração do pitot
int offset1, offset2;
int offset_size = 10;
int veloc_mean_size = 20;
int zero_span = 2;
// Para a média
const int numReadingsPitot = 20; // Tamanho da amostra
int readingsPitot1[numReadingsPitot]; // Vetor que armazena as amostras do Pitot1
int readingsPitot2[numReadingsPitot]; // Vetor que armazena as amostras do Pitot2
int readIndex = 0; // Variável que vai iterar o vetor e apontar para o novo valor

// ================= VARIÁVEIS PARA A LEITURA DOS SERVOS
#define canalProfundor 2
#define canalSV 3
#define canalAileron 4
int pwmProfundor, pwmSV, pwmAileron;


// =========== FUNÇÕES PARA EXECUÇÃO DO LOOP ===============
// ==================== PARA O PITOT =======================
// LEITURA DO PITOT (Nem tente entender, só aceite que funciona kk)
float lerPitot(int porta, int offsetas){
  float adc_avg = 0; 
  float veloc = 0.0;
  for (int ii=0;ii<veloc_mean_size;ii++){
    adc_avg+= analogRead(porta)-offsetas;
  }
  adc_avg/=veloc_mean_size;

  if (adc_avg>512-zero_span and adc_avg<512+zero_span){
  } else{
    if (adc_avg<512){
      veloc = -sqrt((-10000.0*((adc_avg/1023.0)-0.5))/rho);
    } else{
      veloc = sqrt((10000.0*((adc_avg/1023.0)-0.5))/rho);
    }
  }

  if (veloc < 0){
    veloc = -veloc;
  }
  return veloc;
}

// MÉDIAS MÓVEIS DO PITOT
// Pitot 1
float mediaPitot1(float leitPitot1){
  // Rotação do vetor
  for(int i = numReadingsPitot-1; i > 0; i--){
    readingsPitot1[i] = readingsPitot1[i-1];
  }

  readingsPitot1[0] = leitPitot1; // Última leitura

  float soma = 0.0;

  // Somando todo o vetor
  for(int i = 0; i < numReadingsPitot; i++){
    soma += readingsPitot1[i];
  }

  return soma/numReadingsPitot; // Retorno da média
}

// Pitot 2
float mediaPitot2(float leitPitot2){
  // Rotação do vetor
  for(int i = numReadingsPitot-1; i > 0; i--){
    readingsPitot2[i] = readingsPitot2[i-1];
  }

  readingsPitot2[0] = leitPitot2; // Última leitura

  float soma = 0.0;

  // Somando todo o vetor
  for(int i = 0; i < numReadingsPitot; i++){
    soma += readingsPitot2[i];
  }

  return soma/numReadingsPitot; // Retorno da média
}

// ========= PARA A DEFLEXÃO DOS SERVOS =============
int servosPWM(int canal){
  int leitura = pulseIn(canal, HIGH, 25000);
  return leitura;
}


// ====================== SETUP =======================
void setup() {
  Serial.begin(9600);
  offset1 = Calibracao(Pitot1);
  offset2 = Calibracao(Pitot2);

  pinMode(canalProfundor, INPUT);
  pinMode(canalSV, INPUT);
  pinMode(canalAileron, INPUT);
}

// Função de calibração do Pitot
int Calibracao(int porta){
  int offset = 0; // Também variável de calibração
  for (int ii=0;ii<offset_size;ii++){
    offset += analogRead(porta)-(1023/2);
  }
  offset /= offset_size;
  return offset;
}

// ======================= LOOP ========================
void loop() {
  // =========== Pitot
  // Definindo as variáveis e chamando as funções
  float velocidade1 = lerPitot(Pitot1, offset1);
  float velocidade2 = lerPitot(Pitot2, offset2);
  // Chamando o calculo das médias
  float velocPitot1 = mediaPitot1(velocidade1);
  float velocPitot2 = mediaPitot2(velocidade2);
  // Convertendo os valores para string
  String VELOCIDADE1 = String(velocPitot1);
  String VELOCIDADE2 = String(velocPitot2);


  // =========== Deflexão dos servos (PWM)
  pwmProfundor = servosPWM(canalProfundor);
  pwmSV = servosPWM(canalSV);
  pwmAileron = servosPWM(canalAileron);

  // Fazendo o print em 1 linha com o delimitador do algoritmo da aquisição
  Serial.print(VELOCIDADE1);
  Serial.print(",");
  Serial.println(VELOCIDADE2); 
  delay(100); 
}