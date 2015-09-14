const char b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

inline void a3_to_a4(unsigned char * a4, unsigned char * a3) {
  a4[0] = (a3[0] & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}

inline void a4_to_a3(unsigned char * a3, unsigned char * a4) {
  a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
  a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
  a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

inline unsigned char b64_lookup(char c) {
  int i;
  for(i = 0; i < 64; i++) {
    if(b64_alphabet[i] == c) {
      return i;
    }
  }

  return -1;
}

int base64_encode_request(char *output, char *input, int inputLen) {
  int i = 0, j = 0;
  int encLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];

  while(inputLen--) {
    a3[i++] = *(input++);
    if(i == 3) {
      a3_to_a4(a4, a3);

      for(i = 0; i < 4; i++) {
        output[encLen++] = b64_alphabet[a4[i]];
      }

      i = 0;
    }
  }

  if(i) {
    for(j = i; j < 3; j++) {
      a3[j] = '\0';
    }

    a3_to_a4(a4, a3);

    for(j = 0; j < i + 1; j++) {
      output[encLen++] = b64_alphabet[a4[j]];
    }

    while((i++ < 3)) {
      output[encLen++] = '=';
    }
  }
  output[encLen] = '\0';
  return encLen;
}

void http_post(const char * url, const char * post_data, String headers, boolean isget)
{
  // FIXME: get rid of the #anchor part if present.
  
  char hostname[128] = "";
  int port = 80;
  bool secure = false;
  
  bool is_http  = os_strncmp(url, "http://",  strlen("http://"))  == 0;
  bool is_https = os_strncmp(url, "https://", strlen("https://")) == 0;

  if (is_http)
    url += strlen("http://"); // Get rid of the protocol.
  else if (is_https) {
    //port = 443;
    //secure = true;
    url += strlen("https://"); // Get rid of the protocol.
  } else {
    Serial.println("URL is not HTTP or HTTPS");
    return;
  }

  char * path = os_strchr(url, '/');
  if (path == NULL) {
    path = os_strchr(url, '\0'); // Pointer to end of string.
  }
  
  char * colon = os_strchr(url, ':');
  if (colon > path) {
    colon = NULL; // Limit the search to characters before the path.
  }

  if (colon == NULL) { // The port is not present.
    os_memcpy(hostname, url, path - url);
    hostname[path - url] = '\0';
  }
  else {
    port = atoi(colon + 1);
    if (port == 0) {
      Serial.println("Port error");
      return;
    }

    os_memcpy(hostname, url, colon - url);
    hostname[colon - url] = '\0';
  }

  if (path==NULL || path[0] == '\0') { // Empty path is not allowed.
    path = "/";
  }

  //http_raw_request(hostname, port, secure, path, post_data, headers, user_callback);
  
  WiFiClient client;
  boolean isConnected = false;

  unsigned long c=millis();
  while(millis()<c+2000){
    if(client.connect(hostname, port)) {
      isConnected = true;
      break;
    }else{
      delay(200);
    }
  }

  if(!isConnected){
    Serial.println();
    Serial.println("ERROR");
    return;
  }
  
  if(isget)
  {
    client.print(String("GET ") + path +" HTTP/1.1\r\n" +
                "Host: " + hostname + "\r\n" + headers + 
                
                "Connection: close\r\n\r\n");
  }else
  {
    client.print(String("POST ") + path +" HTTP/1.1\r\n" +
                "Host: " + hostname + "\r\n" + headers +
                "Content-Length: "+ strlen(post_data) +"\r\n" + 
                "Connection: close\r\n\r\n"+post_data+"\r\n\r\n");
  }
  delay(10);

  boolean readVariables = false;
  int contentLength = 0;
  int i = 0;
  // Read all the lines of the reply from server and print them to Serial
  unsigned long t=millis();
  while(millis()<t+5000){
    while(client.available()){
      if(readVariables){
        if(i++<contentLength){
          Serial.print((char)client.read());
        }
        if(i>=contentLength){
          Serial.println();
          Serial.println("OK");
          return;
        }
      }else{
        String line = client.readStringUntil('\n');
        if(contentLength == 0 && line.indexOf("Content-Length: ")>=0)
        {
          contentLength = line.substring(line.indexOf("Content-Length: ")+16).toInt();
          if(contentLength == 0){
            Serial.println();
            Serial.println("OK");
            return;
          }
        }
        else if(line.indexOf("\r")==0)
        {
          readVariables = true;
        }
      }
    } 
  }
}

void http_get(const char * url, const char * headers)
{ 
  http_post(url, NULL, "", true);
}

void http_getAuth(const char * url, String auth)
{
  int sizeNameTest = auth.length()+1;
  char __urlTest[sizeNameTest];
  auth.toCharArray(__urlTest, sizeNameTest);
  
  char unameenc[80];
  
  base64_encode_request(unameenc, __urlTest, sizeNameTest-1);//to remove 00
  
  http_post(url, NULL, String("Authorization:Basic ")+ String(unameenc) + "\r\n", true);
}

void http_postAuth(String auth, const char * url, const char * post_data)
{  
  int sizeNameTest = auth.length()+1;
  char __urlTest[sizeNameTest];
  auth.toCharArray(__urlTest, sizeNameTest);
  
  char unameenc[80];
  
  base64_encode_request(unameenc, __urlTest, sizeNameTest-1);//to remove 00
  http_post(url, post_data, String("Authorization:Basic ")+ String(unameenc) + "\r\n", false);
}

