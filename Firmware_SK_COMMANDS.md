# Serial communication
Each attribute can be arrounded with double quotes or not.

##General :
- **SK**
  - Test connection
  - Response : *OK\r\n*
- **SKE0** or **SKE1** 
  - Set echo mode
  - Response : *OK\r\n*
- **SK+BAUD**=`baudRate`
  - Set baudRate of wifi
  - Response : *OK\r\n* (in the past baudRate ;D)


##WIFI :
- **SK+WJAP**=`SSID`,`Password`
  - Connect the sikwi to a wifi with said and password
  - Exemple : SK+WJAP="MyWifi","1234"
  - Response : IP:XX.XX.XX.XX\r\nOK\r\n
- **SK+IP?**
  - get ip adress
  - Reponse : IP:XX.XX.XX.XX\r\nOK\r\n
- **SK+WIFI?**
  - get data about wifi configuration :
  - Response : SSID:<SSID>\r\nIP:<IP>\r\nst:<STATUS>
  - Response can change with new variable with AP mode,...


##DATA :
In the Sikwi Firmware, there is a data manager that manage data, history and more in a json and can be manipulate very easily with SK+DATA commands. This object is sended on each type of server when he change and is stocked in the eeprom to conserve data after a reboot or a DC disconnection,...

- **SK+DATA**=`name`,`value`
  - add or change the `name` property of datas by `value`.
     - value can be
           - int (1,2,3,4,…)
           - double (1.234, 4.567,…)
           - boolean (true or false)
           - string (hello)
     - Int, double or boolean should’nt be arounded by double quotes or it’ll be casted as string.
  - Exemple : 
     - SK+DATA=meteo,"calm"
     - SK+DATA=temp,23.2
     - SK+DATA=hum,52
     - SK+DATA=rain,false
     - Result : {"meteo":"calm", "temp":23.2, "hum":52, "rain":false}
  - data contain two default variable : 
     - « name » : can be modified but not deleted and is the name of the device used in a lot of command
     - « hist » :  can’t be modified or deleted and is an array of history value (see below)
  - Response : OK\r\n
- **SK+DATA_D**=`name`
   - delete a variable in the data manager, its value and its history
   - Exemple : SK+DATA_D=meteo
   - Response : OK\r\n
- **SK+DATA_H**=`name`
   - enable history management for a variable.
   - Each change of a variable will be placed in the « hist » array in this form : 
      - "variableName":[{"d":`UnixTimeStamp>` "v":`value`},...]
   - There is a max number of history item setted by the SK+DATA_M command (see below)
   - Exemple : SK+DATA_H=temp
   - Response : OK\r\n
- **SK+DATA_M**=`number`
   - Set the max item in a variable history (default is 5)
   - Exemple : SK+DATA_M=10 to have 10 item in variable history
   - Response : OK\r\n
- **SK+DATA?**=`name`
   - get the value of a variable in the data
   - Exemple : SK+DATA?=temp
   - Response : `value`\r\nOK\r\n
- **SK+DATA?**
   - get in response the data json
   - Response : `json`\r\nOK\r\n


##TCP :
- **SK+TCP**=`port`
   - Create a tcp server at `port` on the esp8266
   - Exemple : SK+TCP=23 can be accessible by telnet XX.XX.XX.XX 23
   - Response : OK\r\n
   - Response later : 
      -TCP:`message` (will be send when a tcp client send a message)
- **SK+TCP**>`message`
   - Send a message to each client connected on the tcp server
   - Exemple : SK+TCP>Hello
   - Response : OK\r\n


##Websocket :
- **SK+WS**=`port`
   - Create a websocket server at `port` on the esp8266
   - Exemple : SK+WS=8081
   - Response : OK\r\n
   - Response later : 
      - WS:`message` (will be send when a websocket client send a message)
- **SK+WS**>`message`
   - Send a message to each client connected on the websocket server
   - Exemple : SK+WS>Hello
   - Response : OK\r\n


##TIME :
The sikwi firmware get automaticly the time on an ntp server to use it in history data management and be accessed in serial.
The default NTP server is 128.138.141.172 port 123 but can be changed by SK+NTP command.
The time is in UTC.
- **SK+NTP**=`ip1`,`ip2`,`ip3`,`ip4`,`port`
   - change and save in eprom the server to contact to retreive the UTC time
   - Exemple (default) : SK+NTP=128,138,141,172,123
   - Response : HH:mm:ss\r\nOK\r\n
- **SK+TIME?**
   - get the UTC time : Hours:Minutes:Seconds
   - Response : HH:mm:ss\r\nOK\r\n
   - Response Exemple (for 14h 32min 10 sec) : 14:32:10\r\nOK\r\n
- **SK+TIME_S?**
   - get the UTC timestamp since 1st january 1970
   - Response : XXXXXXXXXXX\r\nOK\r\n
   - Response example (for 28/8/2015 at 21:25:42 ): 1440789942\r\nOK\r\n


##Request :
- **SK+GET**=`URL`
   - get url content with GET header
   - Exemple : SK+GET=http://httpbin.org/ip
   - Response : `content`\r\nOK\r\n
- **SK+GETAUTH**=`LOGIN:PASS`,`URL`
   - get url content with GET header and base64 authentification header
   - Exemple : SK+GETAUTH="test@gmail.com:123456","http://httpbin.org/ip"
   - Response : `content`\r\nOK\r\n
- **SK+POST**=`URL`, `VARIABLES`
   - get url content with POST header and variables content
   - Exemple : SK+POST="http://httpbin.org/post","s=3"
   - Response : `content`\r\nOK\r\n
- **SK+POSTAUTH**=`LOGIN:PASS`,`URL`, `VARIABLES`
   - get url content with POST header, variables content and base64 authentification header
   - Login and password should be formated like this `login:pass`
   - Exemple : SK+POSTAUTH="http://httpbin.org/post","s=3"
   - Response : `content`\r\nOK\r\n


##MQTT :
- **SK+MQTT_U**=`USER`,`PASSWORD`
   - Set User and password for MQTT connexion
   - Exemple : SK+MQTT_U="test","1234"
   - Response : OK\r\n
- **SK+MQTT_C**=`SERVER_URL`,`SERVER_PORT`,`NAME`
   - connect to mqtt server with user configuration
   - Exemple : SK+MQTT_C="test.mosquitto.org",1883,dimmer
   - Response : OK\r\n
   - Response later :
       - `MQTT:C` when connected
       - `MQTT:D` when disconnected
- **SK+MQTT_S**=`TOPIC`
   - subscribe to topic
   - Exemple : SK+MQTT_S="sikwi"
   - Response : OK\r\n
   - Response later : 
	   - MQTT:`TOPIC`:`content`
- **SK+MQTT_P**=`TOPIC`,`VALUE`
   - public value to topic
   - Exemple : SK+MQTT_P="sikwi",20
   - Response : OK\r\n


##HTTPServer :
- **SK+HTTP**=`PORT`
    - Run an http server on `Port` (default http port server is 80)
    - Reponse : OK\r\n
- **SK+HTTP_P**=`URL`,`PAGE`
    - Set serveur url and page for the Sikwi Page (see below)
    - Response : OK\r\n
- **SK+HTTP_D**=`delay`
    - Set the delay before send the result to the client in millisecond (default:2000)
    - Exemple : SK+HTTP_D=2000
    - Response : OK\r\n

**Request**

When a new client connect to this server, the next string is send to serial : 
- HTTP:`method`,`url`,`variables`
   - Exemple : HTTP:1,/,s=100&b=12&t=coucou
- `method` is a number : 
   - GET : 1
   - POST : 2
   - PUT : 3
   - PATCH : 4
   - DELETE : 5
- `url` is the path of the request
   - Exemple : /admin/test or / for the root of the server
- `variables` are all the variables sended in the url and in request content
   - Exemple : s=100&b=12&t=coucou

**Default Response **

After the new request, you have X milliseconds (defined by SK+HTTP_D) to parse data and do whatever you want before the response is sended. 
If you don’t send a **SK+RES** command (see bellow), the default response will be : 
   1. 404 not found if the url is not `/`
   2. the data json if the url is `/data`
   3. the data json if the url is `/`  and the method is not GET
   4. the Sikwi page (see bellow) if the method is GET

**SK+RES**

You can send a **SK+RES** command during the delay to send immediately the response and break the delay. This command can be : 
- SK+RES1 
   - Send Sikwi page (see bellow)
- SK+RES2
   - Send only the json data in response (like a REST json service)
- SK+RES3
   - Send a 404 response
- SK+RES4
   - Process the default response (see above) but break the delay.
- SK+RES=`HTTPresponse`
   - Send an http string as response content
   - Exemple : SK+RES=`<h1><b>Hello</b> World<h1>`

**Sikwi Page : **

When the server send the sikwi page, he send an HTML content like this : 
>     <html>
>          <head>
>               <link rel=icon href="{SERVER}favicon.ico">
>               <link rel=stylesheet href="{SERVER}i.css">
>               <meta name=viewport content='initial-scale=1.0'>
>               <title>{NAME}</title>
>          </head>
>          <body>
>               <p>{DATA}</p>
>               <iframe src="{SERVER}{PAGE}"></iframe>
>               <script src="{SERVER}j.js"></script>
>          </body>
>     </html>

It is designed to send the minimum for the maximum ;D
The variables `<SERVER>` and `<PAGE>` can be changed by the **SK+HTTP_P** command above and is the server URL and the page content of your iframe,...
The `<NAME>` variable is the name of your device and can be change with data commands SK+DATA (see above)
The `<DATA>` variable is the json data of your Sikwi and can be manipulate with command SK+DATA (see above)

On the server (`<SERVER>`), you need 
- a page at `<PAGE>` url
- a javascript file at `<SERVER>` root url named j.js. This javascript file can do the communication between your esp8266 and the iframe page
- a css file at `<SERVER>` root url named c.css. This css file will make the presentation of the root file and set the iframe fullscreen if you want.

You can see an exemples on the sikwi’s github https://github.com/sikwi/cdn
