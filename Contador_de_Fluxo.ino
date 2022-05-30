//v1.1

//definicao do pino do sensor e de interrupcao
const int INTERRUPCAO_SENSOR = 0; //interrupt = 0 equivale ao pino digital 2
const int PINO_SENSOR = 2;

//definicao dos pinos conectados ao rele e ao botao
const int PINO_RELE = 7;
const int PINO_BOTAO_RESET_TEMP = 8;
const int PINO_BOTAO_RESET_GERAL = 12;

//definicao da variavel de contagem de voltas
unsigned long contador = 0;

//definicao do fator de calibracao para conversao do valor lido
const float fator_calibracao = 4.5;

//definicao das variaveis de fluxo e volume
float fluxo = 0;
float volume = 0;
float volume_total = 0;

//definicao da variavel de intervalo de tempo
unsigned long tempo_antes = 0;

void setup(){

  //inicializacao do monitor serial
  Serial.begin(9600);

  //mensagem de inicializacao
  Serial.println("Medidor de Fluxo e Volume de Liquidos\n");

  //configuracao do pino do sensor como entrada em nivel logico alto
  pinMode(PINO_SENSOR, INPUT_PULLUP);

  //configuracao do pino do rele como saida em nivel logico baixo
  pinMode(PINO_RELE, OUTPUT);
  digitalWrite(PINO_RELE, LOW);

  //configuracao do pino do botao como entrada
  pinMode(PINO_BOTAO_RESET_TEMP, INPUT);
  pinMode(PINO_BOTAO_RESET_GERAL, INPUT);
  
}

void loop() {

  //executa a contagem de pulsos uma vez por segundo
  if((millis() - tempo_antes) > 1000){

    //desabilita a interrupcao para realizar a conversao do valor de pulsos
    detachInterrupt(INTERRUPCAO_SENSOR);

    //conversao do valor de pulsos para L/min
    fluxo = ((1000.0 / (millis() - tempo_antes)) * contador) / fator_calibracao;

    //exibicao do valor de fluxo
    Serial.print("Fluxo de: ");
    Serial.print(fluxo);
    Serial.println(" L/min");

    //calculo do volume em L passado pelo sensor
    volume = fluxo / 60;

    //armazenamento do volume
    volume_total += volume;

    //exibicao do valor de volume
    Serial.print("Volume: ");
    Serial.print(volume_total);
    Serial.println(" L");
    Serial.println();

    //logica para o acionamento do rele
    if(volume_total < 10){
      digitalWrite(PINO_RELE, HIGH);
    } else {
      digitalWrite(PINO_RELE, LOW);
    }
   
    //reinicializacao do contador de pulsos
    contador = 0;
    
    //atualizacao da variavel tempo_antes
    tempo_antes = millis();

    //contagem de pulsos do sensor
    attachInterrupt(INTERRUPCAO_SENSOR, contador_pulso, FALLING);
    
  }

  //logica para zerar a variavel volume_total
  if(digitalRead(PINO_BOTAO_RESET_TEMP) == HIGH){
    delay(30);
    if(digitalRead(PINO_BOTAO_RESET_TEMP) == HIGH){
      volume = 0;
    }
  }

  //logica para zeral avariavel volume_parcial
  if(digitalRead(PINO_BOTAO_RESET_GERAL) == HIGH){
    delay(30);
    if(digitalRead(PINO_BOTAO_RESET_GERAL) == HIGH){
      volume_total = 0;
    }
  }
  
}

//funcao chamada pela interrupcao para contagem de pulsos
void contador_pulso() {
  
  contador++;
  
}
