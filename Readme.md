# Propósito
Con este proxecto pretendemos crear unha pequena plataforma educacional de xestión de enerxía solar baseada no Hardware e Software empregado por un dos equipos da Industriosa participante nas edicións 2018 e 2019 da Regata Solar de Marine Instruments coa que obtivemos a victoria en ambas edicións na categoría Open.

https://www.regatasolar.org/

A idea é que o sistema sexa o suficientemente sinxelo de montar e integrar que poida ser usado polos equipos de estudantes.

A complexidade de construir un vehículo solar sen acumulación de enerxía radica en conseguir exprimir ó máximo as posibilidades do panel solar.
Os paneles entregan enerxía eléctrica seguindo unha curva característica que fai necesaria unha xestión intelixente da demanda do sistema de propulsión. É necesario atopar o punto de máxima potencia: se demandamos menos do que o panel é capaz de entregar, desaproveitamos enerxía, e se demandamos máis do que é capaz de entregar, o panel vense abaixo e a potencia entregada cae dramáticamente ata provocar un apagado do sistema.

A continuación unha curva característica de entrega de potencia dun panel solar:

<p align="center">
  <img src="/doc/img/solar-panel-power.jpg" width="548" height="291"/>
</p>



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


