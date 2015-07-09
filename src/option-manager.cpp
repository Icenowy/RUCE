/*
    Rocaloid
    Copyright (C) 2015 StarBrilliant <m13253@hotmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program.  If not,
    see <http://www.gnu.org/licenses/>.
*/

#include "option-manager.hpp"
#include <stdexcept>

namespace RUCE {

OptionManager::OptionManager() {
    if(m_option_manager_count) 
        throw std::runtime_error("Cannot create another OptionManager instance");
    m_option_manager_count++;
}

int OptionManager::m_option_manager_count = 0;

WTF8::u8string OptionManager::get_input_file() {
    return m_input_file;
}

}

