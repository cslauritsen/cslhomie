#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#ifndef NO_MBEDTLS
extern "C" {
#include <mbedtls/sha512.h>
}
#endif

#include "version.hpp"
