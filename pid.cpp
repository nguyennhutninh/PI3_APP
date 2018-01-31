/*

   PID Controller

   author : vincent JAUNET
   mail : vincent.jaunet@hotmail.fr
   date : 10-01-2013

   Description:
   the PID class is a collection of routine necessary
   to perform a PID control based on setpoints (remote control),
   inputs (measured attitude).
   Can be used for any type of system and features :
   - Derivative on measurement
   - Windsup of integral errors

   Reference :
   -"A Simple PID Controller with Adaptive Parameter in a dsPIC; Case of Study"
	 http://www.aedie.org/9CHLIE-paper-send/337_CHAINHO.pdf

  Copyright (c) <2014> <Vincent Jaunet>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

*/

#include "pid.h"


PID yprSTAB[3];
PID yprRATE[3];

//default constructor
PID::PID()
{
	//PID constants
	m_Kd = 0;
	m_Ki = 0;
	m_Kp = 0;

	//PID variables
	m_err = 0;
	m_last_err=0;
	m_sum_err = 0;
	m_ddt_err = 0;
	m_lastInput= 0;
	m_outmax =  350;
	m_outmin = -350;
}


PID::PID(float kp_,float ki_,float kd_)
{
	//PID constants
	m_Kp = kp_;
	m_Ki = ki_;
	m_Kd = kd_;

	//PID variables
	m_err = 0;
	m_last_err=0;
	m_sum_err = 0;
	m_ddt_err = 0;
	m_lastInput= 0;
	m_outmax =  350;
	m_outmin = -350;
}

float PID::update_pid_std(float setpoint, float input, float dt)
{

	//Computes error
	m_err = setpoint-input;

	//Integrating errors
	m_sum_err += m_err * m_Ki * dt;

	//calculating error derivative
	//Input derivative is used to avoid derivative kick
	m_ddt_err = -m_Kd / dt * (input - m_lastInput);

	//Calculation of the output
	//winds up boundaries
	m_output = m_Kp*m_err + m_sum_err + m_ddt_err;
	if (m_output > m_outmax) {
		//winds up boundaries
		m_sum_err  = 0.0;
		m_output   = m_outmax;
	}else if (m_output < m_outmin) {
		//winds up boundaries
		m_sum_err  = 0.0;
		m_output   = m_outmin;
	}

	m_lastInput= input;

	// printf("kp %f ki %f kd %f\n", m_Kp, m_Ki, m_Kd);
	// printf("setpt %7.2f input   %7.2f output   %f\n", setpoint, input, m_output);
	// printf("err   %7.2f ddt_err %7.2f sum_err  %7.2f\n", m_err, m_ddt_err, m_sum_err);

	return m_output;
}

void PID::reset()
{
	m_sum_err   = 0;
	m_ddt_err   = 0;
	m_lastInput = 0;
}

void PID::set_Kpid(float Kp,float Ki, float Kd)
{
	m_Kp = Kp;
	m_Ki = Ki;
	m_Kd = Kd;

	// printf("%f %f %f \n", m_Kp, m_Ki, m_Kd);
	// printf("%f %f %f \n", Kp, Ki, Kd);

}

void PID::set_windup_bounds(float Min,float Max)
{
	m_outmax = Max;
	m_outmin = Min;
}

void PID::updateKpKi(float setpoint, float input)
{
	//err calculation
	float err = setpoint-input;
	float derr=0;
	float err_abs = fabs(err);

	if (err - m_last_err == 0.0){
		return;
	}

	derr = (err - m_last_err)/fabs(err - m_last_err);

	printf("Fuzzy PID : %6.2f %6.2f %6.2f %6.2f\n",err,derr,m_Kp,m_Ki);

	if (err_abs >= 7.0){
		m_Kp += derr*0.005*err_abs;
	}

	if (err_abs < 1.0){
		m_Kp -= 0.01*err_abs;
	}

	if (m_Kp < 1.5) {
		m_Kp = 1.5;
	}

	if (m_Kp > 5.0) {
		m_Kp = 5.0;
	}

	m_last_err = err;

	// if (err_abs > 4.0 && err_abs < 6.0){
	//   m_Ki += 0.05*err;
	//   return;
	// }
}
