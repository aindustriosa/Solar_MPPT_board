# Propósito
Con este proxecto pretendemos crear unha pequena plataforma educacional de xestión de enerxía solar baseada no Hardware e Software empregado por un dos equipos da Industriosa participante nas edicións 2018 e 2019 da Regata Solar de Marine Instruments coa que obtivemos a victoria en ambas edicións na categoría Open.

https://www.regatasolar.org/

A idea é que o sistema sexa o suficientemente sinxelo de montar e integrar que poida ser usado polos equipos de estudantes.


# Técnica

A complexidade de construir un vehículo solar sen acumulación de enerxía radica en conseguir exprimir ó máximo as posibilidades do panel solar.
Os paneles entregan enerxía eléctrica seguindo unha curva característica que fai necesaria unha xestión intelixente da demanda do sistema de propulsión. É necesario atopar o punto de máxima potencia: se demandamos menos do que o panel é capaz de entregar, desaproveitamos enerxía, e se demandamos máis do que é capaz de entregar, o panel vense abaixo e a potencia entregada cae dramáticamente ata provocar un apagado do sistema.

A continuación unha curva característica de entrega de potencia dun panel solar:

<p align="center">
  <img src="/doc/img/solar-panel-power.JPG" width="548" height="291"/>
</p>

A estratexia seguida para conseguir o propósito consiste en monitorizar constantemente a corrente e tensión entregadas polo panel (e, por tanto, a potencia) e modular mediante un algoritmo software a demanda do motor de propulsión para conseguir en todo momento a máxima potencia disponible.
Este algoritmo ten que ser capaz de auto-adaptarse para facer fronte ás cambiantes condicións nas que o panel se atopa:
* Cambios na radiación solar
* Modificación na posición do panel con respecto do sol
* Cambios na resistencia do motor segundo as condicións dinámicas da embarcación
* ...


# Hardware

A PCB conta con tódolos seus componentes en montaxe Through Hole (Furado pasante) e módulos insertables:
* Arduino Nano
* INA219
* Bluetooth HC-05

<p align="center">
  <img src="/MPPT PCB/images/Render_pcb_front.jpg" width="548" height="291"/>
</p>

# Software

A peza principal do código é unha implementación do algoritmo MPPT "Perturb and Observe".
Sirva como guía para comprender a implementación o seguinte documento:

http://ww1.microchip.com/downloads/en/appnotes/00001521a.pdf

En esencia, o algoritmo adapta a demanda que se fai do motor do seguinte modo:
1. Mide Corrente entregada polo panel
2. Mide a Tensión á saída do panel
3. Calcula a potencia
4. Compara coa potencia na anterior iteración
5. Incrementa ou decrementa PWM (ver grafo)

<p align="center">
  <img src="/doc/img/P_O.JPG" width="548" height="291"/>
</p>


