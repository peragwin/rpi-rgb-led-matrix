# Mapping for Vusic Panel Adapter

`[1]`=:smile:, `[2]`=:boom: ; signals that go to both chains have all icons.

|               Connection | Pin | Pin | Connection                                    |
| -----------------------: | :-: | :-: | :-------------------------------------------- |
|                        - |  1  |  2  | -                                             |
|                  **SDA** |  3  |  4  | -                                             |
|                  **SCL** |  5  |  6  | **GND** :smile::boom:                         |
| :smile::boom: **strobe** |  7  |  8  | **D** :smile::boom: (for 32 row matrix, 1:16) |
|                        - |  9  | 10  | **E** :smile::boom: (for 64 row matrix, 1:32) |
|  :smile::boom: **clock** | 11  | 12  | **I2S_CLK**                                   |
|       :smile: **[1] G1** | 13  | 14  | -                                             |
|      :smile::boom: **A** | 15  | 16  | **B** :smile::boom:                           |
|                        - | 17  | 18  | **C** :smile::boom:                           |
|       :smile: **[1] B2** | 19  | 20  | -                                             |
|       :smile: **[1] G2** | 21  | 22  | **[2] R1** :boom:                             |
|       :smile: **[1] R1** | 23  | 24  | **[1] R2** :smile:                            |
|                        - | 25  | 26  | **[1] B1** :smile:                            |
|        **GPIO_SHUTDOWN** | 27  | 28  | **GPIO_POWEROFF**                             |
|        :boom: **[2] G1** | 29  | 30  | -                                             |
|        :boom: **[2] B1** | 31  | 32  | **OE-** :smile::boom:                         |
|        :boom: **[2] G2** | 33  | 34  | -                                             |
|               **I2S_WS** | 35  | 36  | **[2] B2** :boom:                             |
|        :boom: **[2] R2** | 37  | 38  | **I2S_IN**                                    |
|                        - | 39  | 40  | **I2S_OUT**                                   |
