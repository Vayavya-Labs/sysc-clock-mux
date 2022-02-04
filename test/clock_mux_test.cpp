/********************************************************************************
 * BSD 3-Clause License
 * 
 * Copyright (c) 2022, Vayavya Labs Pvt. Ltd.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/

#include <iostream>
#include "clock_mux.h"

using namespace sc_core;

class clock_receiver : public sc_module
{
   public:
      sc_in<bool> clock_in;
      clock_mux *m_mux;
      SC_HAS_PROCESS(clock_receiver);
      clock_receiver(sc_module_name n) : sc_module(n) { }
      void end_of_elaboration()
      {
         sc_interface *intfs = clock_in.get_interface();
         m_mux = dynamic_cast<clock_mux *>(intfs);
         if(m_mux==0) {
            std::cout << "Not connected to a clock mux upstream" << std::endl;
         } else {
            std::cout << "Initial clock period: " << m_mux->period() << std::endl;
            SC_METHOD(clock_changed);
            sensitive << m_mux->clock_changed_event();
            dont_initialize();
         }
         
         SC_METHOD(clock_process);
         sensitive << clock_in.posedge_event();
         dont_initialize();
      }
      void clock_process()
      {
         std::cout << "@" << sc_time_stamp() << ": Triggered clock process" << std::endl;
      }
      void clock_changed()
      {
         std::cout << "@" << sc_time_stamp() << ": Clock period changed to:" << m_mux->period() << std::endl;
      }
};

int sc_main(int argc, char *argv[])
{
   sc_clock clock0("clock0", sc_time(2, SC_NS));
   sc_clock clock1("clock1", sc_time(3, SC_NS));
   sc_clock clock2("clock2", sc_time(5, SC_NS));
   sc_signal<int> clock_select;
   clock_mux mux("mux", 3);
   clock_receiver rx("rx");

   mux.clock_in[0](clock0);
   mux.clock_in[1](clock1);
   mux.clock_in[2](clock2);
   mux.clock_select(clock_select);
   rx.clock_in(mux);

   clock_select.write(0);
   sc_start(10, SC_NS);
   clock_select.write(1);
   sc_start(20, SC_NS);
   clock_select.write(2);
   sc_start(30, SC_NS);

   return 0;
}

