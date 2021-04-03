# RoomManager
## Projeto Descontinuado


### Este projeto consiste no firmware de um device IoT para controle de cargas AC via relé.

- Programação baseada na Core Arduino para ESP32
- IDE utilizada,Platformio
- Utiliza a biblioteca ArduinoJson para realizar o parse das respostas da API escrita em JS na plataforma Node-Red da IBM
- Utiliza a biblioteca PubSubClient para se conectar ao broker MQTT
- Atualizações On The Air configuradas via biblioteca ArduinoOTA

### Funcionamento
O device se conectana no broker MQTT da IBM, e se registra no tópico de acionamento. Após isso, sempre que uma mensagem for postada nesse tópico o esp32 atualiza o status do pino de saída para o relé.
