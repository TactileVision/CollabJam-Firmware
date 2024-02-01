# Simplified structure

- no more unnceccersary interfaces between receiving and outputting
- only one output mode, which is realtime interpretation of the protocol buffers stream

```mermaid
---
title: Basic program flow
---
flowchart LR 

 ble[BLE Charactersitic written callback]
 pb[Parsing protocol buffer stream]
 hw[Vibration Output]
 
 ble --> pb
 pb --> hw
```

- Vibration Output could be implemented through an interface so that both DRV as well as ULN2803A Output can be used

```mermaid
---
title: Basic program flow
---
flowchart LR 

 hw[Vibration Output Hardware Interface]
 gpio[Implementation - GPIO based pwm output]
 drv[Implementation - haptics motor driver]
 gpio_c[Current compensation feature]
 gf[implementation specific features]

 hw --> gpio
 gpio --> gpio_c
 hw --> drv
 drv --> gf
 gpio --> gf
 
```
