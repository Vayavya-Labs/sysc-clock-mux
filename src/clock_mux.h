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

#pragma once

#include <systemc>
#include <vector>

/*
 * \file clock_mux.h
 * \brief Header file for the clock mux
 */

/*
 * \brief SystemC module that implements clock mux functionality
 */
class clock_mux : public sc_core::sc_channel, public sc_core::sc_signal_in_if<bool>
{
   public:
      // Input clocks - expected to be connected to sc_clock or clock_mux
      sc_core::sc_vector<sc_core::sc_in<bool>> clock_in;
      // Select which of the inputs is treated as currently active
      sc_core::sc_in<int> clock_select;

      SC_HAS_PROCESS(clock_mux);
      clock_mux(sc_core::sc_module_name n, int num_clocks);

      // Derived from sc_signal_in_if<bool>
      virtual const bool& read() const;
      virtual const sc_core::sc_event& value_changed_event() const;
      virtual const sc_core::sc_event& posedge_event() const;
      virtual const sc_core::sc_event& negedge_event() const;
      virtual bool event() const;
      virtual bool posedge() const;
      virtual bool negedge() const;

      // Deprecated in IEEE1666-2011, but present as pure virtual in Accellera
      // simulator 2.3.3 (hence needs a dummy implementation)
      virtual const bool& get_data_ref() const;

      // Additional methods, similar to sc_clock
      const sc_core::sc_time& period() const;
      double duty_cycle() const;
      sc_core::sc_time start_time() const; // Note: signature differs from LRM - copied from Accellera simulator 2.3.3
      bool posedge_first() const;
      virtual const char* kind() const;

      // Get an event that gets notified when the clock parameters change
      const sc_core::sc_event& clock_changed_event() const;

   private:
      sc_core::sc_event m_value_changed_event;
      sc_core::sc_event m_posedge_event;
      sc_core::sc_event m_negedge_event;
      sc_core::sc_event m_clock_changed_event;
      std::vector<sc_core::sc_process_handle> m_value_changed_handles;
      std::vector<sc_core::sc_process_handle> m_posedge_handles;
      std::vector<sc_core::sc_process_handle> m_negedge_handles;
      int m_current_selected;
      std::vector<sc_core::sc_clock *> m_sc_clock_interfaces;
      std::vector<clock_mux *> m_clock_mux_interfaces;

      void start_of_simulation();
      void end_of_elaboration();
      void selection_changed();
      void handle_value_changed(int index);
      void handle_posedge(int index);
      void handle_negedge(int index);
};
