/*
 *  Created by Phil on 20/05/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED

#ifndef __OBJC__

extern "C" int main(int argc, char * argv[]);

#else // __OBJC__

// Objective-C entry point
extern int main(int argc, char * const argv[]);

#endif // __OBJC__

#endif // TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
