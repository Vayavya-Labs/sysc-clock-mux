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

#include "clock_mux.h"

using namespace sc_core;

clock_mux::clock_mux(sc_module_name n, int num_clocks)
   : sc_module(n),
     clock_in("clock_in", num_clocks),
     clock_select("clock_select"),
     m_value_changed_handles(num_clocks),
     m_posedge_handles(num_clocks),
     m_negedge_handles(num_clocks),
     m_sc_clock_interfaces(num_clocks, 0),
     m_clock_mux_interfaces(num_clocks, 0)
{
   SC_METHOD(selection_changed);
   sensitive << clock_select;
   dont_initialize();
}

void clock_mux::end_of_elaboration()
{
   for(int i=0; i<(int)clock_in.size(); i++) {
      {
         sc_spawn_options opt;
         opt.spawn_method();
         opt.set_sensitivity(&clock_in[i].value_changed_event());
         opt.dont_initialize();

         m_value_changed_handles[i] = sc_spawn(sc_bind(&clock_mux::handle_value_changed, this, i), sc_gen_unique_name("value_change_method"), &opt);
         m_value_changed_handles[i].disable();
      }
      {
         sc_spawn_options opt;
         opt.spawn_method();
         opt.set_sensitivity(&clock_in[i].posedge_event());
         opt.dont_initialize();

         m_posedge_handles[i] = sc_spawn(sc_bind(&clock_mux::handle_posedge, this, i), sc_gen_unique_name("posedge_method"), &opt);
         m_posedge_handles[i].disable();
      }
      {
         sc_spawn_options opt;
         opt.spawn_method();
         opt.set_sensitivity(&clock_in[i].negedge_event());
         opt.dont_initialize();

         m_negedge_handles[i] = sc_spawn(sc_bind(&clock_mux::handle_negedge, this, i), sc_gen_unique_name("negedge_method"), &opt);
         m_negedge_handles[i].disable();
      }

      sc_interface *intfs = clock_in[i].get_interface();
      m_sc_clock_interfaces[i] = dynamic_cast<sc_clock *>(intfs);
      m_clock_mux_interfaces[i] = dynamic_cast<clock_mux *>(intfs);
   }
}

void clock_mux::start_of_simulation()
{
   m_current_selected = clock_select;
   m_value_changed_handles[m_current_selected].enable();
   m_posedge_handles[m_current_selected].enable();
   m_negedge_handles[m_current_selected].enable();
}

/*
 * FIXME
 * Note: If there is an event on the currently active clock (value changed, pos
 * or neg) AND the clock selection changes in the same eval phase, then there is 
 * indeterminism in what events get finally triggered.
 *
 * One way around this is to do delta notification of the event
 * m_value_changed_event (etc), and cancel the notification if clock selection
 * changes.
 */
void clock_mux::selection_changed()
{
   // Disable old
   m_value_changed_handles[m_current_selected].disable();
   m_posedge_handles[m_current_selected].disable();
   m_negedge_handles[m_current_selected].disable();
   // Enable new
   m_current_selected = clock_select;
   m_value_changed_handles[m_current_selected].enable();
   m_posedge_handles[m_current_selected].enable();
   m_negedge_handles[m_current_selected].enable();
   m_clock_changed_event.notify();
}

void clock_mux::handle_value_changed(int index)
{
   // sc_assert(index == m_current_selected);
   m_value_changed_event.notify();
}

void clock_mux::handle_posedge(int index)
{
   // sc_assert(index == m_current_selected);
   m_posedge_event.notify();
}

void clock_mux::handle_negedge(int index)
{
   // sc_assert(index == m_current_selected);
   m_negedge_event.notify();
}

const bool& clock_mux::read() const
{
   return clock_in[m_current_selected].read();
}

const sc_event& clock_mux::value_changed_event() const
{
   return m_value_changed_event;
}

const sc_event& clock_mux::posedge_event() const
{
   return m_posedge_event;
}

const sc_event& clock_mux::negedge_event() const
{
   return m_negedge_event;
}

bool clock_mux::event() const
{
   return clock_in[m_current_selected].event();
}

bool clock_mux::posedge() const
{
   return clock_in[m_current_selected].posedge();
}

bool clock_mux::negedge() const
{
   return clock_in[m_current_selected].negedge();
}

const sc_time& clock_mux::period() const
{
   if(m_sc_clock_interfaces[m_current_selected] != 0)
      return m_sc_clock_interfaces[m_current_selected]->period();
   if(m_clock_mux_interfaces[m_current_selected] != 0)
      return m_clock_mux_interfaces[m_current_selected]->period();
   return SC_ZERO_TIME;
}

double clock_mux::duty_cycle() const
{
   if(m_sc_clock_interfaces[m_current_selected] != 0)
      return m_sc_clock_interfaces[m_current_selected]->duty_cycle();
   if(m_clock_mux_interfaces[m_current_selected] != 0)
      return m_clock_mux_interfaces[m_current_selected]->duty_cycle();
   return 0.0;
}

sc_time clock_mux::start_time() const
{
   if(m_sc_clock_interfaces[m_current_selected] != 0)
      return m_sc_clock_interfaces[m_current_selected]->start_time();
   if(m_clock_mux_interfaces[m_current_selected] != 0)
      return m_clock_mux_interfaces[m_current_selected]->start_time();
   return SC_ZERO_TIME;
}

bool clock_mux::posedge_first() const
{
   if(m_sc_clock_interfaces[m_current_selected] != 0)
      return m_sc_clock_interfaces[m_current_selected]->posedge_first();
   if(m_clock_mux_interfaces[m_current_selected] != 0)
      return m_clock_mux_interfaces[m_current_selected]->posedge_first();
   return false;
}

const char* clock_mux::kind() const
{
   return "clock_mux";
}

const bool& clock_mux::get_data_ref() const
{
   static bool flag = false;
   sc_assert(false); // Tracing not yet supported
   return flag;
}

const sc_event& clock_mux::clock_changed_event() const
{
   return m_clock_changed_event;
}
