#ifndef _LM75_h_
#define _LM75_h_

#include "Arduino.h"

#define LM75_TEMP_REGISTER 0
#define LM75_CONF_REGISTER 1
#define LM75_THYST_REGISTER 2
#define LM75_TOS_REGISTER 3

#define LM75_CONF_SHUTDOWN  0
#define LM75_CONF_OS_COMP_INT 1
#define LM75_CONF_OS_POL 2
#define LM75_CONF_OS_F_QUE 3

class LM75 {
        private:
          int address;
          word float2regdata (float);
          float regdata2float (word);
          word _register16 (byte);
          void _register16 (byte, word);
          word _register8 (byte);
          void _register8 (byte, byte);
        
        public:
          LM75 (byte);
          ~LM75 () {};
          float temp (void);
          byte conf (void);
          void conf (byte);
          float tos (void);
          void tos (float);
          float thyst (void);
          void thyst (float);
          void shutdown (boolean);
          boolean shutdown (void);
};

#endif
