# Projeto: Controle de LEDs e Display com Joystick, Botões e ADC

Este projeto utiliza um **RP2040** (Placa **BitDogLab**) para controlar **LEDs RGB** e um **display SSD1306 OLED** com um **joystick e botões físicos**.  
Foi desenvolvido em **C** utilizando o **SDK do Raspberry Pi Pico**, **interrupções (IRQ)** e **I2C** para comunicação com o display.

## **Funcionalidades**
**Movimentação de um quadrado na tela OLED usando o joystick**  
**Controle da intensidade dos LEDs RGB com o joystick** **Botão A (GPIO 5): Liga/Desliga apenas os LEDs RGB**  
**Botão do Joystick (GPIO 22): Liga/Desliga o LED Verde e adiciona uma borda ao display**  
**Uso de interrupções (IRQ) para os botões, com debounce de 200ms**  
**Comunicação via I2C com o display SSD1306**  

---

## **Componentes Utilizados**
- **RP2040 (Placa BitDogLab)**
- **Display SSD1306 OLED (I2C)**
- **Joystick analógico**
- **Botão A (GPIO 5)**
- **Botão do Joystick (GPIO 22)**
- **LED RGB (Pinos 11, 12, 13)**


## **Como Configurar no VS Code**
### 1 **Instale o SDK do Raspberry Pi Pico**

### 2 Clone o Repositório

### 3 Configure o VSCode

### 4 Compile o projeto

### 5 Carregue o código no Pico
Conecte o RP2040 segurando o botão BOOTSEL e monte a unidade USB.
Copie o arquivo .uf2 gerado para a unidade.

##  Como Usar
### Controle do Joystick
Movimente o joystick → O quadrado se move na tela.
Controle os LEDs RGB → O brilho dos LEDs Vermelho e Azul varia conforme a posição do joystick.
### Botões
Botão A (GPIO 5) → Liga/Desliga apenas os LEDs RGB.
Botão do Joystick (GPIO 22) → Liga/Desliga o LED Verde e a borda do display.

## Vídeo Demonstrativo
https://youtu.be/s7JPx3l4MmI
