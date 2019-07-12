#pragma once

#define LAMBDA(x, r) [](auto x) { return r; }

static_assert(LAMBDA(x, x* x)(3) == 9);

#define VISITOR(x, f) [](auto&& x) { f }
