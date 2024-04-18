#ifndef KERNEL_TESTS_HPP
#define KERNEL_TESTS_HPP

#include "memory_manager.hpp"
#include "ps_pointer.hpp"
#include "io.hpp"

#include "disk_driver.hpp"

/*********************
 * 
 *  KERNEL TEST
 * 
**********************/

void test_memory_manager(memory_manager& mm);
void test_disk_driver(disk_driver& diskDriver);

#endif