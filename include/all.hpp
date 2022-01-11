#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#ifndef NO_MBEDTLS
extern "C" {
#include <mbedtls/sha512.h>
}
#endif

#include "version.hpp"
