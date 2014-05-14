#ifndef _motor_h
#define _motor_h
    
#include <Arduino.h>
#include "mux4067.h"
#include "premenne.h"

// 0 - sm0,1-sm1
const bool motor_mod[3][3] = {{false,false},
                              {true,false},
                              {false,true}};

class drv8805 {
	private:
	motor_control *motor_0;
	motor_control *motor_1;
	
        static drv8805 * instance;
	static void interrupt_motor_0();
        void ink_0();
        
	static void interrupt_motor_1();
        void ink_1();
	  
	public:
	drv8805(motor_control *v_motor_0,motor_control *v_motor_1);
	drv8805(motor_control *v_motor_0);
	~drv8805(){};

	void initial(mux4067 &mux, bool number_motor = false); //reset drv8805, inicializacia prerusenia, vynulovanie premennych, pripravenie na krok, false - inicializacia 0 motora. true - inicializacia 0,1 motora

	uint8_t step(bool dir_0, bool dir_1, uint8_t time=5, bool enable_0 = true, bool enable_1 = true); // krok, vrati sumu poctu krokov
	uint8_t step(bool dir, uint8_t time=5, bool enable = true); // krok, vrati pocet krokov

	void set_mod(uint8_t mod); // nastavenie modu. 0 plny,1 polovicny, 2 jedna cievka "wave mod"
	void set_enable(bool enable_0, bool enable_1); // zapnutie motorov
	void set_enable(bool status); // zapnutie motora
	uint8_t set_disable(mux4067 &mux); // vypnutie prerusenia, motorov, vrati sucet poctu krokov

	bool get_status(uint8_t motor); // ziskanie stavu o zapnuti motora
	uint8_t get_number_step(bool motor=false); // vrati pocet krokov. bez parametra 1 motora, true druheho motora
	uint8_t get_mod(); // vrati aktualny mod
};

#endif
