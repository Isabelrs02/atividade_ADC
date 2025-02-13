Controle de LEDs e Display OLED com Conversor ADC no Raspberry Pi Pico W

Este projeto utiliza o Raspberry Pi Pico W para demonstrar o funcionamento e aplicação de conversores analógico-digitais (ADC). A partir da leitura de sinais analógicos provenientes de um joystick, o sistema controla LEDs RGB e exibe um quadrado no display OLED, utilizando os valores convertidos pelo ADC.

Colaboradora : Isabel Rosa de Santana.

Descrição do Projeto

O objetivo principal deste projeto é explorar a funcionalidade do conversor ADC do Raspberry Pi Pico W, convertendo sinais analógicos em digitais para controlar dispositivos de saída e criar interatividade entre hardware e software.


Funcionalidades

1. Conversão ADC:

Leitura dos sinais dos eixos X e Y do joystick através dos canais ADC.

Conversão dos valores analógicos (0–3,3 V) em valores digitais (0–4095).

2. Controle de LEDs RGB:

Os valores convertidos pelo ADC são utilizados para controlar a intensidade dos LEDs RGB via PWM, criando uma correspondência visual entre os movimentos do joystick e os LEDs.

3. Exibição Gráfica no Display OLED:

Os valores convertidos são mapeados para coordenadas do display, movimentando um quadrado em tempo real de acordo com a posição do joystick.

4. Interatividade com Botões:

Controle adicional para alternar entre ligar/desligar LEDs e o display, utilizando o botão central do joystick e botão A .


Componentes Utilizados

Microcontrolador: Raspberry Pi Pico W

Joystick Analógico:

Saída analógica para os eixos X e Y

Botão digital integrado

Display OLED: Interfaceado via I2C

LEDs RGB: Controlados por PWM

Botão Externo: Para alternância de estados

Outros :  Resistor para o botão externo ; Fios de conexão



Funcionamento Técnico do Conversor ADC

O joystick gera tensões analógicas correspondentes às posições dos eixos X e Y.

Essas tensões são aplicadas aos canais ADC do Raspberry Pi Pico W.

O ADC converte essas tensões em valores digitais de 12 bits (0–4095).

Esses valores são processados para:

Ajustar o brilho dos LEDs RGB via PWM.

Mapear coordenadas no display OLED para posicionar o quadrado.


Instruções de Montagem e Execução

1. Montagem:

Conecte o joystick aos pinos ADC do Pico W.

Conecte os LEDs RGB e o display OLED conforme o esquema.

Adicione o botão externo para maior interatividade.

2. Código:

Certifique-se de ter o Pico SDK instalado.

Compile e carregue o código no Raspberry Pi Pico W.

3. Interação:

Movimente o joystick para controlar os LEDs e o quadrado no display.

Use o botão A para alternar entre os modos de funcionamento.


Links

Repositório GitHub: https://github.com/Isabelrs02/atividade_ADC.git

Demonstração no YouTube: https://youtu.be/plODcX0wh68?si=JIWkEjNId7nXLXqw

