#define DUMMY     0
#define secp160r1 1
#define secp192r1 2
#define secp224r1 3
#define secp256r1 4
#define secp256k1 5

//select benchmark to run
#define BENCHMARK secp160r1

//NOTE: on Arduino you can not set uECC_OPTIMIZATION_LEVEL here, change it in uECC.h
//#define uECC_OPTIMIZATION_LEVEL 0 

/*
Benchmark Results 

ATmega328P 16MHz with Arduino 1.8.12 Compiler

Code size is measured as the increase in code size vs a dummy program.

Runtime is for one uECC_make_key() or one uECC_shared_secret() call, both appear to take the same time.

secp256k1 opt=1 gives the smallest code size (!)

|Curve + Optimizion|Code Size (bytes)|Runtime (ms)|
|:--:|--:|--:|
|secp160r1 opt=0|5556|67780|
|secp160r1 opt=1|5256|1380|
|secp160r1 opt=2|6896|1020|
|secp160r1 opt=3|16718|550|
||||
|secp192r1 opt=1|5264|1860|
|secp192r1 opt=2|6410|1720|
|secp192r1 opt=3|16232|950|
||||
|secp224r1 opt=1|6238|2800|
|secp224r1 opt=2|7408|2640|
|secp224r1 opt=3|17230|1460|
||||
|secp256r1 opt=1|6114|4640|
|secp256r1 opt=2|7160|4300|
|secp256r1 opt=3|17082|2570|
||||
|secp256k1 opt=1|5124|4640|
|secp256k1 opt=2|7338|3670|
|secp256k1 opt=3|17160|1940|
*/


#if BENCHMARK != DUMMY
#include <uECC.h>
#endif

//use random() to make timing predictable - do not use for real life...
int RNG(uint8_t *dest, unsigned size) {
//  Serial.println("RNG " + String(size));
  for(unsigned i=0;i<size;i++) dest[i] = random(256);
  return 1;
}

int RNG2(uint8_t *dest, unsigned size) {  
  // Use the least-significant bits from the ADC for an unconnected pin (or connected to a source of 
  // random noise). This can take a long time to generate random data if the result of analogRead(0) 
  // doesn't change very frequently.
  while (size) {
    uint8_t val = 0;
    for (unsigned i = 0; i < 8; ++i) {
      int init = analogRead(0);
      int count = 0;
      while (analogRead(0) == init) {
        ++count;
      }
      
      if (count == 0) {
         val = (val << 1) | (init & 0x01);
      } else {
         val = (val << 1) | (count & 0x01);
      }
    }
    *dest = val;
    ++dest;
    --size;
  }
  // NOTE: it would be a good idea to hash the resulting random data using SHA-256 or similar.
  return 1;
}


void printbin(String header, uint8_t* b, uint8_t len) {
  Serial.print(header + ": ");
  for (int i=0; i<len; i++) { 
    if (b[i]<0x10) Serial.print("0");
    Serial.print(b[i],HEX); 
  }
  Serial.print(" len=");
  Serial.println(len);
}

void setup() {
  Serial.begin(115200);
  Serial.print("Testing ecc\n");
#if BENCHMARK != DUMMY
  uECC_set_rng(&RNG);
#endif
}

void loop() {
  //buffers sized for max key size
  uint8_t private1[32];
  uint8_t private2[32];
  
  uint8_t public1[64];
  uint8_t public2[64];
  
  uint8_t secret1[32];
  uint8_t secret2[32];

  memset(private1,0x00,32);
  memset(private2,0x00,32);
  memset(public1,0x00,64);
  memset(public2,0x00,64);
  memset(secret1,0x00,32);
  memset(secret2,0x00,32);

#if BENCHMARK == secp160r1 
  const struct uECC_Curve_t * curve = uECC_secp160r1();
  Serial.println("BENCHMARK=secp160r1");  
#elif BENCHMARK == secp192r1 
  const struct uECC_Curve_t * curve = uECC_secp192r1();
  Serial.println("BENCHMARK=secp192r1");  
#elif BENCHMARK == secp224r1 
  const struct uECC_Curve_t * curve = uECC_secp224r1();
  Serial.println("BENCHMARK=secp224r1");  
#elif BENCHMARK == secp256r1 
  const struct uECC_Curve_t * curve = uECC_secp256r1();
  Serial.println("BENCHMARK=secp256r1");  
#elif BENCHMARK == secp256k1 
  const struct uECC_Curve_t * curve = uECC_secp256k1();    
  Serial.println("BENCHMARK=secp256k1");  
#elif BENCHMARK == DUMMY
  RNG(private1,21);
  #define uECC_OPTIMIZATION_LEVEL 0
  Serial.println("BENCHMARK=DUMMY");  
#else 
  #error "unknown BENCHMARK value"
#endif

  int r;

  Serial.println("uECC_OPTIMIZATION_LEVEL=" + String(uECC_OPTIMIZATION_LEVEL));
  
  unsigned long a = millis();
#if BENCHMARK != DUMMY
  uECC_make_key(public1, private1, curve);
#endif
  unsigned long b = millis();
  Serial.print("Made key 1 in "); Serial.println(b-a);
  
  a = millis();
#if BENCHMARK != DUMMY
  uECC_make_key(public2, private2, curve);
#endif
  b = millis();
  Serial.print("Made key 2 in "); Serial.println(b-a);

  a = millis();
#if BENCHMARK != DUMMY
  r = uECC_shared_secret(public2, private1, secret1, curve);
#endif
  b = millis();
  Serial.print("Shared secret 1 in "); Serial.println(b-a);
  if (!r) {
    Serial.print("shared_secret() failed (1)\n");
    return;
  }

  a = millis();
#if BENCHMARK != DUMMY
  r = uECC_shared_secret(public1, private2, secret2, curve);
#endif
  b = millis();
  Serial.print("Shared secret 2 in "); Serial.println(b-a);
  if (!r) {
    Serial.print("shared_secret() failed (2)\n");
    return;
  }
    
  if (memcmp(secret1, secret2, 20) != 0) {
    Serial.print("Shared secrets are not identical!\n");
  } else {
    Serial.print("Shared secrets are identical\n");
  }

  printbin("prv1",private1,sizeof(private1));
  printbin("prv2",private2,sizeof(private2));
  printbin("pub1",public1,sizeof(public1));
  printbin("pub2",public2,sizeof(public2));
  printbin("sec1",secret1,sizeof(secret1));
  printbin("sec2",secret2,sizeof(secret1));  
}
