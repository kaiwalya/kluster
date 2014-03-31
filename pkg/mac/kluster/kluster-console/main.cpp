//
//  main.cpp
//  kluster-console
//
//  Created by Kaiwalya Kher on 3/31/14.
//  Copyright (c) 2014 Kaiwalya Kher. All rights reserved.
//

#include <iostream>
#include <zmq.h>


int main(int argc, const char * argv[])
{
	auto ctx = zmq_ctx_new();
	if (ctx) {
		zmq_ctx_destroy(ctx);
	}
}

