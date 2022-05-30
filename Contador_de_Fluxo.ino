/*Controle de Fluxo de Água (v1.2)
 * 
 * Controle de Fluxo de água com sensor de fluxo para limitar o volume maximo para acionamento da válvula solenoide e interromper o fornecimento de água para as colunas, sendo a válvula
 * controlada por um relé de ativação. O sistema contará com um display LCD para exibição da quantidade total e parcial do volume, sendo elas: Total, irá medir a qauantidade de água já 
 * passada pela coluna, sendo resetada a cada troca de coluna atráves do acionamento de um botão. Parcial, irá medir a quantidade de água para encher o reservatório (1000L),
 * devendo ser resetada a cada troca de reservatório, através de um botão, para que a válvula solenoide sejá ativada e libere o fluxo de água. Todo o sistema será controlado atráves de um
 * Arduino UNO R3.
 * 
 * São componentes deste sistema:
 * - Display LCD.
 * - Modulo Réle.
 * - Sensor de Fluxo de Água.
 * - Válvula Solenoide.
 * - Modulo I2C.
 * 
 * Autor: Valber Drumond (25/05/2022).
 */

//Definição do pino do sensor e de interrupção.
const int INTERRUPCAO_SENSOR = 0; //interrupt = 0 equivale ao pino digital 2
const int PINO_SENSOR = 2;

//Definição dos pinos conectados ao relé e ao botão.
const int PINO_RELE = 7;
const int PINO_BOTAO_RESET_TEMP = 8;
const int PINO_BOTAO_RESET_GERAL = 12;

//Definição da variável de contagem de voltas.
unsigned long contador = 0;

//Definição do fator de calibração para conversão do valor lido (devendo ser alterado mediante testes).
const float fator_calibracao = 4.5;

//Definição das variáveis de fluxo e volume.
float fluxo = 0;
float volume = 0;
float volume_total = 0;

//Definição da variável de intervalo de tempo.
unsigned long tempo_antes = 0;

void setup(){

  //Sendo esta parte devendo ser alterada posteriormente para exibição em LCD (v2.0)
    
  //Inicialização do monitor serial.
  Serial.begin(9600);

  //Mensagem de Inicialização.
  Serial.println("Medidor de Fluxo e Volume de Liquidos\n");

  //Configuração do pino do sensor como entrada em nível lógico alto.
  pinMode(PINO_SENSOR, INPUT_PULLUP);

  //Configuração do pino do relé como saída em nível lógico baixo.
  pinMode(PINO_RELE, OUTPUT);
  digitalWrite(PINO_RELE, LOW);

  //Configuração do pino do botão como entrada.
  pinMode(PINO_BOTAO_RESET_TEMP, INPUT);
  pinMode(PINO_BOTAO_RESET_GERAL, INPUT);
  
}

void loop() {

  //Executa a contagem de pulsos uma vez por segundo.
  if((millis() - tempo_antes) > 1000){

    //Desabilita a interrupção para realizar a conversão do valor de pulsos.
    detachInterrupt(INTERRUPCAO_SENSOR);

    //Conversão do valor de pulsos para L/min.
    fluxo = ((1000.0 / (millis() - tempo_antes)) * contador) / fator_calibracao;

    //Exibicao do valor de fluxo
    Serial.print("Fluxo de: ");
    Serial.print(fluxo);
    Serial.println(" L/min");

    //Cálculo do volume em L passado pelo sensor.
    volume = fluxo / 60;

    //Armazenamento do volume.
    volume_total += volume;

    //Exibicao do valor de volume.
    Serial.print("Volume: ");
    Serial.print(volume_total);
    Serial.println(" L");
    Serial.println();

    //Lógica para o acionamento do relé.
    if(volume_total < 10){
      digitalWrite(PINO_RELE, HIGH);
    } else {
      digitalWrite(PINO_RELE, LOW);
    }
   
    //Reinicializacao do contador de pulsos.
    contador = 0;
    
    //Atualizacão da variável tempo_antes.
    tempo_antes = millis();

    //Contagem de pulsos do sensor.
    attachInterrupt(INTERRUPCAO_SENSOR, contador_pulso, FALLING);
    
  }

  //Lógica para zerar a variável volume_total.
  if(digitalRead(PINO_BOTAO_RESET_TEMP) == HIGH){
    delay(30);
    if(digitalRead(PINO_BOTAO_RESET_TEMP) == HIGH){
      volume = 0;
    }
  }

  //Lógica para zerar a váriavel volume_parcial.
  if(digitalRead(PINO_BOTAO_RESET_GERAL) == HIGH){
    delay(30);
    if(digitalRead(PINO_BOTAO_RESET_GERAL) == HIGH){
      volume_total = 0;
    }
  }
  
}

//Funcão chamada pela interrupção para contagem de pulsos.
void contador_pulso() {
  
  contador++;
  
}
